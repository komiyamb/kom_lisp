/** -*- coding:utf-8 -*-
    \file
    \author komiyamb
*/

#include <stdio.h>
#include "type.h"
#include "gc.h"
#include "io.h"


void test_gc(void)
{
  lisp_object *mc, *test1, *test2;
  mc = create_symbol("memorycompaction");
  test1 = create_symbol("test");
  test2 = create_symbol("test");
  printf("start test gc:\n");
  printf("create_symbol()...%s\n",ok_ng(mc != test1));
  printf("create_symbol()...%s\n",ok_ng(mc != test2));
  printf("create_symbol()...%s\n",ok_ng(test2 == test1));
  printf("strcmpstrobj()...%s\n",
         ok_ng(!strcmpstrobj("test", test1) &&
               !strcmpstrobj("memorycompaction", mc) &&
               strcmpstrobj("memoryconmactio", mc)));
  env = create_cons();
  set_car(env, create_cons());
  set_car(get_car(env), test1);
  set_cdr(get_car(env), create_empty_list());
  set_cdr(env, create_empty_list());

  gc();
  printf("gc()...%s\n",
         ok_ng(get_car(env) != test1));
  printf("gc()...%s\n",
         ok_ng(!strcmpstrobj("test", get_car(get_car(env)))));
  //*/
  printf("memory %.3f%%\n", 100 * using_memory());

  printf("end test gc:\n\n");
  return;
}

void test_type(void)
{
  lisp_object *cons, *sym1, *sym2, *vec , *num, *lcha, *str, *prt, *nil;
  FILE* fp;
  char buf[MAX_SYMBOL_SIZE];

  printf("start test type:\n");
  printf("strlen(\"test\")=%d\n",strlen("test"));
  cons = create_cons();
  sym1 = create_symbol("test1");
  sym2 = create_symbol("longsymbolname");
  set_car(cons, sym1);
  set_cdr(cons, sym2);

  printf("create_symbol(),get_type_tag()...%s\n",
         ok_ng(get_type_tag(sym1) == TYPE_SYMBOL));
  printf("strcmpstrobj()...%s\n",
         ok_ng(!strcmpstrobj("test1", sym1) && 
               !strcmpstrobj("longsymbolname", sym2)));
  printf("copy_symbol_name()...%s\n",
         ok_ng(!strcmp("test1", copy_symbol_name(buf, sym1))));
  lcha = create_char('g');

  printf("create_char()...%s\n",
         ok_ng(get_type_tag(lcha) == TYPE_CHAR));
  printf("get_char()...%s\n",
         ok_ng('g' == get_char(lcha)));

  printf("create_cons(),get_type_tag()...%s\n",
         ok_ng(get_type_tag(cons) == TYPE_CONS)); 
  printf("set_car(),get_car()...%s\n",
         ok_ng(sym1 == get_car(cons)));
  printf("set_cdr(),get_cdr()...%s\n",
         ok_ng((sym2 == get_cdr(cons))));

  printf("create_vector(),get_type_tag()...%s\n",
         ok_ng(get_type_tag(vec = create_vector(8)) == TYPE_VECTOR));
  printf("get_vector_length()...%s\n",
         ok_ng(8 == get_vector_length(vec)));
  set_vector(vec, 3, sym1);
  printf("set_vector(),vector_ref()..%s\n",
         ok_ng(sym1 == vector_ref(vec, 3)));
  num = create_number(999);

  printf("create_number(),get_type_tag()...%s\n",
         ok_ng(get_type_tag(num) == TYPE_NUMBER));
  printf("get_number()...%s\n",
         ok_ng(999 == get_number(num)));

  str = create_string("string");
  printf("create_boolean(),get_type_tag()...%s\n",
         ok_ng(get_type_tag(str) == TYPE_STRING));
  printf("string_length()...%s\n",
         ok_ng(string_length(str) == 6));
  set_string(str, 'j', 4);
  printf("set_string(),string_ref()...%s\n",
         ok_ng( 'j' == string_ref(str, 4)));

  if(NULL == (fp = fopen("test.txt", "w"))){
    fprintf(stderr, "can't open file:test.txt\n");
    exit(1);
  }
  prt = create_port(fp, WRITE);
  printf("create_port()...%s\n",
         ok_ng(get_type_tag(prt) == TYPE_PORT));
  printf("get_port_file_pointer()...%s\n",
         ok_ng(fp == get_port_file_pointer(prt)));
  printf("get_port_direction()...%s\n",
         ok_ng(WRITE == get_port_direction(prt)));

  nil = create_empty_list();
  printf("null()...%s\n",
         ok_ng(null(nil)));
  printf("null()...%s\n",
         ok_ng(!null(str)));
  printf("end test type:\n\n");
  return;
}

void test_io(void)
{
  char buf[] = "(123 100 500 symbol)", b[MAX_SYMBOL_SIZE];
  lisp_object *cons, *vec, *list, *tmp;
  int num;

  printf("start test io\n");
  
  init_token_list();
  printf("init_token_list()...%s\n",
         ok_ng(token.index == 0 && token.size == 0));

  printf("get_next_token()...%s\n",
         ok_ng(NULL == get_next_token()));

    separate_token(buf);
    printf("separate()...%s\n",
           ok_ng('(' == token.token[0][0]));
    printf("separate()...%s,%s\n",
           token.token[1],
           ok_ng(!strcmp("123",token.token[1])));
    printf("separate()...%s,%s\n",
           token.token[2],
           ok_ng(!strcmp("100", token.token[2])));
    printf("separate()...%s,%s\n",
           token.token[3],
           ok_ng(!strcmp("500", token.token[3])));
    printf("separate()...%s,%s\n",
           token.token[4],
           ok_ng(!strcmp("symbol", token.token[4])));
    printf("separate()...%s\n",
           ok_ng(!strcmp(")", token.token[5])));

    list = read_token();
    num = get_number(get_car(list));
    printf("read_s_exp()...%d,%s\n",
           num, ok_ng(num == 123));
    printf("read_s_exp()...%s\n",
           ok_ng(get_number(get_car(get_cdr(list))) == 100));
    tmp = get_car(get_cdr(get_cdr(list)));
    printf("read_s_exp()...%d,%s\n",
           get_number(tmp),
           ok_ng(get_number(tmp) == 500));
    tmp = get_car(get_cdr(get_cdr(get_cdr(list))));
    b[0] = 'g'; b[1] = 'g'; b[2] = 'g'; b[3] = 'g';
    printf("read_s_exp()...%s,%s\n",
           copy_symbol_name(b, tmp),
           ok_ng(!strcmpstrobj("symbol",tmp)));

    printf("write_s_exp()...");
    write_s_exp(list);
    printf("\n");
    
    vec = create_vector(2);
    set_vector(vec, 0, create_symbol("sym0"));
    set_vector(vec, 1, create_empty_list());
        //      set_vector(vec, 2, create_string("str2"));*/
    printf("write_s_exp()...");
    printf("vector_length=%d,",get_vector_length(vec));
    write_s_exp(vec);
    printf("\n");
    
    cons = create_cons();
    set_car(cons, create_number(100));
    set_cdr(cons, create_empty_list());
    printf("write_s_exp()...");
    write_s_exp(cons);
    printf("\n");
    
    printf("end test io\n\n");
    return;
}
