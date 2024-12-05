#ifndef HEALTHCAREMANAGEMENTSYSTEM_DOCTORMANAGEMENTSYSTEM_H
#define HEALTHCAREMANAGEMENTSYSTEM_DOCTORMANAGEMENTSYSTEM_H

#include "PrimaryIndex.h"
#include "SecondaryIndex.h"
#include "AvailList.h"

using namespace std;


// Class representing a doctor in the healthcare management system
class Doctor {
public:
    // Default constructor initializing fields to empty strings
    Doctor() {
        id = name = address = "";
    }

    // Attributes of the Doctor class
    string id;    // Primary Key - Unique identifier for a doctor
    string name;  // Secondary Key - Doctor's name, can be used for searching/sorting
    string address; // Doctor's address

    // Parameterized constructor to initialize a Doctor object with specific details
    Doctor(const string& id, const string& name, const string& address)
            : id(id), name(name), address(address) {}
};


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

            doctorAvailList.remove(node);
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

        cout << "Doctor " << doctor.name << " is added with ID " << stoi(doctor.id) << endl;

        file.close();

        doctorPrimaryIndex.addPrimaryNode(doctor.id, offset);
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

        cout << "Doctor with ID " << stoi(id) << " has been marked as deleted.\n";

        doctorFile.close();

        // Add node to DoctorAvailList
        AvailListNode *newNode = new AvailListNode(offset, lengthIndicator);
        doctorAvailList.insert(newNode);

        // Remove the doctor from the primary index and update the file
        doctorPrimaryIndex.removePrimaryNode(id);
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
             << "  ID: " << stoi(record_id) << '\n'
             << "  Name: " << name << '\n'
             << "  Address: " << address << '\n';
    }

    void doctorPrinter(int choice) {
        ifstream file("doctors.txt", ios::in);
        if (!file) {
            cerr << "Error opening doctor file.\n";
            return;
        }

        string line;
        while (getline(file, line)) {
            if (line[0] != '*') {
                istringstream recordStream(line);
                string status, len, id, name, address;

                // Parse the record
                getline(recordStream, status, '|');
                getline(recordStream, len, '|');
                getline(recordStream, id, '|');
                getline(recordStream, name, '|');
                getline(recordStream, address, '|');

                if (choice == 1) {
                    cout << "ID: " << stoi(id) << '\n';
                }
                else if (choice == 2) {
                    cout << "Name: " << name << '\n';
                }
                else if (choice == 3) {
                    cout << "Address: " << address << '\n';
                }
                else {
                    cout << "ID: " << stoi(id) << " | Name: " << name << " | Address: " << address << '\n';
                }
            }
        }
        file.close();
    }

    vector<string> searchByName(const string &name) {
        // Using the doctor’s secondary index to search by name
        vector<string> doctorIds = doctorSecondaryIndex.getPrimaryKeysBySecondaryKey(name);
        return doctorIds;
    }

    PrimaryIndex &getDoctorPrimaryIndex() {
        return doctorPrimaryIndex;
    }

    SecondaryIndex &getDoctorSecondaryIndex() {
        return doctorSecondaryIndex;
    }

};


#endif //HEALTHCAREMANAGEMENTSYSTEM_DOCTORMANAGEMENTSYSTEM_H
