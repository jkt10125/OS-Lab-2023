#ifndef UTILS_HPP
#define UTILS_HPP
#include <iostream>
#include <vector>
#include "./history.hpp"

extern History history;

// Trims leading and trailing whitespaces of a string
void trim(std::string &str);
// Splits an input string on the basis of a delimiter
std::vector<std::string> split(std::string &str, char delim);

// Sends one line in the form of a string
std::string ReadLine();

void die(const std::string &s);

std::string shellPrompt();

//Function to execute commands 
void execute_command(std::vector<std::string> command);


std::vector<std::string> expand_wildcards(const std::vector<std::string> &args);

#endif
