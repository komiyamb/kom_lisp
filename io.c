/** -*- coding:utf-8 -*-
    \file
    \author komiyamb
*/

#include "io.h"

/**
   トークンリストを初期化する。
*/
void init_token_list(void)
{
  token.index = 0;
  token.size = 0;
  memset(token.token, '\0', sizeof(char)*MAX_TOKEN_SIZE*MAX_SYMBOL_SIZE);
}

lisp_object* read_s_exp(char* str)
{
  init_token_list();
  separate_token(str);
  return read_token();
}

/**
   入力strをトークンに分割し、token[][]にセットする。
*/
void separate_token(char* str)
{
  int i = 0, j = 0;

  init_token_list();

  while(*str != EOF && *str != '\0'){
    while(*str == ' ' || *str == '\n'){ /**< 空白行を飛ばす */
      str++;
      j = 0;
      if(token.token[i][0] != '\0'){i++; }
    }

    if(*str == '(' ||
       *str == ')' ||
       *str == '`' ||
       *str == ',' ||
       *str == '.' ||
       *str == '\''){
     if(token.token[i][0] != '\0'){i++; }
      strncpy(token.token[i], str, 1);
      token.token[i][1] = '\0';
      str++;
      i++;
      j = 0;
    }else if(!strncmp(str, "#(", 2) ||
             !strncmp(str, ",@", 2)){
      if(token.token[i][0] != '\0'){i++; }
      strncpy(token.token[i], str, 2);
      token.token[i][2] = '\0';
      str += 2;
      i++;
      j = 0;
    }else if(*str == ';'){
      j = 0;
      i++;
      while(*str != '\n' && *str != EOF){
        str++;
      }
      if(*str == EOF){ return; }
    }else{
      token.token[i][j] = *str;
      token.token[i][j+1] = '\0';
      j++;
      str++;
    }
  }
  token.size = i;
  return;
}

/**
   呼ぶたびに次のトークンを返す。
*/
char* get_next_token(void)
{
  if(token.size <= token.index){
    return NULL;
  }
  return token.token[token.index++];
}

void unget_token(void)
{
  token.index--;
  return;
}

/**
   tokenからsexpを1つ取り出し、lisp_objectを返す。
 */
lisp_object* read_token(void)
{
  lisp_object* tmp;
  char* tp;
  int i = 0;

  while(1){
    tp = get_next_token();
    if(*tp == '('){
      return read_list();
    }else if(!strncmp(tp, "#(", 2)){
      return read_vector();
    }else if(*tp == '\''){
      tmp = create_cons();
      set_car(tmp, quote());
      set_cdr(tmp, read_token());
      return tmp;
    }else{
      for(i = 0; tp[i] != '\0'; i++){
        if(!isdigit((int)tp[i])){
          return create_symbol(tp);
        }
      }
      /**
         全て数字なら数。
       */
      return create_number(atoi(tp));
    }
  }
}

lisp_object* read_list(void)
{
  char* tmp;
  lisp_object* ret;
  while(1){
    ret = create_cons();
    set_car(ret, read_token());
    tmp = get_next_token();
    if(*tmp == ')'){
      set_cdr(ret, create_empty_list());
      return ret;
    }else if(*tmp == '.'){
      set_cdr(ret, read_token());
      return ret;
    }else{
      unget_token();
      set_cdr(ret, read_list());
      return ret;
    }
  }
}

lisp_object* read_vector(void)
{
  char* tmp;
  lisp_object* ret, *vec_list[MAX_VECTOR_SIZE];
  int i = 0;
  while(*(tmp = get_next_token()) != ')'){
    unget_token();
    vec_list[i] = read_token();
    i++;
  }
  ret = create_vector(i);
  for(i--; 0 <= i; i--){
    set_vector(ret, i, vec_list[i]);
  }
  return ret;
}

/**
   objを印字形式にして表示する。
*/
void write_s_exp(lisp_object* obj)
{
  int len, i;
  char buf[MAX_SYMBOL_SIZE];
  switch(get_type_tag(obj)){
  case TYPE_CONS:
    write_list(obj);
    return;
  case TYPE_VECTOR:
    len = get_vector_length(obj);
    printf("#(");
    for(i = 0; i < len; i++){
      write_s_exp(vector_ref(obj, i));
      printf(" ");
    }
    printf(")");
    return;
  case TYPE_SYMBOL:
  case TYPE_STRING:
  case TYPE_BOOLEAN:
    printf("%s", copy_symbol_name(buf, obj));
    return;
  case TYPE_NUMBER:
    printf("%d", get_number(obj));
    return;
  case TYPE_CHAR:
    printf("#\\%c", get_char(obj));
    return;
  case TYPE_PORT:
    printf("#<port>");
    return;
  case TYPE_SUBR:
    printf("#<subr>");
    return;
  case TYPE_FSUBR:
    printf("#<fsubr>");
    return;
  case TYPE_EXPR:
    printf("#<expr>");
    return;
  case TYPE_FEXPR:
    printf("#<fexpr>");
    return;
  case TYPE_NULL:
    printf("'()");
    return;
  default:
    fprintf(stderr, "write_s_exp:undefined type [%d]\n", get_type_tag(obj));
    return;
  }
}

void write_list(lisp_object* cons)
{
  data_type type;
  lisp_object* cdr;

  printf("(");
  write_s_exp(get_car(cons));

  while(1){
    cdr = get_cdr(cons);
    type = get_type_tag(cdr);
    switch(type){
    case TYPE_CONS:
      printf(" ");
      cons = cdr;
      write_s_exp(get_car(cdr));
      break;
    case TYPE_NULL:
      cons = cdr;
      printf(")");
      return;
    default:
      printf(" . ");
      write_s_exp(cdr);
      printf(")");
      return;
    }
  }
}
