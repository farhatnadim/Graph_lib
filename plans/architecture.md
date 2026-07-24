# Graph_lib rework — architecture specification

Status: **spec only — no code in this document is to be written by an assistant.**
You (Nadim) implement everything; portpal mentors against this file (it is wired
as `docs.porting_notes` in `.portpal.toml`, one `##` section per unit).

The library stays what GOALS.md says it is: a C++23 templated graph library in
the Sedgewick tradition, algorithms as non-member functions — but reworked so
that the remaining algorithm goals (topological sort, SCC, Dijkstra, A*, MST)
and the aspirational Graph-RAG backend both fit without another rewrite.

---

## Why rework: the three root problems

1. **Algorithm state lives inside `Graph<T>`.** `m_explored`, `m_edgeTo`,
   `m_id`, `m_on_stack`, `isCyclic`, `m_cc_count` are members of the container.
   Consequences: algorithms take `Graph&` (nothing is const), two algorithms
   cannot run on the same graph without `reset_explored()` choreography,
   results of one query silently leak into the next (`HasCycle` for digraphs
   reads `m_cycle`, which nothing ever fills), and the class needs eleven
   getter/setter pairs that exist only to let free functions poke its guts.

2. **The template parameter is an illusion.** Adjacency is `map<T, set<T>>`,
   but every state array is a `vector` indexed *directly by `T`*
   (`m_explored[v]`, `m_edgeTo[x]`). The library only works when `T` is a
   dense non-negative integer. A `Graph<std::string>` — which a knowledge
   graph needs — compiles against the map half and is undefined behavior in
   the vector half.

3. **The edge model is too weak.** Edges carry no payload, so the entire
   weighted half of the roadmap (Dijkstra, A*, Kruskal, Prim) has nowhere to
   put a weight, and a knowledge-graph edge has nowhere to put a relation.

## Design decisions

**D1 — Separate structure from algorithms (Sedgewick's actual pattern).**
`Graph` becomes a pure container: construction, mutation (add vertex/edge),
and structural queries only. Every algorithm is a free function that takes
the graph **by const reference** and returns a *result object* that owns all
the state that today pollutes the class. Queries about an algorithm's outcome
(path to, is connected to, component id, has cycle) are methods on the result
object, never on the graph. This is exactly Sedgewick's `DepthFirstPaths` /
`CC` / `Cycle` object design (the book's answer on p. 557 that the current
code half-follows), expressed as function → value instead of class-runs-in-
constructor. Rule of thumb: if a member exists only so an algorithm can write
to it, it moves out.

**D2 — Two-layer vertex identity: label ⇄ dense id.** Internally every vertex
is a dense id (`0 … V−1`, a `size_t`), assigned at first insertion, never
reused, stable for the graph's lifetime. Externally the API speaks the user's
label type `T`. A small bidirectional interner (label→id map + id→label
vector) is the only place the two meet. Everything downstream — adjacency,
every algorithm's state — is contiguous arrays indexed by id: O(1), cache-
friendly, and correct for *any* hashable/orderable `T`. This single decision
fixes problem 2, makes `Graph<std::string>` first-class (the knowledge-graph
prerequisite), and lets algorithm state be `vector`s again *legitimately*.
Results translate ids back to labels at the query boundary so callers never
see ids unless they ask.

**D3 — Edge payload as a template parameter.** `Graph<T, E>` where `E`
defaults to "nothing" (unweighted). Adjacency per vertex is a contiguous
sequence of (target id, payload) entries — bag semantics like Sedgewick's:
parallel edges are allowed, self-loops are allowed, and the *algorithms* that
care (undirected cycle detection, MST) state their own preconditions instead
of the container forbidding things. Instantiate `E` as a numeric weight for
Dijkstra/MST; later a knowledge-graph edge instantiates it as a relation
label. The unweighted algorithms (units below through `scc`) must compile for
any `E` — they simply never read the payload.

**D4 — Directedness is fixed at construction.** One enum argument
(directed / undirected), immutable afterwards; `SetDigraph()` and friends
disappear. Reversal is a pure function returning a *new* graph (as today),
defined for directed graphs only — make that a compile-time or documented
precondition rather than a printed apology.

**D5 — The library is silent and total.** No `iostream` in any library code
path. Structural lookups on a missing vertex return an empty/`optional`
answer or are documented preconditions — never a `cerr` line. Parsing returns
`std::expected<Graph, ParseError>` (this is the C++23 practice run: pick
`expected` over exceptions and be consistent). `PathTo` returns the path or
an empty `optional` — printing is the caller's job.

**D6 — Const-correctness is the proof.** After the rework, every algorithm
signature takes `const Graph&`. If it doesn't compile const, state hasn't
really moved out. This is the cheapest possible regression test for D1 and
the first thing a reviewer checks.

## Proposed file layout

One header per unit, so portpal's file→unit mapping is exact and each header
stays small enough to reason about:

    include/graphlib/graph.hpp            — container + interner (unit graph_core)
    include/graphlib/io.hpp               — file loading, DOT export (unit io)
    include/graphlib/traversal.hpp        — dfs, bfs + their results (unit traversal)
    include/graphlib/components.hpp       — connected components (unit components)
    include/graphlib/cycles.hpp           — undirected + directed cycles (unit cycles)
    include/graphlib/ordering.hpp         — topological sort (unit ordering)
    include/graphlib/scc.hpp              — strong components (unit scc)
    include/graphlib/shortest_paths.hpp   — dijkstra, a_star (unit shortest_paths)
    include/graphlib/mst.hpp              — union-find, kruskal, prim (unit mst)
    tests/<unit>_test.cpp                 — one GoogleTest file per unit

Adapt names as you like — but when you split, uncomment the matching unit in
`.portpal.toml` so hints/status/verify resolve per topic.

## Migration plan (each step leaves the build and tests green)

1. **Interner + dense ids inside the current class.** Introduce the label⇄id
   layer and re-point the state vectors at ids. Behavior identical for
   `Graph<int>`; the tinyG/tinyCG/tinyDG tests must not change.
2. **First result object: traversal.** Move `m_explored`/`m_edgeTo` into a
   DFS/BFS result type; port `dfs`, `bfs`, `PathTo` to it; delete the
   corresponding getters/setters. The graph parameter goes const here.
3. **Components, then cycles**, same treatment — `m_id`/`m_cc_count` into the
   components result, `m_on_stack`/`isCyclic`/`m_cycle` into the cycle
   result. After this step the graph has *no* algorithm members left and the
   duplicate `Get_cc_Count`/`Get_CC_Count` pair is gone by construction.
4. **Silence and totality (D5).** Factory-function parsing with `expected`,
   no printing anywhere, `adj` returns a const view not a copy.
5. **Split files** into the layout above; retire `GraphAlgo.hpp`;
   per-unit test files; uncomment portpal units.
6. **Edge payload (D3).** Thread `E` through container and adjacency;
   existing algorithms compile payload-agnostic.
7. **New algorithms**, in dependency order: ordering → scc (needs reverse +
   postorder) → shortest_paths (needs payload) → mst (needs union-find).
8. **Sweep**: sign-compare warnings, delete the stale `*.hints.md` strays,
   README reflects the new shape.

Stub convention while you work: mark unfinished bodies with
`// TODO: User implements` — that string is the portpal stub marker, so
`portpal status` tracks STUB→IMPL→VERIF per algorithm.

## lean_formalization

**Second target: prove it in Lean 4** (`lean4/` Lake project; `sorry` is the
stub marker there). The Lean side is *not* a port of the C++ — it is the
mathematical companion: define each algorithm functionally, state the
contract from that unit's section as a theorem, and prove it. The eventual
destination is upstreaming verified graph algorithms into leanprover/cslib,
so every representation and naming choice should be made with cslib in mind
(the formalization-engineer instructor searches `~/Source/cslib` declarations
for exactly this reason — prefer reusing its abstractions over inventing).

- **Representation is a separate decision from D2/D3.** The interner and
  contiguous-array design is a C++ performance concern; in Lean start from
  whatever graph representation cslib already has (or an adjacency function /
  finite structure over a vertex type with decidable equality) and keep the
  algorithms' *specifications* representation-independent: reachability,
  connectivity, and cycles are statements about edges and walks, not about
  arrays. This matters doubly because cslib's graph representation is still
  under discussion — a representation-independent spec survives that debate.
- **Termination is the first proof obligation of every traversal.** DFS/BFS
  recurse on "unvisited vertices remaining", not on the input structure —
  expect to justify termination explicitly (a decreasing measure over the
  finite vertex set, or fuel with a proof that enough fuel suffices).
  Decide the pattern once in `traversal` and reuse it.
- **Order of attack mirrors migration step 7's dependency order:**
  core representation → traversal (reachability) → components → cycles
  (witness soundness) → ordering → scc; shortest_paths and mst are the
  long-horizon targets. Each unit's "Lean obligations" paragraph below
  states what to prove.
- **Workflow per algorithm:** professor first for the theorem statements
  (math only), then write the Lean signatures with `sorry` bodies and let
  `portpal watch <file> --instructor formalization-engineer` drive the
  proofs — it brings live `lake` diagnostics, the goal at each `sorry`, and
  matching cslib declarations to every review.

---

## graph_algorithms

*(Transitional unit: while dfs/bfs/cc/cycles still live together in
`GraphAlgo.hpp`, portpal resolves that file here. The real specs are in the
per-topic sections of `plans/architecture.md` — read `## traversal`,
`## components`, `## cycles`. This section disappears at migration step 5.)*

Rework focus for this file: D1 (state out of the graph), D6 (const
parameters), and the `dfs` default-argument trap — `dfs(g, v, s = {})` makes
the undirected cycle check silently compare against a default-constructed
sentinel when called single-argument; the redesign in `## cycles` removes the
sentinel entirely.

Lean obligations: the Lean side has no transitional phase — its unit file
(`lean4/GraphLib/GraphAlgorithms/Basic.lean`) is simply where the first
dfs/bfs/cc/cycle formalizations live until the per-topic split; the real
obligations are stated in `## traversal`, `## components`, `## cycles`, and
the strategy in `## lean_formalization`.

## graph_core

**The container.** Owns: the interner (D2), the adjacency structure (D3),
the directedness flag (D4), and vertex/edge counts — nothing else.

- Operations and their contracts: add-vertex (idempotent per label, returns
  the id), add-edge (interns both endpoints; undirected graphs store the
  edge once but answer adjacency symmetrically, or store twice — pick one
  and document it; edge count must mean *edges*, not stored half-edges),
  adjacency-of (const view over (neighbor, payload) entries; O(degree)
  iteration, no copies — problem 3 of the old `adj()`), has-vertex,
  has-edge, degree, vertex count, edge count, directedness query, and
  iteration over all vertices and all edges.
- The current class trusts the file header for `m_e` and lets
  `SetEdgeNumber` override it; the new edge count is *derived from
  insertions* and cannot be set.
- Invariants to state in comments and test: ids are `0…V−1` with no gaps;
  interner round-trips (label→id→label is identity); undirected adjacency is
  symmetric; counts match iteration.
- Complexity targets: add-edge amortized O(log V) or O(1) depending on your
  interner map choice (ordered vs hashed — pick deliberately and note why);
  adjacency-of O(1) to obtain the view.
- Tests: interner round-trip on `Graph<std::string>` (the D2 acceptance
  test), symmetry, counts, degree on tinyG.
- Lean obligations: pick the representation per `## lean_formalization`
  (cslib-first; interner-free) and prove the container invariants as
  theorems — undirected adjacency is symmetric, an added edge is adjacent,
  vertex/edge counts agree with the underlying finite sets. These small
  lemmas are the API every later proof leans on; name them the way cslib
  names its accessors.

## io

**Loading and DOT export as free functions** (construction from a stream is
not the container's business — problem: today's constructor half-parses and
ignores failures).

- Text format unchanged (V, E, then edge pairs) so `data/*.txt` fixtures
  keep working; the loader takes the directedness choice as an argument.
- Contract: returns `std::expected<Graph, ParseError>` where ParseError
  carries line number and what was expected — malformed header, non-numeric
  token, fewer edges than promised. No partial graphs escape.
- Weighted variant (needed by migration step 7): third column = weight;
  decide and document how the loader for `E`=weight treats a missing column.
- DOT export: current `drawGraph` logic is fine (digraph/`->` vs
  graph/`--`), but as a free function over the const container view; add
  payload rendering as edge labels when `E` is streamable, and quote-escape
  labels (a knowledge-graph label with a `"` must not corrupt the file).
- Tests: round-trip load→export on tinyG/tinyDG fixtures; every ParseError
  case with a small inline-string stream.

## traversal

**DFS and BFS, each returning a result object** (Sedgewick's
`DepthFirstPaths`/`BreadthFirstPaths`, as values).

- Result contract (both): remembers its source; answers reached(v),
  path-to(v) (reconstructed from a parent/edge-to array over ids, translated
  back to labels, source-first; `optional`-empty when unreached — never
  prints), and count of reached vertices. BFS's path is additionally a
  *shortest* path in edge count — say so in its contract.
- DFS: specify **iterative with an explicit stack**. The current recursive
  version is fine for tinyG but a knowledge graph or a long path graph will
  blow the call stack; the explicit-stack discipline also sets up
  `directedDFS`'s on-stack logic and Tarjan later. Preorder/postorder
  sequences are cheap to record and `scc`/`ordering` will need postorder —
  design the result to expose them.
- Multi-source variant (today's `directed_dfs`): same result type,
  constructed from a range of sources — reached(v) means "from any source".
- Complexity: O(V + E) time, O(V) space — state it, and note why the
  interner makes this true for any `T`.
- Tests: tinyCG shortest-path expectations from the current suite carry
  over; add an unreached-vertex query (empty optional, no output) and a
  `Graph<std::string>` traversal.
- Lean obligations: termination first (the measure/fuel pattern of
  `## lean_formalization` is decided here). Then the two directions of
  correctness — soundness: every vertex the traversal marks reached has a
  walk from the source; completeness: every vertex with a walk from the
  source gets marked. For BFS additionally: the recorded depth equals the
  minimum edge-count distance (this is the theorem `## shortest_paths`'
  unit-weight cross-check turns into a test on the C++ side).

## components

**Connected components for undirected graphs** (today's `cc`).

- Contract: result answers count(), id(v), connected(u, v), and the members
  of a component; built by repeated DFS over unvisited ids. Directed input
  is a precondition violation — decide (documented precondition vs
  `expected`) and be consistent with D5.
- The old bug class to design away: `Connected_vertices` on the graph read
  `m_id` whether or not `cc` had ever run. A result object cannot express
  that bug — you can only query what was actually computed. Worth a sentence
  in your PR/commit message; it is the whole argument for D1.
- Tests: tinyG → 3 components (current expectation), tinyCG → 1; ids
  consistent with connected(); a string-labeled graph.
- Lean obligations: connectivity is an equivalence relation — reflexive,
  symmetric (this is where the undirected-symmetry lemma from
  `## graph_core` pays off), transitive — and the component id is its
  quotient: `id(u) = id(v) ↔ connected(u, v)`. Mathlib's equivalence/
  quotient machinery (or cslib's, if it has one by then) is the reuse
  opportunity to search for before proving anything by hand.

## cycles

**Undirected and directed cycle detection, both returning evidence.**

- Undirected contract: has-cycle plus, when cyclic, the witness cycle as a
  vertex sequence (first = last). Design point that removes today's
  default-argument trap: track the *parent* of each visit explicitly in the
  traversal state instead of threading a sentinel source parameter. State
  the parallel-edge/self-loop policy (bag semantics of D3 means two equal
  edges *are* a cycle — decide and test it).
- Directed contract (today's `directedDFS_return_cycle`, which works but
  strands its result): three-state coloring or an on-stack array *inside the
  result computation*; returns the witness cycle. The `HasCycle`-reads-
  empty-`m_cycle` bug disappears because the only way to ask is via the
  returned value.
- The witness matters downstream: `ordering` reports it on failure, and
  verifying "the returned sequence is genuinely a cycle in the graph" is a
  property test, not an example test — write it once, reuse for both.
- Tests: YesCycle/NoCycle fixtures, tinyDG (cyclic), the self-loop and
  parallel-edge edge cases, and the cycle-witness property check.
- Lean obligations: the C++ property test becomes a theorem — witness
  soundness: when the detector returns a sequence, that sequence *is* a
  cycle in the graph (consecutive vertices adjacent, first = last, and
  whatever non-triviality condition your parallel-edge/self-loop policy
  demands — the policy decision above must be made *before* the Lean
  definition of "cycle", not after). Completeness (no cycle reported ⇒
  the graph is acyclic) is the harder direction; it is also exactly the
  lemma `## ordering` needs, so prove it here once.

## ordering

**Topological sort for DAGs** (new; first unchecked algorithm goal).

- Contract: `expected<order, cycle-witness>` — success is a vertex sequence,
  failure *is* the directed-cycle evidence from `## cycles` (this is why
  cycles returns a witness). Precondition: directed input.
- Algorithm: reverse DFS postorder (which the traversal result already
  exposes if you designed it that way — this is the dependency to check
  before implementing). Kahn's queue-based alternative is a fine second
  implementation to cross-test against; pick one as canonical.
- Property test (stronger than any fixture): for every edge u→v, u appears
  before v in the returned order. Run it on tinyDAG-style data you add to
  `data/`; tinyDG must return its cycle instead.
- Lean obligations: the property test is the soundness theorem — the
  returned order is topological (every edge points forward in it) and is a
  permutation of the vertices. The `expected` shape becomes a totality
  theorem: the function returns an order *or* a genuine cycle witness
  (reusing `## cycles`' soundness), which together prove the classic
  equivalence "a finite digraph is a DAG iff it admits a topological
  order" — state that as the headline theorem.

## scc

**Strongly connected components — Kosaraju** (new).

- Contract: same query surface as `## components` (count, id, same-scc), on
  directed graphs.
- Algorithm: DFS the *reverse* graph, take reverse postorder, DFS the
  original graph in that order; each tree is one SCC. Dependencies you
  already have if the earlier units followed spec: `reverse_graph` (exists),
  postorder exposure (traversal), multi-source/ordered-source DFS. Kosaraju
  first *because* it composes those pieces; Tarjan is an optional follow-up
  in one pass with the explicit stack.
- Note for the commit message: SCC condensation is a DAG — one sentence of
  theory that `ordering` + `scc` together let you test (condense tinyDG,
  topologically sort the condensation; it must succeed).
- Tests: tinyDG's known SCC partition (Sedgewick's example: 5 strong
  components), condensation-is-a-DAG property.
- Lean obligations: mutual reachability is an equivalence relation (the
  directed sibling of `## components`' theorem), and Kosaraju's output is
  its set of classes: `same_scc(u, v) ↔ (reaches(u, v) ∧ reaches(v, u))`.
  The condensation-is-a-DAG note above is the second theorem. This is the
  deepest proof of the roadmap before the weighted units — expect the
  correctness argument about reverse-postorder to require real invariant
  work, and lean on the professor for the paper proof first.

## shortest_paths

**Dijkstra, then A\*** (new; needs the weighted payload from migration
step 6).

- Dijkstra contract: single source over non-negative weights; result answers
  distance-to(v) (`optional` — absent means unreachable) and path-to(v).
  Negative weight encountered = precondition violation; decide the D5-
  consistent behavior *before* implementing and test it.
- Algorithm guidance (decisions, not code): `std::priority_queue` with
  lazy deletion — push duplicates, skip entries whose recorded distance
  already improved — is the standard-library-friendly choice vs. a
  decrease-key structure; document why. Complexity O(E log V).
- A* contract: Dijkstra plus a caller-supplied heuristic h(v). State the
  admissibility/consistency requirement in the interface documentation, and
  the degenerate law: h ≡ 0 must reproduce Dijkstra exactly — that law is a
  property test.
- Cross-check property (cheap and strong): on a graph with all weights = 1,
  Dijkstra distances must equal BFS depths. Run it on tinyCG.
- Tests: a small weighted fixture in `data/` (extend the format per `## io`),
  unreachable vertex, the h≡0 law, the BFS-equivalence property.
- Lean obligations: Dijkstra correctness under the non-negativity
  hypothesis (stated as an explicit hypothesis on the theorem, mirroring
  the C++ precondition): the returned distance to every reachable vertex
  is the minimum total weight over all walks from the source, and `none`
  means unreachable. The two C++ property tests are theorems here too —
  unit weights collapse Dijkstra to BFS depth, and A* with h ≡ 0 *equals*
  Dijkstra. Long-horizon: check what cslib/Mathlib already have on
  shortest paths before starting.

## mst

**Minimum spanning tree — Kruskal and Prim** (new; undirected + weighted).

- Shared contract: input undirected, connected (decide: precondition or
  per-component spanning forest — the forest is more useful and barely
  harder; document the choice); result is the edge set and total weight.
- Kruskal needs **union-find** — spec it as its own small, separately tested
  component (union by rank + path compression, near-O(1) amortized ops);
  it will be reused far beyond MST.
- Prim mirrors Dijkstra's lazy-PQ structure — implement second and let the
  structural similarity show.
- Determinism note for testing: with distinct weights the MST is unique —
  build the fixture with distinct weights so both algorithms must return the
  *same* edge set; that equality is the test. Total-weight equality is the
  weaker fallback if you ever allow ties.
- Lean obligations: the cut property and the cycle property are the two
  lemmas everything rests on — prove them abstractly (about any graph, any
  spanning tree), then Kruskal's correctness is an induction using the cut
  property. Union-find deserves its own file and invariants: the structure
  represents a partition, union merges exactly two classes, find respects
  it. Distinct-weights-imply-uniqueness is the same theorem the C++ test
  exploits — proving it justifies the test design.

## graph_rag_readiness

**Deferred — design note only** (per GOALS; the RAG engine itself is your
project and out of scope for this spec). What the knowledge-graph backend
will ask of the library, and which decision already covers it: string/entity
vertices (D2), relation-labeled edges (D3's `E`), k-hop neighborhood
expansion (BFS result already computes depth ≤ k if you expose depth —
cheap to add in `## traversal` now), and graph serialization (the `## io`
loader/exporter pair generalizes). When you write the Graph-RAG design note
in `plans/`, start from these four hooks; nothing else in the library should
need to change.
