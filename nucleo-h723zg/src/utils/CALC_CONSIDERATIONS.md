# Calculation Considerations

---

## 1. Encoder Math

### 1.1 Angular Velocity Derivation

Each encoder edge represents one count. One full wheel revolution produces
`countsPerRev` counts (3 200 for the Pololu 50:1 37D motor).

#### Symbols

| Symbol | Unit | Description |
|---|---|---|
| $\Delta\theta$ | rad | Angular displacement per count |
| $\Delta t$ | s | Time between two successive encoder edges |
| $\Delta t_{\mu s}$ | µs | Same interval, measured in microseconds |
| $N$ | counts/rev | Counts per revolution (`kCountsPerRev`) |
| $\omega$ | rad/s | Angular velocity |

---

#### Step 1 — Angular displacement per count

One full revolution spans $2\pi$ radians and $N$ counts, so each count advances
the wheel by:

$$\Delta\theta = \frac{2\pi}{N} \quad [\text{rad}]$$

---

#### Step 2 — Convert microsecond timestamp to seconds

The DWT timer returns timestamps in microseconds. Converting to SI:

$$\Delta t = \frac{\Delta t_{\mu s}}{10^{6}} \quad [\text{s}]$$

---

#### Step 3 — Angular velocity

$$\omega = \frac{\Delta\theta}{\Delta t}
         = \frac{\dfrac{2\pi}{N}}{\dfrac{\Delta t_{\mu s}}{10^{6}}}$$

Simplifying (multiply numerator and denominator):

$$\boxed{\omega = \frac{2\pi \times 10^{6}}{N \times \Delta t_{\mu s}}} \quad [\text{rad/s}]$$

This is a single multiply + divide with no `pow()` call — important on a
bare-metal target where `<cmath>` transcendentals are expensive.

---

### 1.2 Implementation Notes

#### `constexpr float kTwoPiMicros`

```cpp
constexpr float kTwoPiMicros = 2.0F * std::numbers::pi_v<float> * 1'000'000.0F;
```

`constexpr` forces the compiler to evaluate $2\pi \times 10^{6}$ **at compile
time**. The result is embedded in the binary as a single float literal — there
is zero runtime cost regardless of how often the function is called from an ISR.
Without it, the three-way multiply would execute on every velocity query.

---

#### `static_cast<float>(...)`

```cpp
kTwoPiMicros / (static_cast<float>(countsPerRev) * static_cast<float>(dtUs))
```

`countsPerRev` is `int32_t` and `dtUs` is `uint32_t`. Without explicit casts,
the compiler would perform **integer multiplication** on the denominator first
(risk of overflow at large tick values), then implicitly promote the result to
`float` for the final division.

`static_cast<float>` makes the conversion:

- **Intentional** — the reader sees exactly where integer → float promotion happens.
- **Safe** — floating-point multiply is used throughout, so no intermediate overflow.
- **Auditable** — `static_cast` is searchable and required by the project coding
  standards (MISRA / AUTOSAR) over C-style casts.

---

## 2. `TransposedIIRFilter` — `float` vs `double`

### 2.1 Background

`TransposedIIRFilter` is used in two contexts:

| Context | Input type | Typical magnitude |
|---|---|---|
| PI velocity control | `double velocRef - double measuredVeloc` | Small (rad/s error) |
| Encoder time-delta filtering | `int32_t` DWT timer delta | Up to ~2³¹ µs counts |

### 2.2 Why `float` is insufficient for the coefficients

The sensor low-pass coefficients are:

```
b0 =  0.0591907    (7 significant digits)
b1 =  0.0591907    (7 significant digits)
a1 = -0.88161859   (8 significant digits)
```

`float` provides ~7.2 decimal digits of precision. `a1` requires **8**, so
storing it as `float` silently rounds the last digit. More critically, `a1`
is a **feedback coefficient close to 1** — the filter pole is at |0.88|,
giving a long memory (time constant ≈ 8 samples). A per-step rounding error
in the feedback path never fully decays, causing a slow but unbounded drift
in the filtered output. `double` (15–16 significant digits) stores all 8
digits of `a1` exactly.

### 2.3 Why `float` is insufficient for `int32_t` encoder input

`float` has a **24-bit mantissa**, which means it can only represent integers
exactly up to:

$$2^{24} = 16{,}777{,}216$$

A DWT counter running at 64 MHz reaches that value in just:

$$\frac{16{,}777{,}216}{64 \times 10^{6}} \approx 262 \text{ ms}$$

Any `int32_t` timer delta larger than that is silently truncated when cast to
`float`, introducing a fixed quantisation error in the filtered time measurement.
`double` has a **53-bit mantissa**, covering the full `int32_t` range (2³¹) exactly.

### 2.4 Hardware cost on STM32H723ZG

The STM32H723ZG Cortex-M7 core includes a **double-precision FPU (FPv5-D16)**,
enabled via the toolchain flags:

```
-mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard
```

`double` arithmetic is **hardware-accelerated** — it is not emulated in software.
The throughput difference vs `float` is one extra cycle per operation at most, which
is negligible inside a periodic control task running at a few hundred Hz.

> ⚠️ Avoid FPU operations inside **ISRs** (e.g. the encoder edge handlers):
> the CPU must save/restore the full 32-register FPU context on entry/exit,
> adding ~12 cycles of latency per interrupt. Keep ISR bodies to integer
> arithmetic only; push any float/double computation to the task level.

### 2.5 Decision

| Aspect | Choice | Rationale |
|---|---|---|
| `update()` signature | `double update(double input)` | Covers both use cases without narrowing |
| Coefficient members | `double m_b0, m_b1, m_a1` | Preserves all 8 significant digits of `a1` |
| State variables | `double m_feedForward, m_feedBack, m_output` | Prevents feedback drift over long run times |
| `int32_t` encoder input | Implicit promotion to `double` | Exact for all 32-bit integer values |
| FPU overhead | Negligible | Hardware double-precision FPU present on H723ZG |

