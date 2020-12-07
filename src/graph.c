#include "graph.h"
#include <stdlib.h>

#define NEXT_VERTEX(V, E) (((Edge*)E)->a != ((Vertex*)V) ? ((Edge*)E)->a : ((Edge*)E)->b)

typedef struct Circle
{
  Point p;
  float r;
} Circle;

typedef struct Line
{
  float a, b, c;
} Line;

Line*        make_line(Line* out, const Point* a, const Point* b);
Point        get_mid_point(const Point* a, const Point* b);
BOOL         get_intersection_point(Point* out, const Line* a, const Line* b);
Circle       get_circumcircle(const Point* a, const Point* b, const Point* c);
Line         get_mid_perpendicular(const Point* a, const Point* b);
INLINE float get_distance(const Point* a, const Point* b)
{
  float x = a->x - b->x;
  float y = a->y - b->y;
  return SDL_sqrtf(x * x + y * y);
}
/**********************************************************/
void vertex_add_edge(Vertex* v, Edge* e) { ptr_array_add(v->edges, e); }
void vertex_remove_edge(Vertex* v, Edge* e) { ptr_array_remove(v->edges, e); }

/**********************************************************/

Point get_mid_point(const Point* a, const Point* b)
{
  return (Point){ (a->x + b->x) / 2.f, (a->y + b->y) / 2.f };
}

Line* make_line(Line* out, const Point* a, const Point* b)
{
  out->a = b->y - a->y;
  out->b = a->x - b->x;
  out->c = out->a * a->x + out->b * a->y;
  return out;
}

Line get_mid_perpendicular(const Point* a, const Point* b)
{
  float tmp;
  Line  l;
  Point mid = get_mid_point(a, b);
  make_line(&l, a, b);
  l.c = -l.b * mid.x + l.a * mid.y;
  tmp = l.a;
  l.a = -l.b;
  l.b = tmp;

  return l;
}

BOOL get_intersection_point(Point* out, const Line* l1, const Line* l2)
{
  float d = l1->a * l2->b - l2->a * l1->b;
  if (d != 0.f)
  {
    out->x = (l2->b * l1->c - l1->b * l2->c) / d;
    out->y = (l1->a * l2->c - l2->a * l1->c) / d;
    return true;
  }
  return false;
}

Circle get_circumcircle(const Point* a, const Point* b, const Point* c)
{
  Line  g = get_mid_perpendicular(a, b);
  Line  h = get_mid_perpendicular(a, c);
  Point p;
  get_intersection_point(&p, &g, &h);

  return (Circle){ p, get_distance(&p, a) };
}

/**********************************************************/

Vertex* vertex_new(float x, float y) { return vertex_init(malloc(sizeof(Vertex)), x, y); }

void vertex_delete(Vertex* v)
{
  if (v)
  {
    free(v);
  }
}

Vertex* vertex_init(Vertex* v, float x, float y)
{
  v->edges = ptr_array_new(NULL);
  v->p.x   = x;
  v->p.y   = y;
  v->data  = NULL;
  return (v);
}
/**********************************************************/

Edge* edge_new(Vertex* va, Vertex* vb) { return edge_init(malloc(sizeof(Edge)), va, vb); }

void edge_delete(Edge* e)
{
  if (e)
    free(e);
}

Edge* edge_init(Edge* e, Vertex* va, Vertex* vb)
{
  e->a = va;
  e->b = vb;
  return e;
}

float edge_weigth(Edge* e) { return get_distance(&e->a->p, &e->b->p); }

Graph* graph_new() { return graph_init(malloc(sizeof(Graph))); }

Graph* graph_new_triangulation(const Point* points, void** user_data_array, int cnt)
{
  return graph_init_triangulation(malloc(sizeof(Graph)), points, user_data_array, cnt);
}

Graph* graph_init(Graph* g)
{
  g->edges    = ptr_array_new((destroy_fn_t)edge_delete);
  g->vertices = ptr_array_new((destroy_fn_t)vertex_delete);
  return g;
}

void graph_delete(Graph* g)
{
  if (g)
  {
    graph_fini(g);
    free(g);
  }
}

Graph* graph_init_triangulation(Graph* g, const Point* points, void** user_data_array, int cnt)
{
  graph_init(g);
  for (int i = 0; i < cnt; ++i)
  {
    Vertex* tmp = graph_make_vertex(g, points[i].x, points[i].y);
    tmp->data   = user_data_array[i];
  }

  Vertex** v = (Vertex**)g->vertices->storage;
  Circle   c;
  BOOL     ok;

  for (int i = 0; i < cnt - 2; ++i)
  {
    for (int j = i + 1; j < cnt - 1; ++j)
    {
      for (int k = j + 1; k < cnt; ++k)
      {
        c  = get_circumcircle(&v[i]->p, &v[j]->p, &v[k]->p);
        ok = true;
        for (int l = 0; l < cnt && ok; ++l)
          if (l != i && l != j && l != k)
            if (get_distance(&c.p, &v[l]->p) < c.r)
              ok = false;
        if (ok)
        {
          graph_make_edge(g, i, j);
          graph_make_edge(g, i, k);
          graph_make_edge(g, j, k);
        }
      }
    }
  }

  return g;
}

void graph_fini(Graph* g)
{
  ptr_array_delete(g->edges);
  ptr_array_delete(g->vertices);
}

Vertex* graph_make_vertex(Graph* g, float x, float y)
{
  Vertex* v = vertex_new(x, y);
  return ptr_array_add(g->vertices, v);
}

Edge* graph_make_edge(Graph* g, int va, int vb)
{
  Vertex* a = g->vertices->storage[va];
  Vertex* b = g->vertices->storage[vb];
  Edge*   e = edge_new(a, b);
  vertex_add_edge(a, e);
  vertex_add_edge(b, e);
  return ptr_array_add(g->edges, e);
}

void graph_destroy_vertex(Graph* g, int iv)
{
  Vertex* v  = g->vertices->storage[iv];
  Vertex* ne = NULL;
  Edge*   e;
  for (int i = 0; i < v->edges->cnt; ++i)
  {
    e  = v->edges->storage[i];
    ne = NEXT_VERTEX(v, e);
    vertex_remove_edge(ne, e);
    ptr_array_remove(g->edges, e);
  }
  ptr_array_remove(g->vertices, v);
}

PtrArray* graph_minimum_spanning_tree(Graph* g)
{
  PtrArray *O = NULL, *C = NULL, *ret = NULL, *E = NULL;
  Vertex*   v;
  Edge*     e;
  O   = ptr_array_new_copy(g->vertices); // array<Vertex*> open
  C   = ptr_array_new(NULL);             // array<Vertex*> close
  E   = ptr_array_new(NULL);             // array<Edge*> open_edge
  ret = ptr_array_new(NULL);             // array<Edge*> ret

  v = O->storage[0];

  for (int i = 0; i < v->edges->cnt; ++i)
  {
    ptr_array_add(E, v->edges->storage[i]);
  }

  ptr_array_remove(O, v);
  ptr_array_add(C, v);

  while (O->cnt > 0)
  {
    e = E->storage[0];
    for (int i = 1; i < E->cnt; ++i)
    {
      if (edge_weigth(E->storage[i]) < edge_weigth(e))
      {
        e = E->storage[i];
      }
    }

    ptr_array_add(ret, e);

    v = !ptr_array_contains(C, e->a) ? e->a : e->b;
    ptr_array_add(C, v);
    ptr_array_remove(O, v);
    for (int i = 0; i < v->edges->cnt; ++i)
    {
      Vertex* n = NEXT_VERTEX(v, v->edges->storage[i]);
      if (!ptr_array_contains(C, n))
      {
        ptr_array_add(E, v->edges->storage[i]);
      }
    }

    for (int i = 0; i < E->cnt; ++i)
    {
      while (i < E->cnt && ptr_array_contains(C, ((Edge*)E->storage[i])->a) &&
             ptr_array_contains(C, ((Edge*)E->storage[i])->b))
      {
        ptr_array_swap_and_pop(E, i);
      }
    }
  }

  ptr_array_delete(O);
  ptr_array_delete(C);
  ptr_array_delete(E);

  return ret;
}
