/*
 * Copyright © 2019 Advanced Micro Devices, Inc. All rights reserved.
 *
 */


#ifndef __LIST_H__
#define __LIST_H__


struct list_head {
    struct list_head *prev, *next;
};

#define LIST_HEAD_INIT(x) {&(x), &(x)}

#define list_entry(ptr, type, member)           \
    container_of(ptr, type, member)

#define list_for_each(pos, head)                \
    for (pos = (head)->next; pos != (head);     \
         pos = pos->next)

static inline void INIT_LIST_HEAD(struct list_head *list)
{
    list->prev = list;
    list->next = list;
}

static inline void __list_add(struct list_head *new,
                              struct list_head *prev,
                              struct list_head *next)
{
    new->prev = prev;
    new->next = next;
    prev->next = new;
    next->prev = new;
}


static inline void __list_del(struct list_head *prev,
                              struct list_head *next)
{
    prev->next = next;
    next->prev = prev;
}

static inline void list_add(struct list_head *entry, struct list_head *head)
{
    __list_add(entry, head, head->next);
}

static inline void list_add_tail(struct list_head *entry, struct list_head *head)
{
    __list_add(entry, head->prev, head);
}

static inline void list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
}

static inline int list_empty(const struct list_head *head)
{
    return head->next == head;
}

#endif  /* __LIST_H__ */
