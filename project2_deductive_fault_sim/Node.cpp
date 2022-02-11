
#include <iostream>
#include "Node.h"
#include "HelperFuncs.h"

/*
* Constructor for the class
* @param inName -> Name of the node
*/
Node::Node(int inName) : isStuckAtOne(false), isStuckAtZero(false)
{
    this->name = inName;
    this->value = -1;
}

/*
* Function to get the node deductive fault list
* @return vector of pairs -> deductive faults on node
*/
std::vector<std::pair<int, int>> Node::get_node_deductive_fault_list()
{
    return this->listDeductiveFaults;
}

/*
* Function to set the node deductive fault list
* @param vector of string -> deductive faults on node
*/
void Node::set_node_deductive_fault_list(std::vector<std::pair<int, int>> inDeductiveFaults)
{
    // Merge the new list with the current one
    merge_vectors(this->listDeductiveFaults, inDeductiveFaults);
}

/*
* Setter for the node name
* @param inName -> Name of the node
*/
void Node::update_name(int inName)
{
    this->name = inName;
}

/*
* Setter for the node value
* @param inValue -> Value of the node
*/
void Node::update_value(int inValue)
{
    if (this->value == -1 || this->value == inValue)
    {
        this->value = inValue;
    }
    else
    {
        std::cerr << "Bug in code. Overwriting node value on " << this->name << "\n";
    }
}

/*
* Getter for the node name
*/
int Node::get_name()
{
    return this->name;
}

/*
* Getter for the node value
*/
int Node::get_value()
{
    return this->value;
}

/*
* Check if the value of the node is valid 0 or 1
* It is invalid if value is -1
* Return bool for validitiy
*/
bool Node::is_valid()
{
    return this->get_value() != -1;
}

/*
* Setter for stuck at fault-1
*/
void Node::set_stuck_at_1()
{
    this->isStuckAtOne = true;
}

/*
* Setter for stuck at fault-0
*/
void Node::set_stuck_at_0()
{
    this->isStuckAtZero = true;
}

/*
* Getter for stuck at fault-1
* @return bool -> if stuck at 1
*/
bool Node::get_is_stuck_at_1()
{
    return this->isStuckAtOne;
}

/*
* Getter for stuck at fault-0
* @return bool -> if stuck at 0
*/
bool Node::get_is_stuck_at_0()
{
    return this->isStuckAtZero;
}

/*
* Function to check if there is any sensitized fault on node based on value
* @return bool -> if there is a valid fault
*/
bool Node::is_fault_on_node()
{
    // check value and possible fault on node
    if (this->get_value() == 1 && this->get_is_stuck_at_0())
    {
        // only stuck at 0 sensitized
        return true;
    }
    else if (this->get_value() == 0 && this->get_is_stuck_at_1())
    {
        // value is 0
        // only stuck at 1 sensitized
        return true;
    }
    return false;
}

/*
* Function to get the fault on node based on current value
* 
* Fault pair => first = node name, second = fault type
* 
* @return string -> of the fault sensitized on node
*/
std::pair<int, int> Node::get_fault_on_node()
{
    // check value on node
    if (this->get_value() == 1 && this->get_is_stuck_at_0())
    {
        // only stuck at 0 sensitized
        return std::make_pair(this->get_name(), 0);
    }
    else if (this->get_value() == 0 && this->get_is_stuck_at_1())
    {
        // value is 0
        // only stuck at 1 sensitized
        return std::make_pair(this->get_name(), 1);
    }
}

/*
* Destructor for the class
*/
Node::~Node() {}