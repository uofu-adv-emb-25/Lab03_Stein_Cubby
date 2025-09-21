#include "threads.h"

BaseType_t do_iteration(const CriticalCtx *ctx) {
    if (xSemaphoreTake(ctx->lock, ctx->wait) != pdTRUE) {
        return pdFALSE;
    }
    (*ctx->counter)++;
    xSemaphoreGive(ctx->lock);
    return pdTRUE;
}