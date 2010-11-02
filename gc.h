/** -*- coding:utf-8 -*-
   \file
   \author komiyamb
*/

#ifndef _KOM_GC_
#define _KOM_GC_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "type.h"

#define MEMORY_SIZE (1 << 20)/**< \def メモリサイズ */
#define top_offset 0
#define bottom_offset MEMORY_SIZE
#define STACK_SIZE 256

typedef struct memory{
  lisp_object mem[MEMORY_SIZE*2];
  /**
     memoryの使用領域の開始位置。
     top_offsetもしくはbottom_offset。
   */
  int offset;
  int free_point; /**< offsetからの未使用領域の開始位置。 */
}memory;

/**
   実行時のオブジェクトを配置するメモリ
*/
memory mem;

typedef struct{
  lisp_object* stack[STACK_SIZE];
  int index;
} protect_stack;
/**
   生成途中で、
   envに登録されていないが回収されると困るオブジェクトを格納するリスト。
*/
protect_stack protect;
lisp_object* env;/**< 実行時の環境。*/

void test_gc(void);

void remove_protect(void);
void add_protect(lisp_object* obj);
void init_protect(void);

void gc(void);
lisp_object* alloc_obj(int num);
void init_memory(void);
double using_memory(void);
void flip_offset(void);
void init_gc_tag(void);
void set_copied(lisp_object* src, lisp_object* copied);

lisp_object* copy_obj(lisp_object* src);

#endif /* _KOM_GC_ */
