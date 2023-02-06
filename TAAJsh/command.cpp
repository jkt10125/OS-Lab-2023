#include "./command.hpp"

using namespace std;

Command::Command(const string & __str){
    command = __str;
    parse();
    set_fd();
}
void Command::parse(){
}
void Command::set_fd(){

}