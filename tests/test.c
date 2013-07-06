#include <stdlib.h>
#include <check.h>
#include "../inc/llist.h"


START_TEST ( create_delete_lists )
{
	llist listToTest = NULL;
	listToTest = llist_create ( NULL,NULL );
	ck_assert_ptr_ne ( listToTest,NULL );
}
END_TEST


Suite * liblist_suite ( void )
{
	Suite *s = suite_create ( "Money" );

	/* Core test case */
	TCase *tc_core = tcase_create ( "Core" );
	tcase_add_test ( tc_core, create_delete_lists );
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
