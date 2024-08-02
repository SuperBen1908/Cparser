
#include <stdio.h>
#include <string.h>

#include "../inc/symbols.h"
#include "../inc/preprocessor.h"
#include "../inc/tokens.h"

#include "../../DS/inc/hash.h"
#include "../../DS/inc/vector.h"

#include "../../utils/error.h"
#include "../../utils/constants.h"


/************************/
/* Forward Declerations */
/************************/
static int scope_init(scope_t *scope);
static void scope_clear(scope_t *scope);
static size_t sym_hash(void *item);
static int sym_cmp(void *item1, void *item2);


/*****************/
/* API Functions */
/*****************/
int symbols_start(IN const char *file_name,
                  OUT sym_table_t *symbols)
{
    int err = ERR_OK;

    strncpy(symbols->file_name, file_name, TOKEN_MAX_SIZE);

    if ((err = HashInit(&symbols->symbols, HASH_SIZE_SYM, sizeof(sym_t), sym_hash, sym_cmp))) {
        printf("hash init err!!!: %d\n", err);
        return (err);
    }
    if ((err = scope_init(&symbols->g_scope))) {
        printf("scope init err!!!: %d\n", err);
        return (err);
    }

    return (ERR_OK);
}

void symbols_end(IN sym_table_t symbols)
{
    HashClear(symbols.symbols);
    scope_clear(&symbols.g_scope);
}

int symbols_read(IN file_tokens_t tokens,
                 IN file_defines_t defines,
                 OUT sym_table_t *symbols)
{

    return (ERR_OK);
}

/********************/
/* Helper Functions */
/********************/
static int scope_init(scope_t *scope)
{
    int err = ERR_OK;

    if ((err = VectorInit(&scope->types, 1, sizeof(type_t)))) {
        printf("vector init err!!!: %d\n", err);
        return (err);
    }
    scope->types_count = 0;
    if ((err = VectorInit(&scope->variables, 1, sizeof(variable_t)))) {
        printf("vector init err!!!: %d\n", err);
        return (err);
    }
    scope->variables_count = 0;
    if ((err = VectorInit(&scope->functions, 1, sizeof(function_t)))) {
        printf("vector init err!!!: %d\n", err);
        return (err);
    }
    scope->functions_count = 0;
    if ((err = VectorInit(&scope->scopes, 1, sizeof(scope_t)))) {
        printf("vector init err!!!: %d\n", err);
        return (err);
    }
    scope->scopes_count = 0;

    return (ERR_OK);
}

static void scope_clear(scope_t *scope)
{
    VectorClear(scope->types);
    VectorClear(scope->variables);
    VectorClear(scope->functions);
    while (scope->scopes_count > 0) {
        --scope->scopes_count;
        scope_clear(VectorGet(&scope->scopes, scope->scopes_count, scope_t));
    }
    VectorClear(scope->scopes);
}

static size_t sym_hash(void *item)
{
    return (HashStr(((sym_t *)item)->name)%HASH_SIZE_SYM);
}

static int sym_cmp(void *item1, void *item2)
{
    return (strncmp(((sym_t *)item1)->name, ((sym_t *)item2)->name,
                TOKEN_MAX_SIZE));
}
