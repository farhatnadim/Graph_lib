# Goals — Graph_lib

<!-- project_status.py counts the checklist below: progress = checked / total. -->

A C++23 templated graph library (inspired by Sedgewick's *Algorithms*), where
algorithms are non-member template functions separate from the `Graph<T>` data
structure.

## Core data structure & I/O

- [x] Templated `Graph<T>` with adjacency-list representation (`map<T, set<T>>`)
- [x] Support both directed and undirected graphs
- [x] Load graphs from file (`Graph_Input_type::EXPLICIT`)
- [x] Graphviz DOT export via `GraphDraw` CLI
- [x] GoogleTest suite with `data/` fixtures

## Algorithms

- [x] Depth-first search (`dfs`)
- [x] Breadth-first search (`bfs`) + shortest path on unweighted graphs
- [x] Connected components (`cc`)
- [x] Undirected cycle detection (`detect_cycle`)
- [x] Directed cycle detection with cycle recovery (`directedDFS_return_cycle`)
- [x] Graph reversal (`reverse_graph`)
- [ ] Topological sort for DAGs
- [ ] Strongly connected components (Kosaraju / Tarjan)
- [ ] Weighted edges + Dijkstra shortest paths
- [ ] A* shortest path (heuristic-guided)
- [ ] Minimum spanning tree (Kruskal / Prim)
- [ ] Bipartite check (two-coloring)
- [ ] Articulation points / bridges (Tarjan)

## Graph RAG

<!-- TODO: link the existing Graph RAG plan document here once its location is confirmed. -->

- [ ] Integrate Graph_lib as the graph backend for a Graph RAG pipeline (per existing plan)
- [ ] Build a knowledge graph from a document corpus (entities as vertices, relations as edges)
- [ ] Graph-based retrieval: neighborhood / multi-hop traversal for context expansion
- [ ] Rank and assemble retrieved subgraphs into LLM context

## Code quality

- [ ] Remove duplicate `Get_cc_Count` / `Get_CC_Count` accessors
- [ ] Fix sign-compare warnings in traversal loops
- [ ] Return `adj()` / `adj_list()` by const-reference instead of by value
