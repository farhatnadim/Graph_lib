# CLAUDE.md

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
