/*
 *    Copyright [2013] [Ramon Fried] <ramon.fried at gmail dot com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 */

#include "../inc/llist.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#ifdef LLIST_OPT_SYNCHRONOUS
#include <pthread.h>
#endif

#define LOG_FUNC_ENTRANCE() printf("%lu: In %s\n", time(NULL), __PRETTY_FUNCTION__);

typedef struct __list_node
{
	llist_node node;
	struct __list_node * next;
} _list_node;

typedef struct
{
	unsigned int count;
#ifdef LLIST_OPT_SYNCHRONOUS
	pthread_mutex_t mutex;
#endif
	comperator comp_func;
	equal equal_func;
	_list_node * head;
	_list_node * tail;
} _llist;


llist llist_create ( comperator compare_func, equal equal_func )
{
	_llist * new_list;
	new_list = malloc ( sizeof ( _llist ) );

	if ( new_list == NULL )
	{
		return NULL ;
	}

// These can be NULL, I don't care...
	new_list->equal_func = equal_func;
	new_list->comp_func = compare_func;

// Reset the list
	new_list->count = 0;
	new_list->head = NULL;
	new_list->tail = NULL;

#ifdef LLIST_OPT_SYNCHRONOUS
	pthread_mutex_init ( &new_list->mutex, NULL );
#endif

	return new_list;
}

void llist_destroy ( llist list, bool destroy_nodes, node_func destructor )
{
	_list_node * iterator;
	_list_node * next;

	if ( list == NULL )
	{
		return;
	}
#ifdef LLIST_OPT_SYNCHRONOUS
	pthread_mutex_lock ( & ( ( _llist * ) list )->mutex );
#endif
	// Delete the data contained in the nodes
	iterator = ( ( _llist * ) list )->head;

	while ( iterator != NULL )
	{

		if ( destroy_nodes )
		{

			if ( destructor )
			{
				destructor ( iterator->node );
			}
			else
			{
				free ( iterator->node );
			}
		}

		next = iterator->next;

		free ( iterator ); // Delete's the container

		iterator = next;
	}

	// OK, now the linked list is caput, let's delete the actual list pointer
#ifdef LLIST_OPT_SYNCHRONOUS
	pthread_mutex_unlock ( & ( ( _llist * ) list )->mutex );
	pthread_mutex_destroy ( & ( ( _llist * ) list )->mutex );
#endif
	free ( list );

	return;

}

unsigned int llist_size(llist list)
{
    unsigned int retval;
	if ( list == NULL )
	{
		return 0;
	}
#ifdef LLIST_OPT_SYNCHRONOUS
	pthread_mutex_lock ( & ( ( _llist * ) list )->mutex );
#endif
    retval = (( _llist * ) list)->count;
#ifdef LLIST_OPT_SYNCHRONOUS
    pthread_mutex_unlock ( & ( ( _llist * ) list )->mutex );
#endif

    return retval;
}

int llist_add_node ( llist list, llist_node node, int flags )
{
	_list_node * node_wrapper = NULL;

	if ( list == NULL )
	{
		return LLIST_NULL_ARGUMENT;
	}

	node_wrapper = malloc ( sizeof ( _list_node ) );

	if ( node_wrapper == NULL )
	{
		return LLIST_ERROR;
	}

	node_wrapper->node = node;
#ifdef LLIST_OPT_SYNCHRONOUS
	pthread_mutex_lock ( & ( ( _llist * ) list )->mutex );
#endif

	( ( _llist * ) list )->count++;

	if (( ( _llist * ) list )->head == NULL)
	{// Adding the first node, update head and tail to point to that node
		node_wrapper->next = NULL;
		( ( _llist * ) list )->head = ( ( _llist * ) list )->tail = node_wrapper;
	}
	else if ( flags & ADD_NODE_FRONT )
	{
		node_wrapper->next = ( ( _llist * ) list )->head;
		( ( _llist * ) list )->head = node_wrapper;
	}
	else // add node in the rear
	{
		node_wrapper->next = NULL;
		( ( _llist * ) list )->tail->next = node_wrapper;
		( ( _llist * ) list )->tail = node_wrapper;
	}
#ifdef LLIST_OPT_SYNCHRONOUS
	pthread_mutex_unlock ( & ( ( _llist * ) list )->mutex );
#endif
	return LLIST_SUCCESS;
}

int llist_delete_node ( llist list, llist_node node, equal alternative, bool destroy_node, node_func destructor )
{
	_list_node * iterator;
	_list_node * temp;
	equal    actual_equal;
	
	if ( ( list == NULL ) || ( node == NULL ) )
	{
		return LLIST_NULL_ARGUMENT;
	}
	
	actual_equal = ( ( _llist * ) list )->equal_func;

	if ( alternative )
	{
		actual_equal =  alternative;	
	}

	if ( actual_equal == NULL )
	{
		return LLIST_EQUAL_MISSING;
	}
#ifdef LLIST_OPT_SYNCHRONOUS
	pthread_mutex_lock ( & ( ( _llist * ) list )->mutex );
#endif
	iterator = ( ( _llist * ) list )->head;

	// is it the first node ?
	if ( actual_equal(iterator->node,node) )
	{
		if (destroy_node)
		{
			if (destructor)
			{
				destructor(iterator->node);
			}
			else
			{
				free(iterator->node);
			}
					
		}
		
		( ( _llist * ) list )->head = ( ( _llist * ) list )->head->next;
		free ( iterator );
		( ( _llist * ) list )->count--;

		if (( ( _llist * ) list )->count == 0)
		{
		/*
		 *	if we deleted the last node, we need to reset the tail also
		 *	There's no need to check it somewhere else, because the last node must be the head (and tail)
		 */
			( ( _llist * ) list )->tail = NULL;
		}
		assert ( ( ( _llist * ) list )->count >= 0 );
#ifdef LLIST_OPT_SYNCHRONOUS
		pthread_mutex_unlock ( & ( ( _llist * ) list )->mutex );
#endif
        
		return LLIST_SUCCESS;
	}
	else
	{
		while ( iterator->next != NULL )
		{
			if ( actual_equal(iterator->next->node, node) )
			{
				// found it
				temp = iterator->next;
				iterator->next = temp->next;
				free ( temp );

				( ( _llist * ) list )->count--;
				assert ( ( ( _llist * ) list )->count >= 0 );
#ifdef LLIST_OPT_SYNCHRONOUS                
				pthread_mutex_unlock ( & ( ( _llist * ) list )->mutex );
#endif
				return LLIST_SUCCESS;
			}

			iterator = iterator->next;
		}
	}

	if ( iterator->next == NULL )
	{
#ifdef LLIST_OPT_SYNCHRONOUS        
		pthread_mutex_unlock ( & ( ( _llist * ) list )->mutex );
#endif
		return LLIST_NODE_NOT_FOUND;
	}

	assert ( 1==2 );
	// this assert always failed. we assume that the function never gets here...
	return LLIST_ERROR;
}


int llist_for_each ( llist list, node_func func )
{
	_list_node * iterator;

	if ( ( list == NULL ) || ( func == NULL ) )
	{
		return LLIST_NULL_ARGUMENT;
	}

	iterator = ( ( _llist * ) list )->head;
#ifdef LLIST_OPT_SYNCHRONOUS
	pthread_mutex_lock ( & ( ( _llist * ) list )->mutex );
#endif
	while ( iterator != NULL )
	{
		func ( iterator->node );
		iterator = iterator->next;
	}
#ifdef LLIST_OPT_SYNCHRONOUS
	pthread_mutex_unlock ( & ( ( _llist * ) list )->mutex );
#endif
	return LLIST_SUCCESS;
}

int llist_insert_node ( llist list, llist_node new_node, llist_node pos_node,
                        int flags )
{
	_list_node * iterator;
	_list_node * node_wrapper = NULL;

	if ( ( list == NULL ) || ( new_node == NULL ) || ( pos_node == NULL ) )
	{
		return LLIST_NULL_ARGUMENT;
	}

	node_wrapper = malloc ( sizeof ( _list_node ) );

	if ( node_wrapper == NULL )
	{
		return LLIST_MALLOC_ERROR;
	}

	node_wrapper->node = new_node;
#ifdef LLIST_OPT_SYNCHRONOUS
	pthread_mutex_lock ( & ( ( _llist * ) list )->mutex );
#endif
	( ( _llist * ) list )->count++;

	iterator = ( ( _llist * ) list )->head;

	if ( iterator->node == pos_node )
	{
		// it's the first node

		if ( flags & ADD_NODE_BEFORE )
		{
			node_wrapper->next = iterator;
			( ( _llist * ) list )->head = node_wrapper;
		}
		else
		{
			node_wrapper->next = iterator->next;
			iterator->next = node_wrapper;
		}
#ifdef LLIST_OPT_SYNCHRONOUS
		pthread_mutex_unlock ( & ( ( _llist * ) list )->mutex );
#endif
		return LLIST_SUCCESS;
	}

	while ( iterator->next != NULL )
	{
		if ( iterator->next->node == pos_node )
		{
			if ( flags & ADD_NODE_BEFORE )
			{
				node_wrapper->next = iterator->next;
				iterator->next = node_wrapper;
			}
			else
			{
				iterator = iterator->next;
				// now we stand on the pos node
				node_wrapper->next = iterator->next;
				iterator->next = node_wrapper;
			}
#ifdef LLIST_OPT_SYNCHRONOUS
			pthread_mutex_unlock ( & ( ( _llist * ) list )->mutex );
#endif
			return LLIST_SUCCESS;
		}

		iterator = iterator->next;
	}

	assert ( 1==2 );
	// this assert will always fail. we assume that the function never gets here...
	return LLIST_ERROR;

}

int  llist_find_node ( llist list, void * data, llist_node * found, equal alternative )
{
	_list_node * iterator;
	equal    actual_equal;
	if ( list == NULL )
	{
		return LLIST_NULL_ARGUMENT;
	}

	actual_equal = ( ( _llist * ) list )->equal_func;

	if ( alternative )
	{
		actual_equal =  alternative;
	}

	if ( actual_equal == NULL )
	{
		return LLIST_EQUAL_MISSING;
	}
#ifdef LLIST_OPT_SYNCHRONOUS
	pthread_mutex_lock ( & ( ( _llist * ) list )->mutex );
#endif
	iterator = ( ( _llist * ) list )->head;

	while ( iterator != NULL )
	{
		if ( actual_equal ( iterator->node, data ) )
		{
#ifdef LLIST_OPT_SYNCHRONOUS            
			pthread_mutex_unlock ( & ( ( _llist * ) list )->mutex );
#endif
			*found = iterator->node;
			return LLIST_SUCCESS;
		}
		
		iterator = iterator->next;
	}

	// Didn't find the node
#ifdef LLIST_OPT_SYNCHRONOUS
	pthread_mutex_unlock ( & ( ( _llist * ) list )->mutex );
#endif
	return LLIST_NODE_NOT_FOUND;

}


llist_node	llist_get_head (llist list)
{
	if (list != NULL)
	{
		if ( ( ( _llist * ) list )->head ) // there's at least one node
		{
			return ( ( _llist * ) list )->head->node;
		}
	}
	
	return NULL;
}

llist_node	llist_get_tail	(llist list)
{
	if (list != NULL)
	{
		if ( ( ( _llist * ) list )->tail ) // there's at least one node
		{ 
			return ( ( _llist * ) list )->tail->node;
		}
	}
	
	return NULL;
}

int llist_push (llist list, llist_node node)
{
	return llist_add_node ( list, node, ADD_NODE_FRONT );
}

llist_node llist_peek(llist list)
{
	return llist_get_head ( list);
}

llist_node llist_pop(llist list)
{
	llist_node tempnode = NULL;
	_list_node * tempwrapper;
#ifdef LLIST_OPT_SYNCHRONOUS
	pthread_mutex_lock ( & ( ( _llist * ) list )->mutex );
#endif
	if  ( ( ( _llist * ) list )->count) // There exists at least one node
	{ 	
		tempwrapper = ( ( _llist * ) list )->head;
		tempnode = tempwrapper->node;
		( ( _llist * ) list )->head  = ( ( _llist * ) list )->head->next;
		( ( _llist * ) list )->count--;
		free(tempwrapper);
		
		if (( ( _llist * ) list )->count == 0) // We've deleted the last node
		{
			( ( _llist * ) list )->tail = NULL;
		}
	}
#ifdef LLIST_OPT_SYNCHRONOUS
	pthread_mutex_unlock ( & ( ( _llist * ) list )->mutex );
#endif
	return tempnode;
}
