
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <assert.h>

#ifndef TOKENIZER
#define TOKENIZER
#include "../inc/tokens.h"
#endif

#include "../../DS/inc/vector.h"
#include "../../DS/inc/hash.h"

#include "../../utils/error.h"
#include "../../utils/constants.h"


#define HASH_SIZE_TOKEN (1024)

typedef enum {
    NO_TOK,
    CHAR,
    STRING,
    NUMBER,
    KEYWORD,
    OPERATOR
} token_e;

/***********
 * Globals *
 ***********/
static hash_t base_tokens;
static char tok_buf[TOKEN_MAX_SIZE];
static token_e token_lut[256];
static char char_special_lut[256];

/************************/
/* Forward Declerations */
/************************/
static int parse_char_literal(  IN  token_t *token, IN      char    *buf,
							    IN  size_t  len,    IN_OUT  size_t  *idx);
static int parse_string_literal(IN  token_t *token, IN      char    *buf,
                                IN  size_t  len,    IN_OUT  size_t  *idx);
static int parse_num_literal(   IN  token_t *token, IN      char    *buf,
                                IN  size_t  len,    IN_OUT  size_t  *idx);
static int parse_keyword(       IN  token_t *token, IN      char    *buf,
                                IN  size_t  len,    IN_OUT  size_t  *idx);
static int parse_operator(      IN  token_t *token, IN      char    *buf,
                                IN  size_t  len,    IN_OUT  size_t  *idx);

static size_t token_hash(void *item);
static int token_cmp(void *item1, void *item2);

/*****************/
/* API Functions */
/*****************/
int tokens_start(void)
{
    int err = ERR_OK;
    int i = 0, j = 0, k = 0;

    if ((err = HashInit(&base_tokens, HASH_SIZE_TOKEN, sizeof(token_t), token_hash, token_cmp)))
        return (printf("ERROR!!! hash init err!!!: %d\n", err), err);

    for (i = 0; i < 3; ++i) {
        for (j = 0; tokens_str[i][j]; ++j) {
            for (k = 0; tokens_str[i][j][k]; ++k) {
                token_t token = {0};
                memcpy(token.str, tokens_str[i][j][k], TOKEN_MAX_SIZE);
                token.type = i+1;
                token.subtype = (i+1)*10+(j+1);
                token.value = (i+1)*100+(j)*10+(k+1);
                if ((err = HashAdd(base_tokens, &token)))
                    return (printf("ERROR!!! hash add err!!!: %d\n", err), err);
            }
        }
    }

    for (i = 0; i < 256; ++i) {
        token_lut[i] = NO_TOK;
        if (isdigit(i)) 
            token_lut[i] = NUMBER;
        if (isalpha(i))
            token_lut[i] = KEYWORD;
        if (ispunct(i))
            token_lut[i] = OPERATOR;
    }
    token_lut['_'] = KEYWORD;
    token_lut['\''] = CHAR;
    token_lut['"'] = STRING;

    char_special_lut['a'] = '\a';
    char_special_lut['b'] = '\b';
    char_special_lut['f'] = '\f';
    char_special_lut['n'] = '\n';
    char_special_lut['r'] = '\r';
    char_special_lut['t'] = '\t';
    char_special_lut['v'] = '\v';
    char_special_lut['\\'] = '\\';
    char_special_lut['\''] = '\'';
    char_special_lut['\"'] = '\"';
    char_special_lut['?'] = '\?';
    char_special_lut['0'] = '\0';
    char_special_lut['e'] = '\e';

/*    char_special_lut['o'] = '\o';*/
/*    char_special_lut['x'] = '\x';*/

    return (ERR_OK);
}

void tokens_end(void)
{
    HashClear(base_tokens);
}

int tokens_read(IN  char *buf,
                IN  size_t len,
                OUT file_tokens_t *tokens)
{
    size_t i = 0;
    size_t tok_idx = 0;
    size_t col = 1, row = 1, line_start = 1;

    VectorInit(&tokens->vec, 1, sizeof(token_t));
    
    i = 0;
    while (i < len)
    {
        token_t *token = VectorGet(&tokens->vec, tok_idx, token_t);
        token->type = TOKEN_IDENTIFIER;

        /* jumping over white spaces */
        while (i < len && isspace(buf[i])) {
            if (buf[i] == '\n') {
                ++row;
                line_start = i;
            }
            ++i;
        }
        if (i >= len) {
            break;
        }
        if (!isprint(buf[i])) {
            printf("TOKEN ERROR!! unprintable char during tokenizing\n");
            break;
        }

        col = i-line_start;
        switch (token_lut[buf[i]]) {
            case CHAR:
                if (parse_char_literal(token, buf, len, &i))
                    goto err;
                break;
            case STRING:
                if (parse_string_literal(token, buf, len, &i))
                    goto err;
                break;
            case NUMBER:
                if (parse_num_literal(token, buf, len, &i))
                    goto err;
                break;
            case KEYWORD:
                if (parse_keyword(token, buf, len, &i))
                    goto err;
                break;
            case OPERATOR:
                if (parse_operator(token, buf, len, &i))
                    goto err;
                break;
            default:
                goto err;
        }
        token->col = col;
        token->row = row;
        ++tok_idx;
    }
    tokens->count = tok_idx;

    return (ERR_OK);
err:
    return (ERR_TOKEN);
}

void tokens_destroy(IN file_tokens_t tokens)
{
    VectorClear(tokens.vec);
}

/********************/
/* Helper Functions */
/********************/
static int parse_char_literal(IN     token_t *token,
							  IN     char    *buf,
							  IN     size_t  len,
							  IN_OUT size_t  *idx)
{
    size_t j = *idx+1;
    size_t k = 0;

    memset(tok_buf, 0, TOKEN_MAX_SIZE);
    while (buf[j] != '\'' && j+1 < len) {
        if (buf[j-1] != '\\' && buf[j]   == '\\') {
            tok_buf[k++] = char_special_lut[buf[j+1]];
            j += 2;
        }
        else {
            tok_buf[k++] = buf[j];
            ++j;
        }
    }
    memcpy(token->str, tok_buf, TOKEN_MAX_SIZE);
    token->type = TOKEN_CONSTANT;
    token->subtype = CONSTANT_CHAR;
    token->value = 0;
    *idx = j+1;

    return (ERR_OK);
}

static int parse_string_literal(IN     token_t *token,
                                IN     char    *buf,
                                IN     size_t  len,
                                IN_OUT size_t  *idx)
{
    size_t j = *idx+1;
    size_t k = 0;

    memset(tok_buf, 0, TOKEN_MAX_SIZE);
    while (buf[j] != '\"' && j+1 < len) {
        if (buf[j-1] != '\\' && buf[j]   == '\\') {
            tok_buf[k++] = char_special_lut[buf[j+1]];
            j += 2;
        }
        else {
            tok_buf[k++] = buf[j];
            ++j;
        }
    }
    memcpy(token->str, tok_buf, TOKEN_MAX_SIZE);
    token->type = TOKEN_CONSTANT;
    token->subtype = CONSTANT_STRING;
    token->value = 0;
    *idx = j+1;

    return (ERR_OK);
}

static int parse_num_literal(IN     token_t *token,
                             IN     char    *buf,
                             IN     size_t  len,
                             IN_OUT size_t  *idx)
{
    size_t j = *idx;

    memset(tok_buf, 0, TOKEN_MAX_SIZE);
    while (isdigit(buf[j]) && j < len) ++j;
    assert(j-*idx < TOKEN_MAX_SIZE-1);
    memcpy(tok_buf, &buf[*idx], j-*idx);

    memcpy(token->str, tok_buf, TOKEN_MAX_SIZE);
    token->type = TOKEN_CONSTANT;
    token->subtype = CONSTANT_INT;
    token->value = 0;
    *idx = j;

    return (ERR_OK);
}

static int parse_keyword(IN     token_t *token,
                         IN     char    *buf,
                         IN     size_t  len,
                         IN_OUT size_t  *idx)
{
    token_t tok = {0};
    token_t res_tok = {0};
    size_t j = *idx;

    memset(tok.str, 0, TOKEN_MAX_SIZE);

    while ((isalnum(buf[j]) || buf[j] == '_') && j < len) ++j;
    assert(j-*idx < TOKEN_MAX_SIZE-1);
    memcpy(tok.str, &buf[*idx], j-*idx);
    tok.type = TOKEN_KEYWORD;
    if (HashFind(base_tokens, &tok, &res_tok) == ERR_OK) {
        memcpy(token->str, res_tok.str, TOKEN_MAX_SIZE);
        token->type = res_tok.type;
        token->subtype = res_tok.subtype;
        token->value = res_tok.value;
    }
    else {
        memcpy(token->str, tok.str, TOKEN_MAX_SIZE);
        token->type = TOKEN_IDENTIFIER;
        token->subtype = 0;
        token->value = 0;
    }
    *idx = j;

    return (ERR_OK);
}

static int parse_operator(IN     token_t *token,
                          IN     char    *buf,
                          IN     size_t  len,
                          IN_OUT size_t  *idx)
{
    token_t tok = {0};
    token_t res_tok = {0};

    size_t j = 0;

    memset(tok.str, 0, TOKEN_MAX_SIZE);

    for (j = 0; j < 3; ++j) {
        tok.str[j] = buf[*idx+j];
        tok.type = TOKEN_OPERATOR;
        if (HashFind(base_tokens, &tok, &res_tok) != ERR_OK) {
            tok.type = TOKEN_SPECIAL;
            if (HashFind(base_tokens, &tok, &res_tok) != ERR_OK) {
                tok.str[j] = 0;
                break;
            }
        }
    }
    memcpy(token->str, res_tok.str, TOKEN_MAX_SIZE);
    token->type = res_tok.type;
    token->subtype = res_tok.subtype;
    token->value = res_tok.value;
    *idx += j;

    if (j == 0) {
        printf("ERROR!!!!! unrecognized operator during tokenizing, %c\n", buf[*idx+j]);
        ++*idx;
    }

    return (ERR_OK);
}

static size_t token_hash(void *item)
{
    return (HashStr(((token_t *)item)->str)%HASH_SIZE_TOKEN);
}

static int token_cmp(void *item1, void *item2)
{
    if (strncmp(((token_t *)item1)->str, ((token_t *)item2)->str, TOKEN_MAX_SIZE))
        return (1);
    if (((token_t *)item2)->type != 0)
        return (((token_t *)item1)->type != ((token_t *)item2)->type);
    return (0);
}

