#include "../src/toolbox/rect.h"

#include <check.h>

START_TEST(intersection)
{
  Rect r1;
  Rect r2;

  rect_init_full(&r1, 0, 0, 5, 6, 0.f, 0.f, 0.f);
  rect_init_full(&r2, 2, 2, 5, 7, 0.f, 0.f, 0.f);

  ck_assert(rect_has_intersection(&r1, &r2));
}
END_TEST

START_TEST(nointersection)
{
  Rect r1;
  Rect r2;

  rect_init_full(&r1, 0, 0, 5, 6, 0.f, 0.f, 0.f);
  rect_init_full(&r2, 7, 2, 5, 7, 0.f, 0.f, 0.f);

  ck_assert(!rect_has_intersection(&r1, &r2));
}
END_TEST

START_TEST(rotate_then_check)
{
  Rect r1;
  Rect r2;

  rect_init_full(&r1, 0.f, 0.f, 1.f, 10.f, 0.f, 0.f, -60.0);
  rect_init_full(&r2, 3.1f, 1.f, 1.f, 7.f, 0.f, 0.f, 0.0);

  ck_assert(rect_has_intersection(&r1, &r2));
}
END_TEST

Suite* rect_suite()
{
  TCase* tc_intersection;
  TCase* tc_nointersection;
  TCase* tc_rotate_then_check;
  Suite* suite;

  tc_intersection = tcase_create("intersection");
  tcase_add_test(tc_intersection, intersection);

  tc_nointersection = tcase_create("nointersection");
  tcase_add_test(tc_nointersection, nointersection);

  tc_rotate_then_check = tcase_create("rotate_then_check");
  tcase_add_test(tc_rotate_then_check, rotate_then_check);

  suite = suite_create("rect");
  suite_add_tcase(suite, tc_intersection);
  suite_add_tcase(suite, tc_nointersection);
  suite_add_tcase(suite, tc_rotate_then_check);
  return suite;
}

int main()
{
  SRunner* srunner;
  Suite*   suite;
  int      number_failed;

  suite = rect_suite();
  srunner = srunner_create(suite);
  srunner_run_all(srunner, CK_VERBOSE);
  number_failed = srunner_ntests_failed(srunner);

  srunner_free(srunner);
  Rect r1;
  Rect r2;

  rect_init_full(&r1, 0.f, 0.f, 1.f, 10.f, 0.5f, 5.f, -90.0);
  rect_init_full(&r2, 3.1f, 1.f, 1.f, 7.f, 0.f, 0.f, 0.0);

  printf("v1(%f, %f)\n", r1.v1.x, r1.v1.y);
  printf("v2(%f, %f)\n", r1.v2.x, r1.v2.y);
  printf("v3(%f, %f)\n", r1.v3.x, r1.v3.y);
  printf("v4(%f, %f)\n", r1.v4.x, r1.v4.y);
  return number_failed;
}
