
#include <iostream>
#include "Node.h"

/*
* Constructor for the class
* @param inName -> Name of the node
*/
Node::Node(int inName)
{
    this->name = inName;
    this->value = -1;
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
* Destructor for the class
*/
Node::~Node() {}