#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

#include "common.h"
static uint16_t values[256];
struct list_head *getMiddle(struct list_head *);
struct list_head merge(struct list_head, struct list_head);


struct list_head merge_sort(struct list_head *head)
{
    if (list_empty(head) || list_is_singular(head))
        return *head;

    struct list_head *middle = getMiddle(head);  // get the middle of the list
    struct list_head *left_head, *left_tail, *right_head;
    struct list_head node, res;

    list_add(&node, middle);  // node is the head of the left half list

    // connect two sides of left half list
    left_tail = head->prev;
    left_head = middle->next;
    left_head->prev = left_tail;
    left_tail->next = left_head;

    // connect two sides of right half list
    head->prev = middle;
    middle->next = head;
    right_head = head;
    // struct listitem *item, *is = NULL;
    // int k = 0;
    // list_for_each_entry_safe (item, is, head, list) {
    //     printf("%d: %d\n", k++, item->i);
    // }
    return merge(merge_sort(left_head), merge_sort(right_head));
}

// Merge subroutine to merge two sorted lists
struct list_head merge(struct list_head left, struct list_head right)
{
    struct list_head *l_ptr = (&left)->next;
    struct list_head *l_start = l_ptr->prev;
    struct list_head *r_ptr = (&right)->next;
    struct list_head *r_start = r_ptr->prev;
    struct list_head res;

    INIT_LIST_HEAD(&res);

    for (; l_start != l_ptr && r_start != r_ptr;) {
        int l_data = list_entry(l_ptr, struct listitem, list)->i;
        int r_data = list_entry(r_ptr, struct listitem, list)->i;
        struct listitem *item = (struct listitem *) malloc(sizeof(*item));

        if (l_data <= r_data) {
            item->i = l_data;
            l_ptr = l_ptr->next;
        } else {
            item->i = r_data;
            r_ptr = r_ptr->next;
        }
        list_add_tail(&item->list, &res);
    }

    while (l_start != l_ptr) {
        struct listitem *item = (struct listitem *) malloc(sizeof(*item));
        item->i = list_entry(l_ptr, struct listitem, list)->i;
        list_add_tail(&item->list, &res);
        l_ptr = l_ptr->next;
    }

    while (r_start != r_ptr) {
        struct listitem *item = (struct listitem *) malloc(sizeof(*item));
        item->i = list_entry(r_ptr, struct listitem, list)->i;
        list_add_tail(&item->list, &res);
        r_ptr = r_ptr->next;
    }

    return res;
}

// Finding the middle element of the list for splitting
struct list_head *getMiddle(struct list_head *head)
{
    if (head == NULL)
        return head;

    struct list_head *slow = head, *fast = head;

    while (fast->next != head && fast->next->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}


int main(void)
{
    struct list_head testlist, res;
    struct listitem *item, *is = NULL;
    size_t i;
    random_shuffle_array(values, (uint16_t) ARRAY_SIZE(values));

    INIT_LIST_HEAD(&testlist);

    assert(list_empty(&testlist));

    for (i = 0; i < ARRAY_SIZE(values); i++) {
        item = (struct listitem *) malloc(sizeof(*item));
        assert(item);
        item->i = values[i];
        list_add_tail(&item->list, &testlist);
    }

    assert(!list_empty(&testlist));

    qsort(values, ARRAY_SIZE(values), sizeof(values[0]), cmpint);
    res = merge_sort(&testlist);
    list_del_init(&testlist);
    list_splice(&res, &testlist);

    i = 0;
    list_for_each_entry_safe (item, is, &testlist, list) {
        assert(item->i == values[i]);
        list_del(&item->list);
        free(item);
        i++;
    }

    assert(i == ARRAY_SIZE(values));
    assert(list_empty(&testlist));

    return 0;
}
