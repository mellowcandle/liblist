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

#ifndef LLIST_H_
#define LLIST_H_

#include <stdbool.h>

/** E_LLIST
* This is the return values most of the llist API return,
* don't forget to check for success :)
*/
typedef enum
{
	LLIST_SUCCESS = 0x00, 			/**< Operating success */
	LLIST_NODE_NOT_FOUND, 			/**< Error: No matching node found*/
	LLIST_EQUAL_MISSING,			/**< Error: Equal function is missing*/
	LLIST_COMPERATOR_MISSING,		/**< Error: Comperator function is missing*/
	LLIST_NULL_ARGUMENT,			/**< Error: NULL argument*/
	LLIST_MALLOC_ERROR,				/**< Error: Memory allocation error*/
	LLIST_ERROR						/**< Error: Generic error*/
} E_LLIST;

#define ADD_NODE_FRONT		(1 << 0)
#define ADD_NODE_REAR		~ADD_NODE_FRONT

#define ADD_NODE_BEFORE    ( 1<<0 )
#define ADD_NODE_AFTER      ~ADD_NODE_BEFORE

typedef void * llist;
typedef void * llist_node;

// function prototypes
typedef void ( * node_func ) ( llist_node node );
typedef int ( * comperator ) ( llist_node first,llist_node second );
typedef bool ( * equal ) ( llist_node, llist_node );


/**
 * @brief Create a list
 * @param[in] compare_func a function used to compare elements in the list
 * @param[in] equal_func a function used to check if two elements are equal
 * @return new list if success, NULL on error
 */
llist llist_create ( comperator compare_func, equal equal_func );

/**
 * @brief Destroys a list
 * @param[in] list The list to destroy
 * @param[in] bool true if the nodes should be destryed, false if not
 * @param[in] node_func destructor, if the previous param is true,
 *					if NULL is provided standard library c free() will be used
 */
void llist_destroy ( llist list, bool destroy_nodes, node_func destructor);


/**
 * @brief Add a node to a list
 * @param[in] llist the list to operator upon
 * @param[in] llist_node the node to add
 * @param[in] int flags
 * @return int LLIST_SUCCESS if success
 */
int	llist_add_node ( llist list, llist_node node, int flags );

/**
 * @brief Insert a node at a specific location
 * @param[in] llist the list to operator upon
 * @param[in] llist_node the node to add
 * @param[in] llist_node a position reference node
 * @param[in] int flags
 * @return int LLIST_SUCCESS if success
 */
int llist_insert_node ( llist list,  llist_node new_node, llist_node pos_node, int flags );


/**
 * @brief Delete a node from a list
 * @param[in] llist the list to operator upon
 * @param[in] llist_node the node to delete
 * @param[in] equal an alternative eqauality function
 * @param[in] bool Should we run a destructor
 * @param[in] destructor function, if NULL is provided, free() will be used
 * @return int LLIST_SUCCESS if success
 */
int llist_delete_node ( llist list, llist_node node, equal alternative, bool destroy_node, node_func destructor );

/**
 * @brief Finds a node in a list
 * @param[in]  llist the list to operator upon
 * @param[in]  void * the data to find
 * @param[out] llist_node * a pointer for found node.
 * 				this pointer can be used only if llist_find_node returned LLIST_SUCCESS
 * @param[in] equal an alternative eqauality function
 * @return LLIST_SUCCESS if success
 */
int llist_find_node ( llist list, void * data, llist_node * found, equal alternative );

/**
 * @brief operate on each element of the list
 * @param[in] llist the list to operator upon
 * @param[in] node_func the function to perform
 * @return int LLIST_SUCCESS if success
 */
int llist_for_each ( llist list, node_func func );

/**
 * @brief sort a lists
 * @param[in] list
 * @param[in] alternative if unless an alternative comperator function is provided the default will be used
 * @param[in] flags
 * @return int LLIST_SUCCESS if success
 */
int	llist_sort ( llist list, comperator alternative, int flags );

/**
 * @brief Returns the head node of the list
 * @param list the list to operate on 
 * @return the head node, NULL on error
 */
llist_node	llist_get_head (llist list);

/**
 * @brief Returns the tail node of the list
 * @param list the list to operate on
 * @return the tail node, NULL on error
 */
llist_node	llist_get_tail	(llist list);


/**
 * @brief push a node to the head of the list
 * @param list the list to operate on 
 * @return int LLIST_SUCCESS if success
 */
int llist_push (llist list, llist_node node);

/**
 * @brief peek at the head of the list
 * @param list the list to operate on 
 * @return llist_node the head node
 */
llist_node llist_peek(llist list);

/**
 * @brief pop the head of the list
 * @param list the list to operate on 
 * @return llist_node the head node
 */
llist_node llist_pop(llist list);

/**
 * @brief return the number of elements in the list
 * @param list the list to operate on 
 * @return unsigned int  number of elements in the list
 */
unsigned int llist_size(llist list);
#endif /* LLIST_H_ */
