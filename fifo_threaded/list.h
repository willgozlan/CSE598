// Adopted from https://github.com/skorks/c-linked-list
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct node Node;

typedef struct list List;

struct node
{
  pthread_t data;
  struct node *next;
};

struct list
{
  Node *head;
};

List *make_list();
void list_add(pthread_t data, List *list);
// void delete(pthread_t data, List * list);
// void display(List * list);
// void reverse(List * list);
// void reverse_using_two_pointers(List * list);
void list_destroy(List *list);
Node *create_node(pthread_t data);
