#ifndef DE_CONTAINER_LLIST_HEADER
#define DE_CONTAINER_LLIST_HEADER
#ifdef __cplusplus
extern "C" {
#endif

/*
to get function definitions include DE_CONTAINER_LLIST_IMPLEMENTATION before the #include
*/

/* possible options to set before 'first' include */
#ifndef DE_CONTAINER_LLIST_OPTIONS
#ifdef DE_CONTAINER_LLIST_OPTIONS
/* no options */
#define DE_OPTIONS_LLIST_NO_SAFETY_ASSERTS
#define DE_OPTIONS_LLIST_MALLOC_FUNCTION(size) defaults to malloc from stdlib.h
#define DE_OPTIONS_LLIST_FREE_FUNCTION(ptr) defaults to free from stdlib.h
#define DE_OPTIONS_LLIST_MEMCPY_FUNCTION(src, dest, size) defaults to memcpy from string.h
#define DE_OPTIONS_LLIST_ASSERT_FUNCTION(to_assert) defaults to assert from assert.h
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

// TODO: Add find, remove, merge, split, splice, improve node data allocation, ggf wirth tombstone llist
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

// typedef bool (*de_llist_val_cmp_func)(const u0 *item, u0 *data);


de_llist de_llist_create(const usize _item_size);
u0 de_llist_destroy(de_llist* const _ll);

u0 *de_llist_remove(de_llist* const _ll, const usize _idx);
u0 *de_llist_pop_back(de_llist* const _ll);
u0 *de_llist_pop_front(de_llist* const _ll);

u0* de_llist_get(de_llist* const _ll, const usize _idx);
u0* de_llist_get_front(de_llist* const _ll);
u0* de_llist_get_back(de_llist* const _ll);

u0 de_llist_push_back(de_llist* const _ll, void* _data);
u0 de_llist_push_front(de_llist* const _ll, void* _data);
u0 de_llist_insert(de_llist* const _ll, const usize _idx, void* _data);

usize de_llist_info_size(de_llist* const _ll);
usize de_llist_info_item_size(de_llist* const _ll);

de_llist_node* de_llist_begin(de_llist* _ll);
de_llist_node* de_llist_end(de_llist* _ll); // maybe NULL

de_llist_node* de_llist_next(de_llist_node* _node);
de_llist_node* de_llist_prev(de_llist_node* _node);
u0* de_llist_node_data(de_llist_node* _node);

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
#define DE_OPTIONS_LLIST_MALLOC_FUNCTION(size) malloc(size)
#endif
#ifndef DE_OPTIONS_LLIST_FREE_FUNCTION
#define DE_OPTIONS_LLIST_FREE_FUNCTION(ptr) free(ptr)
#endif
#ifndef DE_OPTIONS_LLIST_MEMCPY_FUNCTION
#define DE_OPTIONS_LLIST_MEMCPY_FUNCTION(src, dest, size) memcpy(src, dest, size)
#endif
#ifndef DE_OPTIONS_LLIST_ASSERT_FUNCTION
#define DE_OPTIONS_LLIST_ASSERT_FUNCTION(to_assert) assert(to_assert)
#endif

#define DE_C_LL_MALLOC(size) DE_OPTIONS_LLIST_MALLOC_FUNCTION(size)
#define DE_C_LL_FREE(ptr) DE_OPTIONS_LLIST_FREE_FUNCTION(ptr)
#define DE_C_LL_MEMCPY(src, dest, size) DE_OPTIONS_LLIST_MEMCPY_FUNCTION(src, dest, size)
#define DE_C_LL_ASSERT(to_assert) DE_OPTIONS_LLIST_ASSERT_FUNCTION(to_assert)

/* implementations */

#ifdef __cplusplus
} // extern "C"
#endif
#endif
#endif