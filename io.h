/** -*- coding:utf-8 -*-
   \file
   \author komiyamb
*/

#ifndef _KOM_IO_
#define _KOM_IO_

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "type.h"

typedef struct token_list{
  char token[MAX_TOKEN_SIZE][MAX_SYMBOL_SIZE];
  int index;
  int size;
} token_list;

/**
   トークンのリストを保持する場所。
*/
token_list token;
//char separater[] = " \n()\"";

extern lisp_object* quote(void);

void test_io(void);

void init_token_list(void);
void separate_token(char* str);
char* get_next_token(void);
void unget_token(void);

lisp_object* read_token(void);
lisp_object* read_s_exp(char*);
lisp_object* read_list(void);
lisp_object* read_vector(void);
lisp_object* quote(void);
lisp_object* quasi_quote(void);

void write_s_exp(lisp_object* obj);
void write_list(lisp_object* cons);

#endif /* _KOM_IO_ */
