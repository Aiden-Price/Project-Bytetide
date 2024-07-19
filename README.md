===============================================================================
 Author      : 530502821
 Assignment  : Project-ByteTide
===============================================================================

Describe the organisation of the software
================================================================================
This organization separates the project into logical directories, each serving a specific purpose. The include directory contains header files for various modules, the resources directory contains package-related and example files, the src directory contains the main source code organized by functionality, and the Tests directory includes configuration and scripts for testing the project.

Bytide Package Loader / Merkle Tree & Configuration, Networking and Program
Directories and Files:

    include/
        chk/ Header files related to checking operations.
            pkgchk.h: Header file for package checking.
        crypt/ Header files for cryptographic operations.
            sha256.h: Header file for SHA-256 cryptographic operations.
        net/ Header files related to networking.
            packet.h: Header file for packet handling.
        tree/ Header files for data structures and tree operations.
            merkletree.h: Header file for Merkle tree implementation.

    resoruces/ 
        pkgs/ contains package-related files
            This includes the .bpkg and .data files to act as input files for the program, the script or executable for package creation (pkgmake), the package validation script (pktval) or executable and script or executable for SHA-256 checksum validation (sha256chk)
    src/ 
        chk/
            pkgchk.c: Source code for package checking.
        crypt/ Contains cryptographic source files.
            sha256.c: Source code for SHA-256 cryptographic operations.
        tree/ Contains source files related to data structure and tree operations.
            merkletree.c: Source code for Merkle tree implementation.
        
        btide.c: Source code for btide functionality.
        config.c: Configuration handling source code.
        config.h: Header file for configuration. 
        package.c: Package handling source code.
        package.h: Header file for package file.
        peer.c: Source code for peer-to-peer operations.
        pkgmain.c: Main package source code.


.gitignore: Git ignore file to exclude specific files from version control.
config.cfg: Configuration file for tests.
Makefile: Makefile for building the project.
p1test.sh: Shell script for test phase 1.
p2test.sh: Shell script for test phase 2.
part1sel.txt: Selection file for part 1 of tests.
pkgchecker: Executable or script for checking packages.
README.md: Documentation file for the project.
    
Where test data is located 
================================================================================
The test data for the project are organized within the testingp<task_num> directory. Each subdirectory under tests represents a specific test case flag, containing the necessary input files, expected output files, and scripts to run the tests. Each test is formally describe in the test_report.md contained in both testing directories. All testcases are automated and provide a final score to maximise efficiency.
Bytide Package Loader / Merkle Tree
    Phase 1 Tests (p1tests)
    Description: (Tests are taken from A3 Tests -> Explained test scaenarios in test_report.md in p1tests directory)
        The Phase 1 tests focus on unit-testing your Merkle tree implementation. These tests use the pkgchk tool to help determine what aspects of the Merkle tree to test, along with some basic functionality tests. Ensure binary file is located in the 
        main tree area (same place as the testp1 directory). 
    Location: 
        Within the main tree in folder -> testingp1 directory 
    Contents:
        .bpkg: A test package file used to test.
        .out: The expected output file for the test.

Merkle Tree & Configuration, Networking and Program
    Phase 2 Tests (p2tests)
    Description: (Tests explained in test_report.md in p2tests directory)
    Focuses on IO testing the btide client. The p2tests target in the Makefile runs the p2test.sh script, which compiles the client, constructs input/output files, and compares the actual results against expected outputs, documenting the testing methods used.
    Location: 
        Within the main tree in folder -> testingp2 directory 
    Contents:
        add_package/
            .bpkg: A test package file used to test.
            .out: The expected output file for the test.

How they are run
================================================================================
Each test (p1, p2) are ran using an automated bash script to loop through the files  and compare the difference to a .out file, a temp.txt is created to store the actual output however this is also automatically cleaned up to ensure a clean repository

Bytide Package Loader / Merkle Tree
1 . Ensure everything is compiled
2 . Within the main tree, simply use the make file command:
            make p1tests

Merkle Tree & Configuration, Networking and Program
1 . Ensure everything is compiled
2 . Within the main tree, simply use the make file command:
            make p2tests

Ensuring No Git Pollution
================================================================================
The following .data files are valid for testing purposes:
    - config_parser
    - btide
    - pkgchecker
    - pkgmain

They will not be filtered out by the .gitignore as they need to be created to ensure the 
file_check functionality is successfully working as requrired