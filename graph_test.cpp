#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <stack>
#include <queue>
#include <gtest/gtest.h>
#include "Graph.hpp"
#include "GraphAlgo.hpp"

using namespace std;


std::vector<std::string> GRAPH_FILENAMES {"tinyG.txt","tinyCG.txt","tinyDG.txt"};
const std::string ROOT_DIR = "../data/";

TEST(Undirected_Graph, ConnectedComponents)
{
    std::string graph_data_file(ROOT_DIR + GRAPH_FILENAMES[0]);
    auto file = ifstream(graph_data_file);
    Graph<int> tinyG(file, Graph_Input_type::EXPLICIT);
    //tinyG.cc();
    cc<int> (tinyG);
    EXPECT_EQ(tinyG.Get_cc_Count(),3) << "Testing tinyG\n";
    file.close();
    graph_data_file = ROOT_DIR + GRAPH_FILENAMES[1];

    file = ifstream(graph_data_file);
    Graph<int> tinyCG(file, Graph_Input_type::EXPLICIT);
    cc<int>(tinyCG);
    EXPECT_EQ(tinyCG.Get_cc_Count(),1) << "Testing tinyCG\n";
    file.close();
}

TEST(Undirected_Graph, ShortestPath)
{
    std::cout << "Tests Shortest Path Via Breadth First Search\n";
    const std::string graph_data_file(ROOT_DIR + GRAPH_FILENAMES [0]);
    auto file = ifstream(graph_data_file);
    Graph<int> tinyG(file, Graph_Input_type::EXPLICIT);
    bfs<int>(tinyG,0);
    auto result = tinyG.PathTo(0,5);
    EXPECT_EQ(result[0],5) << "Testing tinyG\n";
    EXPECT_EQ(result[1],0) << "Testing tinyG\n";
    
    result = tinyG.PathTo(0,3);
    EXPECT_EQ(result[0],3) << "Testing tinyG\n";
    EXPECT_EQ(result[1],5) << "Testing tinyG\n";
    EXPECT_EQ(result[2],0) << "Testing tinyG\n";

    result = tinyG.PathTo(0,6);
    EXPECT_EQ(result[0],6) << "Testing tinyG\n";
    EXPECT_EQ(result[1],0) << "Testing tinyG\n";
    //EXPECT_EQ(result[2],0) << "Testing tinyG\n";
}

TEST(Undirected_Graph, DetectCycle)
{
    std::cout << "Tests cycle detection in a graph\n";
    auto graph_data_file = string(ROOT_DIR + "NoCycle.txt");
    auto file = ifstream(graph_data_file);
    Graph<int> NoCycle(file, Graph_Input_type::EXPLICIT);
    detect_cycle<int>(NoCycle);
    bool result = NoCycle.HasCycle();
    EXPECT_EQ(result,false);
    graph_data_file = string(ROOT_DIR + "YesCycle.txt");
    file = ifstream(graph_data_file);
    Graph<int> YesCycle(file, Graph_Input_type::EXPLICIT);
    detect_cycle<int>(YesCycle);
    result = YesCycle.HasCycle();
    EXPECT_EQ(result,true);
}

// test directed graph dfs from multiple sources
// if single source of 1 , it returns 1
// if single source of 2 , it returns 0,1,2,4,5
// if multiple sources of 1,2,6 it return 0,1,2,3,4,5,6,8,9,10,11,12

TEST(Directed_Graph, DFSFromMultipleSources)
{
    std::cout << "Tests DFS from multiple sources in a directed graph\n";
    auto graph_data_file = string(ROOT_DIR + "tinyDG.txt");
    auto file = ifstream(graph_data_file);
    Graph<int> tinyDG(file, Graph_Input_type::EXPLICIT, true);
   

    // Test single source 1
    directed_dfs<int>(tinyDG, {1});
    auto result = tinyDG.PathTo(1, 1);
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], 1) << "Testing single source 1\n";

    // Test single source 2
    tinyDG.reset_explored();
    directed_dfs<int>(tinyDG, {2});
    result = tinyDG.PathTo(2, 5);
    std::vector<int> expected_path_2 = {5, 0, 2};
    EXPECT_EQ(result, expected_path_2) << "Testing single source 2\n";

    // Test multiple sources 1, 2, 6
    tinyDG.reset_explored();
    directed_dfs<int>(tinyDG, {1, 2, 6});
    std::vector<int> expected_vertices = {0, 1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12};
    for (const auto& vertex : expected_vertices)
    {
        EXPECT_TRUE(tinyDG.Explored(vertex)) << "Vertex " << vertex << " should be explored\n";
    }
}




