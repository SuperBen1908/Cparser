
#ifndef __SYMBOL_H__
#define __SYMBOL_H__


/* types */
typedef struct sym_type sym_type_t;

typedef enum {
    CHAR,
    SHORT,
    INT,
    FLOAT,
    LONG,
    DOUBLE,
    VOID
} type_base_e;

typedef struct enum_field {
} enum_field_t;

typedef struct type_enum {
} type_enum_t;

typedef struct struct_field {
} struct_field_t;

typedef struct type_struct {
} type_struct_t;

typedef struct union_field {
} union_field_t;

typedef struct type_union {
} type_union_t;

typedef struct type_typedef {
    sym_type_t *value;
} type_typedef_t;

typedef struct type_pointer {
    sym_type_t *value;
} type_pointer_t;

/* general type */
typedef enum {
    BASE,
    ENUM,
    STRUCT,
    UNION,
    TYPEDEF,
    POINTER
} sym_type_e;
struct sym_type {
    union {
        type_base_e _base;
        type_enum_t _enum;
        type_struct_t _struct;
        type_union_t _union;
        type_typedef_t _typedef;
        type_pointer_t _pointer;
    } type;
    sym_type_e kind;
};



/* general symbol */
typedef struct sym {

} sym_t;

#endif // __SYMBOL_H__
