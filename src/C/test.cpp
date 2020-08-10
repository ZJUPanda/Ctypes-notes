#include "test.h"

int main(void)
{
    Node *list=NULL;
    int arr[3] = {0, 1, 2};
    Point point = {3.0, 4.0};
    int flag;
    for (int i = 0; i < 3; i++)
        flag = insert_node(&list, i, arr, point); 
    print_list(list);
    printf("deleting node 1...\n");
    delete_node(&list, 1);
    print_list(list);

    return 0;
}

int insert_node(Node **linkp, int i, int arr[3], Point point)
{
    Node *current_p;
    Node *new_p;

    while ((current_p = *linkp) != NULL)
        linkp = &current_p->next;

    new_p = (Node *)malloc(sizeof(Node));
    if (new_p == NULL)
        return 0;
    new_p->i = i;
    for (int j = 0; j < 3; j++)
        new_p->arr[j] = arr[j];
    new_p->point = point;

    new_p->next = current_p;
    *linkp = new_p;
    return 1;
}

int delete_node(Node **linkp, int i)
{
    if (*linkp == NULL)
        return 0;
    Node *current_p;
    while ((current_p = *linkp) != NULL && current_p->i != i)
        linkp = &current_p->next;
    *linkp = current_p->next;
    free(current_p);
    current_p = NULL;
    return 1;
}

void print_list(Node *head)
{
    if (head == NULL)
        printf("empty list\n");
    printf("--------------------------------------------\n");
    while (head != NULL)
    {
        printf("Node %d:\n", head->i);
        printf("arr = [%d, %d, %d],\t", head->arr[0], head->arr[1], head->arr[2]);
        printf("point = (%.1f, %.1f)\n", head->point.x, head->point.y);
        head = head->next;
    }
    printf("--------------------------------------------\n");

}
