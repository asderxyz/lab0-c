#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *q = (struct list_head *) malloc(sizeof(struct list_head));
    if (q == NULL)
        return NULL;

    INIT_LIST_HEAD(q);
    return q;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    // only free one element.
    if (l != NULL) {
        if (list_empty(l))
            free(l);
        else {
            struct list_head *li = l->next;
            element_t *ele = list_entry(li, element_t, list);
            list_del(li);
            q_release_element(ele);
            q_free(l);
        }
    }
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (head == NULL)
        return false;

    element_t *ele = (element_t *) malloc(sizeof(element_t));
    if (ele == NULL)
        return false;

    ele->value = strdup(s);
    if (ele->value == NULL) {
        free(ele);
        return false;
    }

    list_add(&ele->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (head == NULL)
        return false;

    element_t *ele = (element_t *) malloc(sizeof(element_t));
    if (ele == NULL)
        return false;

    ele->value = strdup(s);
    if (ele->value == NULL) {
        free(ele);
        return false;
    }

    list_add_tail(&ele->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || head->next == head)
        return NULL;

    element_t *ele = list_entry(head->next, element_t, list);
    if (sp != NULL) {
        memcpy(sp, ele->value, bufsize);
        sp[bufsize - 1] = '\0';
    }

    list_del(head->next);
    return ele;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || head->next == head)
        return NULL;

    element_t *ele = list_entry(head->prev, element_t, list);
    if (sp != NULL) {
        memcpy(sp, ele->value, bufsize);
        sp[bufsize - 1] = '\0';
    }

    list_del(head->prev);
    return ele;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (head == NULL || head->next == head)
        return false;

    int cnt = q_size(head) >> 1;
    struct list_head *li = head->next;
    while (cnt) {
        li = li->next;
        cnt -= 1;
    }

    list_del(li);
    q_release_element(list_entry(li, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (head == NULL || list_empty(head))
        return false;
    if (list_is_singular(head))
        return true;

    element_t *ele_tmp;
    struct list_head *curr = head->next;
    struct list_head *next = curr->next;
    struct list_head *tmp = NULL;
    while (next != head) {
        element_t *ele_curr = list_entry(curr, element_t, list);
        element_t *ele_next = list_entry(next, element_t, list);
        if (strcmp(ele_curr->value, ele_next->value) == 0) {
            list_del(next);
            q_release_element(ele_next);
            next = curr->next;
            tmp = curr;
        } else {
            curr = curr->next;
            next = curr->next;
            if (tmp != NULL) {
                ele_tmp = list_entry(tmp, element_t, list);
                list_del(tmp);
                q_release_element(ele_tmp);
                tmp = NULL;
            }
        }
    }
    if (tmp != NULL) {
        ele_tmp = list_entry(tmp, element_t, list);
        list_del(tmp);
        q_release_element(ele_tmp);
        tmp = NULL;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (head == NULL)
        return;

    if (!list_empty(head) && !list_is_singular(head)) {
        struct list_head *first = head->next;
        struct list_head *second = first->next;
        element_t *first_ele, *second_ele;
        char *tmp;
        while (first != head && second != head) {
            first_ele = list_entry(first, element_t, list);
            second_ele = list_entry(second, element_t, list);
            tmp = first_ele->value;
            first_ele->value = second_ele->value;
            second_ele->value = tmp;
            first = second->next;
            second = second->next->next;
        }
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (head != NULL && !list_empty(head) && !list_is_singular(head)) {
        struct list_head *li = head->next;
        struct list_head *next = li->next;
        while (li != head) {
            li->next = li->prev;
            li->prev = next;
            li = next;
            next = next->next;
        }
        next = li->next;
        li->next = li->prev;
        li->prev = next;
    }
}

/*
 * Find and return middle node.
 */
struct list_head *find_middle_node(struct list_head *li)
{
    struct list_head *slow = li;
    struct list_head *fast = li;
    while (fast->next != NULL && fast->next->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }
    fast = slow->next;
    slow->next = NULL;
    return fast;
}

/*
 * Merge sort
 */
struct list_head *merge(struct list_head *l, struct list_head *r)
{
    struct list_head *h = NULL;
    struct list_head **curr = &h;
    while (l != NULL && r != NULL) {
        element_t *ele_l = list_entry(l, element_t, list);
        element_t *ele_r = list_entry(r, element_t, list);
        if (strcmp(ele_l->value, ele_r->value) < 0) {
            *curr = l;
            l = l->next;
        } else {
            *curr = r;
            r = r->next;
        }
        curr = &(*curr)->next;
    }
    if (l != NULL)
        *curr = l;
    if (r != NULL)
        *curr = r;

    return h;
}

struct list_head *merge_sort(struct list_head *h)
{
    if (h == NULL || h->next == NULL)
        return h;

    struct list_head *mid = find_middle_node(h);
    struct list_head *l = merge_sort(h);
    struct list_head *r = merge_sort(mid);
    return merge(l, r);
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (head != NULL && !list_empty(head) && !list_is_singular(head)) {
        struct list_head *list = head->next;
        head->prev->next = NULL;

        list = merge_sort(list);

        struct list_head *curr = list->next;
        struct list_head *last = list;
        last->prev = head;
        head->next = last;
        while (curr != NULL) {
            curr->prev = last;
            curr = curr->next;
            last = last->next;
        }
        last->next = head;
        head->prev = last;
    }
}
