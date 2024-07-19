#!/bin/bash

# Directories for each flag
directories=("testingp1/input/all_hashes/" "testingp1/input/file_check/" "testingp1/input/min_hashes/" "testingp1/input/chunk_check/")
# Flags to pass to package_main
flags=("-all_hashes" "-file_check" "-min_hashes" "-chunk_check")

# Initialize counters for passed and total tests
declare -A passed_tests
declare -A total_tests
overall_passed=0
overall_total=0

# Loop over all directories and flags
for ((i=0; i<${#directories[@]}; i++)); do
    directory="${directories[$i]}"
    flag="${flags[$i]}"
    passed_tests[$flag]=0
    total_tests[$flag]=0

    # Loop over all .bpkg files in the specified directory
    for infile in "$directory"*.bpkg; do
        # Corresponding .out file
        outfile="${infile%.bpkg}.out"

        # Execute package_main with input and flag, capture the output
        ./pkgmain "$infile" $flag > temp_output.txt

        # Ensure temp_output.txt ends with a newline
        sed -i -e '$a\' temp_output.txt

        # Ensure outfile ends with a newline
        sed -i -e '$a\' "$outfile"

        # Increment the total tests counter
        ((total_tests[$flag]++))
        ((overall_total++))

        # Compare the output to the expected output file
        if cmp -s temp_output.txt "$outfile"; then
            echo "Test $(basename "$infile") with flag $flag PASSED"
            ((passed_tests[$flag]++))
            ((overall_passed++))
        else
            echo "Test $(basename "$infile") with flag $flag FAILED"
        fi
    done
done

hashes_of_files=("testingp1/input/hashes_of/*.bpkg")
hash="21cddec45c2c26c8dd4f992f8f7315d590c9a12ddc135c053fbde887b65590c1"
flag="-hashes_of"
passed_tests[$flag]=0
total_tests[$flag]=0

for infile in $hashes_of_files; do
    outfile="${infile%.bpkg}.out"

    # Execute package_main with input, flag, and hash, capture the output
    ./pkgmain "$infile" $flag "$hash" > temp_output.txt

    # Ensure temp_output.txt ends with a newline
    sed -i -e '$a\' temp_output.txt

    # Ensure outfile ends with a newline
    sed -i -e '$a\' "$outfile"

    # Increment the total tests counter
    ((total_tests[$flag]++))
    ((overall_total++))

    # Compare the output to the expected output file
    if cmp -s temp_output.txt "$outfile"; then
        echo "Test $(basename "$infile") with flag $flag and hash $hash PASSED"
        ((passed_tests[$flag]++))
        ((overall_passed++))
    else
        echo "Test $(basename "$infile") with flag $flag and hash $hash FAILED"
    fi
done

# Clean up temporary output file
rm temp_output.txt

# Print summary of passed and total tests for each flag
for flag in "${flags[@]}" "-hashes_of"; do
    echo "Passed ${passed_tests[$flag]} / ${total_tests[$flag]} tests with flag $flag"
done

# Print overall summary of passed and total tests
echo "Overall: Passed $overall_passed / $overall_total tests"