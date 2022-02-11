#ifndef __HELPERFUNCS_H__
#define __HELPERFUNCS_H__

#include <algorithm>
#include <string>
#include <locale>
#include <vector>
#include <regex>

/*
* Function to convert string to lower case
* @param inStr -> string to process
*/
inline std::string str_to_lower(std::string inStr)
{
    std::locale loc;
    std::string outStr;
    for (size_t i = 0; i < inStr.length(); ++i)
    {
        outStr += std::tolower(inStr[i], loc);
    }
    return outStr;
}

/*
* Comparator function for comparing two spaces
*/
inline bool check_space(char str1, char str2)
{
    return (str1 == ' ' && str2 == ' ');
}

/*
* Function to remove continuous multiple spaces
* @param inStr -> string to process
*/
inline void remove_multiple_spaces(std::string &inStr)
{
    std::string::iterator endPt = std::unique(inStr.begin(), inStr.end(), check_space);
    inStr.erase(endPt, inStr.end());
}

/*
* Function to remove tabs
* @param inStr -> string to process
*/
inline void remove_tabs(std::string& inStr)
{
    inStr.erase(std::remove(inStr.begin(), inStr.end(), '\t'), inStr.end());
}

/*
* Function to check if line matches netlist format
* @param inStr -> string to check
* @return bool -> if valid netlist string
*/
inline bool check_netlist_str(std::string inStr)
{
    // valid netlist string would have either a gate or input/output as 1st word
    if (inStr.compare("and") == 0 ||
        inStr.compare("or") == 0 ||
        inStr.compare("inv") == 0 ||
        inStr.compare("nor") == 0 ||
        inStr.compare("nand") == 0 ||
        inStr.compare("buf") == 0 ||
        inStr.compare("xor") == 0 ||
        inStr.compare("xnor") == 0 ||
        inStr.compare("input") == 0 ||
        inStr.compare("output") == 0)
    {
        return true;
    }
    return false;
}

/*
* Function to split string across delimiter
* @param inStr -> string to process
* @param delimiter -> to split the string across
*/
inline std::vector<std::string> split_str(std::string inStr, char delimiter = ' ')
{
    std::vector<std::string> strParts;
    std::string newWord = "";

    // add space in the end to make the logic more generic
    // so that if it in the last word 
    inStr += " ";

    // Remove random consecutive spaces
    //remove_multiple_spaces(inStr);
    for (unsigned int i = 0; i < inStr.length(); ++i)
    {
        // if equal to delimiter
        if (inStr[i] == delimiter)
        {
            strParts.push_back(newWord);
            newWord = "";
        }
        else
        {
            newWord += inStr[i];
        }
    }
    return strParts;
}

/*
* Function to remove the newline character
* Updates the reference directly
*/
inline void remove_newline(std::string &inStr)
{
    if (!inStr.empty() && inStr[inStr.length() - 1] == '\n') {
        inStr.erase(inStr.length() - 1);
    }
}

/*
* Function to convert string to int
* @param inStr -> input string to convert
*/
inline int str_to_int(std::string inStr)
{
    try
    {
        return std::stoi(inStr);
    }
    catch (const std::exception&)
    {
        return -1;
    }
}

/*
* Function to merge two vectors into the first one
* Vector 1 is updated in place
* @param inVec1 -> first vector
* @param inVec2 -> second vector
*/
inline void merge_vectors(std::vector<std::pair<int, int>> &inVec1, std::vector<std::pair<int, int>> inVec2)
{
    // only insert entries which are not already in vector1
    for (std::pair<int, int> currElement : inVec2)
    {
        if (std::find(inVec1.begin(), inVec1.end(), currElement) == inVec1.end())
        {
            // element no in vec1 => add
            inVec1.push_back(currElement);
        }
    }
}

/*
* Function to subtract one vector from another
* Action: inVec1 - inVec2
* @param inVec1 -> first vector
* @param inVec2 -> second vector
* @return resulting vector
*/
inline std::vector<std::pair<int, int>> subtract_vectors(std::vector<std::pair<int, int>> inVec1,
    std::vector<std::pair<int, int>> inVec2)
{
    for (std::pair<int, int> faultVec2 : inVec2)
    {
        remove(inVec1.begin(), inVec1.end(), faultVec2);
    }
    return inVec1;
}

/*
* Function to find intersection of two vectors
* @param inVec1 -> first vector
* @param inVec2 -> second vector
* @return resulting vector
*/
inline std::vector<std::pair<int, int>> intersect_vectors(std::vector<std::pair<int, int>> inVec1,
    std::vector<std::pair<int, int>> inVec2)
{
    std::vector<std::pair<int, int>> returnVector;
    for (std::pair<int, int> currElement : inVec2)
    {
        if (std::find(inVec1.begin(), inVec1.end(), currElement) != inVec1.end())
        {
            // element in both vec1 and vec2
            returnVector.push_back(currElement);
        }
    }
    return returnVector;
}

#endif