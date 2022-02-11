/*
Description:
    Project #2 for Digital System Testing course
    Topic: Deductive fault simulator
    Input arguments:
    a.	-a: To run all possible tests from the netlist directory (s27.txt, s298f_2.txt, s344f_2.txt, s349f_2.txt)
    b.	-f <path to netlist file>: Path for single netlist to run (cannot be used with -a)
    c.	-d <path to netlist directory>: Path for all 4 netlists (must be used with -a)
    d.	-l: To run the circuit with all faults activated (cannot be used with -a)
    e.	-p <path to fault list>: Path to input fault list (cannot be used with -a and -l)
    f.	-t <test vector to apply>: Test vector to apply (cannot be used -a)
    Outputs:
    a.	For all netlists run for coverage stats:
        i.	“test_data.csv” in the run directory contains test vectors run and the fault coverage percentage for each circuit.
        ii.	The same data is output in the standard output.
    b.	For run on a single netlist:
        i.	Output vector is printed.
        ii.	List of detectable faults at the output are printed.
        iii.Fault coverage statistics of the run are printed

*/

#include <iostream>
#include <fstream>
#include <random>
#include <set>
#include <vector>
#include "Node.h"
#include "Gate.h"
#include "Circuit.h"
#include "HelperFuncs.h"


/*
* Function to check if command line option provided
*/
bool checkCommandLineOption(int argc, char** argv, std::string inputToCheck)
{
    return std::find(argv, argv + argc, inputToCheck) != (argv + argc);
}

/*
* Function to get command line option from identifier
*/
std::string getCommandLineOption(int argc, char** argv, std::string inputToCheck)
{
    char** itr = std::find(argv, argv + argc, inputToCheck);
    if (itr != argv + argc &&
        // check if the actual input is also not empty
        itr + 1 != argv + argc)
    {
        // return a string form of the iterator
        ++itr;
        std::string returnStr = *itr;
        return returnStr;
    }
    return "";
}

/*
* Function to print stats for all known netlist files in csv for plotting
* NOTE: This applies all faults to circuit
*/
void run_all(std::string parentPath)
{
    std::string outputFileName = "test_data.csv";
    std::ofstream ofOutFile;
    ofOutFile.open(outputFileName, std::ios::trunc);
    std::string netlistNames[] = {"/s27.txt", "/s298f_2.txt", "/s344f_2.txt", "/s349f_2.txt"};
    float inputSize[] = {7 , 17, 24, 24};
    for (int i = 0; i < 4; ++i)
    {
        std::string netlistPath = parentPath + netlistNames[i];
        std::cout << "Running on netlist: " << netlistPath << "\n";
        ofOutFile << "Running on netlist: " << netlistPath << "\n";
        std::cout << "Test vectors run \tFault Coverage %\n";
        ofOutFile << "Test vectors run \tFault Coverage %\n";
        bool activateFaultsOnAllNodes = true;

        int inputRangeMax = pow(2, inputSize[i]) - 1;

        // init the random number generator
        std::default_random_engine generator;
        std::uniform_int_distribution<int> distribution(0, inputRangeMax);

        // set of faults found so far in this check
        std::set<std::pair<int, int>> detectableFaultsSoFar;

        // vector to track if test run already
        std::vector<int> testsRunSoFar;

        // Total faults in circuit
        int totalCircuitFaults = 0;

        // j denotes the run number
        for (int j = 0; j <= 100; ++j)
        {
            int testNumber = -1;
            do
            {
                testNumber = distribution(generator);
            } while (std::find(testsRunSoFar.begin(), testsRunSoFar.end(), testNumber) != testsRunSoFar.end());

            // insert number in tests run so far to track in next test run
            testsRunSoFar.push_back(testNumber);

            // generate the test vector to apply
            std::string testVector = "";
            for (int index = 0; index < inputSize[i]; ++index)
            {
                if ((testNumber & 1) == 0)
                {
                    testVector = "0" + testVector;
                }
                else
                {
                    testVector = "1" + testVector;
                }
                testNumber = testNumber >> 1;
            }
            Circuit testCkt;
            if (testCkt.read_netlist(netlistPath, activateFaultsOnAllNodes) == false)
            {
                // unable to read netlist
                std::cout << "Developer Error #1";
                return;
            }

            if (testCkt.apply_test_vector(testVector) == false)
            {
                // unable to read netlist
                std::cout << "Developer Error #2";
                return;
            }

            std::set<std::pair<int, int>> faultsDetectedInCurretIter = testCkt.get_list_detected_faults();

            totalCircuitFaults = testCkt.get_total_faults_count();

            // Copy these faults in the main set, so as to skip the overlapping faults
            detectableFaultsSoFar.insert(faultsDetectedInCurretIter.begin(), faultsDetectedInCurretIter.end());

            // Print the coverage of current number of tests
            //std::cout << "Tests run: " << j << "\nCoverage: " << detectableFaultsSoFar.size() * 100.0 / totalCircuitFaults << "\n\n";
            std::cout << j << "\t" << detectableFaultsSoFar.size() * 100.0 / totalCircuitFaults << "\n";
            ofOutFile << j << "\t" << detectableFaultsSoFar.size() * 100.0 / totalCircuitFaults << "\n";
        }
        std::cout << "\n\n";
        ofOutFile << "\n\n";
    }
    std::cout << "Check file: " << outputFileName << " for result output.\n";
    ofOutFile.close();
}

/*
* Function to print script usage
*/
void printUsage()
{
    std::cerr << "Usage:\n";
    std::cerr << "\t-a\t->To run all possible tests from the netlist directory (s27.txt, s298f_2.txt, s344f_2.txt, s349f_2.txt)\n";
    std::cerr << "\t-f <path to netlist file> ->Path for single netlist to run (cannot be used with -a)\n";
    std::cerr << "\t-d <path to netlist directory> ->Path for all 4 netlists (must be used with -a)\n";
    std::cerr << "\t-l\t->To run the circuit with all faults activated (cannot be used with -a)\n";
    std::cerr << "\t-p <path to fault list> ->Path to input fault list (cannot be used with -a and -l)\n";
    std::cerr << "\t-t <test vector to apply> ->Test vector to apply (cannot be used -a)\n";
    std::cerr << "Example Usage:\n";
    std::cerr << "\t To run all tests for coverage stats: <exe> -a -d <path_to_netlist_dir>\n";
    std::cerr << "\t To run single netlist with all faults: <exe> -f <path_to_netlist> -t <test_vector> -l\n";
    std::cerr << "\t To run single netlist with custom faults: <exe> -f <path_to_netlist> -t <test_vector> -p <path_to_fault_list>\n";
}

int main(int argc, char* argv[])
{
    // if run all selected for coverage analysis
    if (checkCommandLineOption(argc, argv, "-a"))
    {
        // then it cannot take file specific inputs
        if (checkCommandLineOption(argc, argv, "-f") ||
            checkCommandLineOption(argc, argv, "-l") ||
            checkCommandLineOption(argc, argv, "-p") ||
            checkCommandLineOption(argc, argv, "-t"))
        {
            printUsage();
            return -1;
        }

        // check if parent directory provided
        if (checkCommandLineOption(argc, argv, "-d") == false)
        {
            printUsage();
            return -1;
        }

        // if all inputs are correct
        run_all(getCommandLineOption(argc, argv, "-d"));
    }
    else
    {
        // => run on specific netlist
        if (checkCommandLineOption(argc, argv, "-f") == false ||
            checkCommandLineOption(argc, argv, "-t") == false)
        {
            // no input netlist path provided or input vector
            printUsage();
            return -1;
        }
        std::string netlistPath = getCommandLineOption(argc, argv, "-f");
        std::string testVector = getCommandLineOption(argc, argv, "-t");

        bool activateFaultsOnAllNodes = false;
        std::string faultListPath = "";

        // check if all faults to be applied
        if (checkCommandLineOption(argc, argv, "-l"))
        {
            // it must not have a fault list file path then
            if (checkCommandLineOption(argc, argv, "-p"))
            {
                printUsage();
                return -1;
            }
            activateFaultsOnAllNodes = true;
        }
        else
        {
            // => do not apply all faults => must provide fault list path
            if (checkCommandLineOption(argc, argv, "-p") == false)
            {
                printUsage();
                return -1;
            }
            faultListPath = getCommandLineOption(argc, argv, "-p");
        }

        // Create circuit
        Circuit testCkt;
        if (testCkt.read_netlist(netlistPath, activateFaultsOnAllNodes) == false)
        {
            // unable to read netlist
            std::cerr << "Unable to read netlist. Check netlist. Exiting\n";
            return -1;
        }

        // Apply fault list if custom fault list provided
        if (activateFaultsOnAllNodes == false)
        {
            std::ifstream infile(faultListPath);
            int nodeName, faultType;
            while (infile >> nodeName >> faultType)
            {
                testCkt.apply_fault(nodeName, faultType);
            }
        }

        // Apply the test vector
        if (testCkt.apply_test_vector(testVector) == false)
        {
            // unable to apply test vectors
            std::cerr << "Unable to apply test vector. Checking inputs. Exiting\n";
            return -1;
        }
        std::cout << "Output Vector is " << testCkt.get_output_vector_lin() << "\n";
        std::cout << "Faults detected:\n";
        testCkt.print_detectable_faults();
        std::cout << "\n";
        std::cout << "Coverage Statistics:\n";
        testCkt.print_fault_converage();
    }
    return 0;
}
