#include "common.h"

timetype getTime() {
    timetype now;
#ifdef FORMPU
    now = to_ms_since_boot(get_absolute_time());
#else
    now = Clock::now();
#endif
    return now;
}

uint16_t getTimeDiffMS(timetype start) {
    timetype end = getTime();
    uint16_t timeDiffMS = 0;
#ifdef FORMPU
    timeDiffMS = (end - start);
#else
    milliseconds ms = std::chrono::duration_cast<milliseconds>(end - start);
    timeDiffMS = ms.count();
#endif
    return timeDiffMS;
}
