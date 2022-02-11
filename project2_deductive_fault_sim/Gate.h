#ifndef __GATE_H__
#define __GATE_H__

#include <string>
#include <iostream>
#include "Node.h"
#include "HelperFuncs.h"

typedef enum gateLogic {and_l, or_l, not_l, nand_l, nor_l, xor_l, xnor_l, buf_l, none_l} gateLogic;

/*
* Class for gate
* Hold the input1, input2 and output node
* Activation => Both inputs of the gate have a valid value
* When activated, auto simulate
*/
class Gate
{
    // Pointer to the input1
    Node* input1;
    // Pointer to the input2
    Node* input2;
    // Pointer to the output
    Node* output;
    // Logic for the gate
    gateLogic logic;
    // Bool to tell if simulation done, so that resimulation can be avoided
    bool simulationDone;
public:

    /*
    * Constructor to initializing the object
    */
    Gate();

    /*
    * Setter for input1 to link the node
    * @param inInput1 -> pointer to the input1 node
    */
    void set_input1(Node* inInput1);

    /*
    * Setter for input1 to link the node
    * @param inInput2 -> pointer to the input2 node
    */
    void set_input2(Node* inInput2);

    /*
    * Setter for input1 to link the node
    * @param inOutput -> pointer to the output node
    */
    void set_output(Node* inOutput);

    /*
    * Getter for input1
    */
    Node* get_input1();

    /*
    * Getter for input2
    */
    Node* get_input2();

    /*
    * Getter for output
    */
    Node* get_output();

    /*
    * Setter of the input1 value
    * @param inValue -> value of input1 node
    */
    void set_input1_value(int inValue);

    /*
    * Setter of the input2 value
    * @param inValue -> value of input2 node
    */
    void set_input2_value(int inValue);

    /*
    * Setter of the output value
    * @param inValue -> value of output node
    */
    void set_output_value(int inValue);

    /*
    * Getter for the input1 value
    */
    int get_input1_value();

    /*
    * Getter for the input2 value
    */
    int get_input2_value();

    /*
    * Getter for the output value
    */
    int get_output_value();

    /*
    * Setter for the logic of the gate
    * @param inLogic -> string of the logic
    */
    void set_gate_logic(std::string inLogic);

    /*
    * Getter for the logic value
    */
    gateLogic get_gate_logic();

    /*
    * Function to simulate the gate
    */
    void simulate();

    /*
    * Function to set simulationDone
    */
    void set_simulation_done();

    /*
    * Function to check whether the gate already simulated
    */
    bool is_simulated();

    /*
    * Check if single input gate (inverter or buffer)
    */
    bool is_single_input();

    /*
    * Check if gate activated
    */
    void simulate_when_active();

    /*
    * Function to return string of gate type
    */
    std::string get_gate_logic_str();

    /*
    * Function to get the inversion parity of gate
    * @return int -> inversion parity (0 => no inversion, 1 => inversion)
    */
    int get_inversion_parity();

    /*
    * Function to get the controlling value of gate
    * @return int -> controlling value of gate inputs
    */
    int get_controlling_value();

    /*
    * Function to check if input1 is at controlling value
    * @return bool -> input1 is at controlling value
    */
    bool is_input1_controlling();

    /*
    * Function to check if input1 is at controlling value
    * @return bool -> input1 is at controlling value
    */
    bool is_input2_controlling();

    /*
    * Function to get the input1 fault list
    * @return vector of pairs -> of fault list on input1
    */
    std::vector<std::pair<int, int>> get_input1_fault_list();

    /*
    * Function to get the input2 fault list
    * @return vector of pairs -> of fault list on input2
    */
    std::vector<std::pair<int, int>> get_input2_fault_list();

    /*
    * Destructor for the class
    */
    ~Gate();
};

#endif