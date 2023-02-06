#ifndef PIPELINE_HPP
#define PIPELINE_HPP
#include<iostream>
#include <string>
#include <vector>

class Pipeline{
    public:
        std::string command;
        std::vector<std::string> components;
        bool isBackgroundProcess;
        Pipeline(const std::string & __str);
        ~Pipeline();
        void parse();
        void execute();
};
#endif