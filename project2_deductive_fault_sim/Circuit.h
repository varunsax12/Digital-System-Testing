#ifndef __CIRCUIT_H__
#define __CIRCUIT_H__

#include <vector>
#include <unordered_map>
#include <string>
#include <list>
#include <set>

#include "Node.h"
#include "Gate.h"

/*
* Class for the complete circuit
*/
class Circuit
{
    // Vector to hold the list of node name for iteration
    std::list<int> node_list;

    // Map to hold the node name to node object map
    std::unordered_map<int, Node*> node_map;

    // Map to hold the gate input node to gate mapping
    std::unordered_map<int, std::list<Gate*>> node_to_gate_map;

    // Vector of gates
    std::list<Gate*> gateList;

    // Vector of input nodes
    std::vector<int> inputNodes;

    // Vector of output nodes
    std::vector<int> outputNodes;

public:

    /*
    * Function to read netlist and create circuit
    * @param netlistFile => location of the netlist
    * @param activateFaultsOnAllNodes => activate the s-a-0 and s-a-1 faults on all nodes
    * @return bool -> if successfully read
    */
    bool read_netlist(std::string netlistFile, bool activateFaultsOnAllNodes);

    /*
    * Function to create nodes
    * @param nodeName -> name of node in netlist
    * @param activateFaultsOnAllNodes => activate the s-a-0 and s-a-1 faults on all nodes
    * @return Node* -> pointer to node
    */
    Node* create_node(int nodeName, bool activateFaultsOnAllNodes);

    /*
    * Function to add entry to input list
    * @param nodeName -> name of input node
    */
    void add_input(int nodeName);

    /*
    * Function to add the entry to output list
    * @param nodeName -> name of output node
    */
    void add_output(int nodeName);

    /*
    * Function to add gate to list
    * @param inGate -> pointer to gate object
    */
    void add_gate(Gate* inGate);

    /*
    * Function to get list of input nodes
    * @return int -> Number of input nodes
    */
    int get_num_inputs();

    /*
    * Function to get list of output nodes
    * @return int -> Number of output nodes
    */
    int get_num_outputs();

    /*
    * Function to get node object from name
    * @param nodeName -> name of node in netlist
    * @return Node* -> pointer to node
    */
    Node* get_node_from_name(int nodeName);

    /*
    * Function to apply the test vector on the circuit
    * @param testVector -> test vector applied
    * @return bool -> if test vectors applied successfully
    */
    bool apply_test_vector(std::string testVector);

    /*
    * Function to add the gate input node and gate to 
    * the node to gate map
    * @param inNode -> node name
    * @param inGate -> pointer to gate objects
    */
    void add_node_gate_map(int inNode, Gate* inGate);

    /*
    * Function to print all node values
    */
    void print_node_values();

    /*
    * Function to print output vector linear
    */
    std::string get_output_vector_lin();

    /*
    * Function to print output vector per node
    * @return string -> output vector
    */
    void print_output_vector_node();

    /*
    * Function to print the circuit
    */
    std::string print_circuit();

    /*
    * Function to print the faults detected at all circuits outputs
    */
    void print_detectable_faults();

    /*
    * Function to print the faults detected to a file
    * @param ofstream -> output file stream
    */
    void print_detectable_fault_to_file(std::ofstream& outFileStream);

    /*
    * Function to add custom fault to circuit
    * @param nodeName -> name of node to apply fault
    * @param faultType -> type of fault 0 or 1
    */
    void apply_fault(int nodeName, int faultType);

    /*
    * Function to get total faults in circuit
    * @return int -> total faults in circuit
    */
    int get_total_faults_count();

    /*
    * Function to get faults detected at output
    * @return int -> number of detectable faults
    */
    int get_detectable_faults_count();

    /*
    * Function to get all detected faults at output
    * @return std::set<std::pair<int, int>> -> pair of node-name and stuck at value
    */
    std::set<std::pair<int, int>> get_list_detected_faults();

    /*
    * Function to get the converage value
    */
    float get_fault_coverage_value();

    /*
    * Function to get the converage value
    * @return float -> fault coverage percentage
    */
    void print_fault_converage();

    /*
    * Destructor for the class
    */
    ~Circuit();

};

#endif