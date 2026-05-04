# RTOS Design — ISR / Task Concurrency

This document explains the concurrency model used in this project: how data is safely
shared between FreeRTOS tasks and hardware ISRs, why certain C++ types are chosen,
and what the ARM Cortex-M7 hardware actually does under the hood.

---

## 1. The Two Execution Contexts

```
┌─────────────────────────────────────────────────────┐
│  FreeRTOS Task  (e.g. HostCommandHandlerFreeRTOSTask)│
│  • Runs when the scheduler gives it CPU time         │
│  • Can be preempted at any instruction boundary      │
│  • May call blocking FreeRTOS API (vTaskDelay, etc.) │
└────────────────────────┬────────────────────────────┘
                         │  shared variables
┌────────────────────────▼────────────────────────────┐
│  ISR  (e.g. TIM2_IRQHandler → PiMotorControl::isr)  │
│  • Fires at a fixed hardware period (every 100 µs)   │
│  • Immediately preempts any running task             │
│  • Must NEVER call blocking FreeRTOS functions       │
└─────────────────────────────────────────────────────┘
```

The task writes velocity **references** (`m_left_ref_w`, `m_right_ref_w`).  
The ISR reads them every tick and writes velocity **measurements** (`m_left_read_w`, `m_right_read_w`).  
Both sides access these variables without a mutex — the sections below explain why that
is correct and how it is made safe.

---

## 2. Atomic Variables — What "Atomic" Means

**Atomic** means a read or write completes in a **single, indivisible CPU instruction**.
It either happened fully or not at all — the ISR can never catch it halfway through.

On Cortex-M7 the data bus is **32 bits wide**, so the maximum naturally atomic type is
32 bits:

| C++ type | Size | Atomic on Cortex-M7? | Instruction |
|---|---|---|---|
| `bool`, `uint8_t` | 8-bit | ✅ | `LDRB` / `STRB` |
| `uint16_t` | 16-bit | ✅ | `LDRH` / `STRH` |
| `float`, `uint32_t` | 32-bit | ✅ | `LDR` / `STR` |
| `double`, `uint64_t` | **64-bit** | ❌ | **Two** 32-bit instructions — ISR can fire between them |

### Why `double` is dangerous in ISR-shared variables

The compiler must split a 64-bit write into two 32-bit stores:

```
double value = 1.5;

Memory layout:
┌─────────────────┬─────────────────┐
│  low 32 bits    │  high 32 bits   │
└─────────────────┴─────────────────┘

Emitted instructions:
  STR  low_half,  [address]       ← instruction 1
  STR  high_half, [address + 4]   ← instruction 2
                ↑
     ISR can fire HERE.
     It reads low_half from the NEW value
     and high_half from the OLD value.
     The result is a garbage double —
     possibly NaN, infinity, or a completely wrong number.
```

This corruption is called **tearing**.

### `std::atomic<double>` does NOT solve this on Cortex-M7

ARMv7-M has no 64-bit atomic instruction.  The C++ standard library implements
`std::atomic<double>` using a **software spinlock**.  This is not lock-free and must
**never be used inside an ISR** — if the ISR fires while the task holds the lock, the
system deadlocks.

```cpp
// Compile-time check — will fail on Cortex-M7:
static_assert(std::atomic<double>::is_always_lock_free,
              "atomic<double> is not lock-free — unsafe in ISR context");
```

**Rule: use `std::atomic<float>` (32-bit, always lock-free) for any variable shared
between a task and an ISR.**

---

## 3. `memory_order` — Controlling CPU Synchronisation

`std::atomic` operations accept an optional `memory_order` argument that tells the
compiler (and CPU) **how strictly** to synchronise memory around the operation.

### `memory_order_seq_cst` (the default)

The strictest ordering.  On ARM it emits a **DMB** (Data Memory Barrier) instruction
after every store or load:

```asm
STR  r0, [m_left_ref_w]
DMB                        ← barrier: wait for all pending memory ops to complete
```

**DMB** is designed for **multi-core** CPUs where Core 0's write cache must be flushed
so Core 1 can see the updated value.

### `memory_order_relaxed`

No barrier emitted — just a plain `STR` or `LDR`:

```asm
STR  r0, [m_left_ref_w]    ← that's it
```

### Which to use here?

The STM32H723 Cortex-M7 is a **single-core** CPU.  There is no second core with a
separate cache.  Instructions execute in order.  When the ISR reads a variable the task
just wrote, it will always see the latest value — no barrier needed.

```
seq_cst store:                     relaxed store:
─────────────────────────          ──────────────────
STR  r0, [m_left_ref_w]            STR  r0, [m_left_ref_w]
DMB  ◄── wasted ~3 cycles          (done)
─────────────────────────          ──────────────────
```

The ISR runs at **10 kHz**.  Two `seq_cst` accesses per tick = **20 000 wasted DMB
instructions per second**.

**Rule: use `memory_order_relaxed` for all ISR-shared atomic loads and stores on this
single-core platform.**

---

## 4. The "Atomic Pair" Problem

Each `std::atomic<float>` variable is individually atomic — its own store or load cannot
be torn.  But two *separate* atomic stores are **not jointly atomic** — the ISR can
fire between them:

```
Task calls setVelocities(1.5, 2.0):

  STR  1.5 → m_left_ref_w     ← store #1 completes
  ↑
  TIM2 ISR fires HERE
  ISR reads:  left = 1.5  (new)
              right = 0.0 (old)   ← mismatched pair for one tick!
  ISR finishes, task resumes
  ↓
  STR  2.0 → m_right_ref_w    ← store #2 completes
```

The dangerous window is the **~2 ns gap** between the two store instructions
(one CPU cycle at 550 MHz ≈ 1.8 ns).  The ISR fires every **100 000 ns**.

```
Probability = 2 ns / 100 000 ns = 0.002 % per setVelocities() call
```

At 100 Hz command rate that is once every ~14 hours on average.  The effect is a
single 100 µs tick with slightly asymmetric torque — unobservable in robot motion.

**This is accepted by design and documented.** If it were unacceptable, the fix would
be a PRIMASK critical section:

```cpp
void PiMotorControl::setVelocities(const float left, const float right) {
    __disable_irq();               // set PRIMASK — masks ALL interrupts
    m_left_ref_w  = left;
    m_right_ref_w = right;
    __enable_irq();                // clear PRIMASK — ~3 instructions total
}
```

### Snapshot at the top of handleTick()

Even though the pair race is accepted, `handleTick()` always **snapshots both values
at the very start** before doing any computation:

```cpp
void PiMotorControl::handleTick() {
    // Snapshot first — both wheels use the same command pair for this tick.
    const float leftRefW  = m_left_ref_w.load(std::memory_order_relaxed);
    const float rightRefW = m_right_ref_w.load(std::memory_order_relaxed);
    // ... use leftRefW / rightRefW for the rest of the tick ...
}
```

Without snapshotting, `m_right_ref_w` could theoretically be read later in the function
than `m_left_ref_w`, widening the window slightly and producing less clear code.

---

## 5. FreeRTOS Interrupt Priority Zones

The STM32H723 Cortex-M7 core has **4 NVIC priority bits** = 16 levels (0 = most urgent,
15 = least urgent).

`configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY = 5` (set in `FreeRTOSConfig.h`) divides
all interrupt priorities into two zones:

```
 NVIC priority │ Zone            │ Rule
───────────────┼─────────────────┼────────────────────────────────────────────────
  0            │                 │ Reserved for bare-metal zero-latency work only.
  1            │  UNMANAGED      │
  2            │  (above limit)  │ FreeRTOS CANNOT mask these.  They fire even
  3            │                 │ while the kernel is editing its own task lists.
  4            │  ⚡ danger      │ Calling ANY FreeRTOS function here — even
               │                 │ FromISR variants — risks reading half-updated
               │                 │ kernel data → crash / hang.
───────────────┼─────────────────┼────────────────────────────────────────────────
  5  ◄── limit │                 │ Highest urgency where FromISR calls are safe.
  6            │  MANAGED        │
  7            │  (at/below      │ FreeRTOS writes BASEPRI=5 during its own
  8            │   limit)        │ critical sections, masking priorities 5–15
  ...          │                 │ so its data structures are always consistent
  14           │  ✅ safe        │ when your ISR runs.
  15           │                 │ SysTick + PendSV (kernel internals) live here.
───────────────┴─────────────────┴────────────────────────────────────────────────
```

**BASEPRI register:** FreeRTOS sets `BASEPRI = 5` to enter a critical section.  All
interrupts at priority 5–15 are silenced while the kernel edits its internal lists.
Priorities 0–4 are never silenced — they can fire at any moment, including mid-kernel.

**Rule: assign all encoder and motor-control ISRs to priority 5** — the highest urgency
that is still managed.  This allows `xTaskNotifyFromISR()` or similar calls to be added
later without any reconfiguration.

> ⚠️ Setting any motor/encoder ISR to priority 4 or lower makes `FromISR` calls silently
> unsafe.  In debug builds `configASSERT` will catch this and deliberately trigger a
> HardFault so the CrashHandler can record the fault frame.

---

## 6. Summary — Rules at a Glance

| Situation | Rule |
|---|---|
| Variable shared between task and ISR, 32-bit | Use `std::atomic<float>` or `std::atomic<uint32_t>` |
| Variable shared between task and ISR, 64-bit | **Not possible lock-free on Cortex-M7** — use `float` instead, or protect with `__disable_irq()` |
| `std::atomic` memory order on single-core MCU | Always use `memory_order_relaxed` — no `DMB` needed |
| Two related variables that must be updated together | Document the accepted gap, or use `__disable_irq()` / `__enable_irq()` |
| Snapshot in ISR | Load all shared inputs at the **top** of the ISR function, store in `const` locals |
| ISR priority — FreeRTOS `FromISR` calls | Priority **5–15** only (`configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY = 5`) |
| ISR priority — no FreeRTOS calls at all | Any priority 0–15, but keep ≥ 5 unless sub-microsecond latency is required |

---

## 7. Further Reading

| Resource | Topic |
|---|---|
| [FreeRTOS Cortex-M BASEPRI explanation](https://www.freertos.org/RTOS-Cortex-M3-M4.html) | `configMAX_SYSCALL_INTERRUPT_PRIORITY`, managed vs unmanaged zones |
| [ARM ARMv7-M Architecture Reference Manual](https://developer.arm.com/documentation/ddi0403/latest) | `DMB`, `LDREX`/`STREX`, memory model |
| `HARDWARE_NOTES.md` | Encoder wiring, EXTI lines, NVIC priority assignments |
| `TIMING_CONSIDERATIONS.md` | ISR tick rate, DWT timer, velocity measurement accuracy |
| `CODING_STANDARDS.md §2` | FreeRTOS scheduler-state guards, mutex rules |

