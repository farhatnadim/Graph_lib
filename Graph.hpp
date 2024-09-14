#pragma once
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <memory>
#include <iostream>
#include <stack>
#include <queue>

// Inspired by Sedgewick's Graph implementation (page 540)
// Since the
template <typename T>
class Graph;

// Forward declare the drawGraph function template
template <typename T>
void drawGraph(std::ostream &out, const Graph<T>& graph);

enum class Graph_Input_type { IMPLICIT, EXPLICIT };

template <typename T>
class Graph
{
    using edges_t = std::set<T>;
    using adj_list_t = std::map<T, edges_t>;

public:
    // Constructors
    Graph (int V)
    {   
        initialize_explicit(V,0);
    }

    Graph(std::ifstream& f, Graph_Input_type input, bool diGraph = false) : m_cc_count{0}, isCyclic{false}, isDigraph{diGraph}
    {
        std::string edges;
        std::string vertices;
        if(input == Graph_Input_type::EXPLICIT)
         {
            if (getline(f, vertices) && getline(f, edges))
            {
                initialize_explicit(std::stoi(vertices), std::stoi(edges));
                int u, v;
                while (f >> u >> v)
                {
                    addEdge(u, v);
                }
            }
         }
        
        }
    void initialize_explicit(int V, int E)
    {
        m_v = V;
        m_e = E;
        m_adj_list.clear();
        m_explored.assign(m_v, false);
        m_on_stack.assign(m_v, false);
        m_edgeTo.assign(m_v, 0);
        m_id.assign(m_v, 0);
    }

    void addEdge(T v, T w)
{  
    m_adj_list[v].insert(w);

    if (!isDigraph)
    {
        // If it's an undirected graph, add the reverse edge as well
        m_adj_list[w].insert(v);
    }
    else
    {
        // If it's a directed graph, ensure the destination node exists in the adjacency list
        if (m_adj_list.find(w) == m_adj_list.end())
        {
            m_adj_list[w] = edges_t(); // Create an empty entry for node w
        }
    }

    m_e++;  // Increment edge count
}

    edges_t adj(T v) const
    {   
        try 
        {
            return m_adj_list.at(v);
        }
        catch (std::out_of_range & e)
        {
            std::cerr << "Exception caught: the  " << v  << "  " << e.what() << '\n';
        }
    }

    adj_list_t adj_list() const
    {
        return m_adj_list;
    }

    const int& Get_vertices_number() const
    {
        return m_v;
    }

    const int& Get_edges_number() const
    {
        return m_e;
    }

    bool isGraphConnected() const
    { 
        std::size_t explored_accumulator = 0;
        for (const auto& element : m_explored)
        {
            if (element)
            {
                explored_accumulator++;
            }
        }
        return explored_accumulator == m_v;
    }

    std::vector<T> PathTo(const T &s, const T & v) const
    {
        if (!HasPathTo(v))
        {
            std::cout << "Warning! Did not find a path to the vertex " << v << std::endl;
            return {};
        }
        std::vector<T> path;

        for (T x = v; x != s; x = m_edgeTo[x])
        {
            path.push_back(x);
        }
        path.push_back(s);
        for (auto element : path)
            std::cout << element << "\n";
        return path;
    }

    void reset_explored()
    {
        std::fill(m_explored.begin(), m_explored.end(), false);
    }


    const int& Get_cc_Count() const
    {
        return m_cc_count;
    }

    bool Explored(const T & v)
    {
        return m_explored[v];
    }

    void SetExplored(const T & v)
    {
        m_explored[v] = true;
    }
    const std::vector<bool> & GetExplored()
    {
        return m_explored;
    }

    void SetEdgeTo(const T & e, const T & v)
    {
        m_edgeTo[e] = v; 
    }

    void SetId(const T & v, const int & count)
    {
        m_id[v] = count;
    }

    const int & Get_CC_Count() const 
    {
        return m_cc_count;
    }

    void incrementCC_Count(int amount)
    {
        m_cc_count += amount;
    }

    const std::size_t& Get_vertex_id(const std::size_t& v) const
    {
        return m_id[v];
    }

    bool Connected_vertices(const T& v, const T& w) const
    {
        return m_id[v] == m_id[w];
    }

    bool HasPathTo(const T &v) const
    {
        return m_explored[v];
    }

    bool HasCycle() const 
    {
        return isCyclic;
    }

    void SetCyclic()
    {
        isCyclic = true;
    }



    void SetEdgeNumber(int n_edges)
    { 
        m_e = n_edges;
    }

    bool Digraph() const
    { 
        return isDigraph;
    }

    void SetDigraph() 
    { 
        isDigraph = true;
    }

    bool OnStack(const T & v)
    {
        return m_on_stack[v];
    }

    friend void drawGraph<T>(std::ostream &out, const Graph<T>& graph);

protected:
    int m_v; // number of vertices
    int m_e; // number of edges
private:    
   
    bool isConnected; // Is the graph connected
    bool isCyclic; // Is the graph cyclic
    bool isDigraph; // Is the graph a directed graph
    int m_cc_count; // connected components count

    std::vector<T> m_edgeTo; // edge to vertex
    std::vector<bool> m_explored; //list of explored vertices
    std::vector<std::size_t> m_id; // connected components id
    std::stack<T> m_cycle; // stack for returning the cycle
    std::vector<bool> m_on_stack; // stack for checking if a vertex is on the stack
    adj_list_t m_adj_list; // adjacency list
    
};



/// Move below to a different file called Graph_Algo
template <typename T>
void drawGraph(std::ostream &out, const Graph<T>& graph)
{
    using adj_list_t = typename Graph<T>::adj_list_t;
    using edges_t = typename Graph<T>::edges_t;

    const adj_list_t& list = graph.m_adj_list;
  
    const bool isDigraph = graph.Digraph();
    std::string graph_type = isDigraph   ? "digraph" : "graph";
    std::string edge_type = isDigraph   ? " -> " : " -- ";

    out << graph_type << " G {" << "\n";
    for (const auto& [key, edges] : list)
    {
        for (const auto& element : edges)
            out << "\"" << key << "\"" << edge_type << "\"" << element << "\"" << ";\n";
    }
    out << "}\n";
}




