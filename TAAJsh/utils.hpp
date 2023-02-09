#ifndef UTILS_HPP
#define UTILS_HPP
#include <iostream>
#include <vector>
#include <readline/readline.h>
#include "./history.hpp"

extern History history;

// Trims leading and trailing whitespaces of a string
void trim(std::string &str);
// Splits an input string on the basis of a delimiter
std::vector<std::string> split(std::string &str, char delim);

std::vector<std::string> parseArgs(std::string &str, char delim);

// Sends one line in the form of a string
std::string ReadLine();

int uparrowhandler(int count, int key);
int downarrowhandler(int count, int key);
int ctrlAhandler(int count, int key);
int ctrlEhandler(int count, int key);

void die(const std::string &s);

std::string shellPrompt();

// Function to execute commands
void execute_command(std::vector<std::string> command);

void expand_wildcards(const std::string &arg, std::vector<std::string> &expanded_args);

#endif
