#ifndef HEALTHCAREMANAGEMENTSYSTEM_APPOINTMENT_H
#define HEALTHCAREMANAGEMENTSYSTEM_APPOINTMENT_H
#include <bits/stdc++.h>
using namespace std ;
class  Appointment{
public:
    Appointment(){
        id , date , doctorID = "" ;
    }
    string id;    // Primary Key
    string date;  // Secondary Key
    string doctorID;
};

#endif //HEALTHCAREMANAGEMENTSYSTEM_APPOINTMENT_H
