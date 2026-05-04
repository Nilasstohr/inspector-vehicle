# Design Patterns

This document records the design patterns applied in the project.
Each entry explains the pattern, where it is used, and why it was chosen.

---

## 1. Template Method — `FreeTOSTask`

> **Pattern origin:** Official GoF pattern — *Design Patterns*, Gamma et al. (1994)

**Files:** `src/freetos/FreeTOSTask.h`, `src/freetos/FreeTOSTask.cpp`  
**Example subclass:** `src/host_command_handler/HostCommandHandlerFreeTOSTask.h`

### Intent
Define the skeleton of an algorithm in a base class, deferring one or more steps to subclasses without letting subclasses alter the overall structure.

### How it is applied here

```
FreeTOSTask::start()        ← public, non-virtual  (caller entry point)
    └─ xTaskCreate(taskEntry, ...)
          └─ FreeTOSTask::taskEntry()   ← private static trampoline
                └─ this->run()          ← protected pure-virtual  (subclass fills in)
```

`FreeTOSTask` owns the FreeRTOS task lifecycle:
- `start()` calls `xTaskCreate`, passing `this` and the static trampoline.
- `taskEntry()` is the C-compatible function pointer FreeRTOS invokes; it recovers the object pointer and calls `run()`.
- `run()` is declared `protected virtual = 0` — subclasses must implement it, but cannot bypass the lifecycle managed by the base.

### Why Non-Virtual Interface (NVI)

> **Pattern origin:** Established C++ idiom — named by Herb Sutter in *Exceptional C++* (2000). Not in the original GoF book, but widely recognised.

`start()` and `taskEntry()` are non-virtual intentionally. This is the **Non-Virtual Interface** variant of Template Method:
- The base class retains full control over *when* and *how* the task is created and entered.
- Subclasses can only vary *what* the task does (`run()`), not how it is registered with the kernel.

### Pinned Object (supporting constraint)

> **Pattern origin:** Descriptive name used here — not an official pattern name. The underlying technique (deleting copy/move) is sometimes called a *non-copyable object* or covered under *Object Lifetime Management*, but has no single canonical name.

Copy and move operations are `= delete` because `this` is registered with the FreeRTOS kernel inside `xTaskCreate`. Moving the object would leave the kernel with a dangling pointer. This is enforced at compile time.

```cpp
FreeTOSTask(const FreeTOSTask&)            = delete;
FreeTOSTask& operator=(const FreeTOSTask&) = delete;
FreeTOSTask(FreeTOSTask&&)                 = delete;
FreeTOSTask& operator=(FreeTOSTask&&)      = delete;
```

### Subclass contract
A concrete task must:
1. Inherit from `FreeTOSTask`.
2. Implement `void run() const override` with an infinite loop (`for (;;)`).
3. Call `start(priority, name, stackWords)` once before `vTaskStartScheduler()`.

```cpp
class HostCommandHandlerFreeTOSTask : public FreeTOSTask {
    void run() const override;   // infinite task loop
};
```

---

## 2. Static Singleton Trampoline — `DataSampleTimer` / `Encoder`

> **Pattern origin:** Established embedded-C++ idiom. Not in the GoF catalogue. Widely documented in embedded systems literature (e.g. *Making Embedded Systems*, White, 2011).

**Files:** `src/motors/DataSampleTimer.h`, `src/motors/DataSampleTimer.cpp`, `src/isr_handlers.cpp`  
**Also applied in:** `src/motors/Encoder.h`, `src/motors/Encoder.cpp`

### Intent
Route a hardware interrupt — which the CPU delivers as a C-linkage free function with no `this` pointer — into a method on a specific C++ object instance, with zero heap allocation and zero run-time overhead beyond a single null-check.

### How it is applied here

```
TIM2_IRQHandler()                 ← extern "C" free function in isr_handlers.cpp
    └─ DataSampleTimer::isr()     ← public static trampoline
          └─ s_instance->handleTick()   ← private instance method (ISR context)
```

- `s_instance` is a `static DataSampleTimer*` initialised to `nullptr`.
- `registerInstance(inst)` must be called once before the timer is started; it sets `s_instance = &inst`.
- `isr()` guards against a null `s_instance` before dispatching, making an un-registered call a safe no-op rather than undefined behaviour.
- All application logic resides in `handleTick()`; `isr_handlers.cpp` contains no logic — only flag clearing and trampolining.

### Pinned Object (supporting constraint)

The object is non-copyable and non-movable (`= delete`) because `s_instance` stores the address of the live object. Copying or moving would leave the ISR trampoline pointing to a stale or invalid address, causing undefined behaviour at interrupt time.

```cpp
DataSampleTimer(const DataSampleTimer&)            = delete;
DataSampleTimer& operator=(const DataSampleTimer&) = delete;
DataSampleTimer(DataSampleTimer&&)                 = delete;
DataSampleTimer& operator=(DataSampleTimer&&)      = delete;
```

### Why this pattern was chosen

The constraint is architectural: ARM Cortex-M interrupt vectors are slots in a fixed table of C-linkage function pointers. There is no mechanism for the CPU to pass a `this` pointer when entering an ISR. A bridge from the C interrupt world to a C++ object is therefore always required. The static singleton trampoline is selected here because:

- It incurs **no heap allocation**.
- The trampoline call is a **single null-check + indirect call** — negligible overhead in a 100 µs ISR.
- Each peripheral exists **at most once** on the STM32H723, so the one-instance-per-peripheral limitation is not a real constraint.
- The pattern is **auditable** in safety-critical review: the entire bridging mechanism fits in three lines of code.

### Alternatives considered

| Pattern | Mechanism | Reason not chosen |
|---|---|---|
| **Static singleton trampoline** *(selected)* | `static T* s_instance` + `static void isr()` | Zero overhead; safe; auditable |
| **Plain C global struct** | `extern "C"` global, no class encapsulation | Not object-oriented; difficult to unit test |
| **HAL period-elapsed callback** (`HAL_TIM_PeriodElapsedCallback`) | Override weak HAL symbol | Still requires a global/singleton internally; couples code to HAL callback model |
| **Function pointer registry** | Array of `void(*)(void)` populated at runtime | Additional indirection; more complex; no benefit when only one instance exists |
| **`std::function` / lambda registry** | Type-erased callable stored per peripheral | Requires heap allocation; unsafe in ISR context |

---

*More patterns to be documented as the project grows.*


