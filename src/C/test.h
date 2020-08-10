#ifndef _TEST_H_
#define _TEST_H_

#include<stdio.h>
#include<malloc.h>

typedef struct Point
{
    float x;
    float y;
} Point;

typedef struct Node
{
    int i;
    int arr[3];
    Point point;
    struct Node *next;
} Node;

int insert_node(Node **linkp, int i, int arr[], Point point);
int delete_node(Node **linkp, int i);
void print_list(Node *head);

#endif