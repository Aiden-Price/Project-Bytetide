#!/bin/bash

# Path to the directory containing test input and expected output files
input_directory="testingp2/input/add_package/"
output_directory="testingp2/input/add_package/"
positive_input_file="testingp2/input/e2e_serverAndclient/positive.in" 

# Initialize counters for passed and total tests
passed=0
total=0

# Loop over all .bpkg files in the specified directory
for infile in "$input_directory"*.bpkg; do
    # Determine the expected output file corresponding to the input file
    basefile=$(basename "$infile")
    outfile="${output_directory}${basefile%.bpkg}.out"

    # Run the btide command with the ADDPACKAGE option
    ./btide ADDPACKAGE "$infile" > temp_output.txt

    # Ensure the output file ends with a newline
    sed -i -e '$a\' temp_output.txt

    # Ensure the expected output file also ends with a newline
    sed -i -e '$a\' "$outfile"

    # Increment the total tests counter
    ((total++))

    # Compare the output to the expected output file
    if cmp -s temp_output.txt "$outfile"; then
        echo "Test ${basefile} with ADDPACKAGE PASSED"
        ((passed++))
    else
        echo "Test ${basefile} with ADDPACKAGE FAILED"
    fi

    # Use positive.in as commands in stdin and compare output
    if [ -f "$positive_input_file" ]; then
        # Run the program with positive.in as stdin
        ./btide config.cfg < "$positive_input_file" > temp_output.txt

        # Compare this output to the expected output file
        if cmp -s temp_output.txt "$outfile"; then
            echo "Test ${basefile} with positive.in PASSED"
            ((passed++))
        else
            echo "Test ${basefile} with positive.in FAILED"
        fi

        # Increment the total tests counter again
        ((total++))
    fi
done

# Clean up temporary output files
rm temp_output.txt 

# Print summary of passed and total tests
echo "Overall: Passed $passed / $total tests"