/*
 * object.h
 *
 *  Created on: 2014��8��25��
 *      Author: Xu
 */

#ifndef OBJECT_H_
#define OBJECT_H_

tm_obj string_new(tm_vm* tm, char* s);

tm_obj list_new(tm_vm* tm);

tm_obj dict_new(tm_vm* tm);

#endif /* OBJECT_H_ */
