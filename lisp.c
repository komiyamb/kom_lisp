/** -*- coding:utf-8 -*-
    \file
    \author komiyamb
*/

#include "lisp.h"

char banner[] = 
" XX                        XX       X\n\
  X                         X\n\
  X                         X\n\
  X  XX   XXXXX  XXX X      X     XXX     XXXXX  XXXXXX\n\
  X  X   X     X  X X X     X       X    X     X  X    X\n\
  X X    X     X  X X X     X       X     XXX     X    X\n\
  XXX    X     X  X X X     X       X        XX   X    X\n\
  X  X   X     X  X X X     X       X    X     X  X    X\n\
 XX   XX  XXXXX  XX X XX  XXXXX   XXXXX   XXXXX   XXXXX\n\
                                                  X\n\
                                                 XXX";

int main(void)
{
  char buf[256];
  lisp_object* read, evaled;

  printf(banner);
  /*  init_protect();
  init_symbol_table();
  init_memory();
  test_all();*/

  while(1){
    printf("\n> ");
    fgets(buf, 256, stdin);
    read = read_s_exp(buf);
    evaled = eval(read, env);
    write_s_exp(evaled);
  }
  return 0;
}

void test_all(void)
{
  printf("start test:\n");
  test_type();
  test_gc();
  test_io();
  printf("end test\n");
  return;
  }

