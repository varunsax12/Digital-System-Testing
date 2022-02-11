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
public:

    /*
    * Constructor for the class
    * @param inName -> Name of the node
    */
    Node(int inName);

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
    * Destructor for the class
    */
    ~Node();

};

#endif