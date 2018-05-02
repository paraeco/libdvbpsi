/*
 * Licensed under the ASLv2 license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#ifndef __CVECTOR_H__
#define __CVECTOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <search.h>

typedef struct {
	void * p_root;
	size_t size;
} vector_t;

void vector_init(vector_t * p_vector);
void vector_clear(vector_t * p_vector);

vector_t * vector_create(void);
void vector_destroy(vector_t * p_vector);

size_t vector_size(vector_t * p_vector);
void * vector_at(vector_t * p_vector, size_t pos);
int vector_insert(vector_t * p_vector, size_t pos, void * p_data);
void vector_erase(vector_t * p_vector, size_t pos);

void * vector_pop(vector_t * p_vector);

#ifdef __cplusplus
}
#endif

#endif  /* __CVECTOR_H__ */
