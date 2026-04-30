# Timing Considerations

---

## Encoder

> Motor and encoder specifications (CPR, gear ratio, max RPM) are defined in
> [`HARDWARE_NOTES.md — Robot Platform`](HARDWARE_NOTES.md#robot-platform).
> The calculations below use those values as inputs.

---

### Tick Rate at Maximum Speed

```
Revolutions per second  = 200 rpm ÷ 60          = 3.333 rev/s
Ticks per second        = 3.333 × 3 200 CPR      = 10 666 ticks/s
Time between ticks      = 1 ÷ 10 666 × 1 000     = 0.0938 ms  (93.8 µs)
```

---

### Why `HAL_GetTick()` (1 ms) Is Insufficient

`HAL_GetTick()` increments once per millisecond.  
At maximum motor speed a tick arrives every **0.094 ms** — faster than the timer resolves.

| Scenario | `dt` seen by code | Reported velocity | Actual velocity |
|----------|-------------------|-------------------|-----------------|
| 200 RPM (max) | 0 ms | **0 cps** ❌ | 10 666 cps |
| 100 RPM | 0 or 1 ms | **0 or 1 000 cps** ❌ | 5 333 cps |
| 10 RPM | ~18 ms | ~55 cps ✓ | 53 cps |

At any speed above ~30 RPM the measurement is either zero or wildly quantised.

---

### Resolution Required

Minimum `dt` to measure = **93.8 µs** at maximum speed.  
A timer resolution of **1 µs** gives:

```
Error at max speed  = 1 µs ÷ 93.8 µs  ≈  1.1 %   ✓ acceptable
```

Millisecond resolution gives an error of up to **100 %** at max speed — unusable.

---

### Solution — DWT Cycle Counter (`DwtTimer`)

The ARM Cortex-M7 **DWT** (Data Watchpoint and Trace) unit contains a 32-bit
free-running cycle counter (`DWT->CYCCNT`).  
At 64 MHz it increments every **15.6 ns** and is converted to microseconds by:

```cpp
uint32_t getMicros() {
    return DWT->CYCCNT / (SystemCoreClock / 1'000'000U);  // cycles → µs
}
```

| Property | Value |
|----------|-------|
| Raw resolution | 15.6 ns (@ 64 MHz) |
| Reported resolution | 1 µs |
| Peripheral cost | **zero** — reuses the debug counter |
| `uint32_t` wrap period | 2³² µs ≈ **71 minutes** |
| Wrap-safe? | ✅ unsigned subtraction `current − last` is correct across wrap |

Call `DwtTimer::init()` once at startup (before NVIC enables encoder EXTIs):

```cpp
HAL_Init();
SystemClock_Config();
DwtTimer::init();   // ← must come before any Encoder interrupt fires
```

---

### Velocity Calculation

With µs timestamps the velocity in counts per second is:

```
cps = 1 000 000 µs/s ÷ dt_µs
```

Implemented as a pure, hardware-free function in `src/utils/EncoderMath.h`
so it can be unit-tested on the host without any HAL dependency:

```cpp
inline int32_t deltaUsToCountsPerSecond(uint32_t dtUs) {
    if (dtUs == 0U) { return 0; }
    return static_cast<int32_t>(1'000'000U / dtUs);
}
```

#### Accuracy at key speeds

| Speed (RPM) | dt (µs) | Reported cps | Actual cps | Error |
|-------------|---------|--------------|------------|-------|
| 200 (max)   | 93      | 10 752       | 10 666     | 0.8 % |
| 100         | 187     | 5 347        | 5 333      | 0.3 % |
| 10          | 1 875   | 533          | 533        | 0.1 % |
| 1           | 18 750  | 53           | 53.3       | 0.6 % |

---

### Files

| File | Role |
|------|------|
| `src/utils/DwtTimer.h` | µs timestamp via DWT — header-only, no peripheral |
| `src/utils/EncoderMath.h` | Pure velocity math — header-only, testable on host |
| `src/motors/Encoder.cpp` | Calls `DwtTimer::getMicros()` on every ISR edge |
| `tests/unit/test_EncoderMath.cpp` | Host unit tests for velocity calculation |


