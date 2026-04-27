# NUCLEO-H723ZG Hardware Notes

---

## Robot Platform

| Property | Details |
|----------|---------|
| Drive topology | **Differential drive** |
| Frame shape | **Round** |
| Motors (×2) | **Pololu 50:1 Metal Gearmotor 37Dx70L mm 12V with 64 CPR Encoder (Helical Pinion)** |
| Gear ratio | 50 : 1 |
| Encoder resolution | 64 CPR (counts per revolution) on motor shaft → **3 200 CPR** at gearbox output (64 × 50) |
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




