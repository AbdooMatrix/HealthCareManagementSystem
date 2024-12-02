#ifndef HEALTHCAREMANAGEMENTSYSTEM_DOCTORSECONDARYINDEX_H
#define HEALTHCAREMANAGEMENTSYSTEM_DOCTORSECONDARYINDEX_H
#include <bits/stdc++.h>
using namespace std ;

class DoctorSecondaryIndex {
public:
    string doctorName;
    vector<string> doctorIDs;  // Handles duplicate names
    bool operator < (const DoctorSecondaryIndex& other){ // overload operator <
        return this->doctorName < other.doctorName ;
    }
};


#endif //HEALTHCAREMANAGEMENTSYSTEM_DOCTORSECONDARYINDEX_H
