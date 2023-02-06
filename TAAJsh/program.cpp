#include <iostream>
using namespace std;
int main(int argc, char* argv[]){
    cout<<argc<<endl;
    for(int i=0; i<argc; i++){
        cout<<argv[i]<<endl;
    }
    string name;
    cout<<"Enter your name: ";
    cin>>name;
    cout<< "Welcome to our TAAJ, Mr. "<<name<<endl;
    return 0;
}