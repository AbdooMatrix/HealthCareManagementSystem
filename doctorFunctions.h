#include <bits/stdc++.h>
#include"Doctor.h"
#include <cstring>
using namespace std;

#ifndef HEALTHCAREMANAGEMENTSYSTEM_DOCTORFUNCTIONS_H
#define HEALTHCAREMANAGEMENTSYSTEM_DOCTORFUNCTIONS_H

int getDoctorMaxID(){
    int maxID = 0 ;
    fstream doctors ;
    doctors.open("doctors.txt" , ios::in) ;
    if(!doctors.is_open()){
        cerr << "error opening doctors file\n" ;
        return -1 ;
    }
    string line ;
    while(getline(doctors , line)){
        string currID ;
        for(int i = 3 ; line[i] != '|' ; ++i){
            currID += line[i] ;
        }
        maxID = max(maxID , stoi(currID)) ;
    }
    return maxID ;
}

void AddDoctor(Doctor& doctor) {
    fstream doctorsFile;
    doctorsFile.open("doctors.txt", ios::app); // Open file for appending
    if (!doctorsFile.is_open()) {
        cerr << "Error opening doctors file\n";
        return;
    }

    // Get the next doctor ID as a string
    string IDString = to_string(getDoctorMaxID() + 1);

    // Add a newline if the file is not empty
    if (IDString != "1") {
        doctorsFile << '\n';
    }

    // Copy the ID string to a char array, ensuring null-termination
    char id[15];
    strncpy(id, IDString.c_str(), sizeof(id) - 1);
    id[sizeof(id) - 1] = '\0'; // Ensure null-termination

    // Set the doctor ID
    doctor.setID(id);

    // Calculate the total record length, including the separators '|' and +3 for '|' separators
    int recordLen = strlen(doctor.name) + strlen(doctor.ID) + strlen(doctor.address) + 3;

    // Create the record string
    string record = to_string(recordLen);
    record += '|';
    char c ;
    for(int i = 0 ;i < 15 ; ++i){
        c = doctor.ID[i] ;
        if(c >= '0' && c <= '9'){
            record += c ;
        }
    }
    record += '|';
    for (int i = 0; doctor.name[i] != '\0'; ++i) {
        c = doctor.name[i];
        if ((c >= 'a' && c <= 'z') || (c == ' ') || (c >= '0' && c <= '9')) {
            record += c;
        }
    }
    record += '|';

    for (int i = 0;doctor.address[i] != '\0'; ++i) {
        c = doctor.address[i];
        if ((c >= 'a' && c <= 'z') || (c == ' ') || (c >= '0' && c <= '9')) {
            record += c;
        }
    }


    // Write the record to the file
    doctorsFile << record;

    cout << "Doctor is added successfully\n";
}





#endif //HEALTHCAREMANAGEMENTSYSTEM_DOCTORFUNCTIONS_H
