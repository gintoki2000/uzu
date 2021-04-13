struct A {
  int a;
  int b;
};

struct B
{
  A parent_instance;
  int c;
};

void do_something_w_a(struct A* a);

B b;
do_something_w_a((struct A*) &b);
