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
	LLIST_COMPERATOR_MISSING,		/**< Error: Comparator function is missing*/
	LLIST_NULL_ARGUMENT,			/**< Error: NULL argument*/
	LLIST_MALLOC_ERROR,				/**< Error: Memory allocation error*/
    LLIST_NOT_IMPLEMENTED,          /**< Error: Implementation missing*/
    LLIST_MULTITHREAD_ISSUE,        /**< Error: Multithreading issue*/
	LLIST_ERROR						/**< Error: Generic error*/
} E_LLIST;

#define ADD_NODE_FRONT		(1 << 0)
#define ADD_NODE_REAR		~ADD_NODE_FRONT

#define ADD_NODE_BEFORE    ( 1<<0 )
#define ADD_NODE_AFTER      ~ADD_NODE_BEFORE

#define SORT_LIST_ASCENDING ( 1<<0 )
#define SORT_LIST_DESCENDING ~SORT_LIST_ASCENDING

#define MT_SUPPORT_TRUE  (1)
#define MT_SUPPORT_FALSE (0)

#undef TRUE
#undef FALSE

#define TRUE (1)
#define FALSE (0)

typedef void * llist;
typedef void * llist_node;

// function prototypes
typedef void ( * node_func ) ( llist_node node );

// function prototypes with user arguments
typedef void ( * node_func_arg ) ( llist_node node, void* arg );

/**
* @brief Compares two nodes in a list
* @param[in] first llist_node
* @param[in] second llist_node
* @return an integer less than, equal to, or greater than zero if first,
* respectively, to be less than, to match, or be greater than second.
*/
typedef int ( * comperator ) ( llist_node first,llist_node second );

/**
* @brief Check if two nodes are equal
* @param[in] first llist_node
* @param[in] second llist_node
* @return true if the nodes are equal, false otherwise
*/
typedef bool ( * equal ) ( llist_node, llist_node );

#define LLIST_INITALIZER {0,NULL,NULL,NULL,NULL}

/**
 * @brief Create a list
 * @param[in] compare_func a function used to compare elements in the list
 * @param[in] equal_func a function used to check if two elements are equal
 * @param[in] flags used to identify whether we create a thread safe linked-list
 * @return new list if success, NULL on error
 */
llist llist_create ( comperator compare_func, equal equal_func, unsigned flags );

/**
 * @brief Destroys a list
 * @warning Call this function only if the list was created with llist_create
 *          Static initializer created list cannot be destroyed using this function
 * @param[in] list The list to destroy
 * @param[in] destroy_nodes true if the nodes should be destroyed, false if not
 * @param[in] destructor alternative destructor, if the previous param is true,
 *			  if NULL is provided standard library c free() will be used
 */
void llist_destroy ( llist list, bool destroy_nodes, node_func destructor );


/**
 * @brief Add a node to a list
 * @param[in] list the list to operator upon
 * @param[in] node the node to add
 * @param[in] flags flags
 * @return int LLIST_SUCCESS if success
 */
int llist_add_node ( llist list, llist_node node, int flags );

/**
 * @brief Insert a node at a specific location
 * @param[in] list the list to operator upon
 * @param[in] new_node the node to add
 * @param[in] pos_node a position reference node
 * @param[in] flags flags
 * @return int LLIST_SUCCESS if success
 */
int llist_insert_node ( llist list,  llist_node new_node, llist_node pos_node, int flags );


/**
 * @brief Delete a node from a list
 * @param[in] list the list to operator upon
 * @param[in] node the node to delete
 * @param[in] destroy_node Should we run a destructor
 * @param[in] destructor function, if NULL is provided, free() will be used
 * @return int LLIST_SUCCESS if success
 */
int llist_delete_node ( llist list, llist_node node, bool destroy_node, node_func destructor );

/**
 * @brief Finds a node in a list
 * @param[in]  list the list to operator upon
 * @param[in]  data the data to find
 * @param[out] found a pointer for found node.
 * 				this pointer can be used only if llist_find_node returned LLIST_SUCCESS
 * @return LLIST_SUCCESS if success
 */
int llist_find_node ( llist list, void * data, llist_node * found );

/**
 * @brief operate on each element of the list
 * @param[in] list the list to operator upon
 * @param[in] func the function to perform
 * @return int LLIST_SUCCESS if success
 */
int llist_for_each ( llist list, node_func func );

/**
 * @brief operate on each element of the list
 * @param[in] list the list to operator upon
 * @param[in] func the function to perform
 * @param[in] arg passed to func
 * @return int LLIST_SUCCESS if success
 */
int llist_for_each_arg ( llist list, node_func_arg func, void * arg );
/**
 * @brief sort a lists
 * @param[in] list the list to operator upon
 * @param[in] flags
 * @return int LLIST_SUCCESS if success
 */
int llist_sort ( llist list, int flags );

/**
 * @brief Returns the head node of the list
 * @param[in] list the list to operate on 
 * @return the head node, NULL on error
 */
llist_node llist_get_head ( llist list );

/**
 * @brief Returns the tail node of the list
 * @param[in] list the list to operate on
 * @return the tail node, NULL on error
 */
llist_node llist_get_tail ( llist list );


/**
 * @brief push a node to the head of the list
 * @param[in] list the list to operate on
 * @param[in] node the node to push
 * @return int LLIST_SUCCESS if success
 */
int llist_push ( llist list, llist_node node );

/**
 * @brief peek at the head of the list
 * @param[in] list the list to operate on 
 * @return llist_node the head node
 */
llist_node llist_peek( llist list );

/**
 * @brief pop the head of the list
 * @param[in] list the list to operate on 
 * @return llist_node the head node
 */
llist_node llist_pop( llist list );

/**
 * @brief return the number of elements in the list
 * @param[in] list the list to operate on 
 * @return int  number of elements in the list or -1 if error
 */
int llist_size( llist list );

/**
 * @brief concatenate the second list to the first list
 * @param[in] first the list to operate on 
 * @param[in] second the list to operate on.
 * @warning The nodes from the second list will be deleted and concatenated to the first list
 *          Remember to call llist_destroy() on  the second list (if it was created by llist_create())
 * @return int LLIST_SUCCESS if success
 */
int llist_concat( llist first, llist second );

/**
 * @brief merge the second list to the first list
 * @param[in] first the list to operate on 
 * @param[in] second the list to operate on
 * @warning The nodes from the second list will be deleted and merged to the first list
 *          Remember to call llist_destroy() on  the second list (if it was created by llist_create())
 * @return int LLIST_SUCCESS if success
 */
int llist_merge( llist first, llist second );


/**
 * @brief get the maximum node in a given list
 * @param[in] list the list to operate upon
 * @param[out] maximum node
 * @return int LLIST_SUCCESS if success
 */
int llist_get_max( llist list, llist_node * max);

/**
 * @brief get the minimum node in a given list
 * @param[in] list the list to operate upon
 * @param[out] minumum node
 * @return int LLIST_SUCCESS if success
 */
int llist_get_min(llist list, llist_node * min);

/**
 * @brief Reverse a list
 * @param[in] list the list to operate upon
 * @return int LLIST_SUCCESS if success
 */
int llist_reverse( llist list );

/**
 * @brief check if list is empty
 * @param[in] list the list to operate upon
 * @return bool True if list is empty
 */
bool llist_is_empty( llist list );

#endif /* LLIST_H_ */
