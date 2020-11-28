#include "../src/ecs/pool.h"
#include "ecs/common.h"
#include <check.h>
#include <stdlib.h>

typedef struct
{
  int field_a;
  int field_b;
} TestComponent;

START_TEST(add) { EcsPool* p = ecs_pool_new(sizeof(TestComponent));
	ecs_entity_t e1 = ECS_ENT(0, 1);
	ecs_entity_t e2 = ECS_ENT(1, 1);
	ecs_entity_t e3 = ECS_ENT(2, 1);

	ecs_pool_add(p, e1);
	ecs_pool_add(p, e2);
	ecs_pool_add(p, e3);

	ck_assert(ecs_pool_cnt(p) == 3);
}
END_TEST

static Suite* pool_suite()
{
	Suite* suite;
	TCase* add_test_case;

	add_test_case = tcase_create("add test case");
	tcase_add_test(add_test_case, add);
	suite = suite_create("ecs pool test suite");
	suite_add_tcase(suite, add_test_case);
	
	return suite;
}

int main()
{
	SRunner* sr;
	Suite* s;
	int number_failed;

	s = pool_suite();
	sr = srunner_create(s);
	srunner_run_all(sr, CK_VERBOSE);
	number_failed = srunner_ntests_failed(sr);
	return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
	
}
