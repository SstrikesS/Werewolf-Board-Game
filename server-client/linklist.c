#include "linklist.h"

llist *create_llist(void *data){
    struct node *tmp;
    llist *list = (llist *)malloc(sizeof(llist));
    *list = (struct node *)malloc(sizeof(struct node));
    tmp = *list;
    tmp->data = data;
    tmp->next = NULL;
    return list;
}

void *free_llist(llist *list){
    struct node *current = *list;
    struct node *next;
    while(current != NULL){
        next = current->next;
        free(current);
        current = next;
    }

    free(list);
}

void push_llist(llist *list, void *data){
    struct node *head;
    struct node *tmp;
    if(list == NULL || *list == NULL){
        perror("Link list is null\n");
        exit(0);
    }
    head = *list;
    if(head->data == NULL){
        head->data = data;
    }else{
        tmp = malloc(sizeof(struct node));
        tmp->data = data;
        tmp->next = head;
        *list = tmp;
    }
}

void *pop_llist(llist *list){
    void *data;
    struct node *head = *list;
    if(list == NULL || !head || head->data == NULL){
        perror("Link list is null\n");
        return NULL;
    }
    data = head->data;
    *list = head->next;
    free(head);
    return data;
}

void print_llist(llist *list, void (* print_data)(void *data)){
    struct node *current = *list;
    while(current != NULL){
        print_data(current->data);
        printf("\n");
        current = current->next;
    }
}

void *get_head_data(llist *list){
    struct node *head = *list;
    return head->data;
}

void * find_node(llist *list, void *data, int(* compare_data)(void *data1, void *data2)){
    struct node *current = *list;
    while(current != NULL){
        if(compare_data(current->data, data)){
            return current->data;
        }
        current = current->next;
    }
    return NULL;
}