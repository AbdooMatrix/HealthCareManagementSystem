#ifndef HEALTHCAREMANAGEMENTSYSTEM_DOCTORMANAGEMENTSYSTEM_H
#define HEALTHCAREMANAGEMENTSYSTEM_DOCTORMANAGEMENTSYSTEM_H

#include <bits/stdc++.h>
#include "PrimaryIndex.h"
#include "SecondaryIndex.h"
#include "Doctor.h"
#include "AvailList.h"

using namespace std;

class DoctorManagementSystem {
private:
    PrimaryIndex doctorPrimaryIndex;
    SecondaryIndex doctorSecondaryIndex;
    AvailList doctorAvailList;

public:
    DoctorManagementSystem() {
        doctorPrimaryIndex.setPrimaryIndexFileName("DoctorPrimaryIndex.txt");
        doctorSecondaryIndex.setSecondaryIndexAndLabelIdListFileNames("DoctorSecondaryIndex.txt",
                                                                      "DoctorLabelIdList.txt");
        doctorAvailList.setAvailListFileName("DoctorAvailList.txt");
    }

    void addDoctor(Doctor &doctor) {
        doctor.id = doctorPrimaryIndex.getNewId();

        fstream file("doctors.txt", ios::in | ios::out);
        if (!file.is_open()) {
            cerr << "Error opening file: doctors.txt" << endl;
            return;
        }

        int lengthIndicator = static_cast<int>(doctor.id.size()) +
                              static_cast<int>(doctor.name.size()) +
                              static_cast<int>(doctor.address.size()) + 4;


        AvailListNode *node = doctorAvailList.bestFit(lengthIndicator);

        string newRecord = "";
        int offset;

        if (node != nullptr) { // Node is found with appropriate size
            file.seekp(node->offset, ios::beg);
            file.put(' '); // Mark the record as active

            file.seekp(3, ios::cur);

            newRecord += "|" + doctor.id + "|" + doctor.name + "|" + doctor.address + "|";

            int padding = node->size - static_cast<int>(newRecord.size());
            if (padding >= 0) {
                newRecord.append(padding, '-');
            }

            newRecord += '\n';
            file.write(newRecord.c_str(), node->size);
            offset = node->offset;

            doctorAvailList.remove(node, "DoctorAvailList.txt");
        } else { // No suitable node found, append to the end of the file
            newRecord += " |";
            if (lengthIndicator < 10) {
                newRecord += '0';
            }
            newRecord +=
                    to_string(lengthIndicator) + "|" + doctor.id + "|" + doctor.name + "|" + doctor.address + "|\n";

            file.seekp(0, ios::end);
            offset = static_cast<int>(file.tellp());
            file.write(newRecord.c_str(), newRecord.size());
        }

        cout << "Doctor " << doctor.name << " is added with ID " << doctor.id << endl;

        file.close();

        doctorPrimaryIndex.addPrimaryNode(doctor.id, offset, "DoctorPrimaryIndex.txt");
        doctorSecondaryIndex.addPrimaryKeyToSecondaryNode(doctor.name, doctor.id);
    }


    void updateDoctorName(const string &id, string &newName) {
        int offset = doctorPrimaryIndex.binarySearchPrimaryIndex(id);

        if (offset == -1) {
            cerr << "Error: Doctor ID not found in primary index.\n";
            return;
        }
        fstream doctorFile("doctors.txt", ios::in | ios::out);
        if (!doctorFile.is_open()) {
            cerr << "Error: Could not open doctors.txt file.\n";
            return;
        }
        // Move to the specific offset
        doctorFile.seekg(offset, ios::beg);

        string line;
        getline(doctorFile, line); // Read the record at the offset

        istringstream recordStream(line);
        string status, recordLen, record_id, name, address;

        // Parse the record
        getline(recordStream, status, '|');
        getline(recordStream, recordLen, '|');
        getline(recordStream, record_id, '|');
        getline(recordStream, name, '|');
        getline(recordStream, address, '|');

        int oldSize = static_cast<int>(name.size());
        int newSize = static_cast<int>(newName.size());


        if (newSize <= oldSize) {
            doctorSecondaryIndex.removePrimaryKeyFromSecondaryNode(name, id);
            doctorSecondaryIndex.addPrimaryKeyToSecondaryNode(newName, id);
            size_t nameOffset = offset + status.size() + recordLen.size() + record_id.size() + 3; // Adjust dynamically
            doctorFile.seekp(nameOffset, ios::beg);
            newName.resize(oldSize, '-'); // Ensure consistent size
            doctorFile.write(newName.c_str(), oldSize);

        } else {
            deleteDoctor(record_id);
            Doctor doctor;
            doctor.id = doctorPrimaryIndex.getNewId();
            doctor.name = newName;
            doctor.address = address;
            addDoctor(doctor);

        }
        cout << "Doctor's name updated successfully.\n";

        doctorFile.close();
    }

    void deleteDoctor(const string &id) {
        int offset = doctorPrimaryIndex.binarySearchPrimaryIndex(id);
        if (offset == -1) {
            cout << "Doctor with ID " << id << " not found.\n";
            return;
        }

        fstream doctorFile("doctors.txt", ios::in | ios::out);
        if (!doctorFile.is_open()) {
            cerr << "Error opening file: doctors.txt\n";
            return;
        }

        // Move to the correct offset and replace the status byte with '*'
        doctorFile.seekp(offset, ios::beg);

        doctorFile.put('*'); // Overwrite the status byte with '*'
        doctorFile.seekg(-1, ios::cur);

        string line;
        getline(doctorFile, line); // Read the record at the offset

        istringstream recordStream(line);
        string status, recordLen, record_id, name, address;

        // Parse the record
        getline(recordStream, status, '|');
        getline(recordStream, recordLen, '|');
        getline(recordStream, record_id, '|');
        getline(recordStream, name, '|');
        getline(recordStream, address, '|');

        int lengthIndicator = stoi(recordLen); // Convert the string to an integer

        cout << "Doctor with ID " << id << " has been marked as deleted.\n";

        doctorFile.close();

        // Add node to DoctorAvailList
        AvailListNode *newNode = new AvailListNode(offset, lengthIndicator);
        doctorAvailList.insert(newNode);

        // Remove the doctor from the primary index and update the file
        doctorPrimaryIndex.removePrimaryNode(id, "DoctorPrimaryIndex.txt");
        doctorSecondaryIndex.removePrimaryKeyFromSecondaryNode(name, id);
    }

    void printDoctorInfo(const string &id) {
        int offSet = doctorPrimaryIndex.binarySearchPrimaryIndex(id);
        if (offSet == -1) {
            cout << "Doctor not found. The ID \"" << id << "\" is invalid.\n";
            return;
        }

        fstream file("doctors.txt", ios::in);
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

        istringstream recordStream(line);
        string status, length, record_id, name, address;

        // Parse the record
        getline(recordStream, status, '|');
        getline(recordStream, length, '|');
        getline(recordStream, record_id, '|');
        getline(recordStream, name, '|');
        getline(recordStream, address, '|');

        file.close();

        string temp;
        for (int i = 0; i < name.size(); ++i) {
            if (name[i] != '-')
                temp += name[i];
        }
        name = temp;

        cout << "Doctor's info:\n"
             << "  ID: " << record_id << '\n'
             << "  Name: " << name << '\n'
             << "  Address: " << address << '\n';
    }

    vector<string> searchByName(const string &name) {
        // Using the doctor’s secondary index to search by name
        vector<string> doctorIds = doctorSecondaryIndex.getPrimaryKeysBySecondaryKey(name);
        return doctorIds;
    }

    void printDoctor(const string &id) {
        int offSet = doctorPrimaryIndex.binarySearchPrimaryIndex(id);
        if (offSet == -1) {
            cout << "Doctor not found. The ID \"" << id << "\" is invalid.\n";
            return;
        }

        fstream file("doctors.txt", ios::in);
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

    void printAllDoctors() {
        ifstream file("doctors.txt", ios::in);
        if (!file) {
            cerr << "Error opening doctor file.\n";
            return;
        }

        string line;
        while (getline(file, line)) {
            cout << line << endl;
        }
        file.close();
    }

    void printDoctorAllNames() {
        ifstream file("doctors.txt");
        if (!file.is_open()) {
            cout << "Error: Unable to open the doctors file.\n";
            return;
        }

        string line;
        while (getline(file, line)) {
            // Parse the line
            size_t nameStart = line.find('|', line.find('|', line.find('|') + 1) + 1) + 1;
            size_t nameEnd = line.find('|', nameStart);

            string name = line.substr(nameStart, nameEnd - nameStart);
            cout << name << endl;  // Print the name
        }

        file.close();
    }

    void printDoctorAllIds() {
        ifstream file("doctors.txt");
        if (!file.is_open()) {
            cout << "Error: Unable to open the doctors file.\n";
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

    void printDoctorAllAddresses() {
        ifstream file("doctors.txt");
        if (!file.is_open()) {
            cout << "Error: Unable to open the doctors file.\n";
            return;
        }

        string line;
        while (getline(file, line)) {
            // Parse the line
            size_t addressStart = line.find('|', line.find('|', line.find('|', line.find('|') + 1) + 1) + 1) + 1;
            size_t addressEnd = line.find('|', addressStart);

            string address = line.substr(addressStart, addressEnd - addressStart);
            cout << address << endl;  // Print the address
        }

        file.close();
    }

    void printDoctorId(const string &doctorId) {
        ifstream file("doctors.txt");
        if (!file.is_open()) {
            cout << "Error: Unable to open the doctors file.\n";
            return;
        }

        string line;
        while (getline(file, line)) {
            // Parse the line to extract the ID
            size_t idStart = line.find('|', line.find('|') + 1) + 1;
            size_t idEnd = line.find('|', idStart);

            string id = line.substr(idStart, idEnd - idStart);

            if (id == doctorId) {
                cout << id << endl;  // Print the Doctor ID
                file.close();
                return;
            }
        }

        cout << "Doctor with ID " << doctorId << " not found.\n";
        file.close();
    }

    void printDoctorName(const string &doctorId) {
        ifstream file("doctors.txt");
        if (!file.is_open()) {
            cout << "Error: Unable to open the doctors file.\n";
            return;
        }

        string line;
        while (getline(file, line)) {
            // Parse the line to extract the Doctor ID
            size_t idStart = line.find('|', line.find('|') + 1) + 1;
            size_t idEnd = line.find('|', idStart);

            string id = line.substr(idStart, idEnd - idStart);

            if (id == doctorId) {
                // Extract the Name (3rd field between 3rd and 4th '|')
                size_t nameStart = line.find('|', line.find('|', line.find('|') + 1) + 1) + 1;
                size_t nameEnd = line.find('|', nameStart);

                string name = line.substr(nameStart, nameEnd - nameStart);
                cout << name << endl;
                file.close();
                return;
            }
        }

        cout << "Doctor with ID " << doctorId << " not found.\n";
        file.close();
    }

    void printDoctorAddress(const string &doctorId) {
        ifstream file("doctors.txt");
        if (!file.is_open()) {
            cout << "Error: Unable to open the doctors file.\n";
            return;
        }

        string line;
        while (getline(file, line)) {
            // Parse the line to extract the ID
            size_t idStart = line.find('|', line.find('|') + 1) + 1;
            size_t idEnd = line.find('|', idStart);

            string id = line.substr(idStart, idEnd - idStart);

            if (id == doctorId) {
                // Extract the Address
                size_t addressStart = line.find('|', line.find('|', line.find('|', line.find('|') + 1) + 1) + 1) + 1;
                size_t addressEnd = line.find('|', addressStart);

                string address = line.substr(addressStart, addressEnd - addressStart);
                cout << address << endl;  // Print the Doctor Address
                file.close();
                return;
            }
        }

        cout << "Doctor with ID " << doctorId << " not found.\n";
        file.close();
    }

    PrimaryIndex &getDoctorPrimaryIndex() {
        return doctorPrimaryIndex;
    }

    SecondaryIndex &getDoctorSecondaryIndex() {
        return doctorSecondaryIndex;
    }

};


#endif //HEALTHCAREMANAGEMENTSYSTEM_DOCTORMANAGEMENTSYSTEM_H
