
#ifndef __PREPROCESSOR_H__
#define __PREPROCESSOR_H__

#include <stddef.h>

#include "../inc/tokens.h"

#include "../../DS/inc/hash.h"
#include "../../DS/inc/vector.h"

#include "../../utils/error.h"
#include "../../utils/constants.h"


/*******/
/* API */
/*******/
typedef struct {
    hash_t map;
    vector_t keys;
    size_t count;
} file_defines_t;

typedef struct {
    token_t key;
    vector_t args;
    size_t arg_count;
    vector_t value;
} define_t;

int preprocessor_start(OUT file_defines_t   *defines);

void preprocessor_end(IN file_defines_t      defines);

int preprocessor_read(IN_OUT file_tokens_t  *tokens,
                      IN_OUT file_defines_t *defines);

#endif // __PREPROCESSOR_H__
