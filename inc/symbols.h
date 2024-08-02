
#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include "../../DS/inc/hash.h"
#include "../../DS/inc/vector.h"

#include "../../utils/error.h"
#include "../../utils/constants.h"


/*********/
/* types */
/*********/
typedef struct type type_t;


/********/
/* base */
/********/
typedef enum {
    CHAR,
    SHORT,
    INT,
    FLOAT,
    LONG,
    DOUBLE,
    VOID
} type_base_e;


/********/
/* enum */
/********/
typedef struct type_enum {
    vector_t fields;
    size_t fields_count;
    size_t def_tok_idx;
} type_enum_t;


/**********/
/* struct */
/**********/
typedef struct type_struct {
    vector_t fields;
    size_t fields_count;
    size_t def_tok_idx;
} type_struct_t;


/*********/
/* union */
/*********/
typedef struct type_union {
    vector_t fields;
    size_t fields_count;
    size_t def_tok_idx;
} type_union_t;


/***********/
/* typedef */
/***********/
typedef struct type_typedef {
    type_t *value;
    bool_e is_static;
    bool_e is_const;
} type_typedef_t;


/***********/
/* pointer */
/***********/
typedef struct type_pointer {
    type_t *value;
    bool_e is_const;
} type_pointer_t;


/****************/
/* general type */
/****************/
typedef enum {
    BASE,
    ENUM,
    STRUCT,
    UNION,
    TYPEDEF,
    POINTER
} type_e;
struct type {
    union {
        type_base_e _base;
        type_enum_t _enum;
        type_struct_t _struct;
        type_union_t _union;
        type_typedef_t _typedef;
        type_pointer_t _pointer;
    } type;
    type_e kind;
};


/**********/
/* fields */
/**********/
typedef struct enum_field {
    char name[TOKEN_MAX_SIZE];
    int value;
    size_t def_tok_idx;
} enum_field_t;

typedef struct struct_field {
    char name[TOKEN_MAX_SIZE];
    type_t type;
    size_t def_tok_idx;
} struct_field_t;

typedef struct union_field {
    char name[TOKEN_MAX_SIZE];
    type_t type;
    size_t def_tok_idx;
} union_field_t;


/************/
/* variable */
/************/
typedef struct variable {
    type_t type;
    size_t def_tok_idx;
} variable_t;


/************/
/* function */
/************/
typedef struct function {
    type_t return_type;
    vector_t args;
    size_t args_count;
    size_t def_tok_idx;
} function_t;


/******************/
/* general symbol */
/******************/
typedef struct sym {
    char name[SYMBOL_MAX_SIZE];
    bool_e is_static;

} sym_t;


/*********/
/* scope */
/*********/
typedef struct scope {
    vector_t types;
    size_t types_count;
    vector_t functions;
    size_t functions_count;
    vector_t variables;
    size_t variables_count;
    vector_t scopes;
    size_t scopes_count;
} scope_t;


/*****************/
/* symbols table */
/*****************/
typedef struct sym_table {
    char file_name[FILENAME_MAX_SIZE];
    hash_t symbols;
    scope_t g_scope;
} sym_table_t;

#endif // __SYMBOL_H__
