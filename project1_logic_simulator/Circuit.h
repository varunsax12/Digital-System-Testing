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
    // Vector to hold the list of node name for iteration
    std::list<int> node_list;

    // Map to hold the node name to node object map
    std::unordered_map<int, Node*> node_map;

    // Map to hold the gate input node to gate mapping
    std::unordered_map<int, std::list<Gate*>> node_to_gate_map;

    // Vector of gates
    std::list<Gate*> gateList;

    // Map to hold the node name to fan out nodes
    std::unordered_map<int, std::list<int>> node_to_fanout_map;

    // Vector of input nodes
    std::vector<int> inputNodes;

    // Vector of output nodes
    std::vector<int> outputNodes;

    // Track total number of nodes to name new nodes dynamically
    // without conflicting
    int nextNodeName;

public:

    /*
    * Constructor for the class
    */
    Circuit();

    /*
    * Function to read netlist and create circuit
    * @param netlistFile => location of the netlist
    * @param splitFanout => if fanouts needs to be split
    * @return bool -> if successfully read
    */
    bool read_netlist(std::string netlistFile, bool splitFanout);

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
    * Function to apply the test vector on the circuit
    * @param testVector -> test vector applied
    * @param splitFanout -> if fanouts are split
    * @return bool -> if test vectors applied successfully
    */
    bool apply_test_vector(std::string testVector, bool splitFanout);

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
    * Function to check if node it has fanout nodes
    * @param nodeName -> name of the node to check
    * @return bool -> whether node has fanout
    */
    bool has_fanout(int nodeName);

    /*
    * Split the fanout nodes
    * @oaram nodeName -> name of the node to check
    */
    void split_fanout_node(int nodeName);

    /*
    * Function to print fanout data
    */
    void print_fanout_data();

    /*
    * Destructor for the class
    */
    ~Circuit();

};

#endif