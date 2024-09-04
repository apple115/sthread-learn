#ifndef MY_Clib_LIST_H
#define MY_Clib_LIST_H

#include <stdlib.h>

struct list_head {
    struct list_head* prev;
    struct list_head* next;
};

#ifndef container_of
#define container_of(ptr, type, member) \
    ({ (type *)((char *)ptr - (char *)&((type *)0)->member)})
#endif

#ifndef __container_of
#define __container_of(ptr, type, member) \
    ({ (typeof(type))((char *)ptr - (char *)&((typeof(type))0)->member); })
#endif

#define LIST_HEAD_INIT(name) {&name,&name}
#define LIST_HEAD(name)\
        struct list_head name = LIST_HEAD_INIT(name)

static inline void list_head_init(struct list_head *head)
{
    head->prev = head->next = head;
}

static inline void __list_add(struct list_head *n,
                              struct list_head *prev,
                              struct list_head *next)
{
    next->prev =n;
    prev->next =n;
    n->next = next;
    n->prev = prev;
}

/**
 * list_add  添加一个新的节点在head节点之后
 * @n: 新的节点
 * @head: head节点
 */
static inline void list_add(struct list_head *n, struct list_head *head)
{
    __list_add(n, head, head->next);
}

/**
 * list_add_tail - 添加一个新的节点在head解决的节点之前，或者说是添加到链表的尾部
 * @n: 新的节点
 * @head: head节点
 */
static inline void list_add_tail(struct list_head *n, struct list_head *head)
{
    __list_add(n, head->prev, head);
}

/*
 * 删除一个节点,通过连接前一个节点和后一个节点
 * 前后节点需要存在
 */
static inline void __list_del(struct list_head *prev, struct list_head *next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void list_del(struct list_head *n) {
    __list_del(n->prev, n->next);
}


static inline void list_replace(struct list_head* old,struct list_head* n) {
    n->next=old->next;
    n->next->prev=old;
    n->prev=old->prev;

    n->prev->next=n;
}

static inline void list_swap(struct list_head *entry1,
                             struct list_head *entry2
                            ) {
    struct list_head *pos = entry2->prev;
    list_del(entry2);
    list_replace(entry1, entry2);
    if (pos == entry1) pos = entry2;//如果原来entry1在entry2 前面，需要这个判断
    list_add(entry1, pos);
}

static inline void list_move(struct list_head *list,struct list_head *head) {
    __list_del(list->prev,list->next);
    list_add(list,head);
}

static inline void list_move_tail(struct list_head *list,struct list_head *head) {
    __list_del(list->prev, list->next);
    list_add_tail(list,head);
}

static inline void __list_splice(const struct list_head *list,
                                 struct list_head *prev,
                                 struct list_head *next)
{
    struct list_head *first = list->next;
    struct list_head *last = list->prev;

    first->prev = prev;
    prev->next = first;

    last->next = next;
    next->prev = last;
}
/*
 *判断链表是否为空
 *
 */
static inline int list_empty(struct list_head *head)
{
    return head->next == head;
}

static inline int list_is_first(const struct list_head *list, const struct list_head *head)
{
    return list->prev == head;
}

static inline int list_is_last(const struct list_head *list, const struct list_head *head)
{
    return list->next == head;
}

static inline int list_is_head(const struct list_head *list, const struct list_head *head)
{
    return list == head;
}

static inline void list_splice(const struct list_head *list,
                               struct list_head *head)
{
    if(!list_empty(list))
        __list_splice(list, head->next, head);
}

static inline void list_splice_tail(struct list_head *list,
                                    struct list_head *head)
{
    if (!list_empty(list))
        __list_splice(list, head->prev, head);
}

/**
 * list_entry - 得到包含list_head的结构体的指针
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 */
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

#define list_for_each_entry(pos,head,member) \
    for(pos=__container_of((head)->next,pos,member); \
        &pos->member != head; \
        pos =__container_of(pos->member.next,pos,member))

#define list_for_each_entry_reverse(pos,head,member) \
    for(pos=__container_of((head)->prev,pos,member); \
        &pos->member != head; \
        pos =__container_of(pos->member.prev,pos,member))

#endif //MY_Clib_LIST_H
