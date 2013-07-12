#include <stdlib.h>
#include <check.h>
#include "../inc/llist.h"


START_TEST ( llist_01_create_delete_lists )
{
	llist listToTest = NULL;
	listToTest = llist_create ( NULL,NULL );
	ck_assert_ptr_ne ( listToTest, NULL );
	llist_destroy(listToTest,false,NULL);
}
END_TEST

bool llist_02_insert_equal ( llist_node node1, llist_node node2)
{
	return (node1 == node2);
}

START_TEST ( llist_02_insert_nodes )
{
	int retval;
	llist listToTest = NULL;
	llist_node * retptr;
	listToTest = llist_create ( NULL,NULL );
	
	// Insert a single node
	retval = llist_add_node(listToTest,(llist_node) 1, ADD_NODE_FRONT);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	// Find that specific node, this should fail because no equal function was provided
	retptr = llist_find_node(listToTest, 1, NULL);
	ck_assert_ptr_eq(retptr,NULL);
	
	// find again, but this time, let's provide an equals function
	retptr = llist_find_node(listToTest, 1, llist_02_insert_equal);
	ck_assert_ptr_eq(retptr,1);
	
	llist_destroy(listToTest,false,NULL);
}
END_TEST



Suite * liblist_suite ( void )
{
	Suite *s = suite_create ( "Lib linked list tester" );

	/* Core test case */
	TCase *tc_core = tcase_create ( "Core" );
	tcase_add_test ( tc_core, llist_01_create_delete_lists );
	tcase_add_test ( tc_core, llist_02_insert_nodes );
	suite_add_tcase ( s, tc_core );

	return s;
}

int main ( void )
{
	int number_failed;
	Suite *s = liblist_suite ();
	SRunner *sr = srunner_create ( s );
	srunner_run_all ( sr, CK_NORMAL );
	number_failed = srunner_ntests_failed ( sr );
	srunner_free ( sr );
	return ( number_failed == 0 ) ? EXIT_SUCCESS : EXIT_FAILURE;
}
