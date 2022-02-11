
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <list>
#include <regex>
#include "Circuit.h"


/*
* Function to read netlist and create circuit
* @param netlistFile => location of the netlist
* @param activateFaultsOnAllNodes => activate the s-a-0 and s-a-1 faults on all nodes
* @return bool -> if successfully read
*/
bool Circuit::read_netlist(std::string netlistFile, bool activateFaultsOnAllNodes)
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
            Node* input1Node = this->create_node(input1Name, activateFaultsOnAllNodes);
            currentGate->set_input1(input1Node);
            this->add_node_gate_map(input1Node->get_name(), currentGate);
            currentIndex++;

            if (currentGate->is_single_input() == false)
            {
                // if not single input gate => add second input
                int input2Name = str_to_int(strParts[currentIndex]);
                Node* input2Node = this->create_node(input2Name, activateFaultsOnAllNodes);
                this->add_node_gate_map(input2Node->get_name(), currentGate);
                currentGate->set_input2(input2Node);
                currentIndex++;
            }

            // init the output
            int outputName = str_to_int(strParts[currentIndex]);
            Node* outputNode = this->create_node(outputName, activateFaultsOnAllNodes);
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
    if (activateFaultsOnAllNodes)
    {
        // activate the bool of s-a-1 and s-a-0 faults at all nodes
        for (auto m : this->node_map)
        {
            m.second->set_stuck_at_0();
            m.second->set_stuck_at_1();
        }
    }
    return true;
}

/*
* Function to create nodes
* @param nodeName -> name of node in netlist
* @param activateFaultsOnAllNodes => activate the s-a-0 and s-a-1 faults on all nodes
* @return Node* -> pointer to node
*/
Node* Circuit::create_node(int nodeName, bool activateFaultsOnAllNodes)
{
    // Check if node already created
    if (this->node_map.count(nodeName) > 0)
    {
        // then return the existing node
        return this->node_map[nodeName];
    }

    // Create the node
    Node* newNode = new Node(nodeName);

    // if mode of runs is to apply all types of faults at all nodes
    if (activateFaultsOnAllNodes)
    {
        newNode->set_stuck_at_0();
        newNode->set_stuck_at_1();
    }

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
* @return bool -> if test vectors applied successfully
*/
bool Circuit::apply_test_vector(std::string testVector)
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
        std::vector<std::pair<int, int>> newFaultList;
        this->node_map[inputNodes[i]]->update_value(testVector[i] == '1');
        // Update the fault list based on fault sensitized at input nodes if any
        if (this->node_map[inputNodes[i]]->is_fault_on_node() == true)
        {
            newFaultList.push_back(this->node_map[inputNodes[i]]->get_fault_on_node());
        }
        this->node_map[inputNodes[i]]->set_node_deductive_fault_list(newFaultList);
        readyNodes.push_back(inputNodes[i]);
    }
    // check the list of gates activated
    while (readyNodes.size() > 0)
    {
        int nodeName = readyNodes.front();
        readyNodes.pop_front();
        std::list<Gate*> connectedGates = this->node_to_gate_map[nodeName];
        
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
        std::cout << "\n";
    }
}

/*
* Function to print the circuit
* @return string representing the modified netlist
*/
std::string Circuit::print_circuit()
{
    std::string outString = "";

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
* Function to print the faults detected at all circuits outputs
*/
void Circuit::print_detectable_faults()
{
    // Print the list
    for (std::pair<int, int> fault : this->get_list_detected_faults())
    {
        std::cout << fault.first << " stuck at " << fault.second << "\n";
    }
}

/*
* Function to print the faults detected to a file
* @param ofstream -> output file stream
*/
void Circuit::print_detectable_fault_to_file(std::ofstream& outFileStream)
{
    // Print the list
    for (std::pair<int, int> fault : this->get_list_detected_faults())
    {
        outFileStream << fault.first << " stuck at " << fault.second << "\n";
    }
}

/*
* Function to add custom fault to circuit
* @param nodeName -> name of node to apply fault
* @param faultType -> type of fault 0 or 1
*/
void Circuit::apply_fault(int nodeName, int faultType)
{
    if (faultType == 1)
    {
        this->node_map[nodeName]->set_stuck_at_1();
    }
    else
    {
        this->node_map[nodeName]->set_stuck_at_0();
    }
}

/*
* Function to get total faults in circuit
* @return int -> total faults in circuit
*/
int Circuit::get_total_faults_count()
{
    int totalFaultCount = 0;
    for (auto nodeName : node_list)
    {
        if (node_map[nodeName]->get_is_stuck_at_0())
        {
            ++totalFaultCount;
        }
        if (node_map[nodeName]->get_is_stuck_at_1())
        {
            ++totalFaultCount;
        }
    }
    return totalFaultCount;
}

/*
* Function to get faults detected at output
* @return int -> number of detectable faults
*/
int Circuit::get_detectable_faults_count()
{
    return this->get_list_detected_faults().size();
}

/*
* Function to get all detected faults at output
* @return std::set<std::pair<int, int>> -> pair of node-name and stuck at value
*/
std::set<std::pair<int, int>> Circuit::get_list_detected_faults()
{
    std::set<std::pair<int, int>> finalFaultList;
    for (auto outNodeName : outputNodes)
    {
        for (auto fault : node_map[outNodeName]->get_node_deductive_fault_list())
        {
            finalFaultList.insert(fault);
        }
    }
    return finalFaultList;
}

/*
* Function to get the converage value
* @return float -> fault coverage percentage
*/
float Circuit::get_fault_coverage_value()
{
    return (this->get_detectable_faults_count() * 100.0 / this->get_total_faults_count());
}

/*
* Function to print the fault coverage
*/
void Circuit::print_fault_converage()
{
    std::cout << "Total nodes in circuit:" << this->node_list.size() << "\n";
    std::cout << "Total faults in circuit:" << this->get_total_faults_count() << "\n";
    std::cout << "Total faults detected: " << this->get_detectable_faults_count() << "\n";
    std::cout << "Coverage percentage:" << this->get_fault_coverage_value() << "%\n";
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
