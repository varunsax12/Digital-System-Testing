/*
Description:
    Project #1 for Digital System Testing course
    Topic: Logic simulator
    Inputs:
        1. Circuit netlist supporting logic gates
        2. Test vector to be applied on the circuit
        3. (Optional) Flag to activate fanout branching
    Outputs:
        1. Output values at the mentioned output nodes
        2. (Optional) Mapping of the fanout branch resolution
        3. (Optional) New netlist dump with the resolved fanout branches
*/

#include <iostream>
#include <fstream>
#include "Node.h"
#include "Gate.h"
#include "Circuit.h"
#include "HelperFuncs.h"

int main(int argc, char* argv[])
{
    if (argc != 3 && argc != 4)
    {
        // Check 1: Expected input args = 2 or 3 (+ the executable)
        // Print error if more or less inputs provided
        std::cerr << "Invalid inputs. Expected inputs: <exe> <path_to_netlist> <input_vector> <flag_to_fanout>\n";
        std::cerr << "Usage:\n\t<exe>\texecutable to run\n\t<path_to_netlist> path to netlist to parse";
        std::cerr << "\n\t<input_vector>\tinput vector to run the circuit on";
        std::cerr << "\n\t(Optional): <flag_to_fanout> 0 or 1 to deactivate and activate fanout branching\n";
        return 1;
    }

    std::string netlistPath(argv[1]);
    std::string testVector(argv[2]);
    bool blDoFanout = false;
    if (argc == 4)
    {
        std::string doFanout(argv[3]);
        if (doFanout.compare("1") == 0)
        {
            blDoFanout = true;
        }
    }
    

    Circuit testCkt;
    if (testCkt.read_netlist(netlistPath, blDoFanout) == false)
    {
        // unable to read netlist
        return 1;
    }
    if (testCkt.apply_test_vector(testVector, blDoFanout) == false)
    {
        // unable to apply test vectors
        return 1;
    }
    std::cout << "Output Vector is " << testCkt.get_output_vector_lin() << "\n";

    if (blDoFanout)
    {
        std::cout << "Modified circuit netlist fanout nodes: ";
        std::string outfile = "modifiedNetlist.txt";
        std::ofstream OFH;
        OFH.open(outfile, std::ios::trunc);
        std::string netlistString = testCkt.print_circuit();
        if (!OFH.is_open())
        {
            std::cerr << "Unable to open the file for write: " << outfile << "\n";
            std::cerr << "Writing on console out:" << "\n";
            std::cout << netlistString;
        }
        else
        {
            std::cout << outfile << "\n" << std::flush;
            //OFH << testCkt.print_circuit();
            OFH.write(netlistString.c_str(), netlistString.length());
            OFH.close();
        }
    }

    return 0;
}
