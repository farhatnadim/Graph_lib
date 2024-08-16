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


std::vector<std::string> GRAPH_FILENAMES {"tinyG.txt","tinyCG.txt"};
const std::string ROOT_DIR = "../data/";



TEST(TestTopic, ConnectedComponents)
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

TEST(TestTopic, ShortestPath)
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

TEST(TestTopic, Cycle)
{
    std::cout << "Tests cycle detection in a graph\n";
    const std::string graph_data_file(ROOT_DIR + "NoCycle.txt");
    auto file = ifstream(graph_data_file);
    Graph<int> NoCycle(file, Graph_Input_type::EXPLICIT);
    detect_cycle<int>(NoCycle);
    bool result = NoCycle.HasCycle();
    EXPECT_EQ(result,false);
    /**
    const std::string graph_data_file2(ROOT_DIR + "YesCycle.txt");
    auto file2 = ifstream(graph_data_file2);
    Graph<int> YesCycle(file2, Graph_Input_type::EXPLICIT);
    EXPECT_EQ(result,true);
    */
}
