#include <stdio.h>
#include <string.h>
#include <stdlib.h>


typedef struct node {
    void *data;
    struct node *next;
}node;

typedef struct node * llist;

llist *create_llist(void *data); // create new linklist

void *free_llist(llist *list); // free the link list

void push_llist(llist *list, void *data); // add to the link list

void *pop_llist(llist *list); //remove node from link list

void print_llist(llist *list, void (* print_data)(void * data)); //print link list data

void *get_head_data(llist *list);
void * find_node(llist *list, void *data, int(* compare_data)(void *data1, void* data2));