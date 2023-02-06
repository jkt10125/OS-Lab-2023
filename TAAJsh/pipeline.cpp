#include <./pipeline.hpp>
#include <./utils.hpp>
using namespace std;

Pipeline::Pipeline(const string &  __str): command(__str){
    parse();
}

Pipeline::~Pipeline(){}

void Pipeline::parse(){
    trim(command);
    if(command.back()=='&'){
        isBackgroundProcess = true;
        command.pop_back();
    }
    vector<string> chuncks = split(command, '|');
    for(int i=0; i<chuncks.size(); i++){
        components.push_back(new Command(chuncks[i]));
    }
}

void Pipeline::execute(){
    
}

