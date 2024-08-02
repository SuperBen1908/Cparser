
#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include "../inc/preprocessor.h"
#include "../inc/tokens.h"

#include "../../DS/inc/hash.h"
#include "../../DS/inc/vector.h"

#include "../../utils/error.h"
#include "../../utils/constants.h"


#define HASH_SIZE_SYM (1024)

typedef enum { CHAR, SHORT, INT, FLOAT, LONG, DOUBLE, VOID } type_base_e;
typedef struct type_enum type_enum_t;
typedef struct type_struct type_struct_t;
typedef struct type_union type_union_t;
typedef struct type_typedef type_typedef_t;
typedef enum { BASE, ENUM, STRUCT, UNION, TYPEDEF, POINTER } type_e;
typedef struct type type_t;

typedef struct enum_field enum_field_t;
typedef struct struct_field struct_field_t;
typedef struct union_field union_field_t;

typedef struct variable variable_t;
typedef struct function function_t;

typedef enum { TYPE, ENUM_FIELD, STRUCT_FIELD,  UNION_FIELD, VARIABLE, FUNCTION } sym_kind_e;
typedef struct sym sym_t;
typedef struct scope scope_t;
typedef struct sym_table sym_table_t;


/********/
/* enum */
/********/
struct type_enum {
    vector_t fields;
    size_t fields_count;
    size_t def_tok_idx;
};


/**********/
/* struct */
/**********/
struct type_struct {
    vector_t fields;
    size_t fields_count;
    size_t def_tok_idx;
};


/*********/
/* union */
/*********/
struct type_union {
    vector_t fields;
    size_t fields_count;
    size_t def_tok_idx;
};


/***********/
/* typedef */
/***********/
struct type_typedef {
    type_t *value;
    size_t def_tok_idx;
};


/********/
/* type */
/********/
struct type {
    union {
        type_base_e _base;
        type_enum_t _enum;
        type_struct_t _struct;
        type_union_t _union;
        type_typedef_t _typedef;
    } type;
    type_e kind;
    size_t pointer_level;
    size_t constness;
};


/**********/
/* fields */
/**********/
struct enum_field {
    char name[TOKEN_MAX_SIZE];
    int value;
    size_t def_tok_idx;
};

struct struct_field {
    char name[TOKEN_MAX_SIZE];
    type_t type;
    size_t def_tok_idx;
};

struct union_field {
    char name[TOKEN_MAX_SIZE];
    type_t type;
    size_t def_tok_idx;
};


/************/
/* variable */
/************/
struct variable {
    sym_t *sym;
    type_t type;
    size_t def_tok_idx;
};


/************/
/* function */
/************/
struct function {
    sym_t *sym;
    type_t return_type;
    vector_t args;
    size_t args_count;
    size_t def_tok_idx;
};


/**********/
/* symbol */
/**********/
struct sym {
    char name[SYMBOL_MAX_SIZE];
    union {
        type_t type;
        enum_field_t enum_field;
        struct_field_t struct_field;
        union_field_t union_field;
        variable_t variable;
        function_t function;
    } value;
    sym_kind_e kind;
    bool_e is_static;
};


/*********/
/* scope */
/*********/
struct scope {
    vector_t types;
    size_t types_count;
    vector_t functions;
    size_t functions_count;
    vector_t variables;
    size_t variables_count;
    vector_t scopes;
    size_t scopes_count;
};


/*****************/
/* symbols table */
/*****************/
struct sym_table {
    char file_name[FILENAME_MAX_SIZE];
    hash_t symbols;
    scope_t g_scope;
};


int symbols_start(IN const char *file_name,
                  OUT sym_table_t *symbols);
void symbols_end(IN sym_table_t symbols);
int symbols_read(IN file_tokens_t tokens,
                 IN file_defines_t defines,
                 OUT sym_table_t *symbols);

#endif // __SYMBOL_H__
