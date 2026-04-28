/*
 * TransposedIIRFilter.h
 *
 * First-order transposed direct-form II IIR filter.
 * y[n] = b0*x[n] + b1*x[n-1] + a1*y[n-1]
 *
 * State variables and I/O use double so that:
 *   - velocity-error inputs (double arithmetic) pass through without narrowing.
 *   - int32_t encoder timer deltas (up to 2^31) are represented exactly
 *     (float mantissa is only 24-bit; double covers the full int32_t range).
 *   - the feedback coefficient a1 ≈ -0.88161859 (8 sig figs) is stored
 *     without the ~1 ULP rounding that float storage would introduce.
 */

#ifndef SRC_TRANSPOSEDIIRFILTER_H_
#define SRC_TRANSPOSEDIIRFILTER_H_

// Sensor low-pass IIR coefficients (computed offline, 8 significant digits).
// Stored as double literals — no 'f' suffix — to avoid narrowing to float.
#define VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_B0   0.0591907
#define VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_B1   0.0591907
#define VEHICLE_TRANPOSED_IIR_FILTER_SENSOR_A1  -0.88161859

class TransposedIIRFilter {
public:
    TransposedIIRFilter(double b0, double b1, double a1);

    /**
     * Advance the filter by one sample.
     * @param input  New sample — accepts double velocity error or int32_t
     *               encoder delta (implicitly promoted to double, exact for
     *               all 32-bit integer values).
     * @return       Filtered output.
     */
    double update(double input);

    void   reset();
    double get() const;

    ~TransposedIIRFilter();

private:
    // Coefficients stored as double to preserve all 8 significant digits of a1.
    double m_b0;
    double m_b1;
    double m_a1;

    double m_feedForward;
    double m_feedBack;
    double m_output;
};

#endif /* SRC_TRANSPOSEDIIRFILTER_H_ */
