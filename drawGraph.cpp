#include <iostream>
#include <fstream>
#include "Graph.hpp"
#include "GraphAlgo.hpp"

using std::cout, std::ifstream, std::ofstream;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <input_graph_file> <output_dot_file>\n";
        return 1;
    }

    std::string input_file = argv[1];
    std::string output_file = argv[2];
    
    auto file = ifstream(input_file);
    if (!file) 
    {
        std::cerr << "Error opening input file: " << input_file << "\n";
        return 1;
    }

    Graph<int> Digraph(file, Graph_Input_type::EXPLICIT,true);
    file.close();

    auto out_file = ofstream(output_file);
    if (!out_file) {
        std::cerr << "Error opening output file: " << output_file << "\n";
        return 1;
    }

    auto cycle = directedDFS_return_cycle<int>(Digraph,0);
    for ( auto && element : cycle)
        cout << element ;
    cout << "\n";
    out_file.close();
    return 0;
}
