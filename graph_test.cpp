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

std::vector<std::string> graph_filenames {"tinyG.txt","tinyCG.txt"};
std::vector<std::string> drawing_filenames {"tinyG.dot","tinyCG.dot"}; // me being lazy, need to change it
const std::string GRAPH_DATA_FILE("../../data/" + graph_filenames[0]);

TEST(TestTopic, ConnectedComponents)
{
    const std::string graph_data_file("../../data/" + graph_filenames[0]);
    auto file = ifstream(graph_data_file);
    Graph<int> g(file, Graph_Input_type::EXPLICIT);
    g.cc();
    EXPECT_EQ(g.Get_cc_Count(),3);
}