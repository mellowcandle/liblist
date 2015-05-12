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

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <check.h>
#include "../inc/llist.h"



bool trivial_equal ( llist_node node1, llist_node node2 )
{
    return ( node1 == node2 );
}

int trivial_comperator ( llist_node first, llist_node second )
{
    return ( ( intptr_t )first  - (intptr_t)second );
}

void trivial_node_func ( llist_node node )
{
    int *i = ( int * ) node;
    i = i;
}

void trivial_print_node ( llist_node node )
{
    printf ( "%lu", ( unsigned long ) node );
}

void print_llist ( llist list )
{
    llist_for_each ( list, trivial_print_node );
    printf ( "\n" );
}

START_TEST ( llist_01_create_delete_lists )
{
    llist listToTest = NULL;
    listToTest = llist_create ( NULL, NULL, MT_SUPPORT_FALSE );
    ck_assert_ptr_ne ( listToTest, NULL );
    llist_destroy ( listToTest, false, NULL );
}
END_TEST

START_TEST ( llist_01_create_delete_lists_mt )
{
    llist listToTest = NULL;
    listToTest = llist_create ( NULL, NULL, MT_SUPPORT_TRUE );
    ck_assert_ptr_ne ( listToTest, NULL );
    llist_destroy ( listToTest, false, NULL );
}
END_TEST

START_TEST ( llist_02_add_nodes )
{
    int retval;
    llist listToTest = NULL;
    llist_node retptr;
    listToTest = llist_create ( NULL, NULL, MT_SUPPORT_FALSE );

    // Insert a 5 nodes 1..5
    retval = llist_add_node ( listToTest, ( llist_node ) 1, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 2, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 3, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 4, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 5, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List after adding nodes: " );
    print_llist ( listToTest );

    // Find that specific node, this should fail because no equal function was provided
    retval = llist_find_node ( listToTest, ( llist_node ) 1, &retptr, NULL );
    ck_assert_int_eq ( retval, LLIST_EQUAL_MISSING );

    // find again, but this time, let's provide an equals function
    retval = llist_find_node ( listToTest, ( llist_node ) 1, &retptr, trivial_equal );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );
    ck_assert_ptr_eq ( retptr, ( llist_node ) 1 );

    // find again, but this time, with a non existant node
    retval = llist_find_node ( listToTest, ( llist_node ) 6, &retptr, trivial_equal );
    ck_assert_int_eq ( retval, LLIST_NODE_NOT_FOUND );

    llist_destroy ( listToTest, false, NULL );
}
END_TEST

START_TEST ( llist_02_add_nodes_mt )
{
    int retval;
    llist listToTest = NULL;
    llist_node retptr;
    listToTest = llist_create ( NULL, NULL, MT_SUPPORT_TRUE );

    // Insert a 5 nodes 1..5
    retval = llist_add_node ( listToTest, ( llist_node ) 1, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 2, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 3, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 4, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 5, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List after adding nodes: " );
    print_llist ( listToTest );

    // Find that specific node, this should fail because no equal function was provided
    retval = llist_find_node ( listToTest, ( llist_node ) 1, &retptr, NULL );
    ck_assert_int_eq ( retval, LLIST_EQUAL_MISSING );

    // find again, but this time, let's provide an equals function
    retval = llist_find_node ( listToTest, ( llist_node ) 1, &retptr, trivial_equal );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );
    ck_assert_ptr_eq ( retptr, ( llist_node ) 1 );

    // find again, but this time, with a non existant node
    retval = llist_find_node ( listToTest, ( llist_node ) 6, &retptr, trivial_equal );
    ck_assert_int_eq ( retval, LLIST_NODE_NOT_FOUND );

    llist_destroy ( listToTest, false, NULL );
}
END_TEST

START_TEST ( llist_03_add_dynamic_nodes )
{
    int retval;
    int *data[5];

    for ( int i = 0; i < 5; i++ )
    {
        data[i] = malloc ( sizeof ( int ) );
    }

    llist listToTest = NULL;
    llist_node retptr;
    listToTest = llist_create ( NULL, NULL, MT_SUPPORT_FALSE );

    // Insert a 5 nodes 1..5
    retval = llist_add_node ( listToTest, ( llist_node ) data[0], ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) data[1], ADD_NODE_REAR );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) data[2], ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) data[3], ADD_NODE_REAR );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) data[4], ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    // find node
    retval = llist_find_node ( listToTest, ( llist_node ) data[2], &retptr, trivial_equal );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );
    ck_assert_ptr_eq ( retptr, ( llist_node ) data[2] );

    // destroy list, and also free the data
    llist_destroy ( listToTest, true, NULL );
}
END_TEST

START_TEST ( llist_03_add_dynamic_nodes_mt )
{
    int retval;
    int *data[5];

    for ( int i = 0; i < 5; i++ )
    {
        data[i] = malloc ( sizeof ( int ) );
    }

    llist listToTest = NULL;
    llist_node retptr;
    listToTest = llist_create ( NULL, NULL, MT_SUPPORT_TRUE );

    // Insert a 5 nodes 1..5
    retval = llist_add_node ( listToTest, ( llist_node ) data[0], ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) data[1], ADD_NODE_REAR );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) data[2], ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) data[3], ADD_NODE_REAR );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) data[4], ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    // find node
    retval = llist_find_node ( listToTest, ( llist_node ) data[2], &retptr, trivial_equal );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );
    ck_assert_ptr_eq ( retptr, ( llist_node ) data[2] );

    // destroy list, and also free the data
    llist_destroy ( listToTest, true, NULL );
}
END_TEST

START_TEST ( llist_04_delete_nodes )
{
    int retval;
    //llist_node temp;
    llist listToTest = NULL;
    listToTest = llist_create ( NULL, NULL, MT_SUPPORT_FALSE );

    // Insert a 5 nodes 1..5
    retval = llist_add_node ( listToTest, ( llist_node ) 1, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 2, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 3, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 4, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 5, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List after adding nodes: " );
    print_llist ( listToTest );

    // This should fail because no equal function was given
    retval = llist_delete_node ( listToTest, ( llist_node ) 1, NULL, false, NULL );
    ck_assert_int_eq ( retval, LLIST_EQUAL_MISSING );

    // Delete tail
    retval = llist_delete_node ( listToTest, ( llist_node ) 1, trivial_equal, false, NULL );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List after deleting tail: " );
    print_llist ( listToTest );

    // Delete node in the middle
    retval = llist_delete_node ( listToTest, ( llist_node ) 3, trivial_equal, false, NULL );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List after deleting middle node: " );
    print_llist ( listToTest );

    // Delete head
    retval = llist_delete_node ( listToTest, ( llist_node ) 5, trivial_equal, false, NULL );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List after deleting head node: " );
    print_llist ( listToTest );

    // Delete a node that doesn't exist
    retval = llist_delete_node ( listToTest, ( llist_node ) 6, trivial_equal, false, NULL );
    ck_assert_int_eq ( retval, LLIST_NODE_NOT_FOUND );

    retval = llist_delete_node ( listToTest, ( llist_node ) 2, trivial_equal, false, NULL );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    // The list should not  be empty now
    ck_assert_int_eq(llist_is_empty ( listToTest ), FALSE );

    ck_assert_int_eq(llist_is_empty ( listToTest ), FALSE );

    // Delete last node
    retval = llist_delete_node ( listToTest, ( llist_node ) 4, trivial_equal, false, NULL );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    // The list should be empty now
    ck_assert_int_eq(llist_is_empty ( listToTest ), TRUE );

    llist_destroy ( listToTest, false, NULL );
}
END_TEST


START_TEST ( llist_04_delete_nodes_mt )
{
    int retval;
    //llist_node temp;
    llist listToTest = NULL;
    listToTest = llist_create ( NULL, NULL, MT_SUPPORT_TRUE );

    // Insert a 5 nodes 1..5
    retval = llist_add_node ( listToTest, ( llist_node ) 1, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 2, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 3, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 4, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 5, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List after adding nodes: " );
    print_llist ( listToTest );

    // This should fail because no equal function was given
    retval = llist_delete_node ( listToTest, ( llist_node ) 1, NULL, false, NULL );
    ck_assert_int_eq ( retval, LLIST_EQUAL_MISSING );

    // Delete tail
    retval = llist_delete_node ( listToTest, ( llist_node ) 1, trivial_equal, false, NULL );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List after deleting tail: " );
    print_llist ( listToTest );

    // Delete node in the middle
    retval = llist_delete_node ( listToTest, ( llist_node ) 3, trivial_equal, false, NULL );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List after deleting middle node: " );
    print_llist ( listToTest );

    // Delete head
    retval = llist_delete_node ( listToTest, ( llist_node ) 5, trivial_equal, false, NULL );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List after deleting head node: " );
    print_llist ( listToTest );

    // Delete a node that doesn't exist
    retval = llist_delete_node ( listToTest, ( llist_node ) 6, trivial_equal, false, NULL );
    ck_assert_int_eq ( retval, LLIST_NODE_NOT_FOUND );

    retval = llist_delete_node ( listToTest, ( llist_node ) 2, trivial_equal, false, NULL );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    // The list should not be empty now
    ck_assert_int_eq(llist_is_empty ( listToTest ), FALSE );

    // Delete last node
    retval = llist_delete_node ( listToTest, ( llist_node ) 4, trivial_equal, false, NULL );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    // The list should be empty now
    ck_assert_int_eq(llist_is_empty ( listToTest ), TRUE );

    llist_destroy ( listToTest, false, NULL );
}
END_TEST

START_TEST ( llist_05_list_for_each )
{
    int retval;
    llist listToTest = NULL;
    listToTest = llist_create ( NULL, NULL, MT_SUPPORT_FALSE );

    // Insert a 5 nodes 1..5
    retval = llist_add_node ( listToTest, ( llist_node ) 1, ADD_NODE_REAR );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 2, ADD_NODE_REAR );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 3, ADD_NODE_REAR );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 4, ADD_NODE_REAR );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 5, ADD_NODE_REAR );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_for_each ( listToTest, trivial_node_func );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    llist_destroy ( listToTest, false, NULL );
}
END_TEST


START_TEST ( llist_05_list_for_each_mt )
{
    int retval;
    llist listToTest = NULL;
    listToTest = llist_create ( NULL, NULL, MT_SUPPORT_TRUE );

    // Insert a 5 nodes 1..5
    retval = llist_add_node ( listToTest, ( llist_node ) 1, ADD_NODE_REAR );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 2, ADD_NODE_REAR );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 3, ADD_NODE_REAR );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 4, ADD_NODE_REAR );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 5, ADD_NODE_REAR );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_for_each ( listToTest, trivial_node_func );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    llist_destroy ( listToTest, false, NULL );
}
END_TEST


START_TEST ( llist_06_insert_nodes )
{
    int retval;
    llist listToTest = NULL;
    llist_node retptr;
    listToTest = llist_create ( NULL, NULL, MT_SUPPORT_FALSE );

    // Insert a 5 nodes 1..5
    retval = llist_add_node ( listToTest, ( llist_node ) 1, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 2, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 3, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 4, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 5, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List after adding nodes: " );
    print_llist ( listToTest );

    // Find the middle node (3)
    retval = llist_find_node ( listToTest, ( llist_node ) 3, &retptr, trivial_equal );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    // Add node before
    retval =  llist_insert_node ( listToTest,  ( llist_node ) 7, retptr, ADD_NODE_BEFORE );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List after adding before 3 node: " );
    print_llist ( listToTest );


    // Add node after
    retval =  llist_insert_node ( listToTest,  ( llist_node ) 8, retptr, ADD_NODE_AFTER );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List after adding after 3 node: " );
    print_llist ( listToTest );

    // insert node at the start of the list (before the first node)
    retval = llist_find_node ( listToTest, ( llist_node ) 5, &retptr, trivial_equal );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval =  llist_insert_node ( listToTest,  ( llist_node ) 9, retptr, ADD_NODE_BEFORE );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List after adding 9 before the first node: " );
    print_llist ( listToTest );


    // insert node at the start of the list (after the first node)
    retval = llist_find_node ( listToTest, ( llist_node ) 9, &retptr, trivial_equal );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval =  llist_insert_node ( listToTest,  ( llist_node ) 10, retptr, ADD_NODE_AFTER );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List after adding 10 after the first node: " );
    print_llist ( listToTest );

    // insert node at the end of the list (after the first node)
    retval = llist_find_node ( listToTest, ( llist_node ) 1, &retptr, trivial_equal );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval =  llist_insert_node ( listToTest,  ( llist_node ) 11, retptr, ADD_NODE_AFTER );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List after adding 11 after the last node: " );
    print_llist ( listToTest );

    llist_destroy ( listToTest, false, NULL );
}
END_TEST


START_TEST ( llist_06_insert_nodes_mt )
{
    int retval;
    llist listToTest = NULL;
    llist_node retptr;
    listToTest = llist_create ( NULL, NULL, MT_SUPPORT_TRUE );

    // Insert a 5 nodes 1..5
    retval = llist_add_node ( listToTest, ( llist_node ) 1, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 2, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 3, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 4, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval = llist_add_node ( listToTest, ( llist_node ) 5, ADD_NODE_FRONT );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List after adding nodes: " );
    print_llist ( listToTest );

    // Find the middle node (3)
    retval = llist_find_node ( listToTest, ( llist_node ) 3, &retptr, trivial_equal );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    // Add node before
    retval =  llist_insert_node ( listToTest,  ( llist_node ) 7, retptr, ADD_NODE_BEFORE );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List after adding before 3 node: " );
    print_llist ( listToTest );


    // Add node after
    retval =  llist_insert_node ( listToTest,  ( llist_node ) 8, retptr, ADD_NODE_AFTER );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List after adding after 3 node: " );
    print_llist ( listToTest );

    // insert node at the start of the list (before the first node)
    retval = llist_find_node ( listToTest, ( llist_node ) 5, &retptr, trivial_equal );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval =  llist_insert_node ( listToTest,  ( llist_node ) 9, retptr, ADD_NODE_BEFORE );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List after adding 9 before the first node: " );
    print_llist ( listToTest );


    // insert node at the start of the list (after the first node)
    retval = llist_find_node ( listToTest, ( llist_node ) 9, &retptr, trivial_equal );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval =  llist_insert_node ( listToTest,  ( llist_node ) 10, retptr, ADD_NODE_AFTER );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List after adding 10 after the first node: " );
    print_llist ( listToTest );

    // insert node at the end of the list (after the first node)
    retval = llist_find_node ( listToTest, ( llist_node ) 1, &retptr, trivial_equal );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    retval =  llist_insert_node ( listToTest,  ( llist_node ) 11, retptr, ADD_NODE_AFTER );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List after adding 11 after the last node: " );
    print_llist ( listToTest );

    llist_destroy ( listToTest, false, NULL );
}
END_TEST


START_TEST ( llist_07_test_stack )
{
    int retval;
    llist listToTest = NULL;
    llist_node retptr;
    listToTest = llist_create ( NULL, NULL, MT_SUPPORT_FALSE );

    // Push 1000 nodes
    for ( unsigned long i = 0; i < 1000; i ++ )
    {
        retval = llist_push ( listToTest, ( llist_node ) i );
        ck_assert_int_eq ( retval, LLIST_SUCCESS );
    }

    // Peek at the head
    retptr = llist_peek ( listToTest );
    ck_assert_int_eq ( ( unsigned long ) retptr, 999 );
    for ( unsigned long i = 999; i > 0; i-- )
    {
        retptr = llist_pop ( listToTest );
        ck_assert_int_eq ( ( unsigned long ) retptr, i );
    }

    printf ( "Count = %d\n", llist_size ( listToTest ) );
    retptr = llist_pop ( listToTest );
    ck_assert_ptr_eq ( retptr, NULL );

    llist_destroy ( listToTest, false, NULL );
}
END_TEST

START_TEST ( llist_07_test_stack_mt )
{
    int retval;
    llist listToTest = NULL;
    llist_node retptr;
    listToTest = llist_create ( NULL, NULL, MT_SUPPORT_TRUE );

    // Push 1000 nodes
    for ( unsigned long i = 0; i < 1000; i ++ )
    {
        retval = llist_push ( listToTest, ( llist_node ) i );
        ck_assert_int_eq ( retval, LLIST_SUCCESS );
    }

    // Peek at the head
    retptr = llist_peek ( listToTest );
    ck_assert_int_eq ( ( unsigned long ) retptr, 999 );
    for ( unsigned long i = 999; i > 0; i-- )
    {
        retptr = llist_pop ( listToTest );
        ck_assert_int_eq ( ( unsigned long ) retptr, i );
    }

    printf ( "Count = %d\n", llist_size ( listToTest ) );
    retptr = llist_pop ( listToTest );
    ck_assert_ptr_eq ( retptr, NULL );

    llist_destroy ( listToTest, false, NULL );
}
END_TEST

START_TEST ( llist_08_list_reverse )
{
    int retval;
    llist listToTest = NULL;
    listToTest = llist_create ( NULL, NULL, MT_SUPPORT_FALSE );

    // Insert a 5 nodes 1..5
    retval = llist_add_node ( listToTest, ( llist_node ) 1, ADD_NODE_REAR );
    retval = llist_add_node ( listToTest, ( llist_node ) 2, ADD_NODE_REAR );
    retval = llist_add_node ( listToTest, ( llist_node ) 3, ADD_NODE_REAR );
    retval = llist_add_node ( listToTest, ( llist_node ) 4, ADD_NODE_REAR );
    retval = llist_add_node ( listToTest, ( llist_node ) 5, ADD_NODE_REAR );

    printf ( "List before reversing: " );
    print_llist ( listToTest );

    retval = llist_reverse ( listToTest );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List After reversing: " );
    print_llist ( listToTest );

    llist_destroy ( listToTest, false, NULL );
}
END_TEST

START_TEST ( llist_08_list_reverse_mt )
{
    int retval;
    llist listToTest = NULL;
    listToTest = llist_create ( NULL, NULL, MT_SUPPORT_TRUE );

    // Insert a 5 nodes 1..5
    retval = llist_add_node ( listToTest, ( llist_node ) 1, ADD_NODE_REAR );
    retval = llist_add_node ( listToTest, ( llist_node ) 2, ADD_NODE_REAR );
    retval = llist_add_node ( listToTest, ( llist_node ) 3, ADD_NODE_REAR );
    retval = llist_add_node ( listToTest, ( llist_node ) 4, ADD_NODE_REAR );
    retval = llist_add_node ( listToTest, ( llist_node ) 5, ADD_NODE_REAR );

    printf ( "List before reversing: " );
    print_llist ( listToTest );

    retval = llist_reverse ( listToTest );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List After reversing: " );
    print_llist ( listToTest );

    llist_destroy ( listToTest, false, NULL );
}
END_TEST

START_TEST ( llist_09_list_sort )
{
    int retval;
    llist listToTest = NULL;
    listToTest = llist_create ( trivial_comperator , NULL, MT_SUPPORT_FALSE );

    // Insert a 5 nodes 1..5
    retval = llist_add_node ( listToTest, ( llist_node ) 3, ADD_NODE_REAR );
    retval = llist_add_node ( listToTest, ( llist_node ) 2, ADD_NODE_REAR );
    retval = llist_add_node ( listToTest, ( llist_node ) 1, ADD_NODE_REAR );
    retval = llist_add_node ( listToTest, ( llist_node ) 4, ADD_NODE_REAR );
    retval = llist_add_node ( listToTest, ( llist_node ) 5, ADD_NODE_REAR );

    printf ( "List before sorting: " );
    print_llist ( listToTest );

    retval = llist_sort ( listToTest, NULL, SORT_LIST_ASCENDING );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List After sorting ascending: " );
    print_llist ( listToTest );

    retval = llist_sort ( listToTest, NULL, SORT_LIST_DESCENDING );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List After sorting descending: " );
    print_llist ( listToTest );

    llist_destroy ( listToTest, false, NULL );
}
END_TEST

START_TEST ( llist_09_list_sort_mt )
{
    int retval;
    llist listToTest = NULL;
    listToTest = llist_create ( trivial_comperator , NULL, MT_SUPPORT_TRUE );

    // Insert a 5 nodes 1..5
    retval = llist_add_node ( listToTest, ( llist_node ) 3, ADD_NODE_REAR );
    retval = llist_add_node ( listToTest, ( llist_node ) 2, ADD_NODE_REAR );
    retval = llist_add_node ( listToTest, ( llist_node ) 1, ADD_NODE_REAR );
    retval = llist_add_node ( listToTest, ( llist_node ) 4, ADD_NODE_REAR );
    retval = llist_add_node ( listToTest, ( llist_node ) 5, ADD_NODE_REAR );

    printf ( "List before sorting: " );
    print_llist ( listToTest );

    retval = llist_sort ( listToTest, NULL, SORT_LIST_ASCENDING );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List After sorting ascending: " );
    print_llist ( listToTest );

    retval = llist_sort ( listToTest, NULL, SORT_LIST_DESCENDING );
    ck_assert_int_eq ( retval, LLIST_SUCCESS );

    printf ( "List After sorting descending: " );
    print_llist ( listToTest );

    llist_destroy ( listToTest, false, NULL );
}
END_TEST

void * list_put_data(void * arg)
{
    ptrdiff_t i = 50;
    int retval;
    ck_assert ( arg != NULL );
    llist listToTest = (llist)arg;
    while( i > 0)
    {
        retval = llist_add_node ( listToTest, ( llist_node ) i, ADD_NODE_REAR );
        ck_assert_int_eq ( retval, LLIST_SUCCESS );
        i --;
    }
    return NULL;
}

void * list_get_data(void * arg)
{
    int retval;
    ck_assert ( arg != NULL );
    llist listToTest = (llist)arg;


    start_waiting:

    retval = llist_delete_node ( listToTest, ( llist_node ) 4, trivial_equal, false, NULL );
    if ( LLIST_SUCCESS != retval)
    {
        //wait for 100 milliseconds
        printf("find attempted");
        usleep(1000*100);
        //printf ("the address to jump %p", &&start_waiting);
        goto start_waiting;
    }
    return NULL;
}

START_TEST ( llist_10_pure_add_delete_mt )
{

    int rc = 0;
    int i = 0;
    pthread_t threads[2];

    llist listToTest = NULL;
    listToTest = llist_create ( trivial_comperator , NULL, MT_SUPPORT_TRUE );

    rc = pthread_create(&threads[1], NULL, list_get_data, listToTest);
    ck_assert_int_eq(rc, 0);

    rc = pthread_create(&threads[0], NULL, list_put_data, listToTest);
    ck_assert_int_eq(rc, 0);



    for( i = 0; i < 2; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf ( "list after deletion: " );
    print_llist ( listToTest );


    rc = llist_delete_node ( listToTest, ( llist_node ) 4, trivial_equal, false, NULL );
    ck_assert( LLIST_SUCCESS != rc);


    llist_destroy ( listToTest, false, NULL );

}
END_TEST


Suite *liblist_suite ( void )
{
    Suite *s = suite_create ( "Lib linked list tester" );

    /* Core test case */
    TCase *tc_core = tcase_create ( "Core" );
    tcase_add_test ( tc_core, llist_01_create_delete_lists );
    tcase_add_test ( tc_core, llist_01_create_delete_lists_mt );
    tcase_add_test ( tc_core, llist_02_add_nodes );
    tcase_add_test ( tc_core, llist_02_add_nodes_mt );
    tcase_add_test ( tc_core, llist_03_add_dynamic_nodes );
    tcase_add_test ( tc_core, llist_03_add_dynamic_nodes_mt );
    tcase_add_test ( tc_core, llist_04_delete_nodes );
    tcase_add_test ( tc_core, llist_04_delete_nodes_mt );
    tcase_add_test ( tc_core, llist_05_list_for_each );
    tcase_add_test ( tc_core, llist_05_list_for_each_mt );
    tcase_add_test ( tc_core, llist_06_insert_nodes );
    tcase_add_test ( tc_core, llist_06_insert_nodes_mt );
    tcase_add_test ( tc_core, llist_07_test_stack );
    tcase_add_test ( tc_core, llist_07_test_stack_mt );
    tcase_add_test ( tc_core, llist_08_list_reverse );
    tcase_add_test ( tc_core, llist_08_list_reverse_mt );
    tcase_add_test ( tc_core, llist_09_list_sort );
    tcase_add_test ( tc_core, llist_09_list_sort_mt );

    //really multithreaded test case
    tcase_add_test ( tc_core, llist_10_pure_add_delete_mt );

    suite_add_tcase ( s, tc_core );

    return s;
}

int main ( void )
{
    int number_failed;
    Suite *s = liblist_suite ();
    SRunner *sr = srunner_create ( s );
    srunner_set_fork_status ( sr, CK_NOFORK );
    srunner_run_all ( sr, CK_VERBOSE );
    number_failed = srunner_ntests_failed ( sr );
    srunner_free ( sr );
    return ( number_failed == 0 ) ? EXIT_SUCCESS : EXIT_FAILURE;
}



