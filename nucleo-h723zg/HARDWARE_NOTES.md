# NUCLEO-H723ZG Hardware Notes

---

## Robot Platform

| Property | Details |
|----------|---------|
| Drive topology | **Differential drive** |
| Frame shape | **Round** |
| Motors (×2) | **Pololu 50:1 Metal Gearmotor 37Dx70L mm 12V with 64 CPR Encoder (Helical Pinion)** |
| Gear ratio | 50 : 1 |
| Motor max speed | **200 RPM** at wheel shaft |
| Encoder resolution | 64 CPR on motor shaft → **3 200 CPR** at wheel shaft (64 × 50) |
| Supply voltage | 12 V nominal |

> One motor drives the **left** wheel, the other drives the **right** wheel.
> Forward motion is achieved when both motors spin in the same direction;
> turns are achieved by applying a differential speed or direction between the two sides.

---

| 13-bit @ 64 MHz | 64 MHz | 0 | 8191 | 7 812.5 Hz | **~7 850 Hz ✓** |
| **20 kHz @ 64 MHz** | **64 MHz** | **0** | **3199** | **20 000 Hz** | pending |

### PWM Frequency & Resolution Formula

```
Timer clock  = SYSCLK / (PSC + 1)
PWM freq     = Timer clock / (ARR + 1)
Resolution   = log2(ARR + 1)  bits
```

**Raw calculation (Config B values substituted in):**
```
Step 1 — Choose resolution:
  ARR = 2^13 - 1 = 8191   →   8192 steps

Step 2 — Find required timer clock:
  Timer clock = target_freq × (ARR + 1)
  Timer clock = 18310.55 × 8192
  Timer clock = 150,041,306 Hz  ≈  150 MHz

Step 3 — Choose PSC to hit 150 MHz from SYSCLK:
  PSC = (SYSCLK / Timer clock) - 1
  PSC = (150,000,000 / 150,000,000) - 1 = 0   →   no pre-division

Step 4 — Verify actual PWM frequency:
  PWM freq = 150,000,000 / (0 + 1) / (8191 + 1)
  PWM freq = 150,000,000 / 8192
  PWM freq = 18310.546875 Hz

Step 5 — Verify error:
  Error = |18310.55 - 18310.546875| = 0.003125 Hz   (<< 0.1 Hz ✓)
```

---

### Configuration C — 20 kHz, ~11.6-bit ✅ Current default (no PLL needed)

| Parameter | Value | Note |
|-----------|-------|------|
| SYSCLK    | 64 MHz | HSI, no PLL |
| PSC       | **0** | Timer clock = 64 MHz |
| ARR       | **3199** | PWM freq = 64 MHz / 3200 = **20 000 Hz** |
| Resolution | log2(3200) ≈ **11.64 bits** | 3200 discrete steps |
| Max `setMotorPwm()` | **3199** | = 100 % duty cycle |
| 50 % duty cycle | **1600** | mid-speed |

```cpp
static PwmOutput motor(TIM1, TIM_CHANNEL_1);   // PSC=0, ARR=3199 → 20 kHz (default)
motor.setMotorPwm(1600);                        // 50 %
```

> ✅ **Above human hearing threshold (~20 kHz)** — silent motor operation.
> No PLL required. Works with current HSI 64 MHz clock.

---

### Configuration A — 1 kHz, ~10-bit

| Parameter | Value | Note |
|-----------|-------|------|
| SYSCLK    | 64 MHz | HSI, no PLL |
| PSC       | 63 | Timer clock = 64 MHz / 64 = **1 MHz** |
| ARR       | 999 | PWM freq = 1 MHz / 1000 = **1 kHz** |
| Resolution | log2(1000) ≈ **9.97 bits** | ~1000 steps |

```cpp
static PwmOutput motor(TIM1, TIM_CHANNEL_1, 63, 999);   // 1 kHz
```

---

### Configuration B — 18310.55 Hz, 13-bit resolution ⚠️ Requires PLL

| Parameter | Value | Note |
|-----------|-------|------|
| Required timer clock | 18310.55 × 8192 = **150 000 000 Hz** | 150 MHz |
| SYSCLK needed | **150 MHz** via PLL1 | ⚠️ Current HSI 64 MHz is insufficient |
| PSC | **0** | No pre-division: Timer clock = SYSCLK = 150 MHz |
| ARR | **8191** | 2¹³ − 1 → 13-bit resolution (8192 steps) |
| Actual PWM freq | 150 000 000 / 8192 = **18310.546875 Hz** | Error vs target: < 0.01 Hz |
| Resolution | log2(8192) = **13 bits** | 8192 discrete duty-cycle steps |

```cpp
// ⚠️ Only valid after PLL is configured to 150 MHz on APB2 timer clock
static PwmOutput motor(TIM1, TIM_CHANNEL_1, 0, 8191);   // ~18310.55 Hz, 13-bit
```

> **Note:** TIM1 sits on APB2. With PLL configured to 150 MHz and APB2 prescaler = 1,
> the timer clock = 150 MHz directly.
> `SystemClock_Config()` must be updated before using this configuration.

---

### Other useful frequencies at 13-bit resolution (PSC = 0, ARR = 8191)

| Timer clock | PWM freq |
|-------------|----------|
| 64 MHz  | 7 812.5 Hz  |
| 100 MHz | 12 207.0 Hz |
| 150 MHz | 18 310.5 Hz ← target |
| 200 MHz | 24 414.1 Hz |

---

**To change frequency**, pass custom `prescaler` / `period` to `PwmOutput` constructor:
```cpp
// 50 Hz for servos (PSC=63, ARR=19999 @ 64 MHz timer clock)
static PwmOutput servo(TIM3, TIM_CHANNEL_1, 63, 19999);
```

---

## Encoder GPIO & EXTI Wiring

### Background — EXTI line index equals pin number, not port

On STM32H7 the EXTI multiplexer routes **one port per pin number** to a single EXTI line.
`PA2`, `PB2`, `PC2` … all compete for **EXTI2** — only one port can be active at a time
(selected via `SYSCFG_EXTICRx`).  The NVIC vector that fires depends solely on the line
number, not the port letter.

```
GPIO pin number   NVIC vector              Type
────────────────  ───────────────────────  ──────────────────────
0                 EXTI0_IRQHandler         dedicated ✅
1                 EXTI1_IRQHandler         dedicated ✅
2                 EXTI2_IRQHandler         dedicated ✅
3                 EXTI3_IRQHandler         dedicated ✅
4                 EXTI4_IRQHandler         dedicated ✅
5 – 9             EXTI9_5_IRQHandler       shared ⚠️  must demux
10 – 15           EXTI15_10_IRQHandler     shared ⚠️  must demux
```

---

### Option A — Dedicated vectors ✅ (current implementation)

Use **pin numbers 0–4** on any free port.  Each channel gets its own vector — no
pending-flag demux needed.  Current assignment:

| Signal        | Pin | EXTI line | IRQ vector            | Connector |
|---------------|-----|-----------|-----------------------|-----------|
| Motor 1 Enc A | PB1 | EXTI1     | `EXTI1_IRQHandler`    | CN12      |
| Motor 1 Enc B | PB2 | EXTI2     | `EXTI2_IRQHandler`    | CN12      |

**`isr_handlers.cpp`** — one line per channel, zero demux:

```cpp
void EXTI1_IRQHandler(void) {
    __HAL_GPIO_EXTI_CLEAR_IT(MOTOR1_ENC_A_PIN);
    Encoder::isrChannelA(0);
}

void EXTI2_IRQHandler(void) {
    __HAL_GPIO_EXTI_CLEAR_IT(MOTOR1_ENC_B_PIN);
    Encoder::isrChannelB(0);
}
```

**`main.cpp`** — two separate NVIC enables:

```cpp
HAL_NVIC_SetPriority(MOTOR1_ENC_A_IRQn, MOTOR1_ENC_IRQ_PRIORITY, 0);
HAL_NVIC_EnableIRQ(MOTOR1_ENC_A_IRQn);
HAL_NVIC_SetPriority(MOTOR1_ENC_B_IRQn, MOTOR1_ENC_IRQ_PRIORITY, 0);
HAL_NVIC_EnableIRQ(MOTOR1_ENC_B_IRQn);
```

---

### Option B — Shared vector ⚠️ (pin numbers 5–9 or 10–15)

If encoder pins must land on lines 10–15 (e.g. PE10 / PE12), both share
`EXTI15_10_IRQHandler`.  The pending register must be read to determine which pin
fired, then cleared before returning — failing to clear causes the CPU to re-enter
the ISR immediately in a tight loop, starving FreeRTOS.

**`isr_handlers.cpp`** — must demux and clear each flag individually:

```cpp
void EXTI15_10_IRQHandler(void) {
    if (__HAL_GPIO_EXTI_GET_IT(MOTOR1_ENC_A_PIN) != RESET) {
        __HAL_GPIO_EXTI_CLEAR_IT(MOTOR1_ENC_A_PIN);
        Encoder::isrChannelA(0);
    }
    if (__HAL_GPIO_EXTI_GET_IT(MOTOR1_ENC_B_PIN) != RESET) {
        __HAL_GPIO_EXTI_CLEAR_IT(MOTOR1_ENC_B_PIN);
        Encoder::isrChannelB(0);
    }
}
```

**`MotorPinConfig.h`** — both channels share one IRQn:

```c
#define MOTOR1_ENC_A_PORT   GPIOE
#define MOTOR1_ENC_A_PIN    GPIO_PIN_10   /* EXTI10 */
#define MOTOR1_ENC_B_PORT   GPIOE
#define MOTOR1_ENC_B_PIN    GPIO_PIN_12   /* EXTI12 */
#define MOTOR1_ENC_IRQn     EXTI15_10_IRQn
```

**`main.cpp`** — single NVIC enable covers both pins:

```cpp
HAL_NVIC_SetPriority(MOTOR1_ENC_IRQn, MOTOR1_ENC_IRQ_PRIORITY, 0);
HAL_NVIC_EnableIRQ(MOTOR1_ENC_IRQn);
```

---

### Encoder interrupt priority (`MOTOR1_ENC_IRQ_PRIORITY`)

#### Step 1 — Hardware priority numbers

The STM32H7 Cortex-M7 core has **4 priority bits**, giving **16 levels numbered 0–15**:

- **0 = most urgent** — this IRQ fires first and can interrupt any lower-priority IRQ.
- **15 = least urgent** — any higher-priority IRQ can interrupt this one.

You pass this number to `HAL_NVIC_SetPriority()`.  Lower number = higher urgency.

---

#### Step 2 — What is "the FreeRTOS API"?

FreeRTOS is a mini operating system that runs tasks, queues, semaphores, and timers.
It provides functions your code calls — for example:

| Function | What it does |
|---|---|
| `vTaskDelay(100)` | Pause the current task for 100 ms |
| `xQueueSend(q, &data, 0)` | Put data into a queue from a task |
| `xQueueSendFromISR(q, &data, &woken)` | Same, but called from inside an **interrupt handler** |
| `xTaskNotifyFromISR(handle, ...)` | Wake a sleeping task from inside an **interrupt handler** |

The `...FromISR` suffix marks the **interrupt-safe** versions of these functions.
Regular versions (without `FromISR`) **must never be called from an interrupt** — they
will corrupt the kernel's internal data structures and cause a crash.

---

#### Step 3 — Why does FreeRTOS need to control which interrupts fire?

FreeRTOS maintains internal linked lists (the ready list, the delayed-task list, etc.).
When a `FromISR` function updates one of these lists, it must do so **atomically** —
no other interrupt must be allowed to touch the same list at the same time.

FreeRTOS achieves this on Cortex-M by writing to the **BASEPRI register**, which
masks (temporarily silences) all interrupts **at or below a given priority level**.
When BASEPRI is set to 5, priorities 5–15 are silenced; priorities 0–4 still fire.

---

#### Step 4 — The two zones (the critical concept)

`configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY = 5` (set in `FreeRTOSConfig.h`) is
the dividing line:

```
 NVIC priority │ Zone            │ Behaviour
───────────────┼─────────────────┼───────────────────────────────────────────────
  0            │                 │ e.g. safety hard-stop, zero-latency ADC
  1            │  UNMANAGED      │
  2            │  (above limit)  │ FreeRTOS CANNOT mask these — they fire even
  3            │                 │ while FreeRTOS is editing its own task lists.
  4            │  ⚡ danger      │ Calling ANY FreeRTOS function here (even FromISR)
               │                 │ risks reading half-updated data → crash / hang.
───────────────┼─────────────────┼───────────────────────────────────────────────
  5  ◄── limit │                 │ Highest urgency where FromISR calls are safe.
  6            │  MANAGED        │
  7            │  (at/below      │ FreeRTOS masks priorities 5–15 during its own
  8            │   limit)        │ critical sections, so its data is always in a
  ...          │                 │ consistent state when your ISR reads it.
  14           │  ✅ safe        │
  15           │                 │ SysTick + PendSV live here (kernel internals)
───────────────┴─────────────────┴───────────────────────────────────────────────
```

**"Managed"** means FreeRTOS is aware of the interrupt, can temporarily silence it,
and guarantees it sees consistent internal state → `FromISR` calls are safe.

**"Unmanaged"** means FreeRTOS is completely blind to the interrupt — it never
silences it, so it can fire at any moment, even mid-way through a kernel operation.
These are reserved for bare-metal, zero-OS work (e.g. a microsecond-accurate
hard-stop triggered by a hardware fault signal).

---

#### Step 5 — Why priority 5 for the encoder?

Priority **5** is the sweet spot:

- It is the **highest urgency** number still in the managed zone.
- The encoder fires on every quadrature edge at potentially high frequency — it
  should not sit at a low urgency where a UART or USB IRQ could delay it.
- Even though the encoder ISR currently does **not** call any FreeRTOS function,
  setting it to 5 means that adding `xTaskNotifyFromISR()` later (e.g. to wake a
  PID control task on each tick) requires **zero changes** to the priority config.

> ⚠️ If you change `MOTOR1_ENC_IRQ_PRIORITY` to **4 or lower**, any `FromISR` call
> inside the handler will silently corrupt the kernel.  In debug builds
> `configASSERT` will catch this and deliberately trigger a HardFault so the
> CrashHandler can record the fault frame.

---

#### Further reading

- **FreeRTOS official explanation** (Cortex-M, identical BASEPRI mechanism on M7):
  https://www.freertos.org/RTOS-Cortex-M3-M4.html
  → search the page for `configMAX_SYSCALL_INTERRUPT_PRIORITY`
- **Cortex-M exception model** (ARM):
  https://developer.arm.com/documentation/100235/latest — Chapter "Exception model"

---

### Common pitfalls

| Pitfall | Symptom | Fix |
|---------|---------|-----|
| `GpioInput` constructed as a temporary passed to `Encoder` | Destructor calls `HAL_GPIO_DeInit()` immediately, wiping EXTI config; no interrupts ever fire | Declare `GpioInput` objects as `static` locals so they outlive the constructor call |
| Pending flag not cleared in ISR | CPU re-enters ISR in a tight loop on every edge; FreeRTOS starved; encoder appears dead | Always call `__HAL_GPIO_EXTI_CLEAR_IT(pin)` at the top of every EXTI handler |
| Using `GPIO_MODE_INPUT` instead of `GPIO_MODE_IT_RISING_FALLING` | Pin configured for polling only; EXTI trigger never armed | Pass `GPIO_MODE_IT_RISING_FALLING` as the `mode` argument to `GpioInput` |
| Encoder pin on line 10–15, handler written as `EXTI10_IRQHandler` | Linker resolves to `Default_Handler` (infinite loop); no encoder ticks | Use `EXTI15_10_IRQHandler` with demux for lines 10–15 |

---



