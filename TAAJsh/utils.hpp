#ifndef UTILS_HPP
#define UTILS_HPP
#include <iostream>
#include <vector>
#include <termios.h>

#define CTRL_KEY(k) ((k)&0x1f)

struct termios orig_termios;
pid_t pid;

enum editorKey
{
    TABLINE = 9,
    NEWLINE = 13,
    BACKSPACE = 127,
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN
};

// Trims leading and trailing whitespaces of a string
void trim(std::string &str);
// Splits an input string on the basis of a delimiter
std::vector<std::string> split(std::string &str, char delim);

// Sends one line in the form of a string
std::string ReadLine();

int editorReadKey();

// Ctrl key press handler
void processCtrl(char c);

void die(const std::string &s);

void disableRawMode();

void enableRawMode();

void shellPrompt();

//Function to execute commands 
void execute_command(std::vector<std::string> command);

#endif
