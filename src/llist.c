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
#include <pthread.h>

#define LOG_FUNC_ENTRANCE() printf("%lu: In %s\n", time(NULL), __PRETTY_FUNCTION__);

#define WRITE_LOCK(list, err_code) \
    if ( TRUE == (( _llist * )list)->ismt ) \
    { \
        int rc = pthread_rwlock_wrlock(&( ( _llist * ) list )->llist_lock); \
        if (rc != 0)\
        {\
            return err_code;\
        }\
    }

#define READ_LOCK(list, err_code) \
    if ( TRUE == (( _llist * )list)->ismt ) \
    { \
        int rc = pthread_rwlock_rdlock(&( ( _llist * ) list )->llist_lock); \
        if (rc != 0)\
        {\
            return err_code;\
        }\
    }

#define UNLOCK(list, err_code) \
    if ( TRUE == (( _llist * )list)->ismt ) \
    { \
        int rc = pthread_rwlock_unlock(&( ( _llist * ) list )->llist_lock); \
        if (rc != 0)\
        {\
            return err_code;\
        }\
    }

typedef struct __list_node
{
    llist_node node;
    struct __list_node *next;
} _list_node;

typedef struct
{
    unsigned int count;
    comperator comp_func;
    equal equal_func;
    _list_node *head;
    _list_node *tail;

    //multi-threading support
    unsigned char ismt;
    pthread_rwlockattr_t llist_lock_attr;
    pthread_rwlock_t llist_lock;

    //abort signal identifier
    unsigned char should_abort_looping;

} _llist;

/* Helper functions - not to be exported */
static _list_node *listsort ( _list_node *list, _list_node ** updated_tail, comperator cmp, int flags );

llist llist_create ( comperator compare_func, equal equal_func, unsigned flags)
{
    _llist *new_list;
    int rc = 0;
    new_list = malloc ( sizeof ( _llist ) );

    if ( new_list == NULL )
    {
        return NULL;
    }

// These can be NULL, I don't care...
    new_list->equal_func = equal_func;
    new_list->comp_func = compare_func;

// Reset the list
    new_list->count = 0;
    new_list->head = NULL;
    new_list->tail = NULL;

    new_list->ismt = FALSE;
    if ( flags & MT_SUPPORT_TRUE)
    {
	new_list->should_abort_looping = FALSE;
        new_list->ismt = TRUE;
        rc = pthread_rwlockattr_setpshared( &new_list->llist_lock_attr,  PTHREAD_PROCESS_PRIVATE );
        if ( 0 != rc)
        {
            free(new_list);
            return NULL;
        }
        rc = pthread_rwlock_init( &new_list->llist_lock, &new_list->llist_lock_attr );
        if ( 0 != rc)
        {
            pthread_rwlockattr_destroy(&new_list->llist_lock_attr);
            free(new_list);
            return NULL;
        }
    }

    return new_list;
}


int llist_set_abort_condition( llist list , E_LLIST_LOOP_CONDITION cond )
{
    if ( NULL == list ) return LLIST_NULL_ARGUMENT; 

    ( ( _llist * ) list )->should_abort_looping = cond;
    return LLIST_SUCCESS; 
}

int llist_get_abort_condition( llist list , E_LLIST_LOOP_CONDITION* cond)
{
    if ( NULL == list ) return LLIST_NULL_ARGUMENT;

    *cond = ( ( _llist * ) list )->should_abort_looping;
    return LLIST_SUCCESS; 
}

void llist_destroy ( llist list, bool destroy_nodes, node_func destructor )
{
    _list_node *iterator;
    _list_node *next;

    if ( list == NULL )
    {
        return;
    }
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



    if ( TRUE == (( _llist * )list)->ismt )
    {
        //release any thread related resource, just try to destroy no use checking return code
        pthread_rwlockattr_destroy(&( ( _llist * ) list )->llist_lock_attr);
        pthread_rwlock_destroy( &( ( _llist * ) list )->llist_lock);
    }
    //release the list
    free ( list );

    return;

}

int llist_size ( llist list )
{
    unsigned int retval = 0;
    if ( list == NULL )
    {
        return 0;
    }
    READ_LOCK( list, LLIST_MULTITHREAD_ISSUE )

    {
        //read only critical section
        retval = ( ( _llist * ) list )->count;
    }

    UNLOCK( list, LLIST_MULTITHREAD_ISSUE )

    return retval;
}

int llist_add_node ( llist list, llist_node node, int flags )
{
    _list_node *node_wrapper = NULL;

    if ( list == NULL )
    {
        return LLIST_NULL_ARGUMENT;
    }

    WRITE_LOCK( list, LLIST_MULTITHREAD_ISSUE )

    {   //write critical section
        node_wrapper = malloc ( sizeof ( _list_node ) );

        if ( node_wrapper == NULL )
        {
            UNLOCK( list, LLIST_MULTITHREAD_ISSUE )
            return LLIST_ERROR;
        }

        node_wrapper->node = node;

        ( ( _llist * ) list )->count++;

        if ( ( ( _llist * ) list )->head == NULL ) // Adding the first node, update head and tail to point to that node
        {
            node_wrapper->next = NULL;
            ( ( _llist * ) list )->head = ( ( _llist * ) list )->tail = node_wrapper;
        }
        else if ( flags & ADD_NODE_FRONT )
        {
            node_wrapper->next = ( ( _llist * ) list )->head;
            ( ( _llist * ) list )->head = node_wrapper;
        }
        else   // add node in the rear
        {
            node_wrapper->next = NULL;
            ( ( _llist * ) list )->tail->next = node_wrapper;
            ( ( _llist * ) list )->tail = node_wrapper;
        }
    }

    UNLOCK( list, LLIST_MULTITHREAD_ISSUE )

    return LLIST_SUCCESS;
}

int llist_delete_node ( llist list, llist_node node,
		bool destroy_node, node_func destructor )
{
    _list_node *iterator;
    _list_node *temp;
    equal actual_equal;

    if ( ( list == NULL ) || ( node == NULL ) )
    {
        return LLIST_NULL_ARGUMENT;
    }

    actual_equal = ( ( _llist * ) list )->equal_func;

	if ( actual_equal == NULL )
    {
        return LLIST_EQUAL_MISSING;
    }

    WRITE_LOCK( list, LLIST_MULTITHREAD_ISSUE )

    {

        iterator = ( ( _llist * ) list )->head;

        if ( NULL == iterator)
        {
            UNLOCK( list, LLIST_MULTITHREAD_ISSUE );
            return LLIST_NODE_NOT_FOUND;
        }


        // is it the first node ?
        if ( actual_equal ( iterator->node, node ) )
        {
            if ( destroy_node )
            {
                if ( destructor )
                {
                    destructor ( iterator->node);
                }
                else
                {
                    free ( iterator->node );
                }

            }

            ( ( _llist * ) list )->head = ( ( _llist * ) list )->head->next;
            free ( iterator );
            ( ( _llist * ) list )->count--;

            if ( ( ( _llist * ) list )->count == 0 )
            {
                /*
                 *  if we deleted the last node, we need to reset the tail also
                 *  There's no need to check it somewhere else, because the last node must be the head (and tail)
                 */
                ( ( _llist * ) list )->tail = NULL;
            }
            //assert ( ( ( _llist * ) list )->count >= 0 );
            UNLOCK( list, LLIST_MULTITHREAD_ISSUE );
            return LLIST_SUCCESS;
        }
        else
        {
            while ( iterator->next != NULL )
            {
                if ( actual_equal ( iterator->next->node, node ) )
                {
                    // found it
                    temp = iterator->next;
                    iterator->next = temp->next;
                    free ( temp );

                    ( ( _llist * ) list )->count--;
                    //assert ( ( ( _llist * ) list )->count >= 0 );

                    UNLOCK( list, LLIST_MULTITHREAD_ISSUE );
                    return LLIST_SUCCESS;
                }

                iterator = iterator->next;
            }
        }

        if ( iterator->next == NULL )
        {
            UNLOCK( list,LLIST_MULTITHREAD_ISSUE );
            return LLIST_NODE_NOT_FOUND;
        }

    }

    //assert ( 1 == 2 );
    // this assert always failed. we assume that the function never gets here...
    UNLOCK( list, LLIST_MULTITHREAD_ISSUE );
    return LLIST_ERROR;
}

int llist_for_each ( llist list, node_func func )
{
    _list_node *iterator;

    if ( ( list == NULL ) || ( func == NULL ) )
    {
        return LLIST_NULL_ARGUMENT;
    }


    iterator = ( ( _llist * ) list )->head;

    while ( iterator != NULL )
    {
        if ( ( ( _llist * ) list )->should_abort_looping == LOOP_ABORT_TRUE )
            return LLIST_ITERARTIONS_ABORTED;

        //func should only read the node but not modify its contents
	//user should ensure thread safety
        func ( iterator->node );
        iterator = iterator->next;
    }
    return LLIST_SUCCESS;
}

int llist_for_each_arg_read_only ( llist list, node_func_arg r_only_func, void * arg )
{
    _list_node *iterator;

    if ( ( list == NULL ) || ( r_only_func  == NULL ) )
    {
        return LLIST_NULL_ARGUMENT;
    }

    READ_LOCK( list, LLIST_MULTITHREAD_ISSUE )


    iterator = ( ( _llist * ) list )->head;

    while ( iterator != NULL )
    {
       if ( ( ( _llist * ) list )->should_abort_looping == LOOP_ABORT_TRUE )
       {
           UNLOCK( list, LLIST_MULTITHREAD_ISSUE )
           return LLIST_ITERARTIONS_ABORTED;
       }
       //func should only read the node but not modify its contents
       r_only_func ( iterator->node , arg);
       iterator = iterator->next;
    }

    UNLOCK( list, LLIST_MULTITHREAD_ISSUE )

    return LLIST_SUCCESS;
}


int llist_for_each_arg ( llist list, node_func_arg func, void * arg )
{
    _list_node *iterator;

    if ( ( list == NULL ) || ( func == NULL ) )
    {
        return LLIST_NULL_ARGUMENT;
    }

    WRITE_LOCK( list, LLIST_MULTITHREAD_ISSUE )

    iterator = ( ( _llist * ) list )->head;

    while ( iterator != NULL )
    {
        if ( ( ( _llist * ) list )->should_abort_looping == LOOP_ABORT_TRUE )
        {
           UNLOCK( list, LLIST_MULTITHREAD_ISSUE )
           return LLIST_ITERARTIONS_ABORTED;
        }
        //func can not only read the node but also modify its contents
        func ( iterator->node , arg);
        iterator = iterator->next;
    }

    UNLOCK( list, LLIST_MULTITHREAD_ISSUE )

    return LLIST_SUCCESS;
}


int llist_insert_node ( llist list, llist_node new_node, llist_node pos_node,
                        int flags )
{
    _list_node *iterator;
    _list_node *node_wrapper = NULL;

    if ( ( list == NULL ) || ( new_node == NULL ) || ( pos_node == NULL ) )
    {
        return LLIST_NULL_ARGUMENT;
    }

    WRITE_LOCK( list, LLIST_MULTITHREAD_ISSUE )

    {
        node_wrapper = malloc ( sizeof ( _list_node ) );

        if ( node_wrapper == NULL )
        {
            UNLOCK( list, LLIST_MULTITHREAD_ISSUE )
            return LLIST_MALLOC_ERROR;
        }

        node_wrapper->node = new_node;

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
            UNLOCK( list, LLIST_MULTITHREAD_ISSUE )
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
                UNLOCK( list, LLIST_MULTITHREAD_ISSUE )
                return LLIST_SUCCESS;
            }

            iterator = iterator->next;
        }

    }
    UNLOCK( list, LLIST_MULTITHREAD_ISSUE )

    assert ( 1 == 2 );
    // this assert will always fail. we assume that the function never gets here...
    return LLIST_ERROR;

}

int llist_find_node ( llist list, void *data, llist_node *found)
{
    _list_node *iterator;
    equal actual_equal;
    if ( list == NULL )
    {
        return LLIST_NULL_ARGUMENT;
    }

    actual_equal = ( ( _llist * ) list )->equal_func;

    if ( actual_equal == NULL )
    {
        return LLIST_EQUAL_MISSING;
    }

    READ_LOCK( list, LLIST_MULTITHREAD_ISSUE )

    {
        iterator = ( ( _llist * ) list )->head;

        while ( iterator != NULL )
        {
            if ( actual_equal ( iterator->node, data ) )
            {

                *found = iterator->node;
                UNLOCK( list, LLIST_MULTITHREAD_ISSUE )
                return LLIST_SUCCESS;
            }

            iterator = iterator->next;
        }
    }

    UNLOCK( list, LLIST_MULTITHREAD_ISSUE )

    // Didn't find the node
    return LLIST_NODE_NOT_FOUND;

}

llist_node llist_get_head ( llist list )
{
    READ_LOCK( list, NULL )

    {
        if ( list != NULL )
        {
            if ( ( ( _llist * ) list )->head ) // there's at least one node
            {
                UNLOCK( list, NULL )
                return ( ( _llist * ) list )->head->node;
            }
        }
    }

    UNLOCK( list, NULL )

    return NULL;
}

llist_node llist_get_tail ( llist list )
{
    READ_LOCK( list, NULL )

    {
        if ( list != NULL )
        {
            if ( ( ( _llist * ) list )->tail ) // there's at least one node
            {
                UNLOCK(list, NULL)
                return ( ( _llist * ) list )->tail->node;
            }
        }
     }

    UNLOCK(list, NULL)

    return NULL;
}

int llist_push ( llist list, llist_node node )
{
    return llist_add_node ( list, node, ADD_NODE_FRONT );
}

llist_node llist_peek ( llist list )
{
    return llist_get_head ( list );
}

llist_node llist_pop ( llist list )
{
    llist_node tempnode = NULL;
    _list_node *tempwrapper;

    WRITE_LOCK( list, NULL )

    {
        if ( ( ( _llist * ) list )->count ) // There exists at least one node
        {
            tempwrapper = ( ( _llist * ) list )->head;
            tempnode = tempwrapper->node;
            ( ( _llist * ) list )->head = ( ( _llist * ) list )->head->next;
            ( ( _llist * ) list )->count--;
            free ( tempwrapper );

            if ( ( ( _llist * ) list )->count == 0 ) // We've deleted the last node
            {
                ( ( _llist * ) list )->tail = NULL;
            }
        }
    }

    UNLOCK(list, NULL)

    return tempnode;
}

int llist_concat ( llist first, llist second )
{
    _list_node *end_node;

    if ( ( first == NULL ) || ( second == NULL ) )
    {
        return LLIST_NULL_ARGUMENT;
    }

    WRITE_LOCK( first, LLIST_MULTITHREAD_ISSUE )
    WRITE_LOCK( second, LLIST_MULTITHREAD_ISSUE )

    {

        end_node = ( ( _llist * ) first )->tail;

        ( ( _llist * ) first )->count += ( ( _llist * ) second )->count;

        if ( end_node != NULL ) // if the first list is not empty
        {
            end_node->next = ( ( _llist * ) second )->head;
        }
        else     // It's empty
        {
            ( ( _llist * ) first )->head = ( ( _llist * ) first )->tail =
                    ( ( _llist * ) second )->head;
        }

        // Delete the nodes from the second list. (not really deletes them, only loses their reference.
        ( ( _llist * ) second )->count = 0;
        ( ( _llist * ) second )->head = ( ( _llist * ) second )->tail = NULL;
    }

    UNLOCK( first, LLIST_MULTITHREAD_ISSUE )
    UNLOCK( second, LLIST_MULTITHREAD_ISSUE )

    return LLIST_SUCCESS;
}

int llist_reverse ( llist list )
{
    if ( list == NULL )
    {
        return LLIST_NULL_ARGUMENT;
    }


    WRITE_LOCK( list, LLIST_MULTITHREAD_ISSUE )

    {

        _list_node *iterator = ( ( _llist * ) list )->head;
        _list_node *nextnode = NULL;
        _list_node *temp = NULL;

        /*
         * Swap our Head & Tail pointers
         */
        ( ( _llist * ) list )->head = ( ( _llist * ) list )->tail;
        ( ( _llist * ) list )->tail = iterator;

        /*
         * Swap the internals
         */
        while ( iterator )
        {
            nextnode = iterator->next;
            iterator->next = temp;
            temp = iterator;
            iterator = nextnode;
        }

    }

    UNLOCK( list, LLIST_MULTITHREAD_ISSUE )

    return LLIST_SUCCESS;
}

int llist_sort ( llist list, int flags )
{

    comperator cmp;
    if ( list == NULL )
    {
        return LLIST_NULL_ARGUMENT;
    }

 	_llist *thelist = ( _llist * ) list;

    cmp =  thelist->comp_func;

 	if ( cmp == NULL )
    {
        return LLIST_COMPERATOR_MISSING;
    }
    WRITE_LOCK( list, LLIST_MULTITHREAD_ISSUE )
    thelist->head = listsort ( thelist->head, &thelist->tail, cmp, flags);
    UNLOCK( list, LLIST_MULTITHREAD_ISSUE )
    /*
     * TODO: update list tail.
     */
    return LLIST_SUCCESS;
}

static _list_node *listsort ( _list_node *list, _list_node ** updated_tail, comperator cmp , int flags)
{
    _list_node *p, *q, *e, *tail;
    int insize, nmerges, psize, qsize, i;
    int direction = ( flags & SORT_LIST_ASCENDING ) ? 1 : -1;
    insize = 1;

    while ( 1 )
    {
        p = list;
        list = NULL;
        tail = NULL;

        nmerges = 0; /* count number of merges we do in this pass */

        while ( p )
        {
            nmerges++; /* there exists a merge to be done */
            /* step `insize' places along from p */
            q = p;
            psize = 0;
            for ( i = 0; i < insize; i++ )
            {
                psize++;
                q = q->next;
                if ( !q )
                {
                    break;
                }
            }

            /* if q hasn't fallen off end, we have two lists to merge */
            qsize = insize;

            /* now we have two lists; merge them */
            while ( psize > 0 || ( qsize > 0 && q ) )
            {

                /* decide whether next element of merge comes from p or q */
                if ( psize == 0 )
                {
                    /* p is empty; e must come from q. */
                    e = q;
                    q = q->next;
                    qsize--;
                }
                else
                    if ( qsize == 0 || !q )
                    {
                        /* q is empty; e must come from p. */
                        e = p;
                        p = p->next;
                        psize--;
                    }
                    else
                        if ( ( direction * cmp ( p->node, q->node ) ) <= 0 )
                        {
                            /* First element of p is lower (or same);
                             * e must come from p. */
                            e = p;
                            p = p->next;
                            psize--;
                        }
                        else
                        {
                            /* First element of q is lower; e must come from q. */
                            e = q;
                            q = q->next;
                            qsize--;
                        }

                /* add the next element to the merged list */
                if ( tail )
                {
                    tail->next = e;
                }
                else
                {
                    list = e;
                }

                tail = e;
            }

            /* now p has stepped `insize' places along, and q has too */
            p = q;
        }

        tail->next = NULL;

        /* If we have done only one merge, we're finished. */
        if ( nmerges <= 1 ) /* allow for nmerges==0, the empty list case */
        {
            break;
        }
        /* Otherwise repeat, merging lists twice the size */
        insize *= 2;
    }

    *updated_tail = tail;
    return list;
}

static int llist_get_min_max(llist list, llist_node * output, bool max)
{
    comperator cmp;

	if ( list == NULL )
	{
		return LLIST_NULL_ARGUMENT;
	}

    cmp =  ( ( _llist * ) list )->comp_func;

 	if ( cmp == NULL )
    {
        return LLIST_COMPERATOR_MISSING;
    }

	_list_node *iterator = ( ( _llist * ) list )->head;
	*output = iterator->node;
	iterator = iterator->next;
	while (iterator)
	{
		if (max) // Find maximum
		{
			if ( cmp(iterator->node, *output) > 0 )
			{
				*output = iterator->node;
			}
		}
		else // Find minimum
		{
			if ( cmp(iterator->node, *output) < 0 )
			{
				*output = iterator->node;
			}
		}
		iterator = iterator->next;
	}

	return LLIST_SUCCESS;
}

int llist_get_max(llist list, llist_node * max)
{
	return llist_get_min_max(list,max,true);
}

int llist_get_min(llist list, llist_node * min)
{
	return llist_get_min_max(list,min,false);
}

bool llist_is_empty(llist list)
{
    return ( ! llist_size ( list ) ) ;
}


/*
 * TODO: Implement the below functions
 */

int llist_merge ( llist first, llist second)
{
	assert (1 == 0); // Fail, function not implemented yet.
	return LLIST_NOT_IMPLEMENTED;
}
