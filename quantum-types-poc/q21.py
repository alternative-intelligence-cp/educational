#!/usr/bin/env python3
"""
Q21 Quantum Type - Python POC
==============================
Educational implementation demonstrating:
- 21-state quantum confidence modeling
- Saturation barriers (prevents drift from B→BOTH)
- Crystallization thresholds (requires 60% confidence)
- Symmetric confidence encoding (all states have MIN/LOW/MED/HIGH/MAX)
- Distinguished superposition certainty from uncertainty

Author: Randy Hoggard
Date: February 14, 2026
Version: 1.0.0
"""

from enum import IntEnum
from typing import Optional, Callable, Any


class Q21State(IntEnum):
    """Q21 state value constants with semantic names"""
    # ERROR state
    ERR = -128
    
    # NONE states: "Certain it's NEITHER A nor B"
    NONE_MAX  = -10  # 100% confident it's NONE
    NONE_HIGH = -9   # 80% confident
    NONE_MED  = -8   # 60% confident (crystallization threshold)
    NONE_LOW  = -7   # 40% confident
    NONE_MIN  = -6   # 20% confident (saturation barrier)
    
    # Gap: Prevents drift from NONE to OPTA
    
    # OPTA states: "Confident it's option A"
    OPTA_MAX  = -5   # 100% confident it's A
    OPTA_HIGH = -4   # 80% confident
    OPTA_MED  = -3   # 60% confident (crystallization threshold)
    OPTA_LOW  = -2   # 40% confident
    OPTA_MIN  = -1   # 20% confident
    
    # UNKNOWN: Genuine uncertainty
    UNKNOWN   = 0
    
    # OPTB states: "Confident it's option B"
    OPTB_MIN  = 1    # 20% confident
    OPTB_LOW  = 2    # 40% confident
    OPTB_MED  = 3    # 60% confident (crystallization threshold)
    OPTB_HIGH = 4    # 80% confident
    OPTB_MAX  = 5    # 100% confident it's B
    
    # Gap: Prevents drift from OPTB to BOTH
    
    # BOTH states: "Certain it's BOTH A and B"
    BOTH_MIN  = 6    # 20% confident it's BOTH
    BOTH_LOW  = 7    # 40% confident
    BOTH_MED  = 8    # 60% confident (crystallization threshold)
    BOTH_HIGH = 9    # 80% confident
    BOTH_MAX  = 10   # 100% confident it's BOTH


class Q21:
    """
    Q21 Quantum Type - 21-state confidence model
    
    Storage: tbb8 (Twisted Balanced Binary 8-bit, -128 to +127)
    States: 21 distinct confidence levels across 5 regions
    
    Key Features:
    - Saturation barriers at ±6 (prevents accidental drift)
    - Crystallization thresholds at ±3, ±8 (requires 60% confidence)
    - Symmetric confidence (all states have 5 levels)
    - Distinguishes superposition certainty from uncertainty
    
    State Regions:
        NONE:    -10 to -6  (certain it's NEITHER)
        OPTA:    -5 to -1   (certain it's option A)
        UNKNOWN: 0          (genuine uncertainty)
        OPTB:    +1 to +5   (certain it's option B)
        BOTH:    +6 to +10  (certain it's BOTH)
    """
    
    # Class constants
    MIN_VALUE = -10
    MAX_VALUE = 10
    ERROR_VALUE = -128
    
    # Barrier thresholds
    SATURATION_BARRIER_NEG = -6  # Barrier between NONE and OPTA
    SATURATION_BARRIER_POS = 6   # Barrier between OPTB and BOTH
    
    # Crystallization thresholds (60% confidence)
    CRYSTALLIZE_OPTA = -3
    CRYSTALLIZE_OPTB = 3
    CRYSTALLIZE_NONE = -8
    CRYSTALLIZE_BOTH = 8
    
    def __init__(self, value: int = 0):
        """
        Initialize Q21 with given value.
        
        Args:
            value: Initial state value (-10 to +10, or -128 for ERR)
        
        Raises:
            ValueError: If value is out of valid range
        """
        if value == self.ERROR_VALUE:
            self._value = value
        elif self.MIN_VALUE <= value <= self.MAX_VALUE:
            self._value = value
        else:
            raise ValueError(
                f"Q21 value must be {self.MIN_VALUE} to {self.MAX_VALUE} "
                f"or {self.ERROR_VALUE} (ERR), got {value}"
            )
    
    @property
    def value(self) -> int:
        """Get current state value"""
        return self._value
    
    @value.setter
    def value(self, new_value: int):
        """
        Set state value with validation.
        
        Args:
            new_value: New state value
            
        Raises:
            ValueError: If value is out of valid range
        """
        if new_value == self.ERROR_VALUE:
            self._value = new_value
        elif self.MIN_VALUE <= new_value <= self.MAX_VALUE:
            self._value = new_value
        else:
            raise ValueError(
                f"Q21 value must be {self.MIN_VALUE} to {self.MAX_VALUE} "
                f"or {self.ERROR_VALUE} (ERR), got {new_value}"
            )
    
    def is_error(self) -> bool:
        """Check if in error state"""
        return self._value == self.ERROR_VALUE
    
    def is_unknown(self) -> bool:
        """Check if in UNKNOWN state (genuine uncertainty)"""
        return self._value == 0
    
    def is_none_state(self) -> bool:
        """Check if certain it's NEITHER A nor B"""
        return -10 <= self._value <= -6
    
    def is_opta_state(self) -> bool:
        """Check if confident it's option A"""
        return -5 <= self._value <= -1
    
    def is_optb_state(self) -> bool:
        """Check if confident it's option B"""
        return 1 <= self._value <= 5
    
    def is_both_state(self) -> bool:
        """Check if certain it's BOTH A and B"""
        return 6 <= self._value <= 10
    
    def confidence_level(self) -> int:
        """
        Get confidence level (0-5).
        
        Returns:
            0 = UNKNOWN
            1 = MIN (20%)
            2 = LOW (40%)
            3 = MED (60%)
            4 = HIGH (80%)
            5 = MAX (100%)
        """
        if self.is_error():
            return -1
        if self.is_unknown():
            return 0
        
        abs_val = abs(self._value)
        
        # NONE/BOTH states: 6-10 mapped to 1-5
        if abs_val >= 6:
            return abs_val - 5
        
        # OPTA/OPTB states: 1-5 mapped to 1-5
        return abs_val
    
    def get_state_name(self) -> str:
        """Get human-readable state name"""
        if self.is_error():
            return "ERR"
        
        v = self._value
        
        # NONE states
        if v == -10: return "NONE_MAX"
        if v == -9:  return "NONE_HIGH"
        if v == -8:  return "NONE_MED"
        if v == -7:  return "NONE_LOW"
        if v == -6:  return "NONE_MIN"
        
        # OPTA states
        if v == -5:  return "OPTA_MAX"
        if v == -4:  return "OPTA_HIGH"
        if v == -3:  return "OPTA_MED"
        if v == -2:  return "OPTA_LOW"
        if v == -1:  return "OPTA_MIN"
        
        # UNKNOWN
        if v == 0:   return "UNKNOWN"
        
        # OPTB states
        if v == 1:   return "OPTB_MIN"
        if v == 2:   return "OPTB_LOW"
        if v == 3:   return "OPTB_MED"
        if v == 4:   return "OPTB_HIGH"
        if v == 5:   return "OPTB_MAX"
        
        # BOTH states
        if v == 6:   return "BOTH_MIN"
        if v == 7:   return "BOTH_LOW"
        if v == 8:   return "BOTH_MED"
        if v == 9:   return "BOTH_HIGH"
        if v == 10:  return "BOTH_MAX"
        
        return "INVALID"
    
    def observe(self, evidence_strength: int):
        """
        Accumulate evidence, respecting barriers.
        
        Args:
            evidence_strength: +value strengthens toward OPTB/BOTH,
                             -value strengthens toward OPTA/NONE
        
        Saturation barriers prevent drift:
        - Can't drift from OPTA_MIN directly to NONE_MIN
        - Can't drift from OPTB_MAX directly to BOTH_MIN
        - Must have strong evidence to cross barriers
        """
        if self.is_error():
            return
        
        new_value = self._value + evidence_strength
        
        # Check saturation barriers
        old_val = self._value
        
        # Barrier at -6: Between NONE and OPTA
        if old_val < -6 and new_value > -6:
            # Crossing from NONE toward OPTA
            if evidence_strength < 2:  # Weak evidence
                new_value = -6  # Stop at barrier
        elif old_val > -6 and new_value < -6:
            # Crossing from OPTA toward NONE
            if evidence_strength > -2:  # Weak evidence
                new_value = -6  # Stop at barrier
        
        # Barrier at +6: Between OPTB and BOTH
        if old_val < 6 and new_value > 6:
            # Crossing from OPTB toward BOTH
            if evidence_strength < 2:  # Weak evidence
                new_value = 6  # Stop at barrier
        elif old_val > 6 and new_value < 6:
            # Crossing from BOTH toward OPTB
            if evidence_strength > -2:  # Weak evidence
                new_value = 6  # Stop at barrier
        
        # Clamp to valid range
        new_value = max(self.MIN_VALUE, min(self.MAX_VALUE, new_value))
        self._value = new_value
    
    def crystallize(self) -> Optional[str]:
        """
        Collapse to definitive answer if confidence exceeds threshold.
        
        Returns:
            "A" if confident it's option A (≤ -3)
            "B" if confident it's option B (≥ +3)
            "NONE" if confident it's neither (≤ -8)
            "BOTH" if confident it's both (≥ +8)
            None if below crystallization threshold
        """
        if self.is_error():
            return None
        
        v = self._value
        
        # NONE crystallization (60% confident it's NEITHER)
        if v <= self.CRYSTALLIZE_NONE:
            return "NONE"
        
        # OPTA crystallization (60% confident it's A)
        if v <= self.CRYSTALLIZE_OPTA:
            return "A"
        
        # OPTB crystallization (60% confident it's B)
        if v >= self.CRYSTALLIZE_OPTB:
            return "B"
        
        # BOTH crystallization (60% confident it's BOTH)
        if v >= self.CRYSTALLIZE_BOTH:
            return "BOTH"
        
        # Below threshold: uncertain
        return None
    
    def reset(self):
        """Reset to UNKNOWN state"""
        self._value = 0
    
    def __str__(self) -> str:
        """String representation"""
        if self.is_error():
            return "Q21(ERR)"
        
        state_name = self.get_state_name()
        conf = self.confidence_level()
        
        return f"Q21({self._value:+3d}) = {state_name} (confidence: {conf}/5)"
    
    def __repr__(self) -> str:
        """Developer representation"""
        return f"Q21(value={self._value})"
    
    def __eq__(self, other) -> bool:
        """Equality comparison"""
        if isinstance(other, Q21):
            return self._value == other._value
        if isinstance(other, int):
            return self._value == other
        return False
    
    def __hash__(self) -> int:
        """Hash for use in sets/dicts"""
        return hash(self._value)
    
    def copy(self) -> 'Q21':
        """Create a copy of this Q21 instance"""
        return Q21(self._value)


# Convenience constructor functions
def q21_unknown() -> Q21:
    """Create Q21 in UNKNOWN state"""
    return Q21(0)


def q21_opta(confidence: int = 5) -> Q21:
    """
    Create Q21 confident in option A.
    
    Args:
        confidence: 1-5 (MIN to MAX), default 5 (100%)
    """
    if not 1 <= confidence <= 5:
        raise ValueError("Confidence must be 1-5")
    return Q21(-confidence)


def q21_optb(confidence: int = 5) -> Q21:
    """
    Create Q21 confident in option B.
    
    Args:
        confidence: 1-5 (MIN to MAX), default 5 (100%)
    """
    if not 1 <= confidence <= 5:
        raise ValueError("Confidence must be 1-5")
    return Q21(confidence)


def q21_none(confidence: int = 5) -> Q21:
    """
    Create Q21 confident it's NEITHER.
    
    Args:
        confidence: 1-5 (MIN to MAX), default 5 (100%)
    """
    if not 1 <= confidence <= 5:
        raise ValueError("Confidence must be 1-5")
    return Q21(-(5 + confidence))


def q21_both(confidence: int = 5) -> Q21:
    """
    Create Q21 confident it's BOTH.
    
    Args:
        confidence: 1-5 (MIN to MAX), default 5 (100%)
    """
    if not 1 <= confidence <= 5:
        raise ValueError("Confidence must be 1-5")
    return Q21(5 + confidence)


def q21_error() -> Q21:
    """Create Q21 in ERROR state"""
    return Q21(Q21.ERROR_VALUE)


if __name__ == "__main__":
    print("Q21 Quantum Type - Python POC")
    print("=" * 50)
    print()
    
    # Basic demonstration
    print("1. Basic States")
    print("-" * 50)
    q = q21_unknown()
    print(f"  Unknown:     {q}")
    
    q = q21_opta(5)
    print(f"  Option A:    {q}")
    
    q = q21_optb(3)
    print(f"  Option B:    {q}")
    
    q = q21_none(4)
    print(f"  Neither:     {q}")
    
    q = q21_both(2)
    print(f"  Both:        {q}")
    print()
    
    # Evidence accumulation
    print("2. Evidence Accumulation")
    print("-" * 50)
    q = q21_unknown()
    print(f"  Start:       {q}")
    
    q.observe(2)
    print(f"  +2 evidence: {q}")
    
    q.observe(2)
    print(f"  +2 evidence: {q}")
    
    result = q.crystallize()
    print(f"  Crystallize: {result}")
    print()
    
    # Saturation barrier demonstration
    print("3. Saturation Barriers (prevents drift)")
    print("-" * 50)
    q = Q21(-5)  # OPTA_MAX
    print(f"  OPTA_MAX:       {q}")
    
    q.observe(-1)  # Weak evidence toward NONE
    print(f"  -1 evidence:    {q} (stopped at barrier)")
    
    q.observe(-3)  # Strong evidence toward NONE
    print(f"  -3 evidence:    {q} (crossed barrier)")
    print()
    
    # Crystallization thresholds
    print("4. Crystallization Thresholds (60% confidence)")
    print("-" * 50)
    for val in [-10, -8, -3, 0, 3, 8, 10]:
        q = Q21(val)
        result = q.crystallize()
        print(f"  {q.get_state_name():12s} → {result if result else 'uncertain'}")
