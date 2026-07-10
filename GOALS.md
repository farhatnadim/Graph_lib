# Goals — Graph_lib

<!-- project_status.py counts the checklist below: progress = checked / total. -->

A C++23 templated graph library (inspired by Sedgewick's *Algorithms*), where
algorithms are non-member template functions in `GraphAlgo.hpp`, separate from
the `Graph<T>` data structure in `Graph.hpp`.

## Core data structure & I/O

- [x] Templated `Graph<T>` with adjacency-list representation (`map<T, set<T>>`)
- [x] Support both directed and undirected graphs (`SetDigraph` / `isDigraph`)
- [x] Load graphs from file (`Graph_Input_type::EXPLICIT` ifstream ctor)
- [x] Graphviz DOT export via `GraphDraw` CLI (`drawGraph.cpp`)
- [x] GoogleTest suite (`graph_test.cpp`) with `data/` fixtures, wired through CTest

## Algorithms

- [x] Depth-first search (`dfs`)
- [x] Breadth-first search (`bfs`) + shortest path on unweighted graphs
- [x] Connected components (`cc`)
- [x] Undirected cycle detection (`detect_cycle`)
- [x] Directed DFS from multiple sources (`directed_dfs`)
- [x] Directed cycle detection with cycle recovery (`directedDFS_return_cycle`)
- [x] Graph reversal (`reverse_graph`)
- [ ] Topological sort for DAGs
- [ ] Strongly connected components (Kosaraju / Tarjan)
- [ ] Weighted edges + Dijkstra shortest paths
- [ ] A* shortest path (heuristic-guided)
- [ ] Minimum spanning tree (Kruskal / Prim)
- [ ] Bipartite check (two-coloring)
- [ ] Articulation points / bridges (Tarjan)

## Graph RAG (aspirational)

<!-- The plans/ directory is currently empty; capture the design before coding. -->

- [ ] Write a Graph RAG design note under `plans/` and use Graph_lib as its backend
- [ ] Build a knowledge graph from a document corpus (entities as vertices, relations as edges)
- [ ] Graph-based retrieval: neighborhood / multi-hop traversal to expand LLM context

## Code quality

- [ ] Remove duplicate `Get_cc_Count` / `Get_CC_Count` accessors (Graph.hpp:150, :194)
- [ ] Return `adj()` / `adj_list()` by const-reference instead of by value
- [ ] Fix sign-compare warnings in traversal loops
- [ ] Remove untracked `Graph.cpp.hints.md` / `Graph.hpp.hints.md` or fold into docs
