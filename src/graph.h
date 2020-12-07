#ifndef GRAPH_H
#define GRAPH_H
#include <toolbox/toolbox.h>
typedef struct Graph  Graph;
typedef struct Vertex Vertex;
typedef struct Point
{
  float x, y;
} Point;
typedef struct Edge
{
  Vertex *a, *b;
} Edge;

typedef struct Vertex
{
  Point     p;
  PtrArray* edges;
  void*     data;
} Vertex;

typedef struct Graph
{
  PtrArray* vertices;
  PtrArray* edges;
} Graph;

Vertex* vertex_new(float x, float y);
void    vertex_delete(Vertex* v);
Vertex* vertex_init(Vertex* v, float x, float y);
void    vertex_add_edge(Vertex* v, Edge* e);
void    vertex_remove_edge(Vertex* v, Edge* e);

Edge* edge_new(Vertex* v2, Vertex* v1);
Edge* edge_init(Edge* e, Vertex* va, Vertex* vb);
void  edge_delete(Edge* e);
int   edge_compare(Edge* lhs, Edge* rhs);
float edge_weigth(Edge* e);

Graph*    graph_new();
Graph*    graph_new_triangulation(const Point* points, void** user_data_array, int cnt);
void      graph_delete(Graph* g);
Graph*    graph_init(Graph* g);
Graph*    graph_init_triangulation(Graph* g, const Point* points, void** user_data_array, int cnt);
void      graph_fini(Graph* g);
Vertex*   graph_make_vertex(Graph* g, float x, float y);
Edge*     graph_make_edge(Graph* g, int ia, int ib);
void      graph_destroy_edge(Graph* g, int ie);
void      graph_destroy_vertex(Graph* g, int iv);
PtrArray* graph_minimum_spanning_tree(Graph* g);
#endif // GRAPH_H
