/*
 * llist.h
 *
 *  Created on: Jan 2, 2013
 *      Author: Ramon Fried
 */



#ifndef LLIST_H_
#define LLIST_H_

#include <stdbool.h>

#define LLIST_ERROR 1
#define LLIST_SUCCESS 0

#define ADD_NODE_FRONT		(1 << 0)
#define ADD_NODE_REAR		~ADD_NODE_FRONT

#define ADD_NODE_BEFORE    ( 1<<0 )
#define ADD_NODE_AFTER      ~ADD_NODE_BEFORE

typedef void * llist;
typedef void * llist_node;

// function prototypes
typedef void ( * node_func ) ( llist_node node );
typedef int ( * comperator ) ( llist_node first,llist_node second );
typedef bool ( * equal ) ( llist_node, void * data );


/**
 * @brief Create a list
 * @param compare_func a function used to compare elements in the list
 * @param equal_func a function used to check if two elements are equal
 * @return new list if success, NULL on error
 */
llist		llist_create ( comperator compare_func, equal equal_func );

/**
 * @brief Destroys a list
 * @param list The list to destroy
 */
void		llist_destroy ( llist list );


/**
 * @brief Add a node to a list
 * @param llist the list to operator upon
 * @param llist_node the node to add
 * @param int flags
 * @return int LLIST_SUCCESS if success, LLIST_ERROR on error
 */
int			llist_add_node ( llist list, llist_node node, int flags );

/**
 * @brief Insert a node at a specific location
 * @param llist the list to operator upon
 * @param llist_node the node to add
 * @param llist_node a position reference node
 * @param int flags
 * @return int LLIST_SUCCESS if success, LLIST_ERROR on error
 */
int 		llist_insert_node ( llist list,  llist_node new_node, llist_node pos_node, int flags );


/**
 * @brief Delete a node from a list
 * @param llist the list to operator upon
 * @param llist_node the node to delete
 * @return int LLIST_SUCCESS if success, LLIST_ERROR on error
 */
int			llist_delete_node ( llist list, llist_node node );

/**
 * @brief Finds a node in a list
 * @param llist the list to operator upon
 * @param void * the data to find
 * @param equal an alternative eqauality function
 * @return llist_node The node if found, NULL on error / not found
 */
llist_node	llist_find_node ( llist list, void * data, equal alternative );

/**
 * @brief operate on each element of the list
 * @param llist the list to operator upon
 * @param node_func the function to perform
 * @return int LLIST_SUCCESS if success, LLIST_ERROR on error
 */
int 		llist_for_each ( llist list, node_func func );

/**
 * @brief sort a lists
 * @param list
 * @param alternative if unless an alternative comperator function is provided the default will be used
 * @param flags
 * @return int LLIST_SUCCESS if success, LLIST_ERROR on error
 */
int		llist_sort ( llist list, comperator alternative, int flags );

#endif /* LLIST_H_ */
