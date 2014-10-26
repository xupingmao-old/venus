/*
 * ops.h
 *
 *  Created on: 2014��8��23��
 *      Author: Xu
 */

#ifndef OPS_H_
#define OPS_H_

#include "tm.h"

tm_obj tm_add(tm_obj a, tm_obj b);
tm_obj tm_sub(tm_obj a, tm_obj b);
tm_obj tm_mul(tm_obj a, tm_obj b);
tm_obj tm_div(tm_obj a, tm_obj b);

int tm_eq(tm_obj a, tm_obj b);
#define tm_equals tm_eq


tm_obj tm_get(tm_obj self, tm_obj k);
void tm_set(tm_obj self, tm_obj k, tm_obj v);

tm_obj _tm_str(tm_obj obj);
tm_obj btm_str( tm_obj a);

int _tm_len(tm_obj o);
tm_obj tm_len( tm_obj p);

tm_obj tm_has(tm_obj a, tm_obj b);

int tm_bool(tm_obj v);
int tm_iter(tm_obj self, tm_obj k, tm_obj *v);

#endif /* OPS_H_ */
