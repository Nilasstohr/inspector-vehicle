/**
 * @file    Encoder.cpp
 * @brief   Quadrature encoder implementation — see Encoder.h for design notes.
 */

#include "Encoder.h"
#include "utils/DwtTimer.h"

/* ── Static registry ──────────────────────────────────────────────────────── */
Encoder* Encoder::s_instances[Encoder::kMaxInstances] = {};

/* ── Constructor ──────────────────────────────────────────────────────────── */
Encoder::Encoder(const GpioInput& pinA, const GpioInput& pinB)
    : m_pinA(pinA), m_pinB(pinB) {}

void Encoder::reset() {
    m_count = 0;
    m_tickDeltaUs = 0;
    m_lastTick = DwtTimer::getMicros();
}

/* ── Registration ─────────────────────────────────────────────────────────── */
void Encoder::registerInstance(const uint8_t index, Encoder& inst) {
    if (index < kMaxInstances) {
        s_instances[index] = &inst;
    }
}

/* ── ISR trampolines ──────────────────────────────────────────────────────── */
void Encoder::isrChannelA(const uint8_t index) {
    if (index < kMaxInstances && s_instances[index] != nullptr) {
        s_instances[index]->handleChannelA();
    }
}

void Encoder::isrChannelB(const uint8_t index) {
    if (index < kMaxInstances && s_instances[index] != nullptr) {
        s_instances[index]->handleChannelB();
    }
}

/* ── Pin read helpers ─────────────────────────────────────────────────────── */
uint8_t Encoder::readPinA() const {
    return m_pinA.isHigh() ? 1U : 0U;
}

uint8_t Encoder::readPinB() const {
    return m_pinB.isHigh() ? 1U : 0U;
}

/* ── Quadrature decoding ──────────────────────────────────────────────────── */
/*
 * Pins are read after the edge fires — no toggle tracking, self-correcting.
 *
 * Channel-A edge:   A == B  →  forward (+1),   A != B  →  reverse (-1)
 * Channel-B edge:   B == A  →  reverse (-1),   B != A  →  forward (+1)
 */
void Encoder::handleChannelA() {
    const uint8_t a = readPinA();
    const uint8_t b = readPinB();

    if ((a && b) || (!a && !b)) {   /* A == B → forward */
        m_count++;
    } else {
        m_count--;
    }

    /* Compute delta here — single 32-bit store makes m_tickDeltaUs
     * atomically readable by the lower-priority DataSampleTimer ISR. */
    const uint32_t now = DwtTimer::getMicros();
    m_tickDeltaUs = now - m_lastTick;  /* wraps correctly on overflow */
    m_lastTick    = now;
}

void Encoder::handleChannelB() {
    const uint8_t a = readPinA();
    const uint8_t b = readPinB();

    if ((b && a) || (!b && !a)) {   /* B == A → reverse */
        m_count--;
    } else {
        m_count++;
    }

    const uint32_t now = DwtTimer::getMicros();
    m_tickDeltaUs = now - m_lastTick;
    m_lastTick    = now;
}

/* ── Public API ───────────────────────────────────────────────────────────── */
int32_t  Encoder::getCount()       const { return m_count; }
void     Encoder::resetCount()           { m_count = 0; }
uint32_t Encoder::getTickDeltaUs() const { return m_tickDeltaUs; }
