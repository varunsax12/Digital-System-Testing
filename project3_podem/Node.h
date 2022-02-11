#ifndef __NODE_H__
#define __NODE_H__

#include <vector>

/*
* Logic value representation for faults
*/
typedef enum logicValue
{
    Zero, // logic value zero
    One,  // logic value one
    D,    // value of 1/0
    DBar, // value of 0/1
    X,    // unkown logic value
} logicValue;

/*
* Class to represent a node in the circuit
*/
class Node
{
public:

    // To hold the name of the node as integer
    int name;
    // To hold the boolean value on this node
    logicValue value;
    // Stuck at flags
    bool is_stuck_at_0;
    bool is_stuck_at_1;

    /*
    * Function to check if the node has a fault and update
    * to D or DBar according to current node value
    * @return bool -> if setting fault value leads to conflict or not
    */
    bool set_fault_value();
};

/*
* Function to inver the logic value on node and return inverter
* @param inValue -> logic value to inver
* @return logicValue -> inverted value
*/
logicValue get_not_value(logicValue inValue);

/*
* Function to print logic value
* @param inValue -> value to check
* @return string -> string for of logic value
*/
std::string print_logic_value(logicValue inValue);

#endif