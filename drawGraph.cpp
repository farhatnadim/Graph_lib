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

    std::cout << "Testing cycle detection in a graph\n";
    
    auto file = ifstream(input_file);
    if (!file) 
    {
        std::cerr << "Error opening input file: " << input_file << "\n";
        return 1;
    }

    Graph<int> NoCycle(file, Graph_Input_type::EXPLICIT);
    file.close();

    auto out_file = ofstream(output_file);
    if (!out_file) {
        std::cerr << "Error opening output file: " << output_file << "\n";
        return 1;
    }

    //drawGraph(out_file, NoCycle, false);
    detect_cycle(NoCycle);
    if(NoCycle.HasCycle())
       cout << "Cycle\n";
    else
       cout << "No Cycle\n";
    {
        for (auto & [vertex, edges] : NoCycle.adj_list())
         {  
            for (auto && edge : edges )
                {
                    cout << vertex << " " << edge <<  " ";
                }
                cout << "\n";
         }
    }
    out_file.close();
    return 0;
}
