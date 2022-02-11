#ifndef __CIRCUIT_H__
#define __CIRCUIT_H__

#include <vector>
#include <unordered_map>
#include <string>
#include <list>

#include "Node.h"
#include "Gate.h"

/*
* Class for the complete circuit
*/
class Circuit
{
public:
    // Vector to hold the list of node name for iteration
    std::list<int> node_list;

    // Map to hold the node name to node object map
    std::unordered_map<int, Node*> node_map;

    // Map to hold the gate input node to gate mapping
    std::unordered_map<int, std::list<Gate*>> node_to_gate_map;

    // Map to hold the gate output node to gate mapping
    std::unordered_map<int, std::list<Gate*>> outputnode_to_gate_map;

    // Vector of gates
    std::list<Gate*> gateList;

    // Vector of input nodes
    std::vector<int> inputNodes;

    // Vector of output nodes
    std::vector<int> outputNodes;

    // Single stuck at fault to generate test for
    int faultNodeName;

    /*
    * Function to read netlist and create circuit
    * @param netlistFile => location of the netlist
    * @return bool -> if successfully read
    */
    bool read_netlist(std::string netlistFile);

    /*
    * Function to create nodes
    * @param nodeName -> name of node in netlist
    * @return Node* -> pointer to node
    */
    Node* create_node(int nodeName);

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
    * Function to add the gate input node and gate to 
    * the node to gate map
    * @param inNode -> node name
    * @param inGate -> pointer to gate objects
    */
    void add_node_gate_map(int inNode, Gate* inGate);

    /*
    * Backtrace function for PODEM
    * @param nodeName -> node name to start the back trace from
    * @param nodeValue -> value to obtain on the node
    * @return pair of primary input and logic value required found
    */
    std::pair<int, logicValue> backtracePODEM(int nodeName, logicValue nodeValue);

    /*
    * Objective function for PODEM
    * @return pair of node name and logic value for PODEM algorithm
    */
    std::pair<int, logicValue> objectivePODEM(std::vector<int> dfrontier);

    /*
    * Function to perform imply
    * @param nodeName -> node name to start forward implication from
    * @param nodeValue -> node value to start the forward implication from
    * @return bool -> if forward implication possible with conflicts
    */
    bool implyPODEM(int nodeName, logicValue nodeValue,
        std::vector<std::pair<int, logicValue>>& changeTracker);

    /*
    * Function to update the dfrontier
    * @return vector containing d frontier gates
    */
    std::vector<int> update_dfrontier();

    /*
    * Function to run PODEM on circuit
    * @return if podem run successfully
    */
    bool runPODEM();

    /*
    * Function to print all node values
    */
    void print_node_values();

    /*
    * Function to print output vector linear
    * @return string of output vector
    */
    std::string get_output_vector_lin();

    /*
    * Function to print output vector per node
    */
    void print_output_vector_node();

    /*
    * Function to print input vector per node
    */
    void print_input_vector_node();

    /*
    * Function to print the circuit
    * @return string representing the modified netlist
    */
    std::string print_circuit();

    /*
    * Function to check if node is primary input
    * @param nodeName -> node to check
    * @return bool -> if node is primary input
    */
    bool is_primary_input(int nodeName);

    /*
    * Function to check if node is primary output
    * @param nodeName -> node to check
    * @return bool -> if node is primary output
    */
    bool is_primary_output(int nodeName);

    /*
    * Function to check if node is gate output
    * @param nodeName -> node to check
    * @return bool -> if node is gate output
    */
    bool is_gate_output(int nodeName);

    /*
    * Destructor for the class
    */
    ~Circuit();

};

#endif