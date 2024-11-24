#ifndef HEALTHCAREMANAGEMENTSYSTEM_DOCTOR_H
#define HEALTHCAREMANAGEMENTSYSTEM_DOCTOR_H


class Doctor {
public:
    char ID[15] ;
    char name[30] ;
    char address[30] ;
    Doctor(){} ;
    void setName(char* name){
        strcpy(this->name , name) ;
    }
    void setAddresss(char* address){
        strcpy(this->address , address) ;
    }
    void setID(char* ID){
        strcpy(this->ID , ID) ;
    }


};


#endif //HEALTHCAREMANAGEMENTSYSTEM_DOCTOR_H
