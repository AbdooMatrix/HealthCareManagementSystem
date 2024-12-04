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
    SecondaryIndex appointmentSecondaryIndex;
public:
    AppointmentManagementSystem() {
        appointmentPrimaryIndex.loadPrimaryIndexInMemory("AppointmentPrimaryIndex.txt");
        appointmentAvailList.loadAvailListInMemory("AppointmentAvailList.txt");
        appointmentSecondaryIndex.loadSecondaryIndexInMemory("AppointmentSecondaryIndex.txt");
    }

    PrimaryIndex& getAppointmentPrimaryIndex() { return appointmentPrimaryIndex; }
    SecondaryIndex& getAppointmentSecondaryIndex() { return appointmentSecondaryIndex; }

    void addAppointment(Appointment &appointment) {

        doctorPrimaryIndex.loadPrimaryIndexInMemory("DoctorPrimaryIndex.txt");
        if (doctorPrimaryIndex.binarySearchPrimaryIndex(appointment.doctorID) == -1) {
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

        AvailListNode *node = appointmentAvailList.bestFit(lengthIndicator);

        string newRecord = "";
        int offset;

        if (node != nullptr) { // node is found with appropriate size
            file.seekp(node->offset, ios::beg);
            file.put(' ');

            file.seekp(3, ios::cur);

            newRecord += "|" + appointment.id + "|" + appointment.date + "|" + appointment.doctorID + "|";

            int padding = node->size - static_cast<int>(newRecord.size());

            if (padding >= 0) {
                newRecord.append(padding, '-');
            } else {
                cerr << "Error: Record size exceeds node size in availList!" << endl;
                file.close();
                return;
            }

            newRecord += '\n';

            file.write(newRecord.c_str(), node->size);
            offset = node->offset;
            appointmentAvailList.remove(node, "AppointmentAvailList.txt");

        } else {
            newRecord += " |";
            if (lengthIndicator < 10) {
                newRecord += '0';
            }
            newRecord += to_string(lengthIndicator) + "|" + appointment.id + "|" + appointment.date + "|" +
                         appointment.doctorID + "|\n";

            file.seekp(0, ios::end);
            offset = static_cast<int>(file.tellp());
            file.write(newRecord.c_str(), newRecord.size());
        }

        cout << "Appointment with ID " << appointment.id << " is added." << endl;

        file.close();
        appointmentPrimaryIndex.addPrimaryNode(appointment.id, offset, "AppointmentPrimaryIndex.txt");
        appointmentSecondaryIndex.addPrimaryKeyInSecondaryNode(appointment.doctorID, appointment.id,
                                                               "AppointmentSecondaryIndex.txt");


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
            int writeOffset = offset + status.size() + 1 + recordLen.size() + 1 + record_id.size() +
                              1; // Calculate position dynamically
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

        appointmentFile.seekg(-1, ios::cur);
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

        AvailListNode *newNode = new AvailListNode(offset, lengthIndicator);
        appointmentAvailList.insert(newNode, "AppointmentAvailList.txt");

        // Remove the appointment from the primary index and update the file
        appointmentPrimaryIndex.removePrimaryNode(id, "AppointmentPrimaryIndex.txt");
        appointmentSecondaryIndex.removePrimaryKeyFromSecondaryNode(doctorID, id, "AppointmentSecondaryIndex.txt");

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

    void searchAppointmentsByDoctorID(const string &doctorID) {
        int indexNode = appointmentSecondaryIndex.binarySearchSecondaryIndex(doctorID);

        if (indexNode == -1) {
            cout << "No appointments found for Doctor ID: " << doctorID << endl;
            return;
        }
        vector<string> appointmentIDs = appointmentSecondaryIndex.getPrimaryKeys(indexNode);

        if (appointmentIDs.empty()) {
            cout << "No appointments found for Doctor ID: " << doctorID << endl;
            return;
        }

        for (const string &appointmentID: appointmentIDs) {

            printAppointmentInfo(appointmentID);
        }
    }
vector<string> searchByDate(const string& date) {
        // Using the appointment’s secondary index to search by date
        vector<string> appointmentIds = appointmentSecondaryIndex.getPrimaryKeysBySecondaryKey(date);
        return appointmentIds;
    }

    void printAppointment(const string &id) {
        int offSet = appointmentPrimaryIndex.binarySearchPrimaryIndex(id);
        if (offSet == -1) {
            cout << "appointment not found. The ID \"" << id << "\" is invalid.\n";
            return;
        }

        fstream file("appointments.txt", ios::in);
        if (!file.is_open()) {
            cout << "Error opening file.\n";
            return;
        }

        file.seekg(offSet, ios::beg);
        string line;
        getline(file, line); // Read the record from the file.

        cout << line << endl;

        if (line.empty()) {
            cout << "Error: Empty record at offset " << offSet << ".\n";
            return;
        }

        file.close();

    }

    void printAllAppointments() {
        ifstream file("appointments.txt",ios::in );
        if (!file) {
            cerr << "Error opening appointments file.\n";
            return;
        }

        string line;
        while (getline(file, line)) {
            cout << line << endl;
        }
        file.close();
    }

    void printAppointmentAllDates() {
        ifstream file("appointments.txt");
        if (!file.is_open()) {
            cout << "Error: Unable to open the appointments file.\n";
            return;
        }

        string line;
        while (getline(file, line)) {
            // Parse the line
            size_t dateStart = line.find('|', line.find('|', line.find('|') + 1) + 1) + 1;
            size_t dateEnd = line.find('|', dateStart);

            string date = line.substr(dateStart, dateEnd - dateStart);
            cout << date << endl;  // Print the date
        }

        file.close();
    }

    void printAppointmentAllIds() {
        ifstream file("appointments.txt");
        if (!file.is_open()) {
            cout << "Error: Unable to open the appointments file.\n";
            return;
        }

        string line;
        while (getline(file, line)) {
            // Parse the line
            size_t idStart = line.find('|', line.find('|') + 1) + 1;
            size_t idEnd = line.find('|', idStart);

            string id = line.substr(idStart, idEnd - idStart);
            cout << id << endl;  // Print the ID
        }

        file.close();
    }

    void printAppointmentAllDoctorids() {
        ifstream file("appointments.txt");
        if (!file.is_open()) {
            cout << "Error: Unable to open the appointments file.\n";
            return;
        }

        string line;
        while (getline(file, line)) {
            // Parse the line
            size_t doctoridStart = line.find('|', line.find('|', line.find('|', line.find('|') + 1) + 1) + 1) + 1;
            size_t doctoridEnd = line.find('|', doctoridStart);

            string doctorid = line.substr(doctoridStart, doctoridEnd - doctoridStart);
            cout << doctorid << endl;  // Print the doctor id
        }

        file.close();
    }

    void printAppointmentId(const string& appointmentId) {
    ifstream file("appointments.txt");
    if (!file.is_open()) {
        cout << "Error: Unable to open the appointments file.\n";
        return;
    }

    string line;
    while (getline(file, line)) {
        // Parse the line to extract the ID
        size_t idStart = line.find('|', line.find('|') + 1) + 1;
        size_t idEnd = line.find('|', idStart);

        string id = line.substr(idStart, idEnd - idStart);

        if (id == appointmentId) {
            cout << id << endl;  // Print the appointment ID
            file.close();
            return;
        }
    }

    cout << "appointment with ID " << appointmentId << " not found.\n";
    file.close();
}

void printAppointmentDate(const string& appointmentId) {
        ifstream file("appointments.txt");
        if (!file.is_open()) {
            cout << "Error: Unable to open the appointments file.\n";
            return;
        }

        string line;
        while (getline(file, line)) {
            // Parse the line to extract the appointment ID
            size_t idStart = line.find('|', line.find('|') + 1) + 1;
            size_t idEnd = line.find('|', idStart);

            string id = line.substr(idStart, idEnd - idStart);

            if (id == appointmentId) {
                size_t dateStart = line.find('|', line.find('|', line.find('|') + 1) + 1) + 1;
                size_t dateEnd = line.find('|', dateStart);

                string date = line.substr(dateStart, dateEnd - dateStart);
                cout << date << endl;
                file.close();
                return;
            }
        }

        cout << "appointment with ID " << appointmentId << " not found.\n";
        file.close();
}

void printAppointmentDoctorid(const string& appointmentId) {
    ifstream file("appointments.txt");
    if (!file.is_open()) {
        cout << "Error: Unable to open the appointments file.\n";
        return;
    }

    string line;
    while (getline(file, line)) {
        // Parse the line to extract the ID
        size_t idStart = line.find('|', line.find('|') + 1) + 1;
        size_t idEnd = line.find('|', idStart);

        string id = line.substr(idStart, idEnd - idStart);

        if (id == appointmentId) {
            // Extract the doctor id
            size_t doctoridStart = line.find('|', line.find('|', line.find('|', line.find('|') + 1) + 1) + 1) + 1;
            size_t doctoridEnd = line.find('|', doctoridStart);

            string doctorid = line.substr(doctoridStart, doctoridEnd - doctoridStart);
            cout << doctorid << endl;  // Print the Doctor id
            file.close();
            return;
        }
    }

    cout << "appointment with ID " << appointmentId << " not found.\n";
    file.close();
}
};

#endif // HEALTHCAREMANAGEMENTSYSTEM_APPOINTMENTMANAGEMENTSYSTEM_H


