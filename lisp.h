/** -*- coding:utf-8 -*-
    \file
    \author komiyamb
*/

#ifndef _KOM_MAIN_
#define _KOM_MAIN_

#include <stdio.h>
#include "type.h"

extern lisp_object* eval(lisp_object*, lisp_object*);
extern lisp_object* env;

extern void init_symbol_table(void);
extern void init_memory(void);
extern void init_protect(void);

extern void write_s_exp(lisp_object* sexp);
extern lisp_object* read_s_exp(char* ch);

extern void test_type(void);
extern void test_gc(void);
extern void test_io(void);
void test_all(void);

#endif /* _KOM_MAIN_ */
