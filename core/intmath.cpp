#include "intmath.h"

Unit wrap(Unit value, Unit mod) {
    return value >= 0 ? (value % mod) : (mod + (value % mod) - 1);
}

Unit clamp(Unit v, Unit min, Unit max) {
    return v >= min ? (v <= max ? v : max) : min;
}
