/*
 *  This file is part of the "Coroutine" project and released under the MIT License.
 *
 *  Created by Samuel Williams on 10/5/2018.
 *  Copyright, 2018, by Samuel Williams. All rights reserved.
*/

#pragma once

#include <assert.h>
#include <string.h>

#if __cplusplus
extern "C" {
#endif

#define COROUTINE __declspec(noreturn) void __fastcall

/* This doesn't include thread information block */
enum {COROUTINE_REGISTERS = 4};

typedef struct
{
    void **stack_pointer;
} coroutine_context;

typedef void(__fastcall * coroutine_start)(coroutine_context *from, coroutine_context *self);

static inline void coroutine_initialize(
    coroutine_context *context,
    coroutine_start start,
    void *stack_pointer,
    size_t stack_size
) {
    context->stack_pointer = (void**)stack_pointer;

    if (!start) {
        assert(!context->stack_pointer);
        /* We are main coroutine for this thread */
        return;
    }

    *--context->stack_pointer = (void*)start;

    /* Windows Thread Information Block */
    *--context->stack_pointer = 0; /* fs:[0] */
    *--context->stack_pointer = (void*)stack_pointer; /* fs:[4] */
    *--context->stack_pointer = (void*)((char *)stack_pointer - stack_size);  /* fs:[8] */

    context->stack_pointer -= COROUTINE_REGISTERS;
    memset(context->stack_pointer, 0, sizeof(void*) * COROUTINE_REGISTERS);
}

coroutine_context * __fastcall coroutine_transfer(coroutine_context * current, coroutine_context * target);

static inline void coroutine_destroy(coroutine_context * context)
{
}

#if __cplusplus
}
#endif
