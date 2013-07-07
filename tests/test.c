#include <stdlib.h>
#include <check.h>
#include "../inc/llist.h"


START_TEST ( create_delete_lists )
{
	llist listToTest = NULL;
	listToTest = llist_create ( NULL,NULL );
	ck_assert_ptr_ne ( listToTest, NULL );
	llist_destroy(listToTest,false,NULL);
}
END_TEST

START_TEST ( insert_nodes )
{
	int retval;
	llist listToTest = NULL;
	listToTest = llist_create ( NULL,NULL );
	
	
	retval = llist_add_node(listToTest,(llist_node) 1, ADD_NODE_FRONT);
	ck_assert_int_eq(retval,LLIST_SUCCESS);
	
	llist_destroy(listToTest,false,NULL);
}
END_TEST



Suite * liblist_suite ( void )
{
	Suite *s = suite_create ( "Money" );

	/* Core test case */
	TCase *tc_core = tcase_create ( "Core" );
	tcase_add_test ( tc_core, create_delete_lists );
	tcase_add_test ( tc_core, insert_nodes );
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
