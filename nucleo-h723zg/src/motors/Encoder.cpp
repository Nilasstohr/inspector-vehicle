/**
 * @file    Encoder.cpp
 * @brief   Quadrature encoder implementation — see Encoder.h for design notes.
 */

#include "Encoder.h"
#include "utils/EncoderMath.h"
#include "utils/DwtTimer.h"

/* ── Static registry ──────────────────────────────────────────────────────── */
Encoder* Encoder::s_instances[Encoder::kMaxInstances] = {};

/* ── Constructor ──────────────────────────────────────────────────────────── */
Encoder::Encoder(const GpioInput& pinA, const GpioInput& pinB)
    : m_pinA(pinA), m_pinB(pinB) {}

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

    m_lastTick    = m_currentTick;
    m_currentTick = DwtTimer::getMicros();
}

void Encoder::handleChannelB() {
    const uint8_t a = readPinA();
    const uint8_t b = readPinB();

    if ((b && a) || (!b && !a)) {   /* B == A → reverse */
        m_count--;
    } else {
        m_count++;
    }

    m_lastTick    = m_currentTick;
    m_currentTick = DwtTimer::getMicros();
}

/* ── Public API ───────────────────────────────────────────────────────────── */
int32_t Encoder::getCount() const  { return m_count; }
void    Encoder::resetCount()      { m_count = 0; }

int32_t Encoder::getVelocityCps() const {
    return EncoderMath::deltaUsToCountsPerSecond(m_currentTick - m_lastTick);
}

float Encoder::getAngularVelocityRps() const {
    return EncoderMath::angularVelocity(m_currentTick - m_lastTick, kCountsPerRev);
}

