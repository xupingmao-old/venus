/*
 * object.h
 *
 *  Created on: 2014Äê8ÔÂ25ÈÕ
 *      Author: Xu
 */

#ifndef OBJECT_H_
#define OBJECT_H_

tm_obj str_new(char* s, int size);
tm_obj list_new(int cap);
tm_list* _list_new(int cap);

void list_append( tm_list* list, tm_obj v);
void list_set( tm_list* list, int n, tm_obj v);

tm_obj tm_eval(tm_obj fnc, tm_obj params);

// stream
tm_obj _load(char* fname);
tm_obj tm_load( tm_obj p);
tm_obj tm_save( tm_obj p);

tm_obj module_new(tm_obj file, tm_obj name, tm_obj code);
void module_free( tm_module*);

#endif /* OBJECT_H_ */
