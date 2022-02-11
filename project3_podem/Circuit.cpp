
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <list>
#include <regex>
#include "Circuit.h"
#include "Node.h"

/*
* Function to read netlist and create circuit
* @param netlistFile => location of the netlist
* @return bool -> if successfully read
*/
bool Circuit::read_netlist(std::string netlistFile)
{
    std::ifstream FH(netlistFile);
    if (!FH.is_open())
    {
        std::cerr << "Unable to open the netlist file " << netlistFile << "\n";
        return false;
    }
    std::string currentLine;
    while (std::getline(FH, currentLine))
    {
        remove_newline(currentLine);
        remove_tabs(currentLine);

        // remove any continous space in line
        remove_multiple_spaces(currentLine);

        // vector format:
        // <gate_type> <inputs> <outputs>
        std::vector<std::string> strParts = split_str(currentLine, ' ');
        std::string firstPart = str_to_lower(strParts[0]);

        // if line is empty then continue
        if (check_netlist_str(firstPart) == false)
        {
            continue;
        }

        int outputIndex;

        // if the input is logic gate
        if (firstPart.compare("input") != 0 && 
            firstPart.compare("output") != 0)
        {
            // Index to dynamically decide the read order to handle 1 and 2 input gates
            int currentIndex = 0;
            // init the gate
            Gate* currentGate = new Gate();
            this->add_gate(currentGate);

            // setting the logic of the gate
            currentGate->set_gate_logic(strParts[currentIndex]);
            currentIndex++;

            // init input nodes
            int input1Name = std::stoi(strParts[currentIndex]);
            Node* input1Node = this->create_node(input1Name);
            currentGate->input1 = input1Node;
            this->add_node_gate_map(input1Node->name, currentGate);
            currentIndex++;

            if (currentGate->is_single_input() == false)
            {
                // if not single input gate => add second input
                int input2Name = str_to_int(strParts[currentIndex]);
                Node* input2Node = this->create_node(input2Name);
                currentGate->input2 = input2Node;
                this->add_node_gate_map(input2Node->name, currentGate);
                currentIndex++;
            }

            // init the output
            int outputName = str_to_int(strParts[currentIndex]);
            Node* outputNode = this->create_node(outputName);
            currentGate->output = outputNode;
            // Add to output node to gate map
            this->outputnode_to_gate_map[outputName].push_back(currentGate);
            outputIndex = currentIndex;
            currentIndex++;
        }
        // if input or output type
        else if (firstPart.compare("input") == 0 ||
            firstPart.compare("output") == 0)
        {
            for (unsigned int i = 1; i < strParts.size(); ++i)
            {
                int nodeName = str_to_int(strParts[i]);
                // stop condition is seeing -1
                if (nodeName != -1)
                {
                    if (firstPart.compare("input") == 0)
                    {
                        this->add_input(nodeName);
                    }
                    else
                    {
                        this->add_output(nodeName);
                    }
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            std::cerr << "Unknown character in line -> " << currentLine << "\n";
        }
    }

    // Init all node values to X
    for (auto node : node_map)
    {
        node.second->value = X;
    }
    return true;
}

/*
* Function to create nodes
* @param nodeName -> name of node in netlist
* @return Node* -> pointer to node
*/
Node* Circuit::create_node(int nodeName)
{
    // Check if node already created
    if (this->node_map.count(nodeName) > 0)
    {
        // then return the existing node
        return this->node_map[nodeName];
    }

    // Create the node
    Node* newNode = new Node();
    newNode->name = nodeName;
    // Add to the node map
    node_map[nodeName] = newNode;
    node_list.push_back(nodeName);
    return newNode;
}

/*
* Function to add entry to input list
* @param nodeName -> name of input node
*/
void Circuit::add_input(int nodeName)
{
    this->inputNodes.push_back(nodeName);
}

/*
* Function to add the entry to output list
* @param nodeName -> name of output node
*/
void Circuit::add_output(int nodeName)
{
    this->outputNodes.push_back(nodeName);
}

/*
* Function to add gate to list
* @param inGate -> pointer to gate object
*/
void Circuit::add_gate(Gate* inGate)
{
    this->gateList.push_back(inGate);
}

/*
* Function to get list of input nodes
* @return int -> Number of input nodes
*/
int Circuit::get_num_inputs()
{
    return this->inputNodes.size();
}

/*
* Function to get list of output nodes
* @return int -> Number of output nodes
*/
int Circuit::get_num_outputs()
{
    return this->outputNodes.size();
}

/*
* Function to get node object from name
* @param nodeName -> name of node in netlist
* @return Node* -> pointer to node
*/
Node* Circuit::get_node_from_name(int nodeName)
{
    return this->node_map[nodeName];
}

/*
* Function to add the gate input node and gate to
* the node to gate map
* @param inNode -> node name
* @param inGate -> pointer to gate objects
*/
void Circuit::add_node_gate_map(int inNode, Gate* inGate)
{
    this->node_to_gate_map[inNode].push_back(inGate);
}

/*
* Backtrace function for PODEM
* @param nodeName -> node name to start the back trace from
* @param nodeValue -> value to obtain on the node
* @return pair of primary input and logic value required found
*/
std::pair<int, logicValue> Circuit::backtracePODEM(int nodeName, logicValue nodeValue)
{
    logicValue inputLogicValue = nodeValue;
    while (this->is_gate_output(nodeName))
    {
        logicValue newLogicValue = inputLogicValue;
        bool didBackPropagate = false;
        for (auto gateToCheck : this->outputnode_to_gate_map[nodeName])
        {
            logicValue inversionParity = gateToCheck->get_inversion_parity();
            if ((newLogicValue == One && inversionParity == One) ||
                (newLogicValue == Zero && inversionParity == Zero))
            {
            newLogicValue = Zero;
            }
            else
            {
            newLogicValue = One;
            }
            Node* inputToCheck = NULL;
            if (gateToCheck->input1->value == X)
            {
                nodeName = gateToCheck->input1->name;
                inputLogicValue = newLogicValue;
                didBackPropagate = true;
                break;
            }
            else
            {
                nodeName = gateToCheck->input2->name;
                inputLogicValue = newLogicValue;
                didBackPropagate = true;
                break;
            }
        }
        if (didBackPropagate == false)
        {
            std::cerr << "Dev Error: Unable to back propagate. What to do?\n";
        }
    }
    return std::make_pair(nodeName, inputLogicValue);
}

/*
* Objective function for PODEM
* @return pair of node name and logic value for PODEM algorithm
*/
std::pair<int, logicValue> Circuit::objectivePODEM(std::vector<int> dfrontier)
{
    if (get_node_from_name(this->faultNodeName)->value == X)
    {
        if (get_node_from_name(this->faultNodeName)->is_stuck_at_1)
        {
            return std::make_pair(this->faultNodeName, Zero);
        }
        else
        {
            return std::make_pair(this->faultNodeName, One);
        }
    }
    if (dfrontier.size() == 0)
    {
        return std::make_pair(-1, X);
    }
    Gate* dfrontierGate = this->outputnode_to_gate_map[dfrontier.front()].front();
    logicValue nonControlling = get_not_value(dfrontierGate->get_controlling_value());
    if (dfrontierGate->input1->value == X)
    {
        return std::make_pair(dfrontierGate->input1->name, nonControlling);
    }
    else if (dfrontierGate->input2->value == X)
    {
        return std::make_pair(dfrontierGate->input2->name, nonControlling);
    }
    std::cerr << "Dev Error: Unable to get objective, What to do?\n";
}

void add_to_change_tracker(Circuit *ckt, int nodeName,
    std::vector<std::pair<int, logicValue>>& changeTracker)
{
    bool changeInTracker = false;
    for (auto change : changeTracker)
    {
        if (change.first == nodeName)
        {
            changeInTracker = true;
        }
    }
    if (changeInTracker == false)
    {
        changeTracker.push_back(std::make_pair(nodeName, ckt->node_map[nodeName]->value));
    }
}

/*
* Function to perform imply
* @param nodeName -> node name to start forward implication from
* @param nodeValue -> node value to start the forward implication from
* @param changeTracker -> vector to track the changes made in this imply
* @return bool -> if forward implication possible with conflicts
*/
bool Circuit::implyPODEM(int nodeName, logicValue nodeValue,
    std::vector<std::pair<int, logicValue>> &changeTracker)
{
    add_to_change_tracker(this, nodeName, changeTracker);
    for (auto gate : node_to_gate_map[nodeName])
    {
        // Update changes tracker to be able to revert on failed recursion
        node_map[nodeName]->value = nodeValue;
        if (node_map[nodeName]->set_fault_value() == false)
        {
            return false;
        }
        logicValue oldOutputValue = gate->output->value;
        add_to_change_tracker(this, gate->output->name, changeTracker);
        if (gate->simulate() == false)
        {
            return false;
        }
        if (gate->output->value == oldOutputValue)
        {
            continue;
        }
        // imply further
        if (implyPODEM(gate->output->name, gate->output->value, changeTracker) == false)
        {
            return false;
        }
    }
    return true;
}

/*
* Function to update the dfrontier
* @return vector containing d frontier gates
*/
std::vector<int> Circuit::update_dfrontier()
{
    // Recreate the d-frontier
    std::vector<int> localDFrontier;
    for (auto nodeToGate : node_to_gate_map)
    {
        for (auto gate : nodeToGate.second)
        {
            if (gate->output->value == X)
            {
                if (gate->input1->value == D || gate->input1->value == DBar)
                {
                    // Add gate to dfrontier
                    localDFrontier.push_back(gate->output->name);
                    continue;
                }
                if (gate->is_single_input() == false)
                {
                    if (gate->input2->value == D || gate->input2->value == DBar)
                    {
                        // Add gate to dfrontier
                        localDFrontier.push_back(gate->output->name);
                        continue;
                    }
                }

            }
        }
    }

    // Make the dfrontier entries unique
    std::sort(localDFrontier.begin(), localDFrontier.end());
    localDFrontier.erase(unique(localDFrontier.begin(), localDFrontier.end()), localDFrontier.end());
    return localDFrontier;
}

/*
* Function to revert changes from previous imply
* @param ckt -> circuit object
* @param changesTracker -> vector of nodes changed
*/
void revert_changes(Circuit* ckt, std::vector<std::pair<int, logicValue>> changesTracker)
{
    for (auto change : changesTracker)
    {
        //std::cout << change.first << " " << print_logic_value(ckt->get_node_from_name(change.first)->value) << " " << print_logic_value(change.second) << "\n";
        ckt->get_node_from_name(change.first)->value = change.second;
    }
}

/*
* Function to run PODEM on circuit
* @return if podem run successfully
*/
bool Circuit::runPODEM()
{
    // Check if fault propagated to any of the output nodes
    for (auto nodeName : this->outputNodes)
    {
        if (get_node_from_name(nodeName)->value == D ||
            get_node_from_name(nodeName)->value == DBar)
        {
            return true;
        }
    }

    // Check and update the dfrontier
    std::vector<int> dfrontier = this->update_dfrontier();

    // Check for failure condition -> Target fault cnnot be activated
    if (this->node_map[this->faultNodeName]->value != X &&
        ((this->node_map[this->faultNodeName]->is_stuck_at_0 &&
            this->node_map[this->faultNodeName]->value == Zero) ||
            (this->node_map[this->faultNodeName]->is_stuck_at_1 &&
                this->node_map[this->faultNodeName]->value == One)))
    {
        return false;
    }
    
    // Find the objective to be achieved in this recursion of PODEM run
    std::pair<int, logicValue> objective = this->objectivePODEM(dfrontier);

    // Check failure in object function due to empty d-frontier
    if (objective.first == -1)
    {
        return false;
    }

    // If objective is setting value at primary input => only 2 things possible
    // 1. Fault is at primary input => it is OK if the Dfrontier is empty during initial steps
    // 2. Error condition that the dfrontier is empty
    // Adding check for point 2
    if (is_primary_input(objective.first) == true &&
        objective.first != this->faultNodeName &&
        dfrontier.size() == 0)
    {
        // no test can be found
        return false;
    }

    // Back trace towards a primary input to find the value to be set to achieve objective
    std::pair<int, logicValue> backtrace = this->backtracePODEM(objective.first, objective.second);
    std::cout << "Objective is, Node " << objective.first << " value " << print_logic_value(objective.second) << "\n";
    std::cout << "Backtrace is, Node " << backtrace.first << " value " << print_logic_value(backtrace.second) << "\n\n";
    // Imply the primary input value achieved from backtrace to update node values
    // which can be updated through the path

    // Create change tracker so as to reset the implied value if PODEM recusion fails
    std::vector<std::pair<int, logicValue>> changeTracker;
   // Recursively created more objectives till test generated
    if (this->implyPODEM(backtrace.first, backtrace.second, changeTracker) == true)
    {
        if (this->runPODEM() == true)
        {
            return true;
        }
    }

    // If PODEM run failed, then try
    // Revert changes (important as same circuit object re-used)
    revert_changes(this, changeTracker);
    changeTracker.clear();
    if (this->implyPODEM(backtrace.first, get_not_value(backtrace.second), changeTracker) == true)
    {
        if (this->runPODEM() == true)
        {
            return true;
        }
    }

    // If the reversed decision also failed, revert to unknown value
    // Revert changes (important as same circuit object re-used)
    revert_changes(this, changeTracker);
    changeTracker.clear();
    this->implyPODEM(backtrace.first, X, changeTracker);

    return false;
}

/*
* Function to print node values
*/
void Circuit::print_node_values()
{
    this->node_list.sort();
    for (auto m : this->node_list)
    {
        std::cout << m << " " << print_logic_value(this->node_map[m]->value) << "\n";
    }
}

/*
* Function to print output vector linear
* @return string of output vector
*/
std::string Circuit::get_output_vector_lin()
{
    std::string outputVector = "";
    for (auto nodeName : this->outputNodes)
    {
        outputVector += std::to_string(this->node_map[nodeName]->value);
    }
    return outputVector;
}

/*
* Function to print output vector per node
*/
void Circuit::print_output_vector_node()
{
    for (int nodeName : this->outputNodes)
    {
        std::cout << this->node_map[nodeName]->name << " " << print_logic_value(this->node_map[nodeName]->value) << "\n";
    }
    std::cout << "\n";
}

/*
* Function to print input vector per node
*/
void Circuit::print_input_vector_node()
{
    bool testVectorGenerated = false;
    // Check if any actual test vector generated
    for (int nodeName : this->inputNodes)
    {
        if (this->node_map[nodeName]->value != X)
        {
            testVectorGenerated = true;
            break;
        }
    }
    if (testVectorGenerated == false)
    {
        std::cout << "No test vector generated. Fault is undetectable\n";
        return;
    }
    // Before printing, resolve the D and DBar values set in the primary inputs to the required value
    for (int nodeName : this->inputNodes)
    {
        if (this->node_map[nodeName]->value == One ||
            this->node_map[nodeName]->value == Zero ||
            this->node_map[nodeName]->value == X)
        {
            std::cout << print_logic_value(this->node_map[nodeName]->value);
        }
        else if (this->node_map[nodeName]->value == D)
        {
            // as the value of D = 1/0 where 1 is the applied value to sensitize
            std::cout << print_logic_value(One);
        }
        else if (this->node_map[nodeName]->value == DBar)
        {
            // as the value of DBar = 0/1 where 0 is the applied value to sensitize
            std::cout << print_logic_value(Zero);
        }
    }
    std::cout << "\n";
}

/*
* Function to print the circuit
* @return string representing the modified netlist
*/
std::string Circuit::print_circuit()
{
    std::string outString = "";
    // Print the gate connections
    std::list<Gate*>::iterator itr;
    for (itr = this->gateList.begin(); itr != this->gateList.end(); ++itr)
    {
        Gate* currGate = (*itr);
        outString += currGate->get_gate_logic_str() + " ";
        outString += std::to_string(currGate->input1->name) + " ";
        if (currGate->is_single_input() == false)
        {
            outString += std::to_string(currGate->input2->name) + " ";
        }
        outString += std::to_string(currGate->output->name) + "\n";
        continue;
    }
    return outString;
}

/*
* Function to check if node is primary input
* @param nodeName -> node to check
* @return bool -> if node is primary input
*/
bool Circuit::is_primary_input(int nodeName)
{
    return std::find(this->inputNodes.begin(), this->inputNodes.end(), nodeName) != this->inputNodes.end();
}

/*
* Function to check if node is primary output
* @param nodeName -> node to check
* @return bool -> if node is primary output
*/
bool Circuit::is_primary_output(int nodeName)
{
    return std::find(this->outputNodes.begin(), this->outputNodes.end(), nodeName) != this->outputNodes.end();
}

/*
* Function to check if node is gate output
* @param nodeName -> node to check
* @return bool -> if node is gate output
*/
bool Circuit::is_gate_output(int nodeName)
{
    for (auto node : this->outputnode_to_gate_map)
    {
        if (nodeName == node.first)
        {
            return true;
        }
    }
    return false;
}

/*
* Destructor for the class
*/
Circuit::~Circuit()
{
    // Delete all gates
    while (this->gateList.size() > 0)
    {
        Gate* currentGate = this->gateList.back();
        this->gateList.pop_back();
        delete currentGate;
    }
    // Delete all nodes
    for (auto m : this->node_map)
    {
        delete m.second;
    }
}
