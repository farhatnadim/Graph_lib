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
    const std::string graph_data_file(ROOT_DIR + GRAPH_FILENAMES [1]);
    auto file = ifstream(graph_data_file);
    Graph<int> tinyG(file, Graph_Input_type::EXPLICIT);
    tinyG.bfs(0); // starting at zero 
    auto result = tinyG.PathTo(0,5);
    EXPECT_EQ(result[0],5) << "Testing tinyCG\n";
    EXPECT_EQ(result[1],0) << "Testing tinyCG\n";
    result = tinyG.PathTo(0,4);
    EXPECT_EQ(result[0],4) << "Testing tinyCG\n";
    EXPECT_EQ(result[1],2) << "Testing tinyCG\n";
    EXPECT_EQ(result[2],0) << "Testing tinyCG\n";
}