
#include "Gate.h"

/*
* Setter for the logic of the gate
* @param inLogic -> string of the logic
*/
void Gate::set_gate_logic(std::string inLogic)
{
    std::string inLogicLower = str_to_lower(inLogic);
    if (inLogicLower.compare("and") == 0)
    {
        this->logic = and_l;
    }
    else if (inLogicLower.compare("or") == 0)
    {
        this->logic = or_l;
    }
    else if (inLogicLower.compare("inv") == 0)
    {
        this->logic = not_l;
    }
    else if (inLogicLower.compare("nand") == 0)
    {
        this->logic = nand_l;
    }
    else if (inLogicLower.compare("nor") == 0)
    {
        this->logic = nor_l;
    }
    else if (inLogicLower.compare("xnor") == 0)
    {
        this->logic = xnor_l;
    }
    else if (inLogicLower.compare("xor") == 0)
    {
        this->logic = xor_l;
    }
    else if (inLogicLower.compare("buf") == 0)
    {
        this->logic = buf_l;
    }
    else
    {
        // Add check if unknown value passed
        std::cerr << "Unkown logic value passed: " << inLogic << "\n";
    }
}

/*
* Getter for the logic value
* @return gateLogic -> gate logic from enum type
*/
gateLogic Gate::get_gate_logic()
{
    return this->logic;
}

/*
* Function to simulate the gate
* @return bool -> simulate gates taking into account all input types 0, 1, D, DBar, X
*/
bool Gate::simulate()
{
    // NOTE: Bitwise not was found to work different => switching to boolean not
    logicValue input1Value = this->input1->value;
    logicValue input2Value = X;
    if (this->is_single_input() == false)
    {
        input2Value = this->input2->value;
    };
    switch (this->get_gate_logic())
    {
    case(and_l):
        if (input1Value == Zero || input2Value == Zero)
        {
            this->output->value = Zero;
        }
        else if (input1Value == One)
        {
            this->output->value = input2Value;
        }
        else if (input2Value == One)
        {
            this->output->value = input1Value;
        }
        else if (input1Value == D && input2Value == D)
        {
            this->output->value = D;
        }
        else if (input1Value == DBar && input2Value == DBar)
        {
            this->output->value = DBar;
        }
        else if (input1Value == D && input2Value == DBar)
        {
            this->output->value = Zero;
        }
        else if (input1Value == DBar && input2Value == D)
        {
            this->output->value = Zero;
        }
        break;
    case(or_l):
        if (input1Value == One || input2Value == One)
        {
            this->output->value = One;
        }
        else if (input1Value == Zero)
        {
            this->output->value = input2Value;
        }
        else if (input2Value == Zero)
        {
            this->output->value = input1Value;
        }
        else if (input1Value == D && input2Value == D)
        {
            this->output->value = D;
        }
        else if (input1Value == DBar && input2Value == DBar)
        {
            this->output->value = DBar;
        }
        else if (input1Value == D && input2Value == DBar)
        {
            this->output->value = One;
        }
        else if (input1Value == DBar && input2Value == D)
        {
            this->output->value = One;
        }
        break;
    case(not_l):
        this->output->value = get_not_value(input1Value);
        break;
    case(nand_l):
        if (input1Value == Zero || input2Value == Zero)
        {
            this->output->value = One;
        }
        else if (input1Value == One)
        {
            this->output->value = get_not_value(input2Value);
        }
        else if (input2Value == One)
        {
            this->output->value = get_not_value(input1Value);
        }
        else if (input1Value == D && input2Value == D)
        {
            this->output->value = DBar;
        }
        else if (input1Value == DBar && input2Value == DBar)
        {
            this->output->value = D;
        }
        else if (input1Value == D && input2Value == DBar)
        {
            this->output->value = One;
        }
        else if (input1Value == DBar && input2Value == D)
        {
            this->output->value = One;
        }
        break;
    case(nor_l):
        if (input1Value == One || input2Value == One)
        {
            this->output->value = Zero;
        }
        else if (input1Value == Zero)
        {
            this->output->value = get_not_value(input2Value);
        }
        else if (input2Value == Zero)
        {
            this->output->value = get_not_value(input1Value);
        }
        else if (input1Value == D && input2Value == D)
        {
            this->output->value = DBar;
        }
        else if (input1Value == DBar && input2Value == DBar)
        {
            this->output->value = D;
        }
        else if (input1Value == D && input2Value == DBar)
        {
            this->output->value = Zero;
        }
        else if (input1Value == DBar && input2Value == D)
        {
            this->output->value = Zero;
        }
        break;
    case(xor_l):
        std::cerr << "Dev Error: XOR gate not supported\n";
        break;
    case(xnor_l):
        std::cerr << "Dev Error: XNOR gate not supported\n";
        break;
    case(buf_l):
        this->output->value = input1Value;
        break;
    default:
        std::cerr << "Gate at uninitialized logic." << "\n";
        break;
    }

    // Update output value based on whether a fault exists on the node
    return this->output->set_fault_value();
}

/*
* Check if single input gate (inverter or buffer)
* @return bool -> if the gate is single input gate or not
*/
bool Gate::is_single_input()
{
    if (this->get_gate_logic() == not_l ||
        this->get_gate_logic() == buf_l)
    {
        return true;
    }
    return false;
}

/*
* Function to return string of gate type
* @return string -> string form of gate logic
*/
std::string Gate::get_gate_logic_str()
{
    switch (this->get_gate_logic())
    {
    case(and_l):
        return "AND";
    case(or_l):
        return "OR";
    case(not_l):
        return "INV";
    case(nand_l):
        return "NAND";
    case(nor_l):
        return "NOR";
    case(xor_l):
        return "XOR";
    case(xnor_l):
        return "XNOR";
    case(buf_l):
        return "BUF";
    default:
        break;
    }
    return "";
}

/*
* Function to get controlling value of gate
* @return logicValue -> controlling value
*/
logicValue Gate::get_controlling_value()
{
    switch (this->get_gate_logic())
    {
    case(and_l):
        return Zero;
    case(or_l):
        return One;
    case(not_l):
        std::cerr << "Unknown logic value to NOT return\n";
        return X;
    case(nand_l):
        return Zero;
    case(nor_l):
        return One;
    case(xor_l):
        std::cerr << "Unknown logic value to XOR return\n";
        return X;
    case(xnor_l):
        std::cerr << "Unknown logic value to XNOR return\n";
        return X;
    case(buf_l):
        std::cerr << "Unknown logic value to BUF return\n";
        return X;
    default:
        break;
    }
    return X;
}

/*
* Function to get inversion parity
* @return logicValue -> inversion parity
*/
logicValue Gate::get_inversion_parity()
{
    switch (this->get_gate_logic())
    {
    case(and_l):
        return Zero;
    case(or_l):
        return Zero;
    case(not_l):
        return One;
    case(nand_l):
        return One;
    case(nor_l):
        return One;
    case(xor_l):
        std::cerr << "Unknown logic value to return\n";
        return X;
    case(xnor_l):
        std::cerr << "Unknown logic value to return\n";
        return X;
    case(buf_l):
        return Zero;
    default:
        break;
    }
    return X;
}
