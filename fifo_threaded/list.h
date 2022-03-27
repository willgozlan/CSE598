// Adopted from https://github.com/skorks/c-linked-list
#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef struct node Node;

typedef struct list List;

List * makelist();
void add(int data, List * list);
void delete(int data, List * list);
void display(List * list);
void reverse(List * list);
void reverse_using_two_pointers(List * list);
void destroy(List * list);