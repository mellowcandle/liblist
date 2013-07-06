/*
 * llist.h
 *
 *  Created on: Jan 2, 2013
 *      Author: gombotz
 */

#ifndef LLIST_H_
#define LLIST_H_

#include <stdbool.h>


#define ADD_NODE_FRONT		(1 << 0)
#define ADD_NODE_REAR		~ADD_NODE_FRONT

#define ADD_NODE_BEFORE    ( 1<<0 )
#define ADD_NODE_AFTER      ~ADD_NODE_BEFORE

typedef void * llist;
typedef void * llist_node;

// function prototypes
typedef void (* node_func)(llist_node node);
typedef int (* comperator)(llist_node first,llist_node second );
typedef bool (* equal )(llist_node, void * data);



llist		llist_create ( comperator compare_func, equal equal_func);
void		llist_destroy (llist list);

int		llist_add_node (llist list, llist_node node, int flags);
int 		llist_insert_node (llist list,  llist_node new_node, llist_node pos_node, int flags);

int		llist_delete_node (llist list, llist_node node);
llist_node	llist_find_node (llist list, void * data, equal alternative);

int 		llist_for_each(llist list, node_func func );

void		llist_sort(llist list, comperator alternative, int flags);

#endif /* LLIST_H_ */
