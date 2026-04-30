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

*More patterns to be documented as the project grows.*


