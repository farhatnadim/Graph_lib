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

## Architecture rework (spec: plans/architecture.md — Nadim codes, portpal mentors)

<!-- Migration steps from the spec; each leaves the build/tests green.
     Stub convention: `// TODO: User implements` (portpal's stub marker). -->

- [ ] Review the spec; amend decisions D1–D6 where you disagree
- [ ] Step 1: label⇄dense-id interner; state vectors indexed by id
- [ ] Step 2: traversal result object (dfs/bfs/path-to out of Graph; const graph)
- [ ] Step 3: components + cycles result objects (graph loses all algorithm state)
- [ ] Step 4: silent & total library (expected-based parsing, const adjacency view, no printing)
- [ ] Step 5: split into include/graphlib/*.hpp; uncomment per-topic portpal units
- [ ] Step 6: edge payload template parameter (weighted layer)
- [ ] Step 7: new algorithms — ordering → scc → shortest_paths → mst (tracked above)
- [ ] Step 8: warning sweep, delete stale *.hints.md strays, update README

## Lean 4 formalization (spec: plans/architecture.md `## lean_formalization` — cslib-aligned)

<!-- Stub marker on this track is `sorry`; drive it with
     `portpal watch lean4/... --instructor formalization-engineer`. -->

- [ ] `lake init` the lean4/ project (toolchain pinned, builds clean)
- [ ] Core representation chosen (cslib-first) + container invariant lemmas
- [ ] Traversal: termination pattern settled; dfs/bfs reachability soundness + completeness
- [ ] BFS depth = shortest edge-count distance
- [ ] Components: connectivity is an equivalence; id ↔ connected
- [ ] Cycles: witness soundness; acyclicity completeness
- [ ] Ordering: topological soundness; DAG ↔ order exists (totality)
- [ ] SCC: mutual-reachability equivalence; condensation is a DAG
- [ ] Long horizon: Dijkstra correctness; MST cut/cycle properties + union-find invariants

## Code quality (subsumed by the rework; kept for The_Architect history)

- [ ] Remove duplicate `Get_cc_Count` / `Get_CC_Count` accessors (Graph.hpp:150, :194) — falls out of step 3
- [ ] Return `adj()` / `adj_list()` by const-reference instead of by value — step 4
- [ ] Fix sign-compare warnings in traversal loops — step 8
- [ ] Remove untracked `Graph.cpp.hints.md` / `Graph.hpp.hints.md` or fold into docs — step 8
