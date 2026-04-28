/*
 * TransposedIIRFilter.cpp
 *
 * First-order transposed direct-form II IIR filter.
 * All arithmetic in double to preserve coefficient precision and avoid
 * int32_t encoder input truncation above 2^24.
 */

#include "TransposedIIRFilter.h"

TransposedIIRFilter::TransposedIIRFilter(const double b0, const double b1, const double a1)
    : m_b0(b0)
    , m_b1(b1)
    , m_a1(a1)
    , m_feedForward(0.0)
    , m_feedBack(0.0)
    , m_output(0.0)
{
}

double TransposedIIRFilter::update(const double input) {
    m_output      = m_b0 * input + m_feedForward - m_feedBack;
    m_feedForward = m_b1 * input;
    m_feedBack    = m_a1 * m_output;  // note: a1 is stored negative, subtracted above
    return m_output;
}

void TransposedIIRFilter::reset() {
    m_feedForward = 0.0;
    m_feedBack    = 0.0;
    m_output      = 0.0;
}

double TransposedIIRFilter::get() const {
    return m_output;
}

TransposedIIRFilter::~TransposedIIRFilter() = default;
