#ifndef HEALTHCAREMANAGEMENTSYSTEM_DOCTOR_H
#define HEALTHCAREMANAGEMENTSYSTEM_DOCTOR_H

#include <bits/stdc++.h>

using namespace std;

class Doctor {
public:
    Doctor() {
        id = name = address = "";
    }

    string id;    // Primary Key
    string name;  // Secondary Key
    string address;
    Doctor(const string& id,const string& name,const string& address) : id(id), name(name), address(address){}
};


#endif //HEALTHCAREMANAGEMENTSYSTEM_DOCTOR_H
