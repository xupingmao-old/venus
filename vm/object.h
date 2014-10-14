/*
 * object.h
 *
 *  Created on: 2014��8��25��
 *      Author: Xu
 */

#ifndef OBJECT_H_
#define OBJECT_H_

tm_obj string_new(char* s, int size);
tm_obj list_new(int cap);
tm_list* _list_new(int cap);

void list_append( tm_list* list, tm_obj v);
void list_set( tm_list* list, int n, tm_obj v);

tm_frame* frame_new(tm_func* func);
tm_obj tm_eval(tm_obj mod);

// stream
tm_obj _load(char* fname);
tm_obj tm_load( tm_obj p);
tm_obj tm_save( tm_obj p);

#endif /* OBJECT_H_ */
