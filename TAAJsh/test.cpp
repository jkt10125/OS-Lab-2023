#include <unistd.h>
#include <bits/stdc++.h>

using namespace std;

void execute_command(vector<string> command){
    char * arr[command.size()+1];
    for(int i = 0 ; i < command.size() ; i++){
        arr[i] = (char*)command[i].c_str();
    }
    for(int i = 0 ; i < command.size() ; i++){
        cout<<arr[i]<<endl;
    }
    exit(0);
}

vector<string> InputOutputIdentifier(string str) {
    bool output = (str.find('>')!=string::npos);
    bool input = (str.find('<')!=string::npos);
    vector<string> tokens(3);

    if(input && output){
        int pos1 = str.find('<');
        int pos2 = str.find('>');
        tokens[0] = str.substr(0, pos1);
        tokens[1] = str.substr(pos1+1, pos2-pos1-1);
        tokens[2] = str.substr(pos2+1);
    }
    else if(input){
        int pos = str.find('<');
        tokens[0] = str.substr(0, pos);
        tokens[1] = str.substr(pos+1);
        tokens[2] = "";
    }
    else if(output){
        int pos = str.find('>');
        tokens[0] = str.substr(0, pos);
        tokens[1] = "";
        tokens[2] = str.substr(pos+1);
    }
    else{
        tokens[0] = str;
        tokens[1] = "";
        tokens[2] = "";
    }

    return tokens;
}

int main() {
    // char c;
    // while(1){
    //     string s;
    //     cin>>s;
    //     if(s=="q")
    //         return 0;
    // }

    // vector<string> s = {"ls", "-l"};
    // execute_command(s);

    // vector<string> output;
    // output = InputOutputIdentifier("ls -l > out.txt");
    // cout<<output[0]<<endl;
    // cout<<output[1]<<endl;
    // cout<<output[2]<<endl;
    // cout<<endl;
    // output = InputOutputIdentifier("ls -l < in.txt");
    // cout<<output[0]<<endl;
    // cout<<output[1]<<endl;
    // cout<<output[2]<<endl;
    // cout<<endl;
    // output = InputOutputIdentifier("ls -l < in.txt > out.txt");
    // cout<<output[0]<<endl;
    // cout<<output[1]<<endl;
    // cout<<output[2]<<endl;
    // cout<<endl;
    // output = InputOutputIdentifier("ls -l");
    // cout<<output[0]<<endl;
    // cout<<output[1]<<endl;
    // cout<<output[2]<<endl;
    return 0;
}