#include "threads.h"

BaseType_t do_iteration(const CriticalCtx *ctx) {
    if (xSemaphoreTake(ctx->lock, ctx->wait) != pdTRUE) {
        return pdFALSE;
    }
    (*ctx->counter)++;
    xSemaphoreGive(ctx->lock);
    return pdTRUE;
}

// Take A, wait, then try B

static void taskA(void *pv) {
    DeadlockPair *p = (DeadlockPair*)pv;

    xSemaphoreTake(p->a, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(10));
    xSemaphoreTake(p->b, p->wait); // Should stall here

    xSemaphoreGive(p->b);   // Clean up
    xSemaphoreGive(p->a);
    vTaskDelete(NULL);
}

static void taskB(void *pv) {
    DeadlockPair *p = (DeadlockPair*)pv;

    xSemaphoreTake(p->b, portMAX_DELAY);
    vTaskDelay(pdMS_TO_TICKS(10));
    xSemaphoreTake(p->a, p->wait); // Stalls here

    xSemaphoreGive(p->a);   // Clean up, shouldn't make it here
    xSemaphoreGive(p->b);
    vTaskDelete(NULL);
}

static void start_deadlock_pair(DeadlockPair *p, UBaseType_t prio) {
    // p->a = xSemaphoreCreateCounting(1, 1);   // Create these in test instead
    // p->b = xSemaphoreCreateCounting(1, 1);

    configASSERT(p->a && p->b);
    xTaskCreate(taskA, "A", configMINIMAL_STACK_SIZE, p, prio, &p->thA);
    xTaskCreate(taskB, "B", configMINIMAL_STACK_SIZE, p, prio, &p->thB);
}