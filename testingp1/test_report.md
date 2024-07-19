# Test 1 Description - Valid scenario
Description: .bpkg file that contains correctly formatted data and
        valid hashes to verify the parser and merkle tree construction

Input: .bpkg file that adheres to the to the specifed format and contains 
        consistent hash values

Outcome: The program should successfully parse the file and construct the 
        merkle tree accurately and validate all hashes without errors

# .bpkg file format

# Test 2 Description - Incorrect hash
Description: .bpkg file where one or more chunk hashes are incorrect to 
        check error handling

Input: .bpkg file with atleast one hash value that does not match the 
        corresponding chunk

Outcome: The program detects the incorrect hash and reports a validation 
        error

# Test 3 Description - Mismatch number of chunks
Description: .bpkg file where the nchunks value does not allign with the 
        actual number of chunk enteries provided

Input: .bpkg file with a declared number of chunks that doesnt match the 
        count of chunk records within the file

Outcome: The program identifies the mismatch and raises an error in regards 
        to the chunk count inconsistency

# Test 4 Description - Excessively large identifier
Description: .bpkg file with an identifier string exceeding the maximum 
        allowed length to test the input validation

Input: .bpkg file where the ident field contains more than 1024 characters

Outcome: The program should recognise the oversized identifier and return an 
        error to indicate invalid input

# Test 5 Description - Empty .bpkg file
Description: Testing the programs response to an empty .bpkg file ensuring 
        it handles the error

Input: An empty .bpkg file with no data to parse or load

Outcome: The program should detect the empty file and report an error 
        indicating that no valid data could be loaded

# Merkle Tree

# Test 6 Description - Incorrect non-leaf hashes
Description: Testing the merkle tree construction with incorrect hash 
        values in non-leaf nodes

Input: .bpkg file with one or more non-leaf hashes incorrectly calculated 
        from their childrens hashes

Outcome: The program should identify the hash mismatches in non-leaf nodes 
        and report validation errors

# Test 7 Description - Leaf node data corruption
Description: Evaluates the programs ability to detect corrupted data within 
        leaf nodes based on hash mismatches

Input: .bpkg file with altered chunk data that does not match the precomputed 
        hash in the leaf node

Outcome: The program detects the corrupted data chunk by failing the hash 
        validation for the affected leaf node

# Test 8 Description - Unbalanced tree
Description: .bpkg file structure to create a merkle tree that is not 
        perfectly balanced, to test handling of irregular tree structures

Input: .bpkg file with chunks arranged to form an unbalanced merkle tree 
        possibly missing nodes at certain levels

Outcome: Handles the implementation of an unbalanced tree

# Test 9 Description - All leaves at some depth but incomplete parent level
Description: .bpkg file with all leaves at the same depth but an incomplete 
        parent level, to test the trees integrity

Input: .bpkg file that results in a tree where all leaves are correct but 
        the upper levels are not fully populated

Outcome: The program detects the incomplete parent level and raises an error

# Test 10 Description - Duplicate chunks with different hashes
Description: .bpkg file with duplicate data chunks that have different 
        hashes to test the handling of identical data at different positions

Input: .bpkg file containing identitcal chunk data assigned different 
        hashes due to their positions

Outcome: The program correctly differentiates the chunks based on hashes 
        and positions, handling duplicates appropriatley
