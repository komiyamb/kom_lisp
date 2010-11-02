/** -*- coding:utf-8 -*-
   \file
   \author komiyamb
*/

#include "gc.h"

/** 
   \brief num個のlisp_object領域を確保して最初の場所を返す。
   
   確保できない場合、gc()を呼んでから確保する。
   \param[in] num 確保する領域の数
   \return 確保した最初の領域

 */
lisp_object* alloc_obj(int num)
{
  if(MEMORY_SIZE - mem.free_point < num){
    gc();
    if(MEMORY_SIZE - mem.free_point < num){
      fprintf(stderr, "memory over\n");
      exit(1);
    }
  }

  mem.free_point += num;
  return mem.mem + mem.offset + mem.free_point - num;
}

/**
   メモリを初期化する。
*/
void init_memory()
{
  mem.free_point = 0;
  mem.offset = top_offset;
  return;
}

/**
   メモリの使用場所を反転する。
*/
void flip_offset()
{
  mem.free_point = 0;
  mem.offset = ((top_offset == mem.offset) ? bottom_offset : top_offset);
  return;
}

/**
   \def メモリの使用率を返す。
*/
double using_memory()
{
  return (double)(mem.free_point % MEMORY_SIZE) / MEMORY_SIZE;
}

/**
   porotectを空にする。
 */
void init_protect(void)
{
  protect.index = 0;
  return;
}

/**
   \param obj gcから保護するオブジェクト
   \brief gcから保護するオブジェクトを登録する。
   登録できない場合、強制終了する。
*/
void add_protect(lisp_object* obj)
{
  if(protect.index >= STACK_SIZE){
    fprintf(stderr, "stack over flow\ncan't protect obj[%p]\n", obj);
    exit(1);
  }
  protect.stack[protect.index] = obj;
  protect.index++;
  return;
}

/**
   保護リストの末尾のオブジェクト１つを保護から解除する。
*/
void remove_protect(void)
{
  protect.index--;
  return;
}

/**
   ガーベジコレクションを行う。
   コンパクションしながら、
   envとprotectから辿れるオブジェクトを回収する。
 */
void gc(void)
{
  int i;

  init_gc_tag();//現在の領域のgcタグを初期化
  init_symbol_table();
  
  flip_offset();//次からのalloc_objでは新しい領域を使用
  
  env = copy_obj(env);
  
  for(i = 0; i < protect.index; i++){
    protect.stack[i] = copy_obj(protect.stack[i]);
  }
  
  return;
}

/**
   現在使用中側のメモリの
   gcタグを初期化する。
*/
void init_gc_tag(void)
{
  int i;
  for(i = 0; i <= MEMORY_SIZE; i++){
    set_gc_tag(mem.mem + mem.offset + i, 0);
  }
  return;
}

/**
   \param[in] src コピー済みマークをつけるオブジェクト
   \param[in] copied コピー先のアドレス
   \brief オブジェクトをコピー済みにして、
   コピー先のアドレスを設定する。
*/
void set_copied(lisp_object* src, lisp_object* copied)
{
  set_gc_tag(src, 1);
  (src->obj).copied = copied;
  return;
}

/**
   \param[in] obj 調べるオブジェクト
   \breif オブジェクトがコピー後の領域にあるかどうかを調べる。

   新しい領域なら偽。
 */
int old_p(lisp_object* obj)
{
  if(mem.offset == bottom_offset){
    return obj < mem.mem + bottom_offset;
  }
  return obj > mem.mem + bottom_offset;
}

/**
   オブジェクトsrcをgcコピーする。
*/
lisp_object* copy_obj(lisp_object* src)
{
  lisp_object *tmp, *new;
  int i, len;
  char buf[MAX_SYMBOL_SIZE];

  if(NULL == src){
    return src;
  }

  switch(get_type_tag(src)){
  case TYPE_CONS:
    new = create_cons();
    tmp = get_car(src);
    set_car(new, tmp);
    set_cdr(new, get_cdr(src));
    set_copied(src, new);
    if(old_p(tmp)){
      set_car(new, get_gc_tag(tmp) ? get_copied(tmp) : copy_obj(tmp));
    }
    tmp = get_cdr(new);
    if(old_p(tmp)){
      set_cdr(new, get_gc_tag(tmp) ? get_copied(tmp) : copy_obj(tmp));
    }
    return new;
  case TYPE_VECTOR:
    len = get_vector_length(src);
    new = create_vector(len);
    for(i = 0; i < len; i++){
      set_vector(new, i, vector_ref(src, i));
    }
    set_copied(src, new);
    for(i = 0; i < len; i++){
      tmp = vector_ref(new, i);
      if(old_p(tmp)){
        set_vector(new, i, get_gc_tag(tmp) ? get_copied(tmp) : copy_obj(tmp));
      }
    }
    return new;
  case TYPE_BOOLEAN:
    new = create_boolean(get_boolean(src));
    set_copied(src, new);
    return new;
  case TYPE_CHAR:
    new = create_char(get_char(src));
    set_copied(src, new);
    return new;
  case TYPE_SYMBOL:
    new = create_symbol(copy_symbol_name(buf, src));
    set_copied(src, new);
    return new;
  case TYPE_STRING:
    new = create_string(copy_symbol_name(buf, src));
    set_copied(src, new);
    return new;
  case TYPE_NUMBER:
    new = create_number(get_number(src));
    set_copied(src, new);
    return new;
  case TYPE_NULL:
    return src;
    /*  case TYPE_SUBR:
    return new;
  case TYPE_FSUBR:
    return new;
  case TYPE_EXPR:
    return new;
  case TYPE_FEXPR:
    return new:
  case TYPE_PORT:
  return new;*/
  default:
    fprintf(stderr, "copy_obj:undefined date type [%d]\n", get_type_tag(src));
    return NULL;
  }
}
