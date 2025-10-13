#include <stdio.h>
#define DE_CONTAINER_LLIST_IMPLEMENTATION
#include <de_linked_list.h>

// int main(void) { //
//   int num = 2;
//   de_llist ll = de_llist_create(sizeof(int));
//   de_llist_insert(&ll, 0, &num);
//   printf("%d\n", *(int *)de_llist_pop_back(&ll));
// }

/* tests/de_llist_tests.c
 *
 * Unit tests for the de_llist API described by the user.
 *
 * Build (example):
 *   gcc -std=c11 -Wall -Wextra -I../include -o de_llist_tests tests/de_llist_tests.c ../src/de_llist.c
 *
 * Adjust include paths and linkage to match your project layout.
 *
 * The tests avoid intentionally passing invalid/null pointers or out-of-range indices
 * to prevent triggering the library's hard ASSERTs.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* include your linked-list public header here */

/* Helper macros for readability */
#define TEST_ASSERT(expr)                                                                                              \
  do {                                                                                                                 \
    if (!(expr)) {                                                                                                     \
      fprintf(stderr, "TEST FAILED: %s (at %s:%d)\n", #expr, __FILE__, __LINE__);                                      \
      abort();                                                                                                         \
    }                                                                                                                  \
  } while (0)

static void print_test_header(const char *name) { printf("[TEST] %s\n", name); }

/* A small helper to allocate an int pointer for move tests */
static int *alloc_int(int v) {
  int *p = (int *)malloc(sizeof(int));
  TEST_ASSERT(p != NULL);
  *p = v;
  return p;
}

/* free-func used by de_llist_destroy: must match typedef
 * Provided signature in your header: typedef u0 (*de_llist_del_func)(u0 *const _item);
 * If u0 is 'void' then compiler will accept a function returning void and taking void*.
 * If your project uses different typedefs, adapt this function's signature accordingly.
 */
static void destroy_free_item(void *const _item) {
  if (_item != NULL) { free(_item); }
}

/* Test plan (brief)
 *
 * 1) create/destroy: create a list, destroy empty list with free func (should succeed).
 * 2) push_back / get / info: push ints by value (memcpy variant), check head/tail/data/size/item_size.
 * 3) push_front: push to front, ensure order correct and head updated.
 * 4) insert at middle and at end (idx == size): ensure order and size correct.
 * 5) pop_front / pop_back: after adding elements, pop each and ensure returned data matches, and that returned pointers
 * are heap-allocated copies (caller frees them). 6) remove (by index) and remove_node: remove middle and head/tail
 * nodes and validate pointers returned. 7) move variants (push_back_mv / push_front_mv / insert_mv): transfer ownership
 * of allocated pointer into list, ensure original pointer is set to NULL and the stored pointer equals original pointer
 * value. 8) traversal: next/prev from head/tail for multi-element list, node_data accessor correctness.
 *
 * All tests avoid passing NULL _ll/_data or out-of-range indices that would trigger internal ASSERTs.
 */

/* Helper to read int from item pointer returned by API (u0* is void* in your API) */
static int read_int_from_item(void *item) {
  TEST_ASSERT(item != NULL);
  return *(int *)item;
}

/* ----------------- Tests ----------------- */

static void test_create_destroy() {
  print_test_header("create_destroy");
  de_llist ll = de_llist_create(sizeof(int));
  TEST_ASSERT(ll.size == 0);
  TEST_ASSERT(de_llist_info_size(&ll) == 0);
  TEST_ASSERT(de_llist_info_item_size(&ll) == sizeof(int));
  de_llist_destroy(&ll, destroy_free_item);
}

static void test_push_back_get_pop() {
  print_test_header("push_back_get_pop");
  de_llist ll = de_llist_create(sizeof(int));

  int a = 10;
  int b = 20;
  int c = 30;

  de_llist_push_back(&ll, &a); /* copy 10 */
  TEST_ASSERT(de_llist_info_size(&ll) == 1);
  TEST_ASSERT(read_int_from_item(de_llist_get_front(&ll)) == 10);
  TEST_ASSERT(read_int_from_item(de_llist_get_back(&ll)) == 10);

  de_llist_push_back(&ll, &b); /* copy 20 */
  TEST_ASSERT(de_llist_info_size(&ll) == 2);
  TEST_ASSERT(read_int_from_item(de_llist_get(&ll, 0)) == 10);
  TEST_ASSERT(read_int_from_item(de_llist_get(&ll, 1)) == 20);

  de_llist_push_back(&ll, &c); /* copy 30 */
  TEST_ASSERT(de_llist_info_size(&ll) == 3);

  /* pop front (should return allocated pointer to int with value 10) */
  void *p0 = de_llist_pop_front(&ll);
  TEST_ASSERT(p0 != NULL);
  TEST_ASSERT(read_int_from_item(p0) == 10);
  free(p0);
  TEST_ASSERT(de_llist_info_size(&ll) == 2);

  /* pop back (should return 30) */
  void *p1 = de_llist_pop_back(&ll);
  TEST_ASSERT(p1 != NULL);
  TEST_ASSERT(read_int_from_item(p1) == 30);
  free(p1);
  TEST_ASSERT(de_llist_info_size(&ll) == 1);

  /* remaining element should be 20 */
  TEST_ASSERT(read_int_from_item(de_llist_get_front(&ll)) == 20);

  de_llist_destroy(&ll, destroy_free_item);
}

static void test_push_front_and_insert() {
  print_test_header("push_front_and_insert");
  de_llist ll = de_llist_create(sizeof(int));

  int a = 1, b = 2, c = 3, d = 4;
  de_llist_push_back(&ll, &a); /* [1] */
  de_llist_push_back(&ll, &b); /* [1,2] */

  de_llist_push_front(&ll, &c); /* [3,1,2] */
  TEST_ASSERT(de_llist_info_size(&ll) == 3);
  TEST_ASSERT(read_int_from_item(de_llist_get(&ll, 0)) == 3);

  /* insert at middle index 1 -> before element '1' */
  de_llist_insert(&ll, 1, &d); /* [3,4,1,2] */
  TEST_ASSERT(de_llist_info_size(&ll) == 4);
  TEST_ASSERT(read_int_from_item(de_llist_get(&ll, 1)) == 4);

  /* insert at end index == size (append) */
  int e = 5;
  de_llist_insert(&ll, de_llist_info_size(&ll), &e); /* append [3,4,1,2,5] */
  TEST_ASSERT(de_llist_info_size(&ll) == 5);
  TEST_ASSERT(read_int_from_item(de_llist_get_back(&ll)) == 5);

  de_llist_destroy(&ll, destroy_free_item);
}

static void test_remove_and_remove_node() {
  print_test_header("remove_and_remove_node");
  de_llist ll = de_llist_create(sizeof(int));

  int vals[5] = {10, 11, 12, 13, 14};
  for (int i = 0; i < 5; ++i)
    de_llist_push_back(&ll, &vals[i]); /* [10,11,12,13,14] */
  TEST_ASSERT(de_llist_info_size(&ll) == 5);

  /* remove index 2 -> value 12 */
  void *r = de_llist_remove(&ll, 2);
  TEST_ASSERT(r != NULL);
  TEST_ASSERT(read_int_from_item(r) == 12);
  free(r);
  TEST_ASSERT(de_llist_info_size(&ll) == 4);

  /* remove_node: remove head node */
  de_llist_node *head = de_llist_head(&ll);
  TEST_ASSERT(head != NULL);
  void *r2 = de_llist_remove_node(&ll, head);
  TEST_ASSERT(r2 != NULL);
  TEST_ASSERT(read_int_from_item(r2) == 10); /* originally first element */
  free(r2);
  TEST_ASSERT(de_llist_info_size(&ll) == 3);

  /* remaining list should contain 11,13,14 in order */
  TEST_ASSERT(read_int_from_item(de_llist_get(&ll, 0)) == 11);
  TEST_ASSERT(read_int_from_item(de_llist_get(&ll, 1)) == 13);
  TEST_ASSERT(read_int_from_item(de_llist_get(&ll, 2)) == 14);

  de_llist_destroy(&ll, destroy_free_item);
}

static void test_move_variants() {
  print_test_header("move_variants");
  de_llist ll = de_llist_create(sizeof(int)); /* item_size still matters for some code paths */

  /* Prepare heap integers and transfer ownership with *_mv functions */
  int *p1 = alloc_int(100);
  int *p2 = alloc_int(200);
  int *p3 = alloc_int(300);

  /* push_back_mv: pointer should be moved into list, original pointer set to NULL */
  de_llist_push_back_mv(&ll, (void **)&p1);
  TEST_ASSERT(p1 == NULL);
  TEST_ASSERT(de_llist_info_size(&ll) == 1);
  /* check stored pointer value */
  int *stored = (int *)de_llist_node_data(de_llist_tail(&ll));
  TEST_ASSERT(stored != NULL);
  TEST_ASSERT(*stored == 100);

  /* push_front_mv */
  de_llist_push_front_mv(&ll, (void **)&p2);
  TEST_ASSERT(p2 == NULL);
  TEST_ASSERT(de_llist_info_size(&ll) == 2);
  TEST_ASSERT(*(int *)de_llist_node_data(de_llist_head(&ll)) == 200);

  /* insert_mv in middle (index 1) */
  de_llist_insert_mv(&ll, 1, (void **)&p3);
  TEST_ASSERT(p3 == NULL);
  TEST_ASSERT(de_llist_info_size(&ll) == 3);
  TEST_ASSERT(*(int *)de_llist_get(&ll, 1) == 300);

  /* Remove all nodes but don't leak memory: remove returns the raw pointer that list owned */
  void *x;
  while (de_llist_info_size(&ll) > 0) {
    x = de_llist_pop_front(&ll);
    /* Because these were move-inserted, pop returns the same pointers that were originally allocated */
    TEST_ASSERT(x != NULL);
    free(x);
  }

  de_llist_destroy(&ll, destroy_free_item);
}

static void test_traversal_and_accessors() {
  print_test_header("traversal_and_accessors");
  de_llist ll = de_llist_create(sizeof(int));

  int vals[4] = {7, 8, 9, 10};
  for (int i = 0; i < 4; ++i)
    de_llist_push_back(&ll, &vals[i]); /* [7,8,9,10] */

  de_llist_node *n = de_llist_head(&ll);
  TEST_ASSERT(n != NULL);
  TEST_ASSERT(*(int *)de_llist_node_data(n) == 7);

  /* forward traversal */
  int idx = 0;
  while (n != NULL) {
    TEST_ASSERT(*(int *)de_llist_node_data(n) == vals[idx]);
    n = de_llist_next(n);
    ++idx;
  }
  TEST_ASSERT(idx == 4);

  /* backward traversal from tail */
  n = de_llist_tail(&ll);
  idx = 3;
  while (n != NULL) {
    TEST_ASSERT(*(int *)de_llist_node_data(n) == vals[idx]);
    n = de_llist_prev(n);
    --idx;
  }
  TEST_ASSERT(idx == -1);

  de_llist_destroy(&ll, destroy_free_item);
}

/* Run all tests */
int main(void) {
  printf("Running de_llist unit tests...\n");

  test_create_destroy();
  test_push_back_get_pop();
  test_push_front_and_insert();
  test_remove_and_remove_node();
  test_move_variants();
  test_traversal_and_accessors();

  printf("All tests passed.\n");
  return 0;
}
