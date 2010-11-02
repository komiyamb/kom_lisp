#include "type.h"

lisp_object* apply(lisp_object* opecode, lisp_object* operand);
lisp_object* evls(lisp_object* arg, lisp_object* env);
lisp_object* assoc(lisp_object* obj, lisp_object* env);
void add_bind_to_env(lisp_object* env, lisp_object* sym, lisp_object* obj);
extern lisp_object* create_subr(lisp_object*(f)(lisp_object*));
extern lisp_object* create_fsubr(lisp_object*(f)(lisp_object*, lisp_object*));
extern void add_protect(lisp_object* ogj);
extern lisp_object* env;

lisp_object* eval(lisp_object* obj, lisp_object* env)
{
  data_type type = get_type_tag(obj);
  lisp_object *opecode, *ret;
  
  switch(type){
  case TYPE_BOOLEAN:
  case TYPE_NUMBER:
  case TYPE_CHAR:
  case TYPE_SUBR:
  case TYPE_FSUBR:
  case TYPE_EXPR:
  case TYPE_FEXPR:
  case TYPE_STRING:
  case TYPE_PORT:
  case TYPE_NULL:
  case TYPE_VECTOR:
    return obj;
  case TYPE_SYMBOL:
    ret = assoc(obj, env);
    if(null(ret)){
      fprintf(stderr, "eval:undefined variable\n");
      return create_empty_list();// ‚½‚Ô‚ñAtoplevel‚É–ß‚Á‚½‚Ù‚¤‚ª‚¢‚¢
    }
    return ret;
  case TYPE_CONS:
    opecode = eval(get_car(obj), env);
    type = get_type_tag(opecode);
    switch(type){
    case TYPE_SUBR:
    case TYPE_EXPR:
      return apply(opecode, evls(get_cdr(obj), env));
    case TYPE_FSUBR:
    case TYPE_FEXPR:
      return apply(opecode, get_cdr(obj));
    default:
      fprintf(stderr, "eval:not function\n");
      return create_empty_list();
    }
  default:
    fprintf(stderr, "eval:undefined type\n");
    return create_empty_list();
  }
}

lisp_object* evls(lisp_object* arg, lisp_object* env)
{
  lisp_object *op, *tmp, *ret;
  tmp = ret = create_cons();
  add_protect(ret);

  for(op = arg; !null(op); op = get_cdr(op)){
    set_cdr(tmp, create_cons());
    tmp = get_cdr(tmp);
    add_protect(tmp);
    set_car(tmp,eval(op, env));
  }
  set_cdr(tmp, create_empty_list());
  return get_cdr(ret);
}

lisp_object* apply(lisp_object* opecode, lisp_object* operand)
{
  data_type type = get_type_tag(opecode);
  switch(type){
  case TYPE_SUBR:
    return ((opecode->obj).subr)(operand);
  case TYPE_EXPR:
    //stab
  default:
    return create_empty_list();
  }
}

lisp_object* assoc(lisp_object* obj, lisp_object* env)
{
  //env must be ((dummy . dummy) (a . 1) (b . 3) ...)
  env = get_cdr(env);
  for(env = get_cdr(env); null(env); env = get_cdr(env)){
    if(get_car(get_car(env)) == obj){
      return get_car(env);
    }
  }
  return create_empty_list();
}

lisp_object* LF_car(lisp_object* obj)
{
  return get_car(get_car(obj));
}

lisp_object* LF_cdr(lisp_object* obj)
{
  return get_car(get_cdr(obj));
}

lisp_object* LF_cons(lisp_object* obj)
{
  lisp_object* cons = create_cons();
  set_car(cons, get_car(obj));
  set_cdr(cons, get_car(get_cdr(obj)));
  return cons;
}

lisp_object* LF_eq(lisp_object* obj)
{
  if(get_car(obj) == get_car(get_cdr(obj))){
    return create_boolean(1);
  }
  return create_boolean(0);
}

lisp_object* LF_atom(lisp_object* obj)
{
  if(get_type_tag(get_car(obj)) == TYPE_CONS){
    return create_boolean(0);
  }
  return create_boolean(1);
}

lisp_object* LF_quote(lisp_object* obj, lisp_object* env)
{
  return get_car(obj);
}

void create_env()
{
  //env must be ((dummy . dummy) (a . 1) (b . 3) ...)
  env = create_cons();
  set_car(env, create_cons());
  set_cdr(env, create_empty_list());

  add_bind_to_env(env, create_symbol("car"), create_subr(LF_car));
  add_bind_to_env(env, create_symbol("cdr"), create_subr(LF_cdr));
  add_bind_to_env(env, create_symbol("atom?"), create_subr(LF_cons));
  add_bind_to_env(env, create_symbol("eq?"), create_subr(LF_eq));
  add_bind_to_env(env, create_symbol("quote"), create_fsubr(LF_quote));
  return;
}

void add_bind_to_env(lisp_object* env, lisp_object* sym, lisp_object* obj)
{
  lisp_object* tmp = create_cons();
  //env must be ((dummy . dummy) (a . 1) (b . 3) ...)

  set_cdr(tmp, get_cdr(env));
  set_cdr(get_cdr(env), tmp);

  set_car(tmp, create_cons());

  set_car(get_car(tmp), sym);
  set_cdr(get_car(tmp), obj);
  return;
}
