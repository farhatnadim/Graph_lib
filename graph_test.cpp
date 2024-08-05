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
    tinyG.cc();
    EXPECT_EQ(tinyG.Get_cc_Count(),3) << "Testing tinyG\n";
    file.close();
    graph_data_file = ROOT_DIR + GRAPH_FILENAMES[1];

    file = ifstream(graph_data_file);
    Graph<int> tinyCG(file, Graph_Input_type::EXPLICIT);
    tinyCG.cc();
    EXPECT_EQ(tinyCG.Get_cc_Count(),1) << "Testing tinyCG\n";
    file.close();
}

TEST(TestTopic, ShortestPath)
{
    std::cout << "Tests Shortest Path Via Breadth First Search\n";
    const std::string graph_data_file(ROOT_DIR + GRAPH_FILENAMES [0]);
    auto file = ifstream(graph_data_file);
    Graph<int> g(file, Graph_Input_type::EXPLICIT);
    g.cc();
    EXPECT_EQ(g.Get_cc_Count(),3);
}