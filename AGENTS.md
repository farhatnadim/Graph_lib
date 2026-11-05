# CLAUDE.md

<!-- architect:rules start — shared rules, edited in The_Architect/rules/AGENTS.rules.md -->

## Contribution rules

1. **This file is not an agentic dump.** Its rules are entered only by a
   human: in writing, or by dictation assisted by an agent.
2. **Follow a proper software development life cycle.** Every feature and
   every bug has proper requirements, a proper specification and a proper
   implementation, and everything lands by merge request. A merge request
   may be reviewed by an agent, depending on its severity and the size of
   the change, but not necessarily.
3. **Requirements and specifications may be written by the planning agent
   or by a human.**
4. **Humans or agents write the code, after proper adherence to the
   software development life cycle.** Keep the build and the tests honest;
   write the docs.
5. **Requirements and specification are different things, and everyone,
   human or agent, keeps them apart.** A requirement says what the system
   must do, in general terms, and names no technology. A specification is
   the specific instance of the requirements that led to the current
   program; it names the technology. A technology is named in a
   requirement only when the system is defined by it: if the system must
   run on Linux, Linux is in the requirements; if it happens to be written
   in Python, Python is in the specification.
6. **Name every plan after its objective, and list every plan in
   `plans.csv`.** A plan file is in the `plans/` directory and is named
   `objective-<N>-<short-name>.md`. `<N>` is the number of the objective
   in the AGENTS.md of the project. No other plan name is allowed.
   `plans.csv` is in the project root and has one row for each plan, with
   these columns: `name`, `location`, `summary`, `date`, `implemented`,
   `commit`. `implemented` is `yes` or `no`. `commit` holds the commit
   that implemented the plan, and is empty while `implemented` is `no`.
   The agent that writes a plan adds its row in the same merge request.
   The agent that implements the plan updates its row.

## Communication

**If something can be explained with a picture, a graph, a workflow or an
animated GIF, do it before writing.**

**Write in ASD-STE100, Simplified Technical English, the aerospace
standard.** At least 80% of the text follows it; connective words may be
added if needed.

<!-- architect:rules end -->

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
# Configure (first time or after CMakeLists.txt changes)
cmake -B build

# Build
./runBuild   # or: cmake --build build

# Run tests
./runTest    # or: ctest --test-dir build --output-on-failure -j12

# Run a single test
./build/GraphTest --gtest_filter=TestTopic.ConnectedComponents

# Generate graph visualization
./runDrawDotGraph.sh <input_graph_file> <output_dot_file> <output_png_file>
```

## Architecture

This is a C++23 templated graph library inspired by Sedgewick's "Algorithms" book.

**Key design principle**: Graph algorithms are implemented as non-member template functions (following Sedgewick page 557), separate from the Graph data structure itself.

### Core Files

- **Graph.hpp** - Template class `Graph<T>` using adjacency list representation (map of sets). Supports both directed (`isDigraph=true`) and undirected graphs. Contains graph state: explored vertices, edge paths, connected component IDs.

- **GraphAlgo.hpp** - Non-member algorithm functions:
  - `dfs<T>(g, v, s)` - Depth-first search
  - `bfs<T>(g, v)` - Breadth-first search
  - `cc<T>(g)` - Connected components
  - `detect_cycle<T>(g)` - Cycle detection
  - `reverse_graph<T>(g)` - Reverse a directed graph

- **drawGraph.cpp** - CLI tool (`GraphDraw`) that outputs Graphviz DOT format

### Graph Input Format

Graphs are loaded from files in `data/` with format:
```
<vertex_count>
<edge_count>
<v1> <w1>
<v2> <w2>
...
```

Use `Graph_Input_type::EXPLICIT` when constructing from file.

### Testing

Uses Google Test. Test data files are in `data/` directory, referenced via relative path `../data/` from build directory.
