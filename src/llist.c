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
#include <pthread.h>
#include <assert.h>


#define LOG_FUNC_ENTRANCE() printf("%lu: In %s\n", time(NULL), __PRETTY_FUNCTION__);

typedef struct __list_node _list_node;

typedef struct __list_node
{
	llist_node node;
	_list_node * next;
} _list_node;

typedef struct
{
	unsigned int count;
	pthread_mutex_t mutex;
	comperator comp_func;
	equal equal_func;
	_list_node * head;
} _llist;


llist llist_create ( comperator compare_func, equal equal_func )
{
	_llist * new_list;
	new_list = malloc ( sizeof ( _llist ) );

	if ( new_list == NULL )
	{
		perror ( "Malloc LLIST_ERROR" );
		return NULL ;
	}

// These can be NULL, I don't care...
	new_list->equal_func = equal_func;
	new_list->comp_func = compare_func;

// Reset the list
	new_list->count = 0;
	new_list->head = NULL;
	pthread_mutex_init ( &new_list->mutex, NULL );

	return new_list;
}

void		llist_destroy ( llist list, bool destroy_nodes, node_func destructor )
{
	_list_node * iterator;
	_list_node * next;

	if ( list == NULL )
	{
		return;
	}

	pthread_mutex_lock ( & ( ( _llist * ) list )->mutex );

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

	pthread_mutex_unlock ( & ( ( _llist * ) list )->mutex );
	pthread_mutex_destroy ( & ( ( _llist * ) list )->mutex );

	free ( list );

	return;

}


int llist_add_node ( llist list, llist_node node, int flags )
{
	_list_node * node_wrapper = NULL;
	_list_node * iterator;

	if ( ( list == NULL ) || ( node == NULL ) )
	{
		//Should we enforce NULL checking for the actual NODE, that worth thinking over again...
		perror ( "NULL argument" );
		return LLIST_ERROR;
	}

	node_wrapper = malloc ( sizeof ( _list_node ) );

	if ( node_wrapper == NULL )
	{
		perror ( "Malloc LLIST_ERROR" );
		return LLIST_ERROR;
	}

	node_wrapper->node = node;

	pthread_mutex_lock ( & ( ( _llist * ) list )->mutex );

	( ( _llist * ) list )->count++;

	if ( flags & ADD_NODE_FRONT )
	{
		node_wrapper->next = ( ( _llist * ) list )->head;
		( ( _llist * ) list )->head = node_wrapper;
	}
	else // add node in the rear
	{
		node_wrapper->next = NULL;
		iterator = ( ( _llist * ) list )->head;

		// If the list is empty
		if ( iterator == NULL )
			( ( _llist * ) list )->head = node_wrapper;
		else
		{
			while ( iterator->next != NULL )
			{
				iterator = iterator->next;
			}

			iterator->next = node_wrapper;
		}

	}

	pthread_mutex_unlock ( & ( ( _llist * ) list )->mutex );
	return LLIST_SUCCESS;
}

int llist_delete_node ( llist list, llist_node node, equal alternative )
{
	_list_node * iterator;
	_list_node * temp;
	equal    actual_equal;
	
	if ( ( list == NULL ) || ( node == NULL ) )
	{
		perror ( "NULL argument" );
		return LLIST_ERROR;
	}
	
	actual_equal = ( ( _llist * ) list )->equal_func;

	if ( alternative )
	{
		actual_equal =  alternative;	
	}

	if ( actual_equal == NULL )
	{
		perror ( "equal function was not provided in init or alternative" );
		return LLIST_ERROR;
	}

	pthread_mutex_lock ( & ( ( _llist * ) list )->mutex );

	iterator = ( ( _llist * ) list )->head;

	// is it the first node ?
	if ( actual_equal(iterator->node,node) )
	{
		( ( _llist * ) list )->head = ( ( _llist * ) list )->head->next;
		free ( iterator );
		( ( _llist * ) list )->count--;

		assert ( ( ( _llist * ) list )->count >= 0 );

		pthread_mutex_unlock ( & ( ( _llist * ) list )->mutex );
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
				pthread_mutex_unlock ( & ( ( _llist * ) list )->mutex );
				return LLIST_SUCCESS;
			}

			iterator = iterator->next;
		}
	}

	if ( iterator->next == NULL )
	{
		pthread_mutex_unlock ( & ( ( _llist * ) list )->mutex );
		perror ( "Can't find the node in the list" );
		return LLIST_ERROR;
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
		perror ( "NULL argument" );
		return LLIST_ERROR;
	}

	iterator = ( ( _llist * ) list )->head;

	pthread_mutex_lock ( & ( ( _llist * ) list )->mutex );

	while ( iterator != NULL )
	{
		assert ( iterator->node != NULL );
		func ( iterator->node );
		iterator = iterator->next;
	}

	pthread_mutex_unlock ( & ( ( _llist * ) list )->mutex );

	return LLIST_SUCCESS;
}

int llist_insert_node ( llist list, llist_node new_node, llist_node pos_node,
                        int flags )
{
	_list_node * iterator;
	_list_node * node_wrapper = NULL;

	if ( ( list == NULL ) || ( new_node == NULL ) || ( pos_node == NULL ) )
	{
		perror ( "NULL argument" );
		return LLIST_ERROR;
	}

	node_wrapper = malloc ( sizeof ( _list_node ) );

	if ( node_wrapper == NULL )
	{
		perror ( "Malloc LLIST_ERROR" );
		return LLIST_ERROR;
	}

	node_wrapper->node = new_node;

	pthread_mutex_lock ( & ( ( _llist * ) list )->mutex );

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

		pthread_mutex_unlock ( & ( ( _llist * ) list )->mutex );
		return LLIST_SUCCESS;
	}

	while ( iterator->next != NULL )
	{
		if ( iterator->node == pos_node )
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

			pthread_mutex_unlock ( & ( ( _llist * ) list )->mutex );
			return LLIST_SUCCESS;
		}

		iterator = iterator->next;
	}

	assert ( 1==2 );
	// this assert will always fail. we assume that the function never gets here...
	return LLIST_ERROR;

}

llist_node  llist_find_node ( llist list, void * data, equal alternative )
{
	_list_node * iterator;
	equal    actual_equal;
	if ( list == NULL )
	{
		perror ( "NULL argument" );
		return NULL;
	}

	actual_equal = ( ( _llist * ) list )->equal_func;

	if ( alternative )
		actual_equal =  alternative;

	if ( actual_equal == NULL )
	{
		perror ( "equal function was not provided in init or alternative" );
		return NULL;
	}

	pthread_mutex_lock ( & ( ( _llist * ) list )->mutex );

	iterator = ( ( _llist * ) list )->head;

	while ( iterator != NULL )
	{
		if ( actual_equal ( iterator->node, data ) )
		{
			pthread_mutex_unlock ( & ( ( _llist * ) list )->mutex );
			return iterator->node;
		}
		
		iterator = iterator->next;
	}

	// Didn't find the node
	pthread_mutex_unlock ( & ( ( _llist * ) list )->mutex );
	return NULL;

}
