
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <list>
#include <regex>
#include "Circuit.h"

/*
* Constructor for the class
*/
Circuit::Circuit() : nextNodeName(1) {}

/*
* Function to read netlist and create circuit
* @param netlistFile => location of the netlist
* @param splitFanout => if fanouts needs to be split
* @return bool -> if successfully read
*/
bool Circuit::read_netlist(std::string netlistFile, bool splitFanout)
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
            currentGate->set_input1(input1Node);
            this->add_node_gate_map(input1Node->get_name(), currentGate);
            currentIndex++;

            if (currentGate->is_single_input() == false)
            {
                // if not single input gate => add second input
                int input2Name = str_to_int(strParts[currentIndex]);
                Node* input2Node = this->create_node(input2Name);
                this->add_node_gate_map(input2Node->get_name(), currentGate);
                currentGate->set_input2(input2Node);
                currentIndex++;
            }

            // init the output
            int outputName = str_to_int(strParts[currentIndex]);
            Node* outputNode = this->create_node(outputName);
            currentGate->set_output(outputNode);
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
    if (splitFanout)
    {
        // Expand all fanout nodes
        for (int nodeName : this->node_list)
        {
            if (this->has_fanout(nodeName))
            {
                this->split_fanout_node(nodeName);
            }
        }
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

    // Incremement the next node name tracker
    this->nextNodeName++;

    // Create the node
    Node* newNode = new Node(nodeName);
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
* Function to apply the test vector on the circuit
* @param testVector -> test vector applied
*/
bool Circuit::apply_test_vector(std::string testVector, bool splitFanout)
{
    if (testVector.length() != this->get_num_inputs())
    {
        std::cerr << "Invalid test vectors as size not same as number of inputs" << "\n";
        return false;
    }

    // set to track the value of nodes set to values
    // using list to allow randomization during activation of nodes and gates
    std::list<int> readyNodes;

    // apply the values on the input nodes
    std::list<Node*>::iterator itr;
    int stringIndex = 0;
    for (unsigned int i = 0; i < inputNodes.size(); ++i)
    {
        this->node_map[inputNodes[i]]->update_value(testVector[i] == '1');
        readyNodes.push_back(inputNodes[i]);
    }
    // check the list of gates activated
    while (readyNodes.size() > 0)
    {
        int nodeName = readyNodes.front();
        readyNodes.pop_front();
        std::list<Gate*> connectedGates = this->node_to_gate_map[nodeName];

        // check if node is a fanout branch
        if (this->has_fanout(nodeName))
        {
            // Get values
            int nodeValue = this->node_map[nodeName]->get_value();
            // propagate the value to all fanouts
            for (auto fanoutNodeName : node_to_fanout_map[nodeName])
            {
                // update the fanout branch value
                this->node_map[fanoutNodeName]->update_value(nodeValue);
                // add it to the readyNodes list as the value at fanouts is now
                // ready
                readyNodes.push_back(fanoutNodeName);
            }
        }
        
        bool used = true; // assuming it will activate all connected gates

        //for (unsigned int i = 0; i < connectedGates.size(); ++i)
        std::list<Gate*>::iterator itr;
        for(itr = connectedGates.begin(); itr!=connectedGates.end(); ++itr)
        {
            Gate* currentGate = (*itr);
            // attempt to simulate it
            currentGate->simulate_when_active();
            if (currentGate->is_simulated())
            {
                readyNodes.push_back(currentGate->get_output()->get_name());
            }
            else
            {
                used = false; // as it was unable to activate a given gate we need to keep it
            }
        }
        // if the node still needed based on used flag
        if (!used)
        {
            // add only if not already in the list
            if (std::find(readyNodes.begin(), readyNodes.end(), nodeName) == readyNodes.end())
            {
                readyNodes.push_back(nodeName);
            }
        }
    }
    return true;
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
* Function to print node values
*/
void Circuit::print_node_values()
{
    for (auto m : this->node_map)
    {
        std::cout << m.first << " " << m.second->get_value() << "\n";
    }
}

/*
* Function to print output vector linear
*/
std::string Circuit::get_output_vector_lin()
{
    std::string outputVector = "";
    for (auto nodeName : this->outputNodes)
    {
        outputVector += std::to_string(this->node_map[nodeName]->get_value());
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
        std::cout << this->node_map[nodeName]->get_name() << " " << this->node_map[nodeName]->get_value() << "\n";
    }
}

/*
* Function to print the circuit
* @return string representing the modified netlist
*/
std::string Circuit::print_circuit()
{
    std::string outString = "";
    // Print the fanout node connections
    outString += "* Fanout nodes below if any\n";
    for (auto nodeToFanout : this->node_to_fanout_map)
    {
        outString += "CONNECT ";
        outString += std::to_string(nodeToFanout.first) + " ";
        for (auto fanoutName : nodeToFanout.second)
        {
            outString += std::to_string(fanoutName) + " ";
        }
        outString += "\n";
    }
    outString += "\n* Circuit description post fanout branching if any\n";
    // Print the gate connections
    std::list<Gate*>::iterator itr;
    for (itr = this->gateList.begin(); itr != this->gateList.end(); ++itr)
    {
        Gate* currGate = (*itr);
        outString += currGate->get_gate_logic_str() + " ";
        outString += std::to_string(currGate->get_input1()->get_name()) + " ";
        if (currGate->is_single_input() == false)
        {
            outString += std::to_string(currGate->get_input2()->get_name()) + " ";
        }
        outString += std::to_string(currGate->get_output()->get_name()) + "\n";
    }
    return outString;
}

/*
* Function to check if node it has fanout nodes
* @param nodeName -> name of the node to check
* @return bool -> whether node has fanout
*/
bool Circuit::has_fanout(int nodeName)
{
    // Steps:
    // 1. Get value using number of connected gates when fanout not split
    // 2. Get value from the entry in node_to_fanout_map when fanout split
    return (this->node_to_gate_map[nodeName].size() > 1) ||  (this->node_to_fanout_map.count(nodeName) > 0);
}

/*
* Split the fanout nodes
* @oaram nodeName -> name of the node to check
*/
void Circuit::split_fanout_node(int nodeName)
{
    // iterator over the number of fanout => number of gates the node is
    // connected to
    while (this->node_to_gate_map[nodeName].size() > 0)
    {
        // use the next node name tracker to create unique node names
        int newNodeName = this->nextNodeName;

        // Create node for the new node
        // This will also add it to the node_map
        Node* newNodeObj = this->create_node(newNodeName);

        // add the node as a fanout of the stem
        node_to_fanout_map[nodeName].push_back(newNodeName);

        // change the input of the gate
        Gate* currentGate = this->node_to_gate_map[nodeName].back();
        // remove the gate now as it will get a new connection
        this->node_to_gate_map[nodeName].pop_back();
        // check which input the stem node was connected to for the gate
        if (nodeName == currentGate->get_input1()->get_name())
        {
            // update the input 1
            currentGate->set_input1(newNodeObj);
        }
        else if (currentGate->is_single_input() == false &&
            nodeName == currentGate->get_input2()->get_name())
        {
            // update the input 2
            currentGate->set_input2(newNodeObj);
        }

        // Add gate and new node to node_to_gate_map
        this->add_node_gate_map(newNodeName, currentGate);
    }
}

/*
* Function to print fanout data
*/
void Circuit::print_fanout_data()
{
    for (auto nodePair : this->node_to_fanout_map)
    {
        std::cout << nodePair.first << " -> ";
        for (auto fanoutName : nodePair.second)
        {
            std::cout << fanoutName << " ";
        }
        std::cout << "\n";
    }
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
