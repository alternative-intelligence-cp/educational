#pragma once
/**
 * @file decision_gradient.hpp
 * @brief DecisionGradient — C++ port (header-only)
 *
 * Two-axis gradient decision construct.
 *
 *   pbc  (lean) — where the probability box sits on the A↔B axis
 *   pbm  (tilt) — how wide the probability box is
 *
 * The box overlaps the A and B halves of the [0,1] decision space,
 * producing independent confidence scores that feed a five-token decision:
 *   A | B | BOTH | UNKNOWN | NONE
 *
 * Quadratic factor  f(m) = m² + 0.5·m + 0.5:
 *   m = 0.0  →  0.50×  (none  — narrowest)
 *   m = 0.5  →  1.00×  (unkn  — normal)
 *   m = 1.0  →  2.00×  (both  — widest)
 *
 * Usage:
 *   DecisionGradient dg(100.0f);
 *   dg.lean_opt_a(10.0f);
 *   printf("%s\n", dg.get_decision());   // "A" / "B" / "BOTH" / "UNKNOWN" / "NONE"
 *
 * Author: Randy Hoggard
 */

#include <algorithm>
#include <stdexcept>
#include <string>
#include <cstdio>

class DecisionGradient {
public:
    // ── Construction ─────────────────────────────────────────────────────────

    /**
     * @param width  Total span. Must be > 1. Defaults to 100.
     * @throws std::invalid_argument if width <= 1
     */
    explicit DecisionGradient(float width = 100.0f)
        : width_(width)
        , pbw_(width * 0.5f)
        , pbc_(width * 0.5f)
        , pbm_(0.5f)
    {
        if (width <= 1.0f)
            throw std::invalid_argument(
                "Invalid width: " + std::to_string(width) +
                ", must be greater than 1");
    }

    // ── Lean (pbc axis) ───────────────────────────────────────────────────────

    /** Move pbc toward A end (width * 0.25). */
    DecisionGradient& lean_opt_a(float amount = 1.0f) {
        validate_amount(amount);
        pbc_ = clamp(pbc_ - amount, width_ * 0.25f, width_ * 0.75f);
        return *this;
    }

    /** Move pbc toward Unknown center (width * 0.5). */
    DecisionGradient& lean_unkn(float amount = 1.0f) {
        validate_amount(amount);
        const float center = width_ * 0.5f;
        if      (pbc_ > center) pbc_ = std::max(center, pbc_ - amount);
        else if (pbc_ < center) pbc_ = std::min(center, pbc_ + amount);
        return *this;
    }

    /** Move pbc toward B end (width * 0.75). */
    DecisionGradient& lean_opt_b(float amount = 1.0f) {
        validate_amount(amount);
        pbc_ = clamp(pbc_ + amount, width_ * 0.25f, width_ * 0.75f);
        return *this;
    }

    // ── Tilt (pbm axis) ───────────────────────────────────────────────────────
    // step = amount / pbw so a unit amount has the same feel as lean ops.

    /** Move pbm toward 1.0 (widest: "both"). */
    DecisionGradient& tilt_both(float amount = 1.0f) {
        validate_amount(amount);
        pbm_ = clamp(pbm_ + amount / pbw_, 0.0f, 1.0f);
        return *this;
    }

    /** Move pbm toward 0.5 (normal: "unknown"). */
    DecisionGradient& tilt_unkn(float amount = 1.0f) {
        validate_amount(amount);
        const float step = amount / pbw_;
        if      (pbm_ > 0.5f) pbm_ = std::max(0.5f, pbm_ - step);
        else if (pbm_ < 0.5f) pbm_ = std::min(0.5f, pbm_ + step);
        return *this;
    }

    /** Move pbm toward 0.0 (narrowest: "none"). */
    DecisionGradient& tilt_none(float amount = 1.0f) {
        validate_amount(amount);
        pbm_ = clamp(pbm_ - amount / pbw_, 0.0f, 1.0f);
        return *this;
    }

    // ── Readers ───────────────────────────────────────────────────────────────

    /** Confidence in A: overlap of box with [0, 0.5], normalized to [0, 1]. */
    [[nodiscard]] float get_opt_a() const {
        auto [lo, hi] = box_range();
        const float overlap = std::max(0.0f, std::min(0.5f, hi) - std::max(0.0f, lo));
        return std::min(1.0f, overlap / 0.5f);
    }

    /** Confidence in B: overlap of box with [0.5, 1.0], normalized to [0, 1]. */
    [[nodiscard]] float get_opt_b() const {
        auto [lo, hi] = box_range();
        const float overlap = std::max(0.0f, std::min(1.0f, hi) - std::max(0.5f, lo));
        return std::min(1.0f, overlap / 0.5f);
    }

    /** Current pbm position [0..1]: 0=NONE, 0.5=UNKNOWN, 1=BOTH. */
    [[nodiscard]] float get_nb() const { return pbm_; }

    struct State { float a, b, nb; };

    /** Raw state snapshot. */
    [[nodiscard]] State get_state() const {
        return { get_opt_a(), get_opt_b(), get_nb() };
    }

    enum class Decision { A, B, BOTH, UNKNOWN, NONE };

    /** Highest-probability enum token. */
    [[nodiscard]] Decision get_decision_enum() const {
        const float a = get_opt_a();
        const float b = get_opt_b();
        if (a > 0.25f && b > 0.25f)          return Decision::BOTH;
        if (a > 0.25f)                         return Decision::A;
        if (b > 0.25f)                         return Decision::B;
        if (a >= 0.125f || b >= 0.125f)       return Decision::UNKNOWN;
        return Decision::NONE;
    }

    /** Highest-probability string token: "A" | "B" | "BOTH" | "UNKNOWN" | "NONE". */
    [[nodiscard]] const char* get_decision() const {
        switch (get_decision_enum()) {
            case Decision::A:       return "A";
            case Decision::B:       return "B";
            case Decision::BOTH:    return "BOTH";
            case Decision::UNKNOWN: return "UNKNOWN";
            default:                return "NONE";
        }
    }

    // ── Display ───────────────────────────────────────────────────────────────

    void print_state() const {
        auto s = get_state();
        printf("State --> A: %.4f , B: %.4f , NB: %.4f\n", s.a, s.b, s.nb);
    }

    void print_decision() const {
        printf("Decision --> %s\n", get_decision());
    }

private:
    float width_;
    float pbw_;   // fixed half-span = width * 0.5
    float pbc_;   // lean: [width*0.25 .. width*0.75]
    float pbm_;   // tilt: [0 .. 1]

    // Quadratic factor: maps pbm → box-width multiplier.
    [[nodiscard]] static float factor(float m) {
        return m * m + 0.5f * m + 0.5f;
    }

    // Normalized [lo, hi] of the probability box. May exceed [0, 1].
    struct Range { float lo, hi; };
    [[nodiscard]] Range box_range() const {
        const float c  = pbc_ / width_;
        const float hw = 0.5f * factor(pbm_) / 2.0f;
        return { c - hw, c + hw };
    }

    [[nodiscard]] static float clamp(float x, float lo, float hi) {
        return x < lo ? lo : (x > hi ? hi : x);
    }

    static void validate_amount(float amount) {
        if (amount <= 0.0f)
            throw std::invalid_argument(
                "Invalid amount: " + std::to_string(amount) +
                ", must be greater than 0");
    }
};
