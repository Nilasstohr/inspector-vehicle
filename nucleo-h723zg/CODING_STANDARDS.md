# Embedded C++ Coding Standards

This document defines the coding standards and best practices for this project.
Rules are inspired by **MISRA-C++** and **AUTOSAR** guidelines.

---

## 1. Control Flow

### 1.1 Always use braces for control structures
**Rule:** Every `if`, `else`, `for`, `while`, and `do` body **must** use curly braces, even for single-line bodies.

**Rationale:** Prevents subtle bugs when adding extra lines, improves readability, and is required by MISRA-C++ Rule 6-4-1.

✅ **Good:**
```cpp
if (schedulerRunning) {
    xSemaphoreTake(m_mutex, portMAX_DELAY);
}
```

❌ **Bad:**
```cpp
if (schedulerRunning) xSemaphoreTake(m_mutex, portMAX_DELAY);
```

---

## 2. FreeRTOS

### 2.1 Guard mutex operations against scheduler state
**Rule:** Always check `xTaskGetSchedulerState() == taskSCHEDULER_RUNNING` before calling `xSemaphoreTake` / `xSemaphoreGive`. Calling these before the scheduler starts will trigger an assert.

✅ **Good:**
```cpp
const bool schedulerRunning = (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING);

if (schedulerRunning) {
    xSemaphoreTake(m_mutex, portMAX_DELAY);
}
// ... critical work ...
if (schedulerRunning) {
    xSemaphoreGive(m_mutex);
}
```

---

## 3. General

### 3.1 Const correctness
**Rule:** Apply `const` rigorously throughout the codebase:
- Mark every variable, parameter, or return value `const` when it is not modified after initialisation.
- Mark member functions `const` whenever they do not mutate object state.
- Prefer `const T&` over `T` for non-trivial pass-by-value parameters that are not modified.
- Avoid `const_cast` — its use should be treated as a code smell and require a comment justifying the exception.

**Rationale:** Const correctness prevents accidental mutation, serves as machine-checked documentation of intent, and can enable compiler optimisations. It is consistent with MISRA-C++ Rule 7-1-1 and AUTOSAR A7-1-1.

✅ **Good:**
```cpp
void Foo::process(const Bar& input) const {
    const uint32_t scaled = input.value() * kScale;
    transmit(scaled);
}
```

❌ **Bad:**
```cpp
void Foo::process(Bar input) {   // unnecessary copy, non-const parameter
    uint32_t scaled = input.value() * kScale;  // scaled never changes
    transmit(scaled);
}
```
### 3.1 Prefer `const` where possible
Mark variables and parameters `const` whenever they are not modified.

### 3.2 Use `reinterpret_cast` over C-style casts
Explicit C++ casts make intent clear and are easier to search/audit.

### 3.3 Comment magic numbers
Annotate timeouts, sizes, and hardware constants with a short comment.

```cpp
HAL_UART_Transmit(m_huart, data, len, /*timeout_ms=*/100);
```

---

*This file should be updated as new patterns and rules are adopted by the team.*

