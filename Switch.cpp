#include "Switch.hpp"

using namespace std;

Switch::Switch(int _totalPorts, int _id)
    : totalPorts(_totalPorts), id(_id) {}

void Switch::listen() {
    cout << "im here" << endl;
    cout << "My id:" << this->id << ", My totalPorts: " << this->totalPorts << endl; 
}