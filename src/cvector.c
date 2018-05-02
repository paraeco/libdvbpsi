/*
 * Licensed under the ASLv2 license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include "cvector.h"

typedef struct {
	size_t pos;
	void * p_data;
} vector_node;


static int node_compare(const void *p_node1, const void *p_node2);

static int node_compare(const void *p_node1, const void *p_node2)
{
    size_t pos1, pos2;

    pos1 = ((const vector_node *)p_node1)->pos;        
    pos2 = ((const vector_node *)p_node2)->pos;

    if (pos1 > pos2)
        return 1;
    else if (pos1 < pos2)
        return -1;
    else
        return 0;
}

void vector_init(vector_t * p_vector)
{
	if (p_vector)
		memset(p_vector, 0, sizeof(vector_t));
}

void vector_clear(vector_t * p_vector)
{
	if (NULL == p_vector)
		return;


	while (NULL != p_vector->p_root) {
		vector_node * p_node = *(vector_node **)p_vector->p_root;
		if (NULL == p_node)
			break;

		tdelete(p_node, &p_vector->p_root, node_compare);
		free(p_node);
	}

	p_vector->size = 0;
}

vector_t * vector_create(void)
{
	vector_t * p_vector;
	p_vector = malloc(sizeof(vector_t));
	if (NULL != p_vector)
		vector_init(p_vector);

	return p_vector;
}

void vector_destroy(vector_t * p_vector)
{
	if (NULL == p_vector)
		return;

	vector_clear(p_vector);
	free(p_vector);
}


size_t vector_size(vector_t * p_vector)
{
	size_t size = 0;

	if (NULL != p_vector)
		size = p_vector->size;

	return size;
}

void * vector_at(vector_t * p_vector, size_t pos)
{
	void * p_data = NULL;
	vector_node node;
	void * p;

	if (NULL == p_vector)
		goto error;

	node.pos = pos;
	p = tfind(&node, &p_vector->p_root, node_compare);
	if (NULL != p) {
		vector_node * p_node = *(vector_node **)p;
		if (NULL != p_node)
			p_data = p_node->p_data;
	}
	
error:	
	return p_data;
}

int vector_insert(vector_t * p_vector, size_t pos, void * p_data)
{
	vector_node * p_node;
	void * p;

	if (NULL == p_vector)
		return -1;

	p_node = malloc(sizeof(vector_node));
	if (NULL == p_node)
		return -1;

	p_node->pos = pos;
	p_node->p_data = p_data;
	p = tsearch(p_node, &p_vector->p_root, node_compare);
	/* p is never a NULL pointer */
	if (p_node != *(vector_node **)p) {
		/* a vector_node already exists */
		free (p_node);
		p_node = *(vector_node **)p;
		p_node->p_data = p_data;
	}
	else {
		p_vector->size ++;
	}
	
	return 0;
}

void vector_erase(vector_t * p_vector, size_t pos)
{
	vector_node node;
	void * p;

	if (NULL == p_vector)
		return;

	node.pos = pos;
	p = tdelete(&node, &p_vector->p_root, node_compare);
	if (NULL != p) {
		vector_node * p_node = *(vector_node **)p;
		if (NULL != p_node) {
			free (p_node);
			p_vector->size --;
		}
	}
	
	return;
}

void * vector_pop(vector_t * p_vector)
{
	void * p_data = NULL;

	if (NULL == p_vector)
		goto error;

	vector_node * p_node = *(vector_node **)p_vector->p_root;
	if (NULL != p_node) {
		p_data = p_node->p_data;
		tdelete(p_node, &p_vector->p_root, node_compare);
		free(p_node);
	}

error:
	return p_data;
}
