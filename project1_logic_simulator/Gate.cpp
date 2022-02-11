
#include "Gate.h"

/*
* Constructor to initializing the object
*/
Gate::Gate() : input1(NULL), input2(NULL), output(NULL), logic(none_l), simulationDone(false) {}

/*
* Setter for input1 to link the node
* @param inInput1 -> pointer to the input1 node
*/
void Gate::set_input1(Node* inInput1)
{
    this->input1 = inInput1;
}

/*
* Setter for input1 to link the node
* @param inInput2 -> pointer to the input2 node
*/
void Gate::set_input2(Node* inInput2)
{
    // Check for not gate
    if (this->is_single_input())
    {
        std::cerr << "Access to input2 restricted for single input gate" << "\n";
        return;
    }
    this->input2 = inInput2;
}

/*
* Setter for input1 to link the node
* @param inOutput -> pointer to the output node
*/
void Gate::set_output(Node* inOutput)
{
    this->output = inOutput;
}

/*
* Getter for input1
*/
Node* Gate::get_input1()
{
    return this->input1;
}

/*
* Getter for input2
*/
Node* Gate::get_input2()
{
    // Check for not gate
    if (this->is_single_input())
    {
        std::cerr << "Access to input2 restricted for single_input gate\n";
        return NULL;
    }
    return this->input2;
}

/*
* Getter for output
*/
Node* Gate::get_output()
{
    return this->output;
}

/*
* Setter of the input1 value
* @param inValue -> value of input1 node
*/
void Gate::set_input1_value(int inValue)
{
    this->input1->update_value(inValue);
    // Check if gate is valid, if so simulate it
    this->simulate_when_active();
}

/*
* Setter of the input2 value
* @param inValue -> value of input2 node
*/
void Gate::set_input2_value(int inValue)
{
    // Check for not gate
    if (this->is_single_input())
    {
        std::cerr << "Access to input2 restricted for single input gate\n";
        return;
    }
    this->input2->update_value(inValue);
    this->simulate_when_active();
}

/*
* Setter of the output value
* @param inValue -> value of output node
*/
void Gate::set_output_value(int inValue)
{
    this->output->update_value(inValue);
}

/*
* Getter for the input1 value
*/
int Gate::get_input1_value()
{
    return this->input1->get_value();
}

/*
* Getter for the input2 value
*/
int Gate::get_input2_value()
{
    // Check for not gate
    if (this->is_single_input())
    {
        std::cerr << "Access to input2 restricted for single input gate\n";
        return -1;
    }
    return this->input2->get_value();
}

/*
* Getter for the output value
*/
int Gate::get_output_value()
{
    return this->output->get_value();
}

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
*/
gateLogic Gate::get_gate_logic()
{
    return this->logic;
}

/*
* Function to simulate the gate
*/
void Gate::simulate()
{
    // NOTE: Bitwise not was found to work different => switching to boolean not
    int input1Value = this->get_input1_value();
    int input2Value = -1;
    if (this->is_single_input() == false)
    {
        input2Value = this->get_input2_value();
    }
    Node* outputNode = this->get_output();
    switch (this->get_gate_logic())
    {
    case(and_l):
        outputNode->update_value(input1Value & input2Value);
        break;
    case(or_l):
        outputNode->update_value(input1Value | input2Value);
        break;
    case(not_l):
        outputNode->update_value(!input1Value);
        break;
    case(nand_l):
        outputNode->update_value(!(input1Value & input2Value));
        break;
    case(nor_l):
        outputNode->update_value(!(input1Value | input2Value));
        break;
    case(xor_l):
        outputNode->update_value(input1Value ^ input2Value);
        break;
    case(xnor_l):
        outputNode->update_value(!(input1Value ^ input2Value));
        break;
    case(buf_l):
        outputNode->update_value(input1Value);
        break;
    default:
        std::cerr << "Gate at uninitialized logic." << "\n";
        break;
    }
    this->set_simulation_done();
}

/*
* Function to set simulationDone
*/
void Gate::set_simulation_done()
{
    this->simulationDone = true;
}

/*
* Function to check whether the gate already simulated
*/
bool Gate::is_simulated()
{
    return this->simulationDone;
}

/*
* Check if the gate activated
* Activated => both inputs are valid
*/
void Gate::simulate_when_active()
{
    if (this->get_input1()->is_valid() && 
        (this->is_single_input() || this->get_input2()->is_valid()))
    {
        // If both inputs valid => simulate
        this->simulate();
    }
}

/*
* Check if single input gate (inverter or buffer)
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
* Destructor for the class
*/
Gate::~Gate() {}
