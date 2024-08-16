#pragma once 
// According to sedgwick Page 557 Q&A: and I paraphrase, the graph algorithms are
// too numerous to be included as a part of a class therefore i am going to write all the
// algo here. Naiively i started by implementing them as methods but now i see that i shouldn't
// Should they be in classes ? I am not sure. 

#include "Graph.hpp"
#include <functional>




/** Depth first search for connected Componets */



//According to Lumsdaine , Graph Boost Maintainer, DFS and BFS should be 
// a type of Generic data structure accessors but I guess i will leave it for later because the focus is exposure to graphs
// not software engineering arround a graph

template <typename T>
void detect_cycle (Graph<T> & g) 
{      
 for (std::size_t s = 0; s < g.Get_vertices_number(); s++)
    {
        if (!g.Explored(s))
        {
            dfs<T>(g,s,s);
        }
    }    
}
// 4 



template <typename T> 
void dfs( Graph<T> & g , const T& v, const T & s = {})
{
    g.SetExplored(v);

    g.SetId(v, g.Get_CC_Count());
    for (const auto& edge : g.adj(v))
    {
        if (!g.Explored(edge))
        {
            g.SetEdgeTo(edge,v);
            dfs<T>(g,edge,v);
        }
        else if ( edge != s )
        {
            g.SetCyclic();
        }
    }
} // end of the function DFS 

template <typename T>
void cc(Graph<T> &g)
{
    for (std::size_t s = 0; s < g.Get_vertices_number(); s++)
    {
        if (!g.Explored(s))
        {
            dfs<int>(g,s);
            g.incrementCC_Count(1);
        }
        }    
}

// Breadth first search
template <typename T>
    void bfs(Graph<T> &g,const T& v)
    {
        g.SetExplored(v);
        std::queue<T> q;
        q.push(v);

        while (!q.empty())
        {
            T vertex = q.front();
            q.pop();
            for (const auto& edge : g.adj(vertex))
            {
                if (!g.Explored(edge))
                {
                    g.SetEdgeTo(edge,vertex);
                    g.SetExplored(edge);
                    q.push(edge);
                }
            }
        }
    }