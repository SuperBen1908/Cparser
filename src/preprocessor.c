
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "../inc/preprocessor.h"
#include "../inc/tokens.h"

#include "../../DS/inc/vector.h"
#include "../../DS/inc/hash.h"

#include "../../utils/error.h"


#define HASH_SIZE_DEF (1024)


/************************
 * Forward Declerations *
 ************************/
static int macro_cmp(void *item1, void *item2);
static int parse_define(file_tokens_t *tokens, file_defines_t *defines, size_t *idx);
static int parse_undef(file_tokens_t *tokens, file_defines_t *defines, size_t *idx);
static int parse_include(file_tokens_t *tokens, file_defines_t *defines, size_t *idx);
static int parse_pragma(file_tokens_t *tokens, file_defines_t *defines, size_t *idx);
static int parse_line(file_tokens_t *tokens, file_defines_t *defines, size_t *idx);
static int parse_file(file_tokens_t *tokens, file_defines_t *defines, size_t *idx);
static int parse_error(file_tokens_t *tokens, file_defines_t *defines, size_t *idx);
static int parse_ifdef(file_tokens_t *tokens, file_defines_t *defines, size_t *idx);
static int parse_ifndef(file_tokens_t *tokens, file_defines_t *defines, size_t *idx);
static int parse_if(file_tokens_t *tokens, file_defines_t *defines, size_t *idx);
static int parse_elif(file_tokens_t *tokens, file_defines_t *defines, size_t *idx);
static int parse_else(file_tokens_t *tokens, file_defines_t *defines, size_t *idx);
static int parse_endif(file_tokens_t *tokens, file_defines_t *defines, size_t *idx);

static void clear_define(file_defines_t defines, define_t *def);
static size_t macro_hash(void *item);

/*****************
 * API Functions *
 *****************/
int preprocessor_start(OUT file_defines_t *defines)
{
    int err = ERR_OK;
    if ((err = HashInit(&defines->map, HASH_SIZE_DEF, sizeof(define_t), macro_hash, macro_cmp))) {
        printf("hash init err!!!: %d\n", err);
        return (err);
    }
    if ((err = VectorInit(&defines->keys, 1, sizeof(token_t)))) {
        printf("vector init err!!!: %d\n", err);
        return (err);
    }
    
    return (ERR_OK);
}

void preprocessor_end(IN file_defines_t defines)
{
    size_t i = 0;
    for (i = 0; i < defines.count; ++i) {
        size_t j = 0;
        define_t key = {0}, def = {0};
        memcpy(&key.key, VectorGet(&defines.keys, i, token_t), sizeof(token_t));
        if (HashFind(defines.map, &key, &def)) {
            printf("macro not found in destroy!!!\n");
            return ;
        }

        VectorClear(def.args);
        VectorClear(def.value);
    }
    HashClear(defines.map);
    VectorClear(defines.keys);
}

int preprocessor_read(IN_OUT file_tokens_t  *tokens,
                      IN_OUT file_defines_t *defines)
{
    int err = ERR_OK;
    size_t i = 0;
    token_t *tok = NULL;

    for (i = 0; i < tokens->count; ++i) {
        tok = VectorGet(&tokens->vec, i, token_t);
        if (tok->value == SPECIAL_HASH) {
            if (i == tokens->count-1) {
                printf("preprocessor err!!!\n");
                return (ERR_PREPROCESSOR);
            }
            tok = VectorGet(&tokens->vec, ++i, token_t);
            switch (tok->value) {
                case KEYWORD_DEFINE:
                    if ((err = parse_define(tokens, defines, &i)))
                        return (err);
                    break;
                case KEYWORD_UNDEF:
                    if ((err = parse_undef(tokens, defines, &i)))
                        return (err);
                    break;
                case KEYWORD_INCLUDE:
                    if ((err = parse_include(tokens, defines, &i)))
                        return (err);
                    break;
                case KEYWORD_PRAGMA:
                    if ((err = parse_pragma(tokens, defines, &i)))
                        return (err);
                    break;
                case KEYWORD_LINE:
                    if ((err = parse_line(tokens, defines, &i)))
                        return (err);
                    break;
                case KEYWORD_FILE:
                    if ((err = parse_file(tokens, defines, &i)))
                        return (err);
                    break;
                case KEYWORD_ERROR:
                    if ((err = parse_error(tokens, defines, &i)))
                        return (err);
                    break;
                case KEYWORD_IFDEF:
                    if ((err = parse_ifdef(tokens, defines, &i)))
                        return (err);
                    break;
                case KEYWORD_IFNDEF:
                    if ((err = parse_ifndef(tokens, defines, &i)))
                        return (err);
                    break;
                case KEYWORD_PP_IF:
                    if ((err = parse_if(tokens, defines, &i)))
                        return (err);
                    break;
                case KEYWORD_PP_ELIF:
                    if ((err = parse_elif(tokens, defines, &i)))
                        return (err);
                    break;
                case KEYWORD_PP_ELSE:
                    if ((err = parse_else(tokens, defines, &i)))
                        return (err);
                    break;
                case KEYWORD_PP_ENDIF:
                    if ((err = parse_endif(tokens, defines, &i)))
                        return (err);
                    break;
                default:
                    break;
            }
        }
    }

    return (ERR_OK);
}
/********************
 * Helper Functions *
 ********************/
static int parse_define(file_tokens_t *tokens, file_defines_t *defines, size_t *idx)
{
    int err = ERR_OK;
    define_t define = {0}, res = {0};
    size_t i = 0, j = 0, k = 0 ;
    size_t line = 0;
    size_t value_idx = 0;
    token_t *tok = NULL;
    int has_args = FALSE;
    
    if (*idx == tokens->count-1) {
        printf("define token err!!!\n");
        return (ERR_DEFINE);
    }
    tok = VectorGet(&tokens->vec, ++*idx, token_t);
    if (tok->type != TOKEN_IDENTIFIER) {
        printf("define variable name err!!!\n");
        return (ERR_DEFINE);
    }

    /* removes existing defines with overlapping name */
    define.key = *tok;
    memcpy(define.key.str, tok->str, TOKEN_MAX_SIZE);
    if (HashFind(defines->map, &define, &res) == ERR_OK) {
        printf("WARNING macro redefinition!\n");
        clear_define(*defines, &res);
    }

    if ((err = VectorInit(&define.value, 4, sizeof(token_t))))
        return err;
    if ((err = VectorInit(&define.args, 4, sizeof(token_t))))
        return err;

    /* reading macro args */
    {
        size_t name_end = tok->col+strlen(tok->str);
        size_t parenthesis_start = VectorGet(&tokens->vec, ++*idx, token_t)->col;
        has_args = (name_end == parenthesis_start);
    }
    define.arg_count = 0;
    tok = VectorGet(&tokens->vec, *idx, token_t);
    line = tok->row;
    i = *idx;
    if (has_args && tok->value == SPECIAL_O_PARENTHESIS) {
        tok = VectorGet(&tokens->vec, ++*idx, token_t);
        for (i = *idx; i < tokens->count; ++i) {
            tok = VectorGet(&tokens->vec, i, token_t);
            if (i+1 < tokens->count && 
                    tok->value == SPECIAL_BACKSLASH &&
                    VectorGet(&tokens->vec, i+1, token_t)->row != line) {
                ++i;
                line = VectorGet(&tokens->vec, i, token_t)->row;
            }
            /* arg name */
            tok = VectorGet(&tokens->vec, i, token_t);
            if (tok->type != TOKEN_IDENTIFIER)
                return (ERR_DEFINE_ARGS);
            *VectorGet(&define.args, define.arg_count, token_t) = *tok;
            ++define.arg_count;

            /* comma */
            tok = VectorGet(&tokens->vec, ++i, token_t);
            if (tok->value == SPECIAL_C_PARENTHESIS) {
                /* ending parenthesis and macro arguments */
                ++i;
                break;
            }
            if (tok->value != SPECIAL_COMMA) {
                return (ERR_DEFINE_ARGS);
            }

            if (tok->row != line)
                return (ERR_PREPROCESSOR);
        }
    }
    *idx = i;
    line = VectorGet(&tokens->vec, *idx, token_t)->row;

    /* reading macro value */
    value_idx = 0;
    for (i = *idx; i < tokens->count; ++i) {
        tok = VectorGet(&tokens->vec, i, token_t);
        if (i+1 < tokens->count && 
                tok->value == SPECIAL_BACKSLASH &&
                VectorGet(&tokens->vec, i+1, token_t)->row != line) {
            tok = VectorGet(&tokens->vec, ++i, token_t);
            line = tok->row;
        }
        if (tok->row != line)
            break;

        if (tok->type == TOKEN_IDENTIFIER) {
            define_t expended_define = {0};
            k = 0;
            memcpy(&expended_define.key.str, tok->str, TOKEN_MAX_SIZE);
            if (HashFind(defines->map, &expended_define, &expended_define) == ERR_OK) {
                while (VectorGet(&expended_define.value, k, token_t)->str[0]) {
                    memcpy(VectorGet(&define.value, value_idx, token_t),
                           VectorGet(&expended_define.value, k, token_t), sizeof(token_t));
                    ++k;
                    ++value_idx;
                }
                continue;
            }
        }
        memcpy(VectorGet(&define.value, value_idx, token_t), tok, sizeof(token_t));
        ++value_idx;
    }
    /* update tok index to be the closing bracket */
    *idx = i-1;

    memcpy(VectorGet(&defines->keys, defines->count++, token_t),
            &define.key, sizeof(token_t));
    HashAdd(defines->map, &define);

    return (ERR_OK);
}

static int parse_undef(file_tokens_t *tokens, file_defines_t *defines, size_t *idx)
{
    define_t define = {0};
    token_t *tok = VectorGet(&tokens->vec, *idx, token_t);
    define.key = *tok;
    memcpy(define.key.str, tok->str, TOKEN_MAX_SIZE);
    if (HashFind(defines->map, &define, NULL) == ERR_OK)
        clear_define(*defines, &define);

    return (ERR_OK);
}

static int parse_include(file_tokens_t *tokens, file_defines_t *defines, size_t *idx)
{
    return (ERR_OK);
}

static int parse_pragma(file_tokens_t *tokens, file_defines_t *defines, size_t *idx)
{
    return (ERR_OK);
}

static int parse_line(file_tokens_t *tokens, file_defines_t *defines, size_t *idx)
{
    return (ERR_OK);
}

static int parse_file(file_tokens_t *tokens, file_defines_t *defines, size_t *idx)
{
    return (ERR_OK);
}

static int parse_error(file_tokens_t *tokens, file_defines_t *defines, size_t *idx)
{
    return (ERR_OK);
}

static int parse_ifdef(file_tokens_t *tokens, file_defines_t *defines, size_t *idx)
{
    return (ERR_OK);
}

static int parse_ifndef(file_tokens_t *tokens, file_defines_t *defines, size_t *idx)
{
    return (ERR_OK);
}
static int parse_(file_tokens_t *tokens, file_defines_t *defines, size_t *idx)
{
    return (ERR_OK);
}

static int parse_if(file_tokens_t *tokens, file_defines_t *defines, size_t *idx)
{
    return (ERR_OK);
}

static int parse_elif(file_tokens_t *tokens, file_defines_t *defines, size_t *idx)
{
    return (ERR_OK);
}

static int parse_else(file_tokens_t *tokens, file_defines_t *defines, size_t *idx)
{
    return (ERR_OK);
}

static int parse_endif(file_tokens_t *tokens, file_defines_t *defines, size_t *idx)
{
    return (ERR_OK);
}

static void clear_define(file_defines_t defines, define_t *def)
{
    HashRemove(defines.map, def);
    VectorClear(def->value);
}

static size_t macro_hash(void *item)
{
    return (HashStr(((define_t *)item)->key.str)%HASH_SIZE_DEF);
}

static int macro_cmp(void *item1, void *item2)
{
    return (strncmp(((define_t *)item1)->key.str, ((define_t *)item2)->key.str,
                TOKEN_MAX_SIZE));
}
