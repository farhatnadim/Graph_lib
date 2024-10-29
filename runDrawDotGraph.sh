#!/bin/bash

if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <input_graph_file> <output_dot_file> <output_png_file>"
    exit 1
fi

input_file="$1"
output_dot_file="$2"
output_png_file="$3"

# Run the C++ program to generate the DOT file
./build/GraphDraw "$input_file" "$output_dot_file"

# Check if the DOT file was created successfully
if [ -f "$output_dot_file" ]; then
    # Convert the DOT file to a PNG using the dot command
    dot -Tpng "$output_dot_file" -o "$output_png_file"
    
    if [ $? -eq 0 ]; then
        echo " "
        echo "PNG generated successfully: $output_png_file"
    else
        echo "Error generating PNG."
    fi
else
    echo "Error: DOT file not generated."
fi
