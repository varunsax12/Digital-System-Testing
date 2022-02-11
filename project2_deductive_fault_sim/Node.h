#ifndef __NODE_H__
#define __NODE_H__

#include <vector>

/*
* Class to represent a node in the circuit
*/

class Node
{
    // To hold the name of the node as integer
    int name;
    // To hold the boolean value on this node
    // Default value on the nnode is -1
    int value;

    // To hold whether the node is stuck at 1 fault
    bool isStuckAtOne;
    // To hold whether the node is stuck at 0 fault
    bool isStuckAtZero;

    // To hold all the faults in the current node
    // Pair of entry => first = node name, second = fault type
    std::vector<std::pair<int, int>> listDeductiveFaults;

public:

    /*
    * Constructor for the class
    * @param inName -> Name of the node
    */
    Node(int inName);

    /*
    * Function to get the node deductive fault list
    * @return vector of string -> deductive faults on node
    */
    std::vector<std::pair<int, int>> get_node_deductive_fault_list();

    /*
    * Function to set the node deductive fault list
    * @param vector of string -> deductive faults on node
    */
    void set_node_deductive_fault_list(std::vector<std::pair<int, int>> inDeductiveFaults);

    /*
    * Setter for the node name
    * @param inName -> Name of the node
    */
    void update_name(int inName);

    /*
    * Setter for the node value
    * @param inValue -> Value of the node
    */
    void update_value(int inValue);

    /*
    * Getter for the node name
    */
    int get_name();

    /*
    * Getter for the node value
    */
    int get_value();

    /*
    * Check if the value of the node is valid 0 or 1
    * It is invalid if value is -1
    * Return bool for validitiy
    */
    bool is_valid();

    /*
    * Setter for stuck at fault-1
    */
    void set_stuck_at_1();

    /*
    * Setter for stuck at fault-0
    */
    void set_stuck_at_0();

    /*
    * Getter for stuck at fault-1
    * @return bool -> if stuck at 1
    */
    bool get_is_stuck_at_1();

    /*
    * Getter for stuck at fault-0
    * @return bool -> if stuck at 0
    */
    bool get_is_stuck_at_0();

    /*
    * Function to check if there is any sensitized fault on node based on value
    * @return bool -> if there is a valid fault
    */
    bool is_fault_on_node();

    /*
    * Function to get the fault on node based on current value
    * @return pair -> of the fault sensitized on node
    */
    std::pair<int, int> get_fault_on_node();

    /*
    * Destructor for the class
    */
    ~Node();

};

#endif