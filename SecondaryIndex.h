#ifndef HEALTHCAREMANAGEMENTSYSTEM_SECONDARYINDEX_H
#define HEALTHCAREMANAGEMENTSYSTEM_SECONDARYINDEX_H
#include <bits/stdc++.h>
using namespace std ;

class SecondaryIndex {
public:
    string doctorName;
    vector<string> doctorIDs;  // Handles duplicate names

    bool operator < (const SecondaryIndex& other){ // overload operator <
        return this->doctorName < other.doctorName ;
    }
};


#endif //HEALTHCAREMANAGEMENTSYSTEM_SECONDARYINDEX_H
