
/** -*- coding:utf-8 -*-
   \file
   \author komiyamb
*/

#ifndef _KOM_TYPE_
#define _KOM_TYPE_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum lisp_object_data_type{
  TYPE_BOOLEAN,
  TYPE_SYMBOL,
  TYPE_CHAR,
  TYPE_VECTOR,
  TYPE_SUBR,TYPE_FSUBR,TYPE_EXPR,TYPE_FEXPR,
  TYPE_CONS,
  TYPE_NUMBER,
  TYPE_STRING,
  TYPE_PORT,
  TYPE_NULL,
  TYPE_CONTINUE,//< continue of symbol or string or vector
}data_type;

typedef enum lisp_object_port_direction{
  READ,
  WRITE,
}port_direction;

typedef struct cons{
  struct lisp_object* car;
  struct lisp_object* cdr;
}cons;

struct first_vector{
  int length;
  struct lisp_object* zero;
};

struct rest_vector{
  struct lisp_object* first;
  struct lisp_object* second;
};

typedef struct port{
  FILE* file;
  int direction;
}port;

#define CHAR_SIZE_OF_ONE_OBJECT 8
#define STRING_BUF_SIZE 512
#define MAX_TOKEN_SIZE 1024
#define MAX_SYMBOL_SIZE 256
#define MAX_VECTOR_SIZE 256

typedef union lisp_object_body{
  cons cons;
  cons expr;
  cons fexpr;
  struct first_vector first_vector;
  struct rest_vector rest_vector;
  struct lisp_object* (*subr)(struct lisp_object*);
  struct lisp_object* (*fsubr)(struct lisp_object*, struct lisp_object*);
  struct lisp_object* copied;
  port port;
  int number;
  char symbol[CHAR_SIZE_OF_ONE_OBJECT];
  char cont_symbol[CHAR_SIZE_OF_ONE_OBJECT];
  char string[CHAR_SIZE_OF_ONE_OBJECT];
  char cont_string[CHAR_SIZE_OF_ONE_OBJECT];
  int boolean;
  char l_char;
}lisp_object_body;

typedef struct lisp_object{
  lisp_object_body obj;
  int type_tag:31;
  int gc_tag:1;
  //  int protect:1;
}lisp_object;

#define MAX_SYMBOL 1000//00

typedef struct symbol_table{
  lisp_object* table[MAX_SYMBOL];
  int index;//< symbol_tableに保存されているsymbolの数。
}symbol_table;

/**
   symbolの同一性を保つための表。
*/
symbol_table symtable;
extern struct memory mem;
extern lisp_object* alloc_obj(int n);

void test_type(void);
#define ok_ng(bool) ((bool)?"OK":"NG")

lisp_object* create_cons(void);
lisp_object* get_car(lisp_object* cons);
lisp_object* get_cdr(lisp_object* cons);
void set_car(lisp_object* cons, lisp_object* obj);
void set_cdr(lisp_object* cons, lisp_object* obj);

lisp_object* create_empty_list(void);

lisp_object* create_vector(int len);
lisp_object* vector_ref(lisp_object* vec, int num);
void set_vector(lisp_object* vec, int len, lisp_object* obj);
int get_vector_length(lisp_object* vec);

lisp_object* create_boolean(int bool);
int get_boolean(lisp_object* bool);

int null(lisp_object* obj);

lisp_object* create_symbol(char* str);
int strcmpstrobj(char* str, lisp_object* obj);
void init_symbol_table(void);
char* copy_symbol_name(char* dast, lisp_object* symbol);

lisp_object* create_char(char c);
char get_char(lisp_object* obj);

lisp_object* create_port(FILE* fp, port_direction dir);
FILE* get_port_file_pointer(lisp_object* obj);
port_direction get_port_direction(lisp_object* obj);

lisp_object* create_number(int num);
int get_number(lisp_object* obj);

lisp_object* create_string(char* str);
int string_length(lisp_object* obj);
void set_string(lisp_object* obj, char c, int len);
char string_ref(lisp_object* obj, int len);

lisp_object* create_subr(lisp_object*(func)(lisp_object* args));
lisp_object* create_fsubr(lisp_object*(funct)(lisp_object* args, lisp_object* env));

data_type get_type_tag(lisp_object* obj);
void set_type_tag(lisp_object* obj, data_type type);
int get_gc_tag(lisp_object *obj);
void set_gc_tag(lisp_object *obj, int flag);

lisp_object* get_copied(lisp_object* src);
int copied_p(lisp_object* src);
lisp_object* copied_to(lisp_object* src);

#endif /* _KOM_TYPE_ */
