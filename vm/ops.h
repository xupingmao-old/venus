/*
 * ops.h
 *
 *  Created on: 2014��8��23��
 *      Author: Xu
 */

#ifndef OPS_H_
#define OPS_H_

#include "tm.h"

tm_obj tm_str(tm_obj v);
tm_obj tm_add(tm_obj a, tm_obj b);

int tm_eq(tm_obj a, tm_obj b);

tm_obj tm_get(tm_obj self, tm_obj k);
void tm_set(tm_obj self, tm_obj k, tm_obj v);

#endif /* OPS_H_ */
