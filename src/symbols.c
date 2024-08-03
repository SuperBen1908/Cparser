
#include <stdio.h>
#include <string.h>

#include "../inc/symbols.h"
#include "../inc/preprocessor.h"
#include "../inc/tokens.h"

#include "../../DS/inc/hash.h"
#include "../../DS/inc/vector.h"
#include "../../DS/inc/stack.h"

#include "../../utils/error.h"
#include "../../utils/constants.h"

typedef enum {
    NO_PARENTHESIS,
    PARENTHESIS,
    BRACE,
    BRACKET
} parenthesis_e;

typedef struct parenthesis_pair {
    parenthesis_e type;
    size_t open_idx;
    size_t close_idx;
} parenthesis_pair_t;


/************************/
/* Forward Declerations */
/************************/
static int scope_init(scope_t *scope, scope_t * parent_scope);
static void scope_clear(scope_t *scope);
static int check_parenthesis(file_tokens_t tokens, vector_t *parenthesis);
static int read_keyword(IN file_tokens_t tokens,
                        IN file_defines_t defines,
                        IN_OUT sym_table_t *symbols,
                        IN_OUT size_t *tok_idx,
                        IN_OUT scope_t **scope);
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
        printf("ERROR!!! hash init\n");
        return (err);
    }
    if ((err = VectorInit(&symbols->functions, 1, sizeof(function_t)))) {
        symbols_end(*symbols);
        printf("ERROR!!! vector init\n");
        return (err);
    }
    symbols->functions_count = 0;
    if ((err = scope_init(&symbols->g_scope, NULL))) {
        symbols_end(*symbols);
        printf("ERROR!!! scope init\n");
        return (err);
    }

    return (ERR_OK);
}

void symbols_end(IN sym_table_t symbols)
{
    if (symbols.symbols) HashClear(symbols.symbols);
    if (symbols.functions) VectorClear(symbols.functions);
    scope_clear(&symbols.g_scope);
}

int symbols_read(IN file_tokens_t tokens,
                 IN file_defines_t defines,
                 OUT sym_table_t *symbols)
{
    int err = ERR_OK;
    vector_t parenthesis = NULL;
    size_t tok_idx = 0;
    scope_t *scope = &symbols->g_scope;

    if ((err = check_parenthesis(tokens, &parenthesis))) {
        printf("ERROR!!! parenthesis check failed\n");
        goto err;
    }

    for (tok_idx = 0; tok_idx < tokens.count; ++tok_idx) {
        token_t *tok = VectorGet(&tokens.vec, tok_idx, token_t);

        if (tok->type == TOKEN_KEYWORD) {
            if ((err = read_keyword(tokens, defines, symbols, &tok_idx, &scope))) {
                printf("ERROR!!! keyword read failed\n");
                goto err;
            }
            continue;
        }

        if (tok->value == SPECIAL_O_BRACE) {

            continue;
        }
    }

    if (parenthesis) VectorClear(parenthesis);
    return (ERR_OK);
err:
    if (parenthesis) VectorClear(parenthesis);
    symbols_end(*symbols);
    return (err);
}

/********************/
/* Helper Functions */
/********************/
static int scope_init(scope_t *scope, scope_t * parent_scope)
{
    int err = ERR_OK;

    if ((err = VectorInit(&scope->types, 1, sizeof(type_t)))) {
        printf("ERROR!!! vector init\n");
        goto err;
    }
    scope->types_count = 0;
    if ((err = VectorInit(&scope->variables, 1, sizeof(variable_t)))) {
        printf("ERROR!!! vector init\n");
        goto err;
    }
    scope->variables_count = 0;
    if ((err = VectorInit(&scope->scopes, 1, sizeof(scope_t)))) {
        printf("ERROR!!! vector init\n");
        goto err;
    }
    scope->scopes_count = 0;
    scope->parent_scope = parent_scope;

    return (ERR_OK);
err: 
    scope_clear(scope);
    return (err);
}

static void scope_clear(scope_t *scope)
{
    if (scope->types)
        VectorClear(scope->types);
    if (scope->variables)
        VectorClear(scope->variables);
    if (scope->scopes) {
        while (scope->scopes_count > 0) {
            --scope->scopes_count;
            if (VectorGet(&scope->scopes, scope->scopes_count, scope_t))
                scope_clear(VectorGet(&scope->scopes, scope->scopes_count, scope_t));
        }
        VectorClear(scope->scopes);
    }
}

static int check_parenthesis(file_tokens_t tokens, vector_t *parenthesis)
{
    int err = ERR_OK;
    size_t tok_idx = 0;
    size_t parenthesis_idx = 0;
    stack_t stack = NULL;
    int parenthesis_item[2] = {0, 0};

    if ((err = StackInit(&stack, 2*sizeof(int)))) {
        printf("ERROR!!! stack init\n");
        goto err;
}
    if ((err = VectorInit(parenthesis, 1, sizeof(parenthesis_pair_t)))) {
        printf("ERROR!!! vector init\n");
        goto err;
    }
    for (tok_idx = 0; tok_idx < tokens.count; ++tok_idx) {
        memset(&parenthesis_item, 0, 2*sizeof(int));
        switch (VectorGet(&tokens.vec, tok_idx, token_t)->value) {
            case SPECIAL_O_PARENTHESIS:
            case SPECIAL_C_PARENTHESIS:
                parenthesis_item[0] = PARENTHESIS;
                break;
            case SPECIAL_O_BRACE:
            case SPECIAL_C_BRACE:
                parenthesis_item[0] = BRACE;
                break;
            case SPECIAL_O_BRACKET:
            case SPECIAL_C_BRACKET:
                parenthesis_item[0] = BRACKET;
                break;
            default:
                continue;
        }
        switch (VectorGet(&tokens.vec, tok_idx, token_t)->value) {
            case SPECIAL_O_PARENTHESIS:
            case SPECIAL_O_BRACE:
            case SPECIAL_O_BRACKET:
                parenthesis_item[1] = parenthesis_idx;
                StackPush(&stack, &parenthesis_item);
                VectorGet(parenthesis, parenthesis_idx, parenthesis_pair_t)->open_idx = tok_idx;
                VectorGet(parenthesis, parenthesis_idx, parenthesis_pair_t)->type = parenthesis_item[0];
                ++parenthesis_idx;
                continue;
        }
        if (StackIsEmpty(stack)) {
            err = ERR_PARENTHESIS_MISSING;
            goto err;
        }
        switch (VectorGet(&tokens.vec, tok_idx, token_t)->value) {
            case SPECIAL_C_PARENTHESIS:
            case SPECIAL_C_BRACE:
            case SPECIAL_C_BRACKET:
                if (parenthesis_item[0] != ((int*)StackPeek(stack))[0]) {
                    err = ERR_PARENTHESIS_MISSING;
                    goto err;
                }
                VectorGet(parenthesis, ((int*)StackPeek(stack))[1], parenthesis_pair_t)->close_idx = tok_idx;
                StackPop(stack);
                break;
        }
    }
    if (!StackIsEmpty(stack)) {
        err = ERR_PARENTHESIS_MISSING;
        goto err;
    }

    StackClear(stack);
    return (ERR_OK);
err:
    if (stack) StackClear(stack);
    if (*parenthesis) VectorClear(*parenthesis);
    return (err);
}

static int read_keyword(IN file_tokens_t tokens,
                        IN file_defines_t defines,
                        IN_OUT sym_table_t *symbols,
                        IN_OUT size_t *tok_idx,
                        IN_OUT scope_t **scope)
{
    return (ERR_OK);
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
