#ifndef HEALTHCAREMANAGEMENTSYSTEM_DOCTORPRIMARYINDEX_H
#define HEALTHCAREMANAGEMENTSYSTEM_DOCTORPRIMARYINDEX_H
#include<string>
using namespace std ;

class DoctorPrimaryIndex {
public:
    string doctorID;
    int fileOffset;
    bool operator < (const DoctorPrimaryIndex& other){ // overload operator <
        return (this->doctorID < other.doctorID) ;
    }
};


#endif //HEALTHCAREMANAGEMENTSYSTEM_DOCTORPRIMARYINDEX_H
