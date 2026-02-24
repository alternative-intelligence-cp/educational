#pragma once
/**
 * @file sliding_window.hpp
 * @brief SlidingWindow — C++ port (header-only)
 *
 * Zero-sum A/B sliding window. The invariant  a + b == width  is always
 * maintained. Moving toward A nudges away from B and vice versa.
 *
 * Usage:
 *   SlidingWindow sw(100.0f);
 *   sw.shift_a(10.0f);
 *   float a = sw.get_a();   // [0, 1]
 *   auto  s = sw.get_state();
 *
 * Author: Randy Hoggard
 */

#include <stdexcept>
#include <string>

class SlidingWindow {
public:
    // ── Construction ─────────────────────────────────────────────────────────

    /**
     * @param width  Total span. Must be > 1. Defaults to 100.
     * @throws std::invalid_argument if width <= 1
     */
    explicit SlidingWindow(float width = 100.0f)
        : max_(width), a_(width * 0.5f), b_(width * 0.5f)
    {
        if (width <= 1.0f)
            throw std::invalid_argument(
                "Invalid width: " + std::to_string(width) +
                ", must be greater than 1");
    }

    // ── Mutators ──────────────────────────────────────────────────────────────

    /** Increase weight on A (decreases B by the same amount). */
    SlidingWindow& shift_a(float amount = 1.0f) {
        validate_amount(amount);
        if (a_ + amount > max_) { a_ = max_; b_ = 0.0f; }
        else                    { a_ += amount; b_ -= amount; }
        return *this;
    }

    /** Increase weight on B (decreases A by the same amount). */
    SlidingWindow& shift_b(float amount = 1.0f) {
        validate_amount(amount);
        if (b_ + amount > max_) { b_ = max_; a_ = 0.0f; }
        else                    { b_ += amount; a_ -= amount; }
        return *this;
    }

    // ── Readers ───────────────────────────────────────────────────────────────

    /** Normalized A weight in [0, 1]. */
    [[nodiscard]] float get_a() const { return a_ / max_; }

    /** Normalized B weight in [0, 1]. */
    [[nodiscard]] float get_b() const { return b_ / max_; }

    struct State { float a, b; };

    /** Raw state snapshot. */
    [[nodiscard]] State get_state() const { return { get_a(), get_b() }; }

    enum class Best { UNKNOWN = 0, A = 1, B = 2 };

    /** Returns UNKNOWN / A / B. */
    [[nodiscard]] Best get_best() const {
        if (a_ == b_) return Best::UNKNOWN;
        return (a_ > b_) ? Best::A : Best::B;
    }

    /** Returns "UNKNOWN" / "A" / "B". */
    [[nodiscard]] const char* get_best_semantic() const {
        switch (get_best()) {
            case Best::A: return "A";
            case Best::B: return "B";
            default:      return "UNKNOWN";
        }
    }

    // ── Display ───────────────────────────────────────────────────────────────

    void print_state() const {
        auto s = get_state();
        printf("State --> A: %.4f , B: %.4f\n", s.a, s.b);
    }

private:
    float max_;
    float a_;
    float b_;

    static void validate_amount(float amount) {
        if (amount <= 0.0f)
            throw std::invalid_argument(
                "Invalid amount: " + std::to_string(amount) +
                ", must be greater than 0");
    }
};
