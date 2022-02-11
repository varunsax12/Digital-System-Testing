/*
Description:
    Project #3 for Digital System Testing course
    Topic: PODEM test generator
    Inputs:
        1. Circuit netlist supporting logic gates
        2. Stuck at fault details (node name and stuck at value)
    Outputs:
        1. Test vector to detect the input fault
*/

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include "Node.h"
#include "Gate.h"
#include "Circuit.h"
#include "HelperFuncs.h"

/*
* Function to check if command line option provided
* @param argc -> number of command line args
* @param argv -> array of command line args chars
* @param inputToCheck -> input argument to find
* @return int -> index of command line argument
*/
int checkCommandLineOption(int argc, char** argv, const char inputToCheck[])
{
    for (int i = 1; i < argc; ++i)
    {
        // compare strings
        if (strlen(argv[i]) != strlen(inputToCheck))
        {
            continue;
        }
        bool isAllEqual = true;
        for (int j = 0; j < strlen(inputToCheck); ++j)
        {
            if (argv[i][j] != inputToCheck[j])
            {
                isAllEqual = false;
                break;
            }
        }
        if (isAllEqual == true)
        {
            return i;
        }
    }
    return -1;
}

/*
* Function to generate test vectors for all the known netlist files
*/
void simulateAll(std::string parentPath)
{
     std::string netlistName[] = { "/s27.txt", "/s298f_2.txt", "/s344f_2.txt", "/s349f_2.txt" };

    for (auto netlist : netlistName)
    {
        std::string netlistPath = parentPath + netlist;
        std::cout << "Simulating Circuit: " << netlistPath << "\n";
        std::vector<int> simulatedSoFar;

        bool switchFault = false;

        while (1)
        {
            // Create the circuit data structure
            Circuit testCkt;
            // Read the netlist and init the circuit DS values
            if (testCkt.read_netlist(netlistPath) == false)
            {
                // unable to read netlist
                return;
            }
            // Track which faults have already been simulated
            if (simulatedSoFar.size() == testCkt.node_list.size())
            {
                if (switchFault == false)
                {
                    switchFault = true;
                    simulatedSoFar.clear();
                }
                else
                {
                    break;
                }
            }
            testCkt.node_list.sort();

            // Simulate the PODEM algo to generate the test vector
            for (auto m : testCkt.node_list)
            {
                if (std::find(simulatedSoFar.begin(),
                    simulatedSoFar.end(), m) == simulatedSoFar.end())
                {
                    // Assign the fault node name
                    testCkt.faultNodeName = m;
                    // Assign the fault stuck at value to the node
                    if (switchFault == false)
                    {
                        testCkt.node_map[testCkt.faultNodeName]->is_stuck_at_0 = true;
                    }
                    else
                    {
                        testCkt.node_map[testCkt.faultNodeName]->is_stuck_at_1 = true;
                    }
                    // Run PODEM algo
                    bool podemSuccessful = testCkt.runPODEM();
                    std::cout << "Node name: " << m << ", Stuck at value: ";
                    if (testCkt.node_map[testCkt.faultNodeName]->is_stuck_at_0)
                    {
                        std::cout << "0 ";
                    }
                    else
                    {
                        std::cout << "1 ";
                    }
                    std::cout << "Test Vector: ";
                    if (podemSuccessful == false)
                    {
                        std::cout << "No test vector generated. Fault is undetectable\n";
                    }
                    else
                    {
                        // Print the test vector generated
                        testCkt.print_input_vector_node();
                    }
                    simulatedSoFar.push_back(m);
                    break;
                }
            }
        }
    }
}

/*
* Function to print error if invalid command line input given
*/
void printCmdError()
{
    std::cerr << "*ERROR* Invalid command line inputs.\n";
}

/*
* Function to print the usage details
*/
void printUsage()
{
    std::cerr << "Usage:\n";
    std::cerr << "\t-f <netlist_path>     ->Path to netlist file\n";
    std::cerr << "\t-d <netlist_dir_path> ->Path to directory with known netlists(s27.txt, s298f_2.txt, s344f_2.txt, s349f_2.txt)\n";
    std::cerr << "\t-n <fault_node_name>  ->Node name with fault\n";
    std::cerr << "\t-v <stuck_at_value>   ->Stuck at value for fault\n";
    std::cerr << "Example Usage:\n";
    std::cerr << "\t To generate tests for all faults in netlist directory: <exe> -d <netlist_dir_path>\n";
    std::cerr << "\t To generate test vector for SSF in netlist file: <exe> -f <netlist_path> -n <fault_node_name> -v <stuck_at_value>\n";
}


/*
* Main function
*/
int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        printUsage();
        return 1;
    }
    // Check command line inputs and populate the required variables
    if (checkCommandLineOption(argc, argv, "-h") != -1)
    {
        printUsage();
        return 1;
    }

    // Check for netlist option
    if (checkCommandLineOption(argc, argv, "-d") != -1 &&
        checkCommandLineOption(argc, argv, "-f") != -1)
    {
        printCmdError();
        printUsage();
        return 1;
    }

    // Check if netlist directory run initiated
    int index = checkCommandLineOption(argc, argv, "-d") + 1;
    if (index != 0)
    {
        std::string netlistDir = argv[index];
        simulateAll(netlistDir);
        return 1;
    }

    // Check if SSF run initiated on netlist file
    std::string netlistPath, nodeName, sfaultValue;
    index = checkCommandLineOption(argc, argv, "-f") + 1;
    if (index != 0)
    {
        netlistPath = argv[index];
    }
    index = checkCommandLineOption(argc, argv, "-n") + 1;
    if (index != 0)
    {
        nodeName = argv[index];
    }
    index = checkCommandLineOption(argc, argv, "-v") + 1;
    if (index != 0)
    {
        sfaultValue = argv[index];
    }

    // Init the circuit DS
    Circuit testCkt;
    // Read netlist and create complete circuit DS
    if (testCkt.read_netlist(netlistPath) == false)
    {
        // unable to read netlist
        return 1;
    }
    // Assign the fault node name
    testCkt.faultNodeName = std::stoi(nodeName);
    // Assign the stuck at value for the node
    int ifaultValue = std::stoi(sfaultValue);

    if (ifaultValue == 1)
    {
        testCkt.node_map[testCkt.faultNodeName]->is_stuck_at_1 = true;
    }
    else if (ifaultValue == 0)
    {
        testCkt.node_map[testCkt.faultNodeName]->is_stuck_at_0 = true;
    }
    else
    {
        // unknown fault value
        printCmdError();
        printUsage();
        return 1;
    }
    
    // Run PODEM algo
    bool podemSuccessful = testCkt.runPODEM();
    if (podemSuccessful == false)
    {
        std::cout << "No test vector generated. Fault is undetectable\n";
    }
    else
    {
        // Print the test vector generated
        testCkt.print_input_vector_node();
    }
    return 0;
}
