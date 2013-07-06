#include "../inc/llist.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>

#define ERROR 0
#define SUCCESS 1
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


llist llist_create(comperator compare_func, equal equal_func)
{
    _llist * new_list;
    new_list = malloc(sizeof(_llist));
    if (new_list == NULL )
        {
            perror("Malloc error");
            return NULL ;
        }

// These can be NULL, I don't care...
    new_list->equal_func = equal_func;
    new_list->comp_func = compare_func;

// Reset the list
    new_list->count = 0;
    new_list->head = NULL;
    pthread_mutex_init(&new_list->mutex, NULL );

    return new_list;
}

int llist_add_node(llist list, llist_node node, int flags)
{
    _list_node * node_wrapper = NULL;
    _list_node * iterator;
    if ((list == NULL )|| (node == NULL) ){
    perror("NULL argument");
    return ERROR;
}

    node_wrapper = malloc(sizeof(_list_node));
    if (node_wrapper == NULL )
        {
            perror("Malloc error");
            return ERROR;
        }

    node_wrapper->node = node;

    pthread_mutex_lock(&((_llist *) list)->mutex);

    ((_llist *) list)->count++;

    if (flags & ADD_NODE_FRONT)
        {
            node_wrapper->next = ((_llist *) list)->head;
            ((_llist *) list)->head = node_wrapper;
        }
    else // add node in the rear
        {
            node_wrapper->next = NULL;
            iterator = ((_llist *) list)->head;

            // If the list is empty
            if (iterator == NULL )
                ((_llist *) list)->head = node_wrapper;
            else
                {
                    while (iterator->next != NULL )
                        {
                            iterator = iterator->next;
                        }
                    iterator->next = node_wrapper;
                }

        }
    pthread_mutex_unlock(&((_llist *) list)->mutex);
    return SUCCESS;
}

int llist_delete_node(llist list, llist_node node)
{
    _list_node * iterator;
    _list_node * temp;

    if ((list == NULL )|| (node == NULL) ){
    perror("NULL argument");
    return ERROR;
}

    pthread_mutex_lock(&((_llist *) list)->mutex);

    iterator = ((_llist *) list)->head;

    // is it the first node ?
    if (iterator->node == node)
        {
            ((_llist *) list)->head = ((_llist *) list)->head->next;
            free(iterator);
            ((_llist *) list)->count--;

            assert(((_llist *) list)->count >= 0);

            pthread_mutex_unlock(&((_llist *) list)->mutex);
            return SUCCESS;
        }
    else
        {
            while (iterator->next != NULL )
                {
                    if (iterator->next->node == node)
                        { // found it
                            temp = iterator->next;
                            iterator->next = temp->next;
                            free(temp);

                            ((_llist *) list)->count--;
                            assert(((_llist *) list)->count >= 0);
                            pthread_mutex_unlock(&((_llist *) list)->mutex);
                            return SUCCESS;
                        }
                    iterator = iterator->next;
                }
        }
    if (iterator->next == NULL )
        {
            pthread_mutex_unlock(&((_llist *) list)->mutex);
            perror("Can't find the node in the list");
            return ERROR;
        }
    assert(1==2);
    // this assert always failed. we assume that the function never gets here...
    return ERROR;
}

//TODO: Add multithread support here
int llist_for_each(llist list, node_func func)
{
    _list_node * iterator;
    if ((list == NULL )|| (func == NULL) ){
    perror("NULL argument");
    return ERROR;
}
    iterator = ((_llist *) list)->head;

    while (iterator != NULL )
        {
            func(iterator->node);
            iterator = iterator->next;
        }
    return SUCCESS;
}

int llist_insert_node(llist list, llist_node new_node, llist_node pos_node,
        int flags)
{
    _list_node * iterator;
    _list_node * node_wrapper = NULL;
    if ((list == NULL )|| (new_node == NULL) || (pos_node == NULL)){
    perror("NULL argument");
    return ERROR;
}

    node_wrapper = malloc(sizeof(_list_node));
    if (node_wrapper == NULL )
        {
            perror("Malloc error");
            return ERROR;
        }

    node_wrapper->node = new_node;

    pthread_mutex_lock(&((_llist *) list)->mutex);

    ((_llist *) list)->count++;

    iterator = ((_llist *) list)->head;

    if (iterator->node == pos_node)
        { // it's the first node

            if (flags & ADD_NODE_BEFORE)
                {
                    node_wrapper->next = iterator;
                    ((_llist *) list)->head = node_wrapper;
                }
            else
                {
                    node_wrapper->next = iterator->next;
                    iterator->next = node_wrapper;
                }
            pthread_mutex_unlock(&((_llist *) list)->mutex);
            return SUCCESS;
        }

    while (iterator->next != NULL )
        {
            if (iterator->node == pos_node)
                {
                    if (flags & ADD_NODE_BEFORE)
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
                    pthread_mutex_unlock(&((_llist *) list)->mutex);
                    return SUCCESS;
                }

            iterator = iterator->next;
        }
    assert(1==2);
    // this assert will always fail. we assume that the function never gets here...
    return ERROR;

}

llist_node  llist_find_node (llist list, void * data, equal alternative)
{
    _list_node * iterator;
    equal    actual_equal;

    if (list == NULL ){
       perror("NULL argument");
       return NULL;
   }

    actual_equal = ((_llist *) list)->equal_func;
    if (alternative)
        actual_equal =  alternative;

    if (actual_equal == NULL)
        {
            perror("equal function was not provided in init or alternative");
            return NULL;
        }

    pthread_mutex_lock(&((_llist *) list)->mutex);

    iterator = ((_llist *) list)->head;

    while (iterator != NULL)
        {
            if (actual_equal(iterator->node, data))
                {
                    pthread_mutex_unlock(&((_llist *) list)->mutex);
                    return iterator->node;
                }
        }
    // Didn't find the node
    pthread_mutex_unlock(&((_llist *) list)->mutex);
    return NULL;

}
