#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

typedef struct {
    SemaphoreHandle_t lock;
    volatile int *counter;
    TickType_t wait;
} CriticalCtx;

// Outline for deadlock test later

/*
typedef struct {
    SemaphoreHandle_t a;
    SempahoreHandle_t b;
    TickType_t wait;
    TaskHandle_t thA;
    TaskHandle_t thB;
} DeadlockPair;
 */

 BaseType_t do_iteration(const CriticalCtx *ctx);