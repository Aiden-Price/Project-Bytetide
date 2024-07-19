The Function Descriptions are explained in p1tests/test_report.md
This just provides a general overview of tests
This purpose is even if the program isnt complete to full required 
functionality the tests can still be ran to show the intentions

# Test btide.c + config.c + package.c (e2e_package)
Analysisng CLI scenarios of ADDPACKAGE, REMPACKAGE and PACKAGES
The commands use the predefined bpkg files from A3 tests to analyse 
the range of possible scenarios including a mixed range of positive, 
negative and edge cases. Each of these are further explained in 
p1tests/test_report.md

# Test btide.c (e2e_serverAndClient)
The mix of positive, negative and edge test cases are purposefully linked
to testing the signal handling, connection management, message handling, 
thread mangement and configuration parsing in the server and client 
communication. The tests are testing concurrency and usability/CLI 
interactions

# Test (fetch)
The fetch testcases test requesting chunks of data based on a hash, possibly 
with an offset, from a specified peer in a networked environment, ensuring
argument validation, netork conditions, protocol compliance.

This section of the testcases was not fully completed but test designs and
functionalities are present supported by stylish comments to highlight 
intentions