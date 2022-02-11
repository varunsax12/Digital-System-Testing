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
public:
    // Pointer to the input1
    Node* input1;
    // Pointer to the input2
    Node* input2;
    // Pointer to the output
    Node* output;
    // Logic for the gate
    gateLogic logic;

    /*
    * Setter for the logic of the gate
    * @param inLogic -> string of the logic
    */
    void set_gate_logic(std::string inLogic);

    /*
    * Getter for the logic value
    * @return gateLogic -> gate logic from enum type
    */
    gateLogic get_gate_logic();

    /*
    * Function to simulate the gate
    * @return bool -> simulate gates taking into account all input types 0, 1, D, DBar, X
    */
    bool simulate();

    /*
    * Check if single input gate (inverter or buffer)
    * @return bool -> if the gate is single input gate or not
    */
    bool is_single_input();

    /*
    * Function to return string of gate type
    * @return string -> string form of gate logic
    */
    std::string get_gate_logic_str();

    /*
    * Function to get controlling value of gate
    * @return logicValue -> controlling value
    */
    logicValue get_controlling_value();

    /*
    * Function to get inversion parity
    * @return logicValue -> inversion parity
    */
    logicValue get_inversion_parity();
};

#endif