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
#include <check.h>
#include "../inc/llist.h"



bool trivial_equal ( llist_node node1, llist_node node2)
{
	return (node1 == node2);
}

void trivial_node_func ( llist_node node )
{
	int * i = (int *) node;
	i = i;
}

START_TEST ( llist_01_create_delete_lists )
{
	llist listToTest = NULL;
	listToTest = llist_create ( NULL,NULL );
	ck_assert_ptr_ne ( listToTest, NULL );
	llist_destroy(listToTest,false,NULL);
}
END_TEST

START_TEST ( llist_02_add_nodes )
{
	int retval;
	llist listToTest = NULL;
	llist_node retptr;
	listToTest = llist_create ( NULL,NULL );
	
	// Insert a 5 nodes 1..5
	retval = llist_add_node(listToTest,(llist_node) 1, ADD_NODE_FRONT);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	retval = llist_add_node(listToTest,(llist_node) 2, ADD_NODE_FRONT);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	retval = llist_add_node(listToTest,(llist_node) 3, ADD_NODE_FRONT);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	retval = llist_add_node(listToTest,(llist_node) 4, ADD_NODE_FRONT);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	retval = llist_add_node(listToTest,(llist_node) 5, ADD_NODE_FRONT);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	// Find that specific node, this should fail because no equal function was provided
	retval = llist_find_node(listToTest,(llist_node) 1, &retptr, NULL);
	ck_assert_int_eq(retval,LLIST_EQUAL_MISSING);

	// find again, but this time, let's provide an equals function
	retval = llist_find_node(listToTest,(llist_node) 1, &retptr, trivial_equal);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	ck_assert_ptr_eq(retptr,(llist_node) 1);
	
	// find again, but this time, with a non existant node
	retval = llist_find_node(listToTest,(llist_node) 6, &retptr, trivial_equal);
	ck_assert_int_eq(retval,LLIST_NODE_NOT_FOUND);
	
	llist_destroy(listToTest,false,NULL);
}
END_TEST


START_TEST ( llist_03_add_dynamic_nodes )
{
	int retval;
	int *data[5];
	
	for (int i = 0; i < 5; i++)
	{
		data[i] = malloc(sizeof(int));
	}
	
	llist listToTest = NULL;
	llist_node retptr;
	listToTest = llist_create ( NULL,NULL );
	
	// Insert a 5 nodes 1..5
	retval = llist_add_node(listToTest,(llist_node) data[0], ADD_NODE_FRONT);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	retval = llist_add_node(listToTest,(llist_node) data[1], ADD_NODE_REAR);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	retval = llist_add_node(listToTest,(llist_node) data[2], ADD_NODE_FRONT);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	retval = llist_add_node(listToTest,(llist_node) data[3], ADD_NODE_REAR);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	retval = llist_add_node(listToTest,(llist_node) data[4], ADD_NODE_FRONT);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	// find node
	retval = llist_find_node(listToTest,(llist_node) data[2], &retptr, trivial_equal);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	ck_assert_ptr_eq(retptr,(llist_node) data[2]);
	
	// destroy list, and also free the data
	llist_destroy(listToTest,true,NULL);
}
END_TEST

START_TEST ( llist_04_delete_nodes )
{
	int retval;
	llist listToTest = NULL;
	listToTest = llist_create ( NULL,NULL );
	
	// Insert a 5 nodes 1..5
	retval = llist_add_node(listToTest,(llist_node) 1, ADD_NODE_FRONT);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	retval = llist_add_node(listToTest,(llist_node) 2, ADD_NODE_FRONT);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	retval = llist_add_node(listToTest,(llist_node) 3, ADD_NODE_FRONT);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	retval = llist_add_node(listToTest,(llist_node) 4, ADD_NODE_FRONT);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	retval = llist_add_node(listToTest,(llist_node) 5, ADD_NODE_FRONT);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	// This should fail because no equal function was given
	retval = llist_delete_node(listToTest, (llist_node) 1, NULL, false, NULL);
	ck_assert_int_eq(retval, LLIST_EQUAL_MISSING);
	
	// Delete tail
	retval = llist_delete_node(listToTest, (llist_node) 1, trivial_equal, false, NULL);
	ck_assert_int_eq(retval, LLIST_SUCCESS);
	
	
	// Delete node in the middle
	retval = llist_delete_node(listToTest, (llist_node) 3, trivial_equal, false, NULL);
	ck_assert_int_eq(retval, LLIST_SUCCESS);
	
	// Delete head
	retval = llist_delete_node(listToTest, (llist_node) 5, trivial_equal, false, NULL);
	ck_assert_int_eq(retval, LLIST_SUCCESS);

	// Delete a node that doesn't exist
	retval = llist_delete_node(listToTest, (llist_node) 6, trivial_equal, false, NULL);
	ck_assert_int_eq(retval, LLIST_NODE_NOT_FOUND);	

	llist_destroy(listToTest,false,NULL);
}
END_TEST


START_TEST ( llist_05_list_for_each )
{
	int retval;
	llist listToTest = NULL;
	listToTest = llist_create ( NULL,NULL );
	
	// Insert a 5 nodes 1..5
	retval = llist_add_node(listToTest,(llist_node) 1, ADD_NODE_REAR);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	retval = llist_add_node(listToTest,(llist_node) 2, ADD_NODE_REAR);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	retval = llist_add_node(listToTest,(llist_node) 3, ADD_NODE_REAR);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	retval = llist_add_node(listToTest,(llist_node) 4, ADD_NODE_REAR);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	retval = llist_add_node(listToTest,(llist_node) 5, ADD_NODE_REAR);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	retval = llist_for_each(listToTest, trivial_node_func);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	llist_destroy(listToTest,false,NULL);
}
END_TEST


START_TEST ( llist_06_insert_nodes )
{
	int retval;
	llist listToTest = NULL;
	llist_node retptr;
	listToTest = llist_create ( NULL,NULL );
	
	// Insert a 5 nodes 1..5
	retval = llist_add_node(listToTest,(llist_node) 1, ADD_NODE_FRONT);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	retval = llist_add_node(listToTest,(llist_node) 2, ADD_NODE_FRONT);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	retval = llist_add_node(listToTest,(llist_node) 3, ADD_NODE_FRONT);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	retval = llist_add_node(listToTest,(llist_node) 4, ADD_NODE_FRONT);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	retval = llist_add_node(listToTest,(llist_node) 5, ADD_NODE_FRONT);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	
	// Find the middle node (3)
	retval = llist_find_node(listToTest,(llist_node) 3, &retptr, trivial_equal);
	ck_assert_int_eq(retval,LLIST_SUCCESS);

	// Add node before
	retval =  llist_insert_node ( listToTest,  (llist_node) 7, retptr, ADD_NODE_BEFORE );
	ck_assert_int_eq(retval,LLIST_SUCCESS);

	// Add node after
	retval =  llist_insert_node ( listToTest,  (llist_node) 8, retptr, ADD_NODE_AFTER );
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	/*
	 * TODO: Check inserting to head and tail
	 */
	
	llist_destroy(listToTest,false,NULL);
}
END_TEST


Suite * liblist_suite ( void )
{
	Suite *s = suite_create ( "Lib linked list tester" );

	/* Core test case */
	TCase *tc_core = tcase_create ( "Core" );
	tcase_add_test ( tc_core, llist_01_create_delete_lists );
	tcase_add_test ( tc_core, llist_02_add_nodes );
	tcase_add_test ( tc_core, llist_03_add_dynamic_nodes );
	tcase_add_test ( tc_core, llist_04_delete_nodes);
	tcase_add_test ( tc_core, llist_05_list_for_each);
	tcase_add_test ( tc_core, llist_06_insert_nodes);
	suite_add_tcase ( s, tc_core );

	return s;
}

int main ( void )
{
	int number_failed;
	Suite *s = liblist_suite ();
	SRunner *sr = srunner_create ( s );
	srunner_set_fork_status (sr, CK_NOFORK);
	srunner_run_all ( sr, CK_VERBOSE );
	number_failed = srunner_ntests_failed ( sr );
	srunner_free ( sr );
	return ( number_failed == 0 ) ? EXIT_SUCCESS : EXIT_FAILURE;
}
