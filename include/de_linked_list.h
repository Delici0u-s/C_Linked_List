#ifndef DE_CONTAINER_LLIST_HEADER
#define DE_CONTAINER_LLIST_HEADER
#ifdef __cplusplus
extern "C" {
#endif

/*
to get function definitions include DE_CONTAINER_LLIST_IMPLEMENTATION before the #include
simple implementation of Doubly Linked List, may be changed to an indext doubly linked list or sth, all change will
happen internally though, so exposed function will stay the same, ensuring if only the funcions are used as provided
behaviour will not change
*/

/* possible options to set before 'first' include */
#ifndef DE_CONTAINER_LLIST_OPTIONS
#ifdef DE_CONTAINER_LLIST_OPTIONS
/* no options */
#define DE_OPTIONS_LLIST_NO_SAFETY_ASSERTS
#define DE_OPTIONS_LLIST_MALLOC_FUNCTION defaults to malloc from stdlib.h this header assuemes malloc wont fail
#define DE_OPTIONS_LLIST_FREE_FUNCTION defaults to free from stdlib.h
#define DE_OPTIONS_LLIST_MEMCPY_FUNCTION defaults to memcpy from string.h
#define DE_OPTIONS_LLIST_ASSERT_FUNCTION defaults to assert from assert.h
#endif
#endif

#ifdef DE_CONTAINER_LLIST_IMPLEMENTATION
#define DE_CONTAINER_LLIST_API
#else
#define DE_CONTAINER_LLIST_API extern
#endif

#define DE_CONTAINER_LLIST_INTERNAL

#include <common.h>

/* clang-format off */
/* declarations */
/* de_llist stands for de_linked_list */

typedef u0 (*de_llist_del_func)(u0 *const _item);

// TODO: Add find, remove, merge, split, splice, improve node data allocation, ggf with tombstone llist
typedef struct de_llist_node {
  void *data;
  struct de_llist_node *next;
  struct de_llist_node *prev;
} de_llist_node;

typedef struct de_linked_list{
de_llist_node* head;
de_llist_node* tail;
usize item_size;
usize size;
} de_llist;

// typedef bool (*de_llist_val_cmp_func)(const u0 *const item, u0 *const data);

// basic api, will not be changed, but may be expanded in the future
DE_CONTAINER_LLIST_API de_llist de_llist_create(const usize _item_size);
DE_CONTAINER_LLIST_API u0 de_llist_destroy(de_llist* const _ll, de_llist_del_func _free_func);

DE_CONTAINER_LLIST_API u0 *de_llist_remove(de_llist* const _ll, const usize _idx);
DE_CONTAINER_LLIST_API u0 *de_llist_remove_node(de_llist* const _ll, de_llist_node* _node);
DE_CONTAINER_LLIST_API u0 *de_llist_pop_back(de_llist* const _ll);
DE_CONTAINER_LLIST_API u0 *de_llist_pop_front(de_llist* const _ll);

DE_CONTAINER_LLIST_API u0* de_llist_get(de_llist* const _ll, const usize _idx);
DE_CONTAINER_LLIST_API de_llist_node* de_llist_get_node(de_llist* const _ll, const usize _idx);
DE_CONTAINER_LLIST_API u0* de_llist_get_front(de_llist* const _ll);
DE_CONTAINER_LLIST_API u0* de_llist_get_back(de_llist* const _ll);

/* memcpys data */
DE_CONTAINER_LLIST_API u0 de_llist_push_back(de_llist* const _ll,const  void*const _data);
DE_CONTAINER_LLIST_API u0 de_llist_push_front(de_llist* const _ll, const void*const _data);
DE_CONTAINER_LLIST_API u0 de_llist_insert(de_llist* const _ll, const usize _idx,const void*const _data);

/* copies the data pointer, not the contents. Sets original pointer to NULL */
DE_CONTAINER_LLIST_API u0 de_llist_push_back_mv(de_llist* const _ll, void** _data);
DE_CONTAINER_LLIST_API u0 de_llist_push_front_mv(de_llist* const _ll, void** _data);
DE_CONTAINER_LLIST_API u0 de_llist_insert_mv(de_llist* const _ll, const usize _idx, void** _data);

DE_CONTAINER_LLIST_API usize de_llist_info_size(de_llist* const _ll);
DE_CONTAINER_LLIST_API usize de_llist_info_item_size(de_llist* const _ll);

DE_CONTAINER_LLIST_API de_llist_node* de_llist_head(de_llist* _ll);
DE_CONTAINER_LLIST_API de_llist_node* de_llist_tail(de_llist* _ll); // maybe NULL

DE_CONTAINER_LLIST_API de_llist_node* de_llist_next(de_llist_node* _node);
DE_CONTAINER_LLIST_API de_llist_node* de_llist_prev(de_llist_node* _node);
DE_CONTAINER_LLIST_API u0* de_llist_node_data(de_llist_node* _node);

/* clang-format on */
#ifdef __cplusplus
} // extern "C"
#endif

#endif

#define DE_CONTAINER_LLIST_IMPLEMENTATION_DEVELOPMENT
#if defined(DE_CONTAINER_LLIST_IMPLEMENTATION) || defined(DE_CONTAINER_LLIST_IMPLEMENTATION_DEVELOPMENT)
#ifndef DE_CONTAINER_LLIST_IMPLEMENTATION_INTERNAL
#define DE_CONTAINER_LLIST_IMPLEMENTATION_INTERNAL
#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#ifndef DE_OPTIONS_LLIST_MALLOC_FUNCTION
#define DE_OPTIONS_LLIST_MALLOC_FUNCTION malloc
#endif
#ifndef DE_OPTIONS_LLIST_FREE_FUNCTION
#define DE_OPTIONS_LLIST_FREE_FUNCTION free
#endif
#ifndef DE_OPTIONS_LLIST_MEMCPY_FUNCTION
#define DE_OPTIONS_LLIST_MEMCPY_FUNCTION memcpy
#endif
#ifndef DE_OPTIONS_LLIST_ASSERT_FUNCTION
#define DE_OPTIONS_LLIST_ASSERT_FUNCTION assert
#endif

#define DE_C_LL_MALLOC DE_OPTIONS_LLIST_MALLOC_FUNCTION
#define DE_C_LL_FREE DE_OPTIONS_LLIST_FREE_FUNCTION
#define DE_C_LL_MEMCPY DE_OPTIONS_LLIST_MEMCPY_FUNCTION
#define DE_C_LL_ASSERT DE_OPTIONS_LLIST_ASSERT_FUNCTION

/* implementations */
DE_CONTAINER_LLIST_INTERNAL de_llist de_llist_create(const usize _item_size) { //
  return (de_llist){.item_size = _item_size, .size = 0, .head = NULL, .tail = NULL};
}

DE_CONTAINER_LLIST_INTERNAL u0 de_llist_destroy(de_llist *const _ll, de_llist_del_func _free_func_or_NULL) {
#ifndef DE_OPTIONS_LLIST_NO_SAFETY_ASSERTS
  DE_C_LL_ASSERT(_ll != NULL);
#endif

  if (_free_func_or_NULL == NULL) { _free_func_or_NULL = DE_C_LL_FREE; }

  for (de_llist_node *cur = _ll->head, *next; cur; cur = next) {
    next = cur->next;
    _free_func_or_NULL(cur->data);
    DE_C_LL_FREE(cur);
  }

  *_ll = (de_llist){0};
}

DE_CONTAINER_LLIST_INTERNAL u0 *de_llist_remove(de_llist *const _ll, const usize _idx) { //
#ifndef DE_OPTIONS_LLIST_NO_SAFETY_ASSERTS
  DE_C_LL_ASSERT(_ll != NULL);
  DE_C_LL_ASSERT(_idx < _ll->size);
#endif
  de_llist_node *_node = de_llist_get_node(_ll, _idx);
  return de_llist_remove_node(_ll, _node);
}

DE_CONTAINER_LLIST_INTERNAL u0 *de_llist_remove_node(de_llist *const _ll, de_llist_node *_node) {
#ifndef DE_OPTIONS_LLIST_NO_SAFETY_ASSERTS
  DE_C_LL_ASSERT(_ll != NULL);
  DE_C_LL_ASSERT(_node != NULL);
#endif

  de_llist_node *prev = _node->prev;
  de_llist_node *next = _node->next;

  if (prev) prev->next = next;
  else _ll->head = next;

  if (next) next->prev = prev;
  else _ll->tail = prev;

  u0 *data = _node->data;
  DE_C_LL_FREE(_node);
  --_ll->size;

  return data; // head/tail are already correct
}

DE_CONTAINER_LLIST_INTERNAL u0 *de_llist_pop_back(de_llist *const _ll) { //
#ifndef DE_OPTIONS_LLIST_NO_SAFETY_ASSERTS
  DE_C_LL_ASSERT(_ll != NULL);
  DE_C_LL_ASSERT(_ll->size != 0);
#endif

  return de_llist_remove_node(_ll, _ll->tail);
}
DE_CONTAINER_LLIST_INTERNAL u0 *de_llist_pop_front(de_llist *const _ll) { //
#ifndef DE_OPTIONS_LLIST_NO_SAFETY_ASSERTS
  DE_C_LL_ASSERT(_ll != NULL);
  DE_C_LL_ASSERT(_ll->size != 0);
#endif

  return de_llist_remove_node(_ll, _ll->head);
}

de_llist_node *de_llist_get_node(de_llist *const _ll, const usize _idx) {
#ifndef DE_OPTIONS_LLIST_NO_SAFETY_ASSERTS
  DE_C_LL_ASSERT(_ll != NULL);
  DE_C_LL_ASSERT(_idx < _ll->size);
#endif

  const usize middle = _ll->size / 2;
  de_llist_node *cur;

  if (_idx < middle) {
    cur = _ll->head;
    // advance _idx times from head to reach index _idx
    for (usize i = 0; i < _idx; ++i) {
      cur = cur->next;
    }
  } else {
    cur = _ll->tail;
    // start from last index (_ll->size - 1) and step backwards until _idx
    for (usize i = _ll->size - 1; i > _idx; --i) {
      cur = cur->prev;
    }
  }

  return cur;
}

DE_CONTAINER_LLIST_INTERNAL u0 *de_llist_get(de_llist *const _ll, const usize _idx) { //
  return de_llist_get_node(_ll, _idx)->data;
}

DE_CONTAINER_LLIST_INTERNAL u0 *de_llist_get_front(de_llist *const _ll) { //
#ifndef DE_OPTIONS_LLIST_NO_SAFETY_ASSERTS
  DE_C_LL_ASSERT(_ll != NULL);
  DE_C_LL_ASSERT(_ll->size != 0);
#endif
  return _ll->head->data;
}
DE_CONTAINER_LLIST_INTERNAL u0 *de_llist_get_back(de_llist *const _ll) { //
#ifndef DE_OPTIONS_LLIST_NO_SAFETY_ASSERTS
  DE_C_LL_ASSERT(_ll != NULL);
  DE_C_LL_ASSERT(_ll->size != 0);
#endif
  return _ll->tail->data;
}

/* memcpys data */
DE_CONTAINER_LLIST_INTERNAL u0 de_llist_push_back(de_llist *const _ll, const void *const _data) {
#ifndef DE_OPTIONS_LLIST_NO_SAFETY_ASSERTS
  DE_C_LL_ASSERT(_ll != NULL);
  DE_C_LL_ASSERT(_data != NULL);
#endif

  /* allocate node */
  de_llist_node *node = (de_llist_node *)DE_C_LL_MALLOC(sizeof(de_llist_node));

  /* allocate and copy payload */
  node->data = DE_C_LL_MALLOC(_ll->item_size);
  DE_C_LL_MEMCPY(node->data, _data, _ll->item_size);

  /* link node */
  node->next = NULL;
  node->prev = _ll->tail;

  if (_ll->size == 0) {
    /* empty list: head and tail point to the new node */
    _ll->head = node;
    _ll->tail = node;
  } else {
    /* append after current tail */
    _ll->tail->next = node;
    _ll->tail = node;
  }

  ++_ll->size;
}

DE_CONTAINER_LLIST_INTERNAL u0 de_llist_push_front(de_llist *const _ll, const void *const _data) {
#ifndef DE_OPTIONS_LLIST_NO_SAFETY_ASSERTS
  DE_C_LL_ASSERT(_ll != NULL);
  DE_C_LL_ASSERT(_data != NULL);
#endif

  /* allocate node */
  de_llist_node *node = (de_llist_node *)DE_C_LL_MALLOC(sizeof(de_llist_node));

  /* allocate and copy payload */
  node->data = DE_C_LL_MALLOC(_ll->item_size);
  DE_C_LL_MEMCPY(node->data, _data, _ll->item_size);

  /* link node at front */
  node->prev = NULL;
  node->next = _ll->head;

  if (_ll->size == 0) {
    /* empty list: head and tail point to the new node */
    _ll->head = node;
    _ll->tail = node;
  } else {
    /* insert before current head */
    _ll->head->prev = node;
    _ll->head = node;
  }

  ++_ll->size;
}

DE_CONTAINER_LLIST_INTERNAL u0 de_llist_insert(de_llist *const _ll, const usize _idx, const void *const _data) {
#ifndef DE_OPTIONS_LLIST_NO_SAFETY_ASSERTS
  DE_C_LL_ASSERT(_ll != NULL);
  DE_C_LL_ASSERT(_data != NULL);
  DE_C_LL_ASSERT(_idx <= _ll->size); /* allowed range: 0 .. size (inserting at size == push_back) */
#endif

  /* special cases: front or back */
  if (_idx == 0) {
    de_llist_push_front(_ll, _data);
    return;
  }
  if (_idx == _ll->size) {
    de_llist_push_back(_ll, _data);
    return;
  }

  /* find node currently at index _idx and insert BEFORE it */
  de_llist_node *target = de_llist_get_node(_ll, _idx);

  /* allocate node */
  de_llist_node *node = (de_llist_node *)DE_C_LL_MALLOC(sizeof(de_llist_node));

  /* allocate and copy payload */
  node->data = DE_C_LL_MALLOC(_ll->item_size);
  DE_C_LL_MEMCPY(node->data, _data, _ll->item_size);

  /* link between target->prev and target */
  node->next = target;
  node->prev = target->prev;

  if (target->prev != NULL) { target->prev->next = node; }
  target->prev = node;

  /* if we inserted at index 0 (shouldn't happen because handled above), update head */
  if (_idx == 0) { _ll->head = node; }

  ++_ll->size;
}

/* copies the data pointer, not the contents. Sets original pointer to NULL */
DE_CONTAINER_LLIST_INTERNAL u0 de_llist_push_back_mv(de_llist *const _ll, void **_data) {
#ifndef DE_OPTIONS_LLIST_NO_SAFETY_ASSERTS
  DE_C_LL_ASSERT(_ll != NULL);
  DE_C_LL_ASSERT(_data != NULL);
  DE_C_LL_ASSERT(*_data != NULL);
#endif

  /* allocate node */
  de_llist_node *node = (de_llist_node *)DE_C_LL_MALLOC(sizeof(de_llist_node));

  /* move payload ownership into node */
  node->data = *_data;
  *_data = NULL;

  /* link node */
  node->next = NULL;
  node->prev = _ll->tail;

  if (_ll->size == 0) {
    /* empty list: head and tail point to the new node */
    _ll->head = node;
    _ll->tail = node;
  } else {
    _ll->tail->next = node;
    _ll->tail = node;
  }

  ++_ll->size;
}

DE_CONTAINER_LLIST_INTERNAL u0 de_llist_push_front_mv(de_llist *const _ll, void **_data) {
#ifndef DE_OPTIONS_LLIST_NO_SAFETY_ASSERTS
  DE_C_LL_ASSERT(_ll != NULL);
  DE_C_LL_ASSERT(_data != NULL);
  DE_C_LL_ASSERT(*_data != NULL);
#endif

  /* allocate node */
  de_llist_node *node = (de_llist_node *)DE_C_LL_MALLOC(sizeof(de_llist_node));

  /* move payload ownership into node */
  node->data = *_data;
  *_data = NULL;

  /* link node at front */
  node->prev = NULL;
  node->next = _ll->head;

  if (_ll->size == 0) {
    _ll->head = node;
    _ll->tail = node;
  } else {
    _ll->head->prev = node;
    _ll->head = node;
  }

  ++_ll->size;
}

DE_CONTAINER_LLIST_INTERNAL u0 de_llist_insert_mv(de_llist *const _ll, const usize _idx, void **_data) {
#ifndef DE_OPTIONS_LLIST_NO_SAFETY_ASSERTS
  DE_C_LL_ASSERT(_ll != NULL);
  DE_C_LL_ASSERT(_data != NULL);
  DE_C_LL_ASSERT(*_data != NULL);
  DE_C_LL_ASSERT(_idx <= _ll->size); /* allowed: 0 .. size */
#endif

  /* special cases: front or back */
  if (_idx == 0) {
    de_llist_push_front_mv(_ll, _data);
    return;
  }
  if (_idx == _ll->size) {
    de_llist_push_back_mv(_ll, _data);
    return;
  }

  /* target is the node currently at _idx; we will insert BEFORE it */
  de_llist_node *target = de_llist_get_node(_ll, _idx);

  /* allocate node */
  de_llist_node *node = (de_llist_node *)DE_C_LL_MALLOC(sizeof(de_llist_node));

  /* move payload ownership into node */
  node->data = *_data;
  *_data = NULL;

  /* link between target->prev and target */
  node->next = target;
  node->prev = target->prev;

  if (target->prev != NULL) {
    target->prev->next = node;
  } else {
    /* inserting at head (shouldn't happen because _idx == 0 handled above) */
    _ll->head = node;
  }
  target->prev = node;

  ++_ll->size;
}

DE_CONTAINER_LLIST_INTERNAL usize de_llist_info_size(de_llist *const _ll) { //
  return _ll->size;
}
DE_CONTAINER_LLIST_INTERNAL usize de_llist_info_item_size(de_llist *const _ll) { //
  return _ll->item_size;
}

DE_CONTAINER_LLIST_INTERNAL de_llist_node *de_llist_head(de_llist *_ll) { //
  return _ll->head;
}
DE_CONTAINER_LLIST_INTERNAL de_llist_node *de_llist_tail(de_llist *_ll) { //
  return _ll->tail;
} // maybe NULL

DE_CONTAINER_LLIST_INTERNAL de_llist_node *de_llist_next(de_llist_node *_node) { //
  return _node->next;
}
DE_CONTAINER_LLIST_INTERNAL de_llist_node *de_llist_prev(de_llist_node *_node) { //
  return _node->prev;
}
DE_CONTAINER_LLIST_INTERNAL u0 *de_llist_node_data(de_llist_node *_node) { //
  return _node->data;
}
#ifdef __cplusplus
} // extern "C"
#endif
#endif
#endif