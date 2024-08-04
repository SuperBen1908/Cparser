
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "inc/symbols.h"
#include "inc/preprocessor.h"
#include "inc/tokens.h"

#include "../utils/error.h"
#include "../utils/constants.h"

#define MAX_BUF_SIZE (8<<20)


void PrintFile(file_tokens_t tokens, file_defines_t defines);

int main(void)
{
    int err = ERR_OK;
    int fd = 0;
    int len = 0;
    static char buf[MAX_BUF_SIZE];
    file_defines_t defines = {0};
    file_tokens_t tokens = {0};
    sym_table_t sym_table = {0};

    if ((fd = open("file", 0)) == -1) {
        printf("ERROR!!! file didnt open\n");
        goto err;
    }
    memset(buf, 0, MAX_BUF_SIZE);
    if ((len = read(fd, buf, MAX_BUF_SIZE)) == -1) {
        printf("ERROR!!! file read err!!!\n");
        goto err;
    }

    if ((err = tokens_start())) {
        printf("ERROR!!! tokens start\n");
        goto err;
    }
    if ((err = tokens_read(buf, len, &tokens))) {
        printf("ERROR!!! preprocessor read\n");
        goto err;
    }

    if ((err = preprocessor_start(&defines))) {
        printf("ERROR!!! tokens start\n");
        goto err;
    }
    if ((err = preprocessor_read(&tokens, &defines))) {
        printf("ERROR!!! preprocessor read\n");
		goto err;
	}

    if ((err = symbols_init("file", &sym_table))) {
        printf("ERROR!!! symbol table start\n");
        goto err;
    }
    if ((err = symbols_read(tokens, defines, &sym_table))) {
        printf("ERROR!!! symbol table read\n");
		goto err;
	}

    PrintFile(tokens, defines);

    symbols_clear(sym_table);
    preprocessor_end(defines);

    if (close(fd) == -1)
        return (printf("ERROR!!! file didnt close\n"), -1);
    tokens_destroy(tokens);
    tokens_end();

    return (0);
err:
    symbols_clear(sym_table);
    preprocessor_end(defines);
    tokens_destroy(tokens);
    tokens_end();
    if (fd > 0 && close(fd) == -1) return (printf("ERROR!!! file didnt close\n"), -1);
    return (err);
}

void PrintFile(file_tokens_t tokens, file_defines_t defines)
{
    size_t i = 0;
    printf("tokens count: %ld\n", tokens.count);
    for (i = 0; *VectorGet(&tokens.vec, i, token_t)->str; ++i) {
        token_t *tok = VectorGet(&tokens.vec, i, token_t);
        printf("token %ld: [%-13s], type: %1d,%2d,%3d, row:%3ld, col:%3ld\n",
                i, tok->str, tok->type, tok->subtype, tok->value, tok->row, tok->col);
    }
    for (i = 0; i < defines.count; ++i) {
        size_t j;
        define_t key, def;
        memcpy(&key.key, VectorGet(&defines.keys, i, token_t), sizeof(token_t));
        if (HashFind(defines.map, &key, &def)) {
            printf("macro not found in destroy!!!\n");
            return ;
        }
        printf("def key: [%-5s]: ", def.key.str);
        for (j = 0; *VectorGet(&def.value, j, token_t)->str; ++j) {
            printf("%s ", VectorGet(&def.value, j, token_t)->str);
        }
        printf("\n");
        for (j = 0; j < def.arg_count; ++j) {
            token_t *tok = VectorGet(&def.args, j, token_t);
            printf("arg[%ld]: %s\n", j, tok->str);
        }
    }
}
