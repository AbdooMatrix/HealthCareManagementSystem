#ifndef HEALTHCAREMANAGEMENTSYSTEM_APPOINTMENTMANAGEMENTSYSTEM_H
#define HEALTHCAREMANAGEMENTSYSTEM_APPOINTMENTMANAGEMENTSYSTEM_H
#include <bits/stdc++.h>
#include "Appointment.h"
#include "DoctorManagementSystem.h"
#include "AvailList.h"
#include "PrimaryIndex.h"
#include "SecondaryIndex.h"

using namespace std;

class AppointmentManagementSystem {
private:
    PrimaryIndex appointmentPrimaryIndex;  // Primary index for Appointment ID
    AvailList appointmentAvailList;
    PrimaryIndex doctorPrimaryIndex;
public:
    AppointmentManagementSystem() {
        appointmentPrimaryIndex.loadPrimaryIndexInMemory("AppointmentPrimaryIndex.txt");
        appointmentAvailList.loadAvailListInMemory("AppointmentAvailList.txt");
    }

    void addAppointment(Appointment &appointment) {

        doctorPrimaryIndex.loadPrimaryIndexInMemory("DoctorPrimaryIndex.txt");
        if ( doctorPrimaryIndex.binarySearchPrimaryIndex(appointment.doctorID)==-1){
            cout << "Error: Doctor ID " << appointment.doctorID << " does not exist, Cannot add appointment.\n";
            return;
        }
        appointment.id = appointmentPrimaryIndex.getNewId();
        fstream file("appointments.txt", ios::in | ios::out);
        if (!file.is_open()) {
            cerr << "Error opening file: appointments.txt" << endl;
            return;
        }


        int lengthIndicator = static_cast<int>(appointment.id.size()) +
                              static_cast<int>(appointment.date.size()) +
                              static_cast<int>(appointment.doctorID.size()) + 4;

        AvailListNode* node = appointmentAvailList.bestFit(lengthIndicator);

        string newRecord = "" ;
        int offset  ;

        if (node != nullptr) { // node is found with appropriate size
            file.seekp(node->offset, ios::beg);
            file.put(' ') ;

            file.seekp(3, ios::cur);

            newRecord +=  "|" + appointment.id + "|" + appointment.date + "|" + appointment.doctorID + "|";

            int padding = node->size - static_cast<int>(newRecord.size()) ;

            if (padding >= 0) {
                newRecord.append(padding , '-');
            } else {
                cerr << "Error: Record size exceeds node size in availList!" << endl;
                file.close();
                return;
            }

            newRecord += '\n' ;

            file.write(newRecord.c_str(), node->size);
            offset = node->offset;
            appointmentAvailList.remove(node,"AppointmentAvailList.txt");

        } else {
            newRecord += " |" ;
            if (lengthIndicator < 10) {
                newRecord += '0';
            }
            newRecord += to_string(lengthIndicator) + "|" + appointment.id + "|" + appointment.date + "|" + appointment.doctorID + "|\n";

            file.seekp(0, ios::end);
            offset = static_cast<int>(file.tellp());
            file.write(newRecord.c_str(), newRecord.size());
        }

        cout << "Appointment with ID " << appointment.id << " is added." << endl;

        file.close();
        appointmentPrimaryIndex.addPrimaryNode(appointment.id,offset,"AppointmentPrimaryIndex.txt");


    }

    void updateAppointmentDate(const string &id, string &newDate) {
        // Locate the record using the primary index
        int offset = appointmentPrimaryIndex.binarySearchPrimaryIndex(id);

        if (offset == -1) {
            cout << "Appointment with ID: " << id << " does not exist.\n";
            return;
        }

        fstream appointmentFile("appointments.txt", ios::in | ios::out | ios::binary);
        if (!appointmentFile.is_open()) {
            cout << "Error: Could not open the file.\n";
            return;
        }

        // Move to the specific offset
        appointmentFile.seekg(offset, ios::beg);

        // Read the record at the offset
        string line;
        getline(appointmentFile, line);

        // Parse the record
        istringstream recordStream(line);
        string status, recordLen, record_id, date, doctorID;

        if (!(getline(recordStream, status, '|') &&
              getline(recordStream, recordLen, '|') &&
              getline(recordStream, record_id, '|') &&
              getline(recordStream, date, '|') &&
              getline(recordStream, doctorID, '|'))) {
            cout << "Error: Failed to parse record.\n";
            appointmentFile.close();
            return;
        }

        int oldSize = static_cast<int>(date.size());
        int newSize = static_cast<int>(newDate.size());

        if (newSize <= oldSize) {
            // Overwrite the date field with padding if necessary
            int writeOffset = offset + status.size() + 1 + recordLen.size() + 1 + record_id.size() + 1; // Calculate position dynamically
            appointmentFile.seekp(writeOffset, ios::beg);

            newDate.append(oldSize - newSize, '-'); // Pad with '-' if new date is shorter
            appointmentFile.write(newDate.c_str(), oldSize);

            cout << "Appointment's date updated successfully.\n";
        } else {
            cout << "Error: New date is too long to fit in the existing record.\n";
        }

        appointmentFile.close();
    }

    void deleteAppointment(const string &id) {
        int offset = appointmentPrimaryIndex.binarySearchPrimaryIndex(id);
        if (offset == -1) {
            cout << "Appointment with ID " << id << " not found.\n";
            return;
        }

        fstream appointmentFile("appointments.txt", ios::in | ios::out);
        if (!appointmentFile.is_open()) {
            cerr << "Error opening file: appointments.txt\n";
            return;
        }

        // Move to the correct offset and replace the status byte with '*'
        appointmentFile.seekp(offset, ios::beg);

        appointmentFile.put('*'); // Overwrite the status byte with '*'

        appointmentFile.seekg(-1,ios::cur) ;
        string line;
        getline(appointmentFile, line); // Read the record at the offset

        istringstream recordStream(line);
        string status, recordLen, record_id, date, doctorID;

        // Parse the record
        getline(recordStream, status, '|');
        getline(recordStream, recordLen, '|');
        getline(recordStream, record_id, '|');
        getline(recordStream, date, '|');
        getline(recordStream, doctorID, '|');

        int lengthIndicator = stoi(recordLen); // Convert the string to an integer

        cout << "Appointment with ID " << id << " has been marked as deleted.\n";

        appointmentFile.close();

        AvailListNode* newNode= new AvailListNode(offset , lengthIndicator) ;
        appointmentAvailList.insert(newNode,"AppointmentAvailList.txt");

        // Remove the appointment from the primary index and update the file
        appointmentPrimaryIndex.removePrimaryNode(id,"AppointmentPrimaryIndex.txt");
    }

    void printAppointmentInfo(const string &id) {
        int offSet = appointmentPrimaryIndex.binarySearchPrimaryIndex(id);
        if (offSet == -1) {
            cout << "Appointment not found. The ID \"" << id << "\" is invalid.\n";
            return;
        }

        fstream file("appointments.txt", ios::in);
        if (!file.is_open()) {
            cerr << "Error opening file: appointments.txt\n";
            return;
        }

        // Move to the correct offset
        file.seekg(offSet, ios::beg);

        string line;
        getline(file, line); // Read the record at the offset

        istringstream recordStream(line);
        string status, length, appointmentID, date, doctorID;

        // Parse the record fields
        getline(recordStream, status, '|');
        getline(recordStream, length, '|');
        getline(recordStream, appointmentID, '|');
        getline(recordStream, date, '|');
        getline(recordStream, doctorID, '|');
        string temp;
        for (int i = 0; i < date.size(); ++i) {
            if (date[i] != '-')
                temp += date[i];
        }
        date = temp;

        cout << "Appointment ID: " << appointmentID << endl;
        cout << "Appointment Date: " << date << endl;
        cout << "Doctor ID: " << doctorID << endl;

        file.close();
    }

};

#endif // HEALTHCAREMANAGEMENTSYSTEM_APPOINTMENTMANAGEMENTSYSTEM_H


