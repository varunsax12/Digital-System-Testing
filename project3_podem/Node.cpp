#include <iostream>
#include "Node.h"

/*
* Function to check if the node has a fault and update
* to D or DBar according to current node value
* @return bool -> if setting fault value leads to conflict or not
*/
bool Node::set_fault_value()
{
    // Check if the output change here leads to a D or DBar
    if (this->is_stuck_at_0 && this->value == One)
    {
        this->value = D;
    }
    else if (this->is_stuck_at_1 && this->value == Zero)
    {
        this->value = DBar;
    }
    else
    {
        // check if unable to sensitize the node
        if ((this->is_stuck_at_0 && this->value == Zero) ||
            (this->is_stuck_at_1 && this->value == One))
        {
            return false;
        }
    }
    return true;
}

/*
* Function to inver the logic value on node and return inverter
* @param inValue -> logic value to inver
* @return logicValue -> inverted value
*/
logicValue get_not_value(logicValue inValue)
{
    if (inValue == One)
    {
        return Zero;
    }
    else if (inValue == Zero)
    {
        return One;
    }
    else if (inValue == D)
    {
        return DBar;
    }
    else if (inValue == DBar)
    {
        return D;
    }
    else
    {
        return X;
    }
}

/*
* Function to print logic value
* @param inValue -> value to check
* @return string -> string for of logic value
*/
std::string print_logic_value(logicValue inValue)
{
    switch (inValue)
    {
    case X:
        return "X";
    case D:
        return "D";
    case DBar:
        return "DBar";
    case One:
        return "1";
    case Zero:
        return "0";
    default:
        break;
    }
    return "";
}
