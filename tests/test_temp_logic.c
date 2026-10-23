/*
 * Host-side unit tests for STM32_Project/App/temp_logic.c.
 * Build and run with:   make -C STM32_Project test
 * Needs only gcc (no ARM toolchain, no HAL).
 */
#include "temp_logic.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int failures = 0;
static int checks = 0;

#define CHECK(cond)                                                        \
    do {                                                                   \
        ++checks;                                                          \
        if (!(cond)) {                                                     \
            ++failures;                                                    \
            printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);        \
        }                                                                  \
    } while (0)

#define CHECK_NEAR(actual, expected, tol)                                  \
    do {                                                                   \
        ++checks;                                                          \
        double a_ = (actual), e_ = (expected);                             \
        if (fabs(a_ - e_) > (tol)) {                                       \
            ++failures;                                                    \
            printf("FAIL %s:%d: %s = %f, expected %f\n",                   \
                   __FILE__, __LINE__, #actual, a_, e_);                   \
        }                                                                  \
    } while (0)

/* Smallest raw value whose displayed temperature is >= tenths. */
static uint32_t raw_for_tenths(uint32_t tenths)
{
    for (uint32_t raw = 0U; raw <= 4095U; ++raw)
        if (TempLogic_CelsiusToTenths(TempLogic_RawToCelsius(raw)) >= tenths)
            return raw;
    return 4095U;
}

static void test_conversion(void)
{
    CHECK_NEAR(TempLogic_RawToCelsius(0U), 0.0, 0.001);
    CHECK_NEAR(TempLogic_RawToCelsius(310U), 24.98, 0.05);    /* ~25 C   */
    CHECK_NEAR(TempLogic_RawToCelsius(2048U), 165.04, 0.05);  /* mid     */
    CHECK_NEAR(TempLogic_RawToCelsius(4095U), 330.0, 0.01);   /* full    */

    CHECK(TempLogic_CelsiusToTenths(0.0f) == 0U);
    CHECK(TempLogic_CelsiusToTenths(-5.0f) == 0U);
    CHECK(TempLogic_CelsiusToTenths(24.96f) == 250U);
    CHECK(TempLogic_CelsiusToTenths(49.95f) == 500U);
    CHECK(TempLogic_CelsiusToTenths(1e9f) == 9999U);
    CHECK(TempLogic_CelsiusToTenths(NAN) == 0U);
}

static void test_averaging(void)
{
    const uint32_t same[4] = {620U, 620U, 620U, 620U};
    const uint32_t mixed[4] = {600U, 610U, 620U, 631U};   /* 615.25 -> 615 */
    const uint32_t round_up[2] = {1U, 2U};                /* 1.5 -> 2       */
    const uint32_t bad[3] = {500U, LM35_RAW_INVALID, 500U};
    uint32_t full[LM35_SAMPLE_COUNT];

    for (uint32_t i = 0U; i < LM35_SAMPLE_COUNT; ++i)
        full[i] = 4095U;

    CHECK(TempLogic_AverageRaw(same, 4U) == 620U);
    CHECK(TempLogic_AverageRaw(mixed, 4U) == 615U);
    CHECK(TempLogic_AverageRaw(round_up, 2U) == 2U);
    CHECK(TempLogic_AverageRaw(full, LM35_SAMPLE_COUNT) == 4095U);
    CHECK(TempLogic_AverageRaw(bad, 3U) == LM35_RAW_INVALID);
    CHECK(TempLogic_AverageRaw(same, 0U) == LM35_RAW_INVALID);
    CHECK(TempLogic_AverageRaw(NULL, 4U) == LM35_RAW_INVALID);
}

static void test_fault_detection(void)
{
    CHECK(TempLogic_IsSensorFault(0U));
    CHECK(TempLogic_IsSensorFault(5U));          /* low: disconnected */
    CHECK(!TempLogic_IsSensorFault(6U));
    CHECK(!TempLogic_IsSensorFault(620U));
    CHECK(!TempLogic_IsSensorFault(4089U));
    CHECK(TempLogic_IsSensorFault(4090U));       /* high: shorted     */
    CHECK(TempLogic_IsSensorFault(4095U));
    CHECK(TempLogic_IsSensorFault(LM35_RAW_INVALID));

    TempAlarmState s;
    TempLogic_Init(&s);
    TempStatus st = TempLogic_Update(&s, 0U);
    CHECK(st.fault == 1 && st.alarm == 1);       /* fail safe */
    st = TempLogic_Update(&s, LM35_RAW_INVALID);
    CHECK(st.fault == 1 && st.alarm == 1);
}

static void test_hysteresis(void)
{
    const uint32_t raw_50 = raw_for_tenths(500U);   /* first raw showing 50.0 */
    const uint32_t raw_48 = raw_for_tenths(480U);   /* first raw showing 48.0 */
    TempAlarmState s;
    TempStatus st;

    TempLogic_Init(&s);
    st = TempLogic_Update(&s, raw_50 - 1U);          /* 49.9: stays off */
    CHECK(st.alarm == 0 && st.tenths == 499U);
    st = TempLogic_Update(&s, raw_50);               /* 50.0: turns on  */
    CHECK(st.alarm == 1 && st.tenths == 500U);
    st = TempLogic_Update(&s, raw_48 + 10U);         /* ~49 C: still on */
    CHECK(st.alarm == 1);
    st = TempLogic_Update(&s, raw_48);               /* 48.0: still on  */
    CHECK(st.alarm == 1 && st.tenths == 480U);
    st = TempLogic_Update(&s, raw_48 - 1U);          /* 47.9: turns off */
    CHECK(st.alarm == 0 && st.tenths == 479U);
    st = TempLogic_Update(&s, raw_48 + 10U);         /* back up in band: stays off */
    CHECK(st.alarm == 0);
}

static void test_no_flicker_at_threshold(void)
{
    const uint32_t raw_50 = raw_for_tenths(500U);
    TempAlarmState s;
    int toggles = 0;
    int last;

    TempLogic_Init(&s);
    last = TempLogic_Update(&s, raw_50).alarm;
    CHECK(last == 1);
    /* Noise of +-2 counts (about +-0.16 C) around 50.0 C must not toggle it. */
    for (int i = 0; i < 200; ++i)
    {
        uint32_t raw = raw_50 + (uint32_t)(i % 5) - 2U;
        int now = TempLogic_Update(&s, raw).alarm;
        if (now != last)
            ++toggles;
        last = now;
    }
    CHECK(toggles == 0);
    CHECK(last == 1);
}

static void test_display_matches_alarm(void)
{
    /* Rising from cold: the alarm is on exactly when the LCD shows >= 50.0 C. */
    char line[TEMP_LCD_COLUMNS + 1U];
    TempAlarmState s;

    TempLogic_Init(&s);
    for (uint32_t raw = LM35_SENSOR_LOW_RAW_THRESHOLD + 1U;
         raw < LM35_SENSOR_HIGH_RAW_THRESHOLD; ++raw)
    {
        TempStatus st = TempLogic_Update(&s, raw);
        TempLogic_FormatLine(line, sizeof(line), st.tenths);
        double shown = strtod(line, NULL);
        if ((shown >= 50.0) != (st.alarm == 1))
        {
            CHECK(0 && "LCD value and alarm disagree");
            printf("     raw=%u line='%s' alarm=%d\n", (unsigned)raw, line, st.alarm);
            break;
        }
    }
    ++checks;
}

static void test_fault_recovery_resets_hysteresis(void)
{
    const uint32_t raw_50 = raw_for_tenths(500U);
    const uint32_t raw_49 = raw_for_tenths(490U);
    TempAlarmState s;
    TempStatus st;

    /* Fault, then recovery inside the 48-50 C band: alarm must go off,
     * because it was only on due to the fault. */
    TempLogic_Init(&s);
    CHECK(TempLogic_Update(&s, 300U).alarm == 0);
    CHECK(TempLogic_Update(&s, 0U).alarm == 1);
    st = TempLogic_Update(&s, raw_49);
    CHECK(st.fault == 0 && st.alarm == 0);

    /* Fault, then recovery above 50 C: alarm stays on. */
    CHECK(TempLogic_Update(&s, 4095U).alarm == 1);
    st = TempLogic_Update(&s, raw_50 + 5U);
    CHECK(st.fault == 0 && st.alarm == 1);
    /* ...and normal hysteresis continues from there. */
    CHECK(TempLogic_Update(&s, raw_49).alarm == 1);

    /* Alarm on from heat, then a fault, then recovery in the band: off. */
    TempLogic_Init(&s);
    CHECK(TempLogic_Update(&s, raw_50).alarm == 1);
    CHECK(TempLogic_Update(&s, 0U).alarm == 1);
    CHECK(TempLogic_Update(&s, raw_49).alarm == 0);
}

static void test_format_line(void)
{
    char line[TEMP_LCD_COLUMNS + 1U];

    memset(line, 'x', sizeof(line));
    TempLogic_FormatLine(line, sizeof(line), 250U);
    CHECK(strcmp(line, "25.0 C          ") == 0);
    CHECK(strlen(line) == TEMP_LCD_COLUMNS);

    TempLogic_FormatLine(line, sizeof(line), 0U);
    CHECK(strcmp(line, "0.0 C           ") == 0);

    TempLogic_FormatLine(line, sizeof(line), 3300U);
    CHECK(strcmp(line, "330.0 C         ") == 0);

    TempLogic_FormatLine(line, sizeof(line), 123456U);   /* clamped */
    CHECK(strcmp(line, "999.9 C         ") == 0);
    CHECK(strlen(line) == TEMP_LCD_COLUMNS);
}

int main(void)
{
    test_conversion();
    test_averaging();
    test_fault_detection();
    test_hysteresis();
    test_no_flicker_at_threshold();
    test_display_matches_alarm();
    test_fault_recovery_resets_hysteresis();
    test_format_line();

    if (failures != 0)
    {
        printf("%d of %d checks failed\n", failures, checks);
        return 1;
    }
    printf("All %d checks passed\n", checks);
    return 0;
}
