#include <stdio.h>
#include <pico/stdlib.h>
#include <stdint.h>
#include <unity.h>
#include "unity_config.h"
#include "threads.h"

void setUp(void) {}

void tearDown(void) {}

// ---- Activity 2 ----

void test_incr_unlocked(void)
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

// ---- Activity 4 ----

void test_deadlock_pair(void) {
    DeadlockPair p = {0};

    p.a = xSemaphoreCreateCounting(1, 1);
    p.b = xSemaphoreCreateCounting(1, 1);
    p.wait = portMAX_DELAY;
    TEST_ASSERT_NOT_NULL(p.a);
    TEST_ASSERT_NOT_NULL(p.b);

    start_deadlock_pair(&p, tskIDLE_PRIORITY + 1);

    // Wait to reach deadlock
    vTaskDelay(pdMS_TO_TICKS(50));

    // Both should be blocked
    eTaskState sa = eTaskGetState(p.thA);
    eTaskState sb = eTaskGetState(p.thB);
    TEST_ASSERT_TRUE(sa == eBlocked || sa == eSuspended);
    TEST_ASSERT_TRUE(sb == eBlocked || sb == eSuspended);

    // Cleanup
    vTaskSuspend(p.thA);
    vTaskSuspend(p.thB);
    vTaskDelete(p.thA);
    vTaskDelete(p.thB);
    vSemaphoreDelete(p.a);
    vSemaphoreDelete(p.b);
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
