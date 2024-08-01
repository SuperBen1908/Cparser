
#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <stddef.h>

#include "../../DS/inc/vector.h"

#include "../../utils/error.h"

#define TOKEN_MAX_SIZE (128)

/***************/
/* ENUMS START */
/***************/

/*********/
/* TYPES */
/*********/
typedef enum {
    TOKEN_KEYWORD = 1,
    TOKEN_SPECIAL,
    TOKEN_OPERATOR,
    TOKEN_CONSTANT,
    TOKEN_IDENTIFIER,

    TOKEN_TYPE_COUNT = 5
} token_type_e;



/************/
/* SUBTYPES */
/************/
/* KEYWORDS */
typedef enum {
    KEYWORD_BASETYPE = 11,
    KEYWORD_SIGN,
    KEYWORD_TYPE,
    KEYWORD_CONTROL_1_FLOW,
    KEYWORD_CONTROL_2_FLOW,
    KEYWORD_STORAGE_CLASS,
    KEYWORD_CONSTNESS,
    KEYWORD_FUNCTION,
    KEYWORD_PREPROCESSOR,

    KEYWORD_COUNT = 9
} token_keyword_subtype_e;


/* SPECIAL SYMBOLS */
typedef enum {
    SPECIAL_PARENTHESES = 21,
    SPECIAL_BRACE,
    SPECIAL_BRACKET,
    SPECIAL_SEPERATOR,
    SPECIAL_MEMORY,
    SPECIAL_PREPROCESSOR,

    SPECIAL_COUNT = 6
} token_special_subtype_e;


/* OPERATORS */
typedef enum {
    OPERATOR_ARITHMETIC = 31,
    OPERATOR_RELATIONAL,
    OPERATOR_LOGICAL,
    OPERATOR_BITWISE,
    OPERATOR_ASSIGN_1,
    OPERATOR_ASSIGN_2,
    OPERATOR_UNARY,
    OPERATOR_TRINARY,

    OPERATOR_COUNT = 8
} token_operator_subtype_e;


/* CONSTANTS */
typedef enum {
    CONSTANT_STRING = 41,
    CONSTANT_CHAR,
    CONSTANT_FLOAT,
    CONSTANT_INT,

    CONSTANT_COUNT = 4
} token_constant_subtype_e;



/***********/
/* ENTRIES */
/***********/
/* KEYWORDS */
typedef enum {
    KEYWORD_CHAR = 101,
    KEYWORD_SHORT,
    KEYWORD_INT,
    KEYWORD_LONG,
    KEYWORD_FLOAT,
    KEYWORD_DOUBLE,
    KEYWORD_VOID,

    KEYWORD_BASETYPE_COUNT = 7
} token_keyword_basetype_e;

typedef enum {
    KEYWORD_SIGNED = 111,
    KEYWORD_UNSIGNED,

    KEYWORD_SIGN_COUNT = 2
} token_keyword_sign_e;

typedef enum {
    KEYWORD_STRUCT = 121,
    KEYWORD_UNION,
    KEYWORD_ENUM,
    KEYWORD_TYPEDEF,

    KEYWORD_TYPE_COUNT = 4
} token_keyword_type_e;

typedef enum {
    KEYWORD_IF = 131,
    KEYWORD_ELSE,
    KEYWORD_FOR,
    KEYWORD_WHILE,
    KEYWORD_DO,
    KEYWORD_SWITCH,

    KEYWORD_CONTROL_1_COUNT = 6
} token_keyword_control_1_e;

typedef enum {
    KEYWORD_CONTINUE = 141,
    KEYWORD_BREAK,
    KEYWORD_CASE,
    KEYWORD_DEFAULT,
    KEYWORD_GOTO,
    KEYWORD_RETURN,

    KEYWORD_CONTROL_2_COUNT = 6
} token_keyword_control_2_e;

typedef enum {
    KEYWORD_AUTO = 151,
    KEYWORD_REGISTER,
    KEYWORD_VOLATILE,
    KEYWORD_STATIC,
    KEYWORD_EXTERN,

    KEYWORD_STORAGE_COUNT = 5
} token_keyword_storage_e;

typedef enum {
    KEYWORD_CONST = 161,

    KEYWORD_CONSTNESS_COUNT = 1
} token_keyword_constness_e;

typedef enum {
    KEYWORD_SIZEOF = 171,

    KEYWORD_FUNCTION_COUNT = 1
} token_keyword_function_e;

typedef enum {
    KEYWORD_DEFINE = 181,
    KEYWORD_UNDEF,
    KEYWORD_INCLUDE,
    KEYWORD_PRAGMA,
    KEYWORD_LINE,
    KEYWORD_FILE,
    KEYWORD_ERROR,
    KEYWORD_IFDEF,
    KEYWORD_IFNDEF,
    KEYWORD_PP_IF,
    KEYWORD_PP_ELIF,
    KEYWORD_PP_ELSE,
    KEYWORD_PP_ENDIF,

    KEYWORD_PREPROCESSOR_COUNT = 13
} token_keyword_preprocessor_e;

/*******************/
/* SPECIAL SYMBOLS */
/*******************/
typedef enum {
    SPECIAL_O_PARENTHESIS = 201,
    SPECIAL_C_PARENTHESIS,

    SPECIAL_PARENTHESES_COUNT = 2
} token_special_parentheses_e;

typedef enum {
    SPECIAL_O_BRACE = 211,
    SPECIAL_C_BRACE,

    SPECIAL_BRACES_COUNT = 2
} token_special_brace_e;

typedef enum {
    SPECIAL_O_BRACKET = 221,
    SPECIAL_C_BRACKET,

    SPECIAL_BRACKETS_COUNT = 2
} token_special_bracket_e;

typedef enum {
    SPECIAL_COMMA = 231,
    SPECIAL_COLON,
    SPECIAL_SEMICOLON,

    SPECIAL_SEPERATOR_COUNT = 3
} token_special_seperator_e;

typedef enum {
    SPECIAL_DOT = 241,
    SPECIAL_ARROW,

    SPECIAL_MEMORY_COUNT = 2
} token_special_memory_e;

typedef enum {
    SPECIAL_HASH = 251,
    SPECIAL_BACKSLASH,

    SPECIAL_PREPROCESSOR_COUNT = 2
} token_special_preprocessor_e;


/*************/
/* OPERATORS */
/*************/
typedef enum {
    OPERATOR_PLUS = 301,
    OPERATOR_MINUS,
    OPERATOR_MUL,
    OPERATOR_DIV,
    OPERATOR_MOD,

    OPERATOR_ARITHMETIC_COUNT
} token_operator_arithmetic_e;

typedef enum {
    OPERATOR_EQ = 311,
    OPERATOR_LESS,
    OPERATOR_GREATER,
    OPERATOR_LEQ,
    OPERATOR_GEQ,
    OPERATOR_NOT_EQ,

    OPERATOR_RELATIONAL_COUNT = 6
} token_operator_relational_e;

typedef enum {
    OPERATOR_LOGICAL_NOT = 321,
    OPERATOR_LOGICAL_OR,
    OPERATOR_LOGICAL_AND,

    OPERATOR_LOGICAL_COUNT = 3
} token_operator_logical_e;

typedef enum {
    OPERATOR_BITWISE_NOT = 331,
    OPERATOR_BITWISE_OR,
    OPERATOR_BITWISE_AND,
    OPERATOR_BITWISE_XOR,
    OPERATOR_BITWISE_RSHIFT,
    OPERATOR_BITWISE_LSHIFT,

    OPERATOR_BITWISE_COUNT = 6
} token_operator_bitwise_e;

typedef enum {
    OPERATOR_ASSIGN = 341,
    OPERATOR_ASSIGN_PLUS,
    OPERATOR_ASSIGN_MINUS,
    OPERATOR_ASSIGN_MUL,
    OPERATOR_ASSIGN_DIV,
    OPERATOR_ASSIGN_MOD,

    OPERATOR_ASSIGN_1_COUNT = 6
} token_operator_assign_1_e;

typedef enum {
    OPERATOR_ASSIGN_OR = 351,
    OPERATOR_ASSIGN_AND,
    OPERATOR_ASSIGN_XOR,
    OPERATOR_ASSIGN_LSHIFT,
    OPERATOR_ASSIGN_RSHIFT,

    OPERATOR_ASSIGN_2_COUNT = 5
} token_operator_assign_2_e;

typedef enum {
    OPERATOR_INCREMENT = 361,
    OPERATOR_DECREMENT,

    OPERATOR_UNARY_COUNT = 2
} token_operator_unary_e;

typedef enum {
    OPERATOR_QUESTION_MARK = 371,

    OPERATOR_TRINARY_COUNT = 1
} token_operator_trinary_e;


/*************/
/* ENUMS END */
/*************/


#ifdef TOKENIZER

/* KEYWORDS */
const char *const tokens_keyword_basetype[KEYWORD_BASETYPE_COUNT+1] = {
    "char", "short", "int", "long", "float", "double", "void" };
const char *const tokens_keyword_sign[KEYWORD_SIGN_COUNT+1] = {
    "signed", "unsigned" };
const char *const tokens_keyword_type[KEYWORD_TYPE_COUNT+1] = {
    "struct", "union", "enum", "typedef" };
const char *const tokens_keyword_control_1[KEYWORD_CONTROL_1_COUNT+1] = {
    "if", "else", "for", "while", "do", "switch"};
const char *const tokens_keyword_control_2[KEYWORD_CONTROL_2_COUNT+1] = {
    "continue", "break", "case", "default", "goto", "return" };
const char *const tokens_keyword_storage[KEYWORD_STORAGE_COUNT+1] = {
    "auto", "register", "volatile", "static", "extern" };
const char *const tokens_keyword_constness[KEYWORD_CONSTNESS_COUNT+1] = {
    "const" };
const char *const tokens_keyword_function[KEYWORD_FUNCTION_COUNT+1] = {
    "sizeof" };
const char *const tokens_keyword_preprocessor[KEYWORD_PREPROCESSOR_COUNT+1] = {
    "define", "undef", "include", "pragma", "line", "file", "error",
    "ifdef", "ifndef", "if", "elif", "else", "endif" };

/* SPECIAL */
const char *const tokens_special_parentheses[SPECIAL_PARENTHESES_COUNT+1] = { "(", ")" };
const char *const tokens_special_brace[SPECIAL_BRACES_COUNT+1] = { "{", "}" };
const char *const tokens_special_bracket[SPECIAL_BRACKETS_COUNT+1] = { "[", "]" };
const char *const tokens_special_seperator[SPECIAL_SEPERATOR_COUNT+1] = { ",", ":", ";" };
const char *const tokens_special_memory[SPECIAL_MEMORY_COUNT+1] = { ".", "->" };
const char *const tokens_special_preprocessor[SPECIAL_PREPROCESSOR_COUNT+1] = { "#", "\\" };

/* OPERATORS */
const char *const tokens_operator_arithmetic[OPERATOR_ARITHMETIC_COUNT+1] = {
    "+", "-", "*", "/", "%" };
const char *const tokens_operator_relational[OPERATOR_RELATIONAL_COUNT+1] = {
    "==", "<", ">", "<=", ">=", "!=" };
const char *const tokens_operator_logical[OPERATOR_LOGICAL_COUNT+1] = {
    "!", "||", "&&" };
const char *const tokens_operator_bitwise[OPERATOR_BITWISE_COUNT+1] = {
    "~", "|", "&", "^", ">>", "<<" };
const char *const tokens_operator_assign_1[OPERATOR_ASSIGN_1_COUNT+1] = {
    "=", "+=", "-=", "*=", "/=", "%=" };
const char *const tokens_operator_assign_2[OPERATOR_ASSIGN_2_COUNT+1] = {
    "|=", "&=", "^=", ">>=", "<<=" };
const char *const tokens_operator_unary[OPERATOR_UNARY_COUNT+1] = {
    "++", "--" };
const char *const tokens_operator_trinary[OPERATOR_TRINARY_COUNT+1] = {
    "?" };

const char *const *const tokens_str[3][10] = { {
    tokens_keyword_basetype,    tokens_keyword_sign,    tokens_keyword_type,
    tokens_keyword_control_1,   tokens_keyword_control_2,
    tokens_keyword_storage,     tokens_keyword_constness,
    tokens_keyword_function,    tokens_keyword_preprocessor
    }, {
    tokens_special_parentheses, tokens_special_brace,
    tokens_special_bracket,     tokens_special_seperator,
    tokens_special_memory,      tokens_special_preprocessor,
    }, {
    tokens_operator_arithmetic, tokens_operator_relational,
    tokens_operator_logical,    tokens_operator_bitwise,
    tokens_operator_assign_1,   tokens_operator_assign_2,
    tokens_operator_unary,      tokens_operator_trinary,
    } };

#endif  /* #ifdef TOKENIZER */


/*******/
/* API */
/*******/
typedef struct {
    vector_t vec;
    size_t count;
} file_tokens_t;

typedef struct {
    char str[TOKEN_MAX_SIZE];
    size_t row;
    size_t col;
    int value;
    int subtype;
    int type;
} token_t;

int tokens_start(void);
void tokens_end(void);

int tokens_read(IN  char *buf, IN  size_t len,
                OUT file_tokens_t *tokens);
void tokens_destroy(IN file_tokens_t tokens);

#endif // __TOKEN_H__
