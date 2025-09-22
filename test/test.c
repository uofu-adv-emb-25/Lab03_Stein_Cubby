#include <stdio.h>
#include <pico/stdlib.h>
#include <stdint.h>
#include <unity.h>
#include "unity_config.h"
#include "threads.h"

void setUp(void) {}

void tearDown(void) {}

void test_incr_unlocked(void)  // ---- Activity 2 ----
{
    SemaphoreHandle_t s = xSemaphoreCreateCounting(1, 1);
    volatile int counter = 0;
    CriticalCtx ctx = { .lock = s, .counter = &counter, .wait = 0};
    
    TEST_ASSERT_EQUAL(pdTRUE, do_iteration(&ctx));
    TEST_ASSERT_EQUAL(1, counter);

    vSemaphoreDelete(s);
}

void test_incr_locked(void)
{
    SemaphoreHandle_t s = xSemaphoreCreateCounting(1, 1);
    volatile int counter = 0;
    CriticalCtx ctx = { .lock = s, .counter = &counter, .wait = portMAX_DELAY};

    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(ctx.lock, ctx.wait));
    TEST_ASSERT_EQUAL(pdFALSE, do_iteration(&ctx));
    TEST_ASSERT_EQUAL(0, counter);
    xSemaphoreGive(ctx.lock);
    vSemaphoreDelete(s);
}

int main (void)
{
    stdio_init_all();
    sleep_ms(5000); // Give time for TTY to attach.
    while(1){
        printf("Start tests\n");
        UNITY_BEGIN();
        RUN_TEST(test_incr_unlocked);
        RUN_TEST(test_incr_locked);
        sleep_ms(5000);
        UNITY_END();
    }
}
