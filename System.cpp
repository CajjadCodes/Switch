#include "System.hpp"

using namespace std;

System::System(int _id)
    : id(_id){}

void System::listen() {
    cout << "im here" << endl;
    cout << "My id:" << this->id << endl; 
}