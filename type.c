/** -*- coding:utf-8 -*-
   \file
   \author komiyamb
*/

#include "type.h"

/**
   consのcarを返す。
*/
lisp_object* get_car(lisp_object* cons)
{
  return (cons->obj).cons.car;
}

/**
   consのcdrを返す。
*/
lisp_object* get_cdr(lisp_object* cons){
  return (cons->obj).cons.cdr;
}

/**
   consを生成し、返す。
*/
lisp_object* create_cons(void)
{
  lisp_object* tmp = alloc_obj(1);
  set_type_tag(tmp, TYPE_CONS);
  return tmp;
}

/**
   consのcarに新しい値objを設定する。
*/
void set_car(lisp_object* cons, lisp_object* obj)
{
  (cons->obj).cons.car = obj;
  return;
}

/**
   consのcdrに新しい値objを設定する。
*/
void set_cdr(lisp_object* cons, lisp_object* obj)
{
  (cons->obj).cons.cdr = obj;
  return;
}

/**
   空リストを返す。
*/
lisp_object* create_empty_list(void)
{
  static lisp_object empty_list;
  set_type_tag(&empty_list, TYPE_NULL);
  return &empty_list;
}

/**
   objがnilならば真。
*/
int null(lisp_object* obj)
{
  if(TYPE_NULL == get_type_tag(obj)){
    return 1;
  }
  return 0;
}

/**
   長さlenのvectorを生成し、返す。
*/
lisp_object* create_vector(int len)
{
  lisp_object* tmp = alloc_obj(len / 2+1);
  (tmp->obj).first_vector.length = len;
  set_type_tag(tmp, TYPE_VECTOR);
  return tmp;
}

/**
   vecのnum番目の値を返す。
   値は0番目から数える。
*/
lisp_object* vector_ref(lisp_object* vec, int num)
{
  if(num == 0){
    return (vec->obj).first_vector.zero;
  }else if((num % 2) != 0){
    return ((vec + (num+1) / 2)->obj).rest_vector.first;
  }else{
    return ((vec + (num+1) / 2)->obj).rest_vector.second;
  }
}

/**
   vecのlen番目に新しい値objを設定する。
*/
void set_vector(lisp_object* vec, int len, lisp_object* obj)
{
  if(len == 0){
      (vec->obj).first_vector.zero = obj;
  }else if((len % 2) != 0){
    ((vec + (len+1) / 2)->obj).rest_vector.first = obj;
  }else{
    ((vec + (len+1) / 2)->obj).rest_vector.second = obj;
  }
  return;
}

/**
   vecの長さを返す。
 */
int get_vector_length(lisp_object* vec)
{
  return (vec->obj).first_vector.length;
}

/**
   ブーリアンを生成する。
*/
lisp_object* create_boolean(int bool)
{
  lisp_object* tmp;
  tmp = create_symbol(bool ? "#t" : "#f");
  set_type_tag(tmp, TYPE_BOOLEAN);
  return tmp;
}

/**
   bool型のオブジェクトobjの真偽値を返す。
*/
int get_boolean(lisp_object* obj)
{
  return strcmpstrobj("#t", obj);
}

/**
   char*の文字列strとlisp_objectの文字列を比較する。
   一致した場合0を返す。
*/
int strcmpstrobj(char* str, lisp_object* obj)
{
  int i = 0;
  char tmp = (obj->obj).symbol[0];
  while(*str == tmp){
    if(*str == '\0'){
      return 0;
    }
    str++;
    i++;
    if(i == 8){
      obj++;
      i = 0;
    }
    tmp = (obj->obj).symbol[i];
  }
  return *str - tmp;
}

lisp_object* create_string(char* str)
{
  int i, len = strlen(str);
  lisp_object* ret= alloc_obj(len/CHAR_SIZE_OF_ONE_OBJECT + 1);
  set_type_tag(ret, TYPE_STRING);
  strncpy((ret->obj).string, str, CHAR_SIZE_OF_ONE_OBJECT);
  for(i = 1; i < len/CHAR_SIZE_OF_ONE_OBJECT + 1; i++){
    set_type_tag(ret + i, TYPE_CONTINUE);
    strncpy((ret+i)->obj.string,
            str + CHAR_SIZE_OF_ONE_OBJECT * i,
            CHAR_SIZE_OF_ONE_OBJECT);
  }
  return ret;
}

int string_length(lisp_object* obj)
{
  int len = 0;
  while(string_ref(obj, len) != '\0'){
    len++;
  }
  printf("string_length %d\n",len);
  return len;
}

void set_string(lisp_object* obj, char c, int len)
{
  ((obj + len / CHAR_SIZE_OF_ONE_OBJECT)->obj).
    string[len % CHAR_SIZE_OF_ONE_OBJECT] = c;
  return;
}

char string_ref(lisp_object* obj, int len)
{
  return ((obj + len / CHAR_SIZE_OF_ONE_OBJECT)->obj).
    string[len % CHAR_SIZE_OF_ONE_OBJECT];
}

/**
   symbol_tableを初期化する。
*/
void init_symbol_table(void)
{
  symtable.index = 0;
}

/**
   symbolを生成し、返す。
   同名のシンボルがすでにある場合、生成せず、それを返す。
*/
lisp_object* create_symbol(char* str)
{
  int i, len = strlen(str) + 1;
  
  for(i = 0; i < symtable.index; i++){
    if(!strcmpstrobj(str, symtable.table[i])){
      return symtable.table[i];
    }
  }

  symtable.table[symtable.index] = alloc_obj(len/CHAR_SIZE_OF_ONE_OBJECT+1);
  set_type_tag(symtable.table[symtable.index], TYPE_SYMBOL);
  strncpy((symtable.table[symtable.index]->obj).symbol,
          str, CHAR_SIZE_OF_ONE_OBJECT);
  for(i = 1; i < len/CHAR_SIZE_OF_ONE_OBJECT+1; i++){
    set_type_tag(symtable.table[symtable.index]+i, TYPE_CONTINUE);
    strncpy((symtable.table[symtable.index] + i)->obj.symbol,
            str + CHAR_SIZE_OF_ONE_OBJECT * i,
            CHAR_SIZE_OF_ONE_OBJECT);
  }
  symtable.index++;
  
  return symtable.table[symtable.index-1];
}

/**
   '\0'を含めてsymの印字名をdstにコピーし、dstを返す。
   \param sym コピー元のシンボル型オブジェクト
   \param dst コピー先の文字列
   \return dst コピー先の文字列
*/
char* copy_symbol_name(char* dst, lisp_object* sym)
{
  int i = 0;
  while((dst[i] = 
         ((sym + i / CHAR_SIZE_OF_ONE_OBJECT)->obj)
         .symbol[i % CHAR_SIZE_OF_ONE_OBJECT]) != '\0'){
    i++;
  }
  return dst;
}

/**
   文字cを生成する。
*/
lisp_object* create_char(char c)
{
  lisp_object* tmp = alloc_obj(1);
  set_type_tag(tmp, TYPE_CHAR);
  (tmp->obj).l_char = c;
  return tmp;
}

/**
   objの文字を返す。
*/
char get_char(lisp_object* obj)
{
  return (obj->obj).l_char;
}

/**
   ポートを生成する。
*/
lisp_object* create_port(FILE* fp, port_direction direction)
{
  lisp_object* tmp = alloc_obj(1);
  set_type_tag(tmp, TYPE_PORT);
  (tmp->obj).port.file = fp;
  (tmp->obj).port.direction = direction;
  return tmp;
}

/**
   port型のobjのファイルポインタを返す。
*/
FILE* get_port_file_pointer(lisp_object* obj)
{
  return (obj->obj).port.file;
}

/**
   port型のobjのdirectionを返す。
*/
port_direction get_port_direction(lisp_object* obj)
{
  return (obj->obj).port.direction;
}

/**
   numの値を持つオブジェクトを返す。
*/
lisp_object* create_number(int num)
{
  lisp_object* tmp = alloc_obj(1);
  set_type_tag(tmp, TYPE_NUMBER);
  (tmp->obj).number = num;
  return tmp;
}

/**
   number型のobjの数値を返す。
*/
int get_number(lisp_object* obj)
{
  return (obj->obj).number;
}


lisp_object* create_subr(lisp_object*(func)(lisp_object* args))
{
  lisp_object* ret = alloc_obj(1);
  set_type_tag(ret, TYPE_SUBR);
  (ret->obj).subr = func;
  return ret;
}

lisp_object* create_fsubr(lisp_object*(func)(lisp_object*, lisp_object*))
{
  lisp_object* ret = alloc_obj(1);
  set_type_tag(ret, TYPE_FSUBR);
  (ret->obj).fsubr = func;
  return ret;  
}

//stab
lisp_object* get_next(lisp_object* env){return env;}
lisp_object* get_symbol(lisp_object* obj){return obj;}
lisp_object* get_data(lisp_object* obj){return obj;}
//stab

/**
   objの型タグを返す。
*/
data_type get_type_tag(lisp_object* obj)
{
  return obj->type_tag;
}

/**
   objの型タグにtypeを設定する。
*/
void set_type_tag(lisp_object* obj, data_type type)
{
  obj->type_tag = type;
  return;
}

/**
   objのgcタグにflagを設定する。
 */
void set_gc_tag(lisp_object *obj, int flag)
{
  obj->gc_tag = flag;
  return;
}

/**
   objのgcタグを返す。
*/
int get_gc_tag(lisp_object *obj)
{
  return obj->gc_tag;
}
/**
   gcコピー済みのsrcに設定されたgcコピー先のアドレスを返す。
*/
lisp_object* get_copied(lisp_object* src)
{
  return (src->obj).copied;
}

/**
   srcがgcコピー済みなら1を返す。
*/
int copied_p(lisp_object* src)
{
  return src->gc_tag;
}
