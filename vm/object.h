/*
 * object.h
 *
 *  Created on: 2014Äê8ÔÂ25ÈÕ
 *      Author: Xu
 */

#ifndef OBJECT_H_
#define OBJECT_H_

tm_obj string_new(char* s, int size);
tm_obj list_new(int cap);
tm_list* _list_new(int cap);

void list_push( tm_list* list, tm_obj v);
void list_set( tm_list* list, int n, tm_obj v);

#endif /* OBJECT_H_ */
