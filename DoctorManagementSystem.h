#ifndef HEALTHCAREMANAGEMENTSYSTEM_DOCTORMANAGEMENTSYSTEM_H
#define HEALTHCAREMANAGEMENTSYSTEM_DOCTORMANAGEMENTSYSTEM_H

#include <bits/stdc++.h>
#include "DoctorPrimaryIndex.h"
#include "DoctorSecondaryIndex.h"
#include "Doctor.h"
#include "AvailList.h"
#include "AvailListNode.h"

using namespace std;

class DoctorManagementSystem {
private:
    vector<DoctorPrimaryIndex> primaryIndex;  // Primary index for Doctor ID
    vector<DoctorSecondaryIndex> secondaryIndex;  // Secondary index for Doctor Name
    AvailList availList;
public:
    DoctorManagementSystem() {
        loadPrimaryIndexInMemory();
    }

    string getNewId() {
        if (primaryIndex.empty()) {
            return "1";
        } else {
            int newId = static_cast<int>(stol(primaryIndex.back().doctorID));
            newId++;
            return to_string(newId);
        }
    }

    void sortIndexes() {
        sort(primaryIndex.begin(), primaryIndex.end());
        sort(secondaryIndex.begin(), secondaryIndex.end());
    }

    static bool isFileEmpty(const string &filename) {
        ifstream file(filename, ios::in);
        return file.peek() == ifstream::traits_type::eof();  // Check if the file is empty
    }

    void loadPrimaryIndexInMemory() {
        ifstream file("DoctorPrimaryIndex.txt", ios::in);
        if (!file.is_open()) {
            cerr << "Error opening file: DoctorPrimaryIndex.txt\n";
            return;
        }
        if (isFileEmpty("DoctorPrimaryIndex.txt")) {
            return;
        }

        string line;
        while (getline(file, line)) {

            istringstream recordStream(line);
            string id, offset;

            getline(recordStream, id, '|');
            getline(recordStream, offset, '|');

            primaryIndex.emplace_back(id, stoi(offset)); // Save the correct offset.
        }

        file.close();
    }

    void updatePrimaryIndexFile() {
        fstream outFile("DoctorPrimaryIndex.txt", ios::out | ios::trunc);
        if (!outFile.is_open()) {
            cerr << "Error opening file: DoctorPrimaryIndex.txt\n";
            return;
        }
        for (const auto &ele: primaryIndex) {
            outFile << ele.doctorID << '|' << ele.fileOffset << '\n';
        }
        outFile.close();
    }

    int binarySearchPrimaryIndex(const string &id) {
        int left = 0;
        int right = primaryIndex.size() - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (primaryIndex[mid].doctorID == id) {
                return primaryIndex[mid].fileOffset;
            } else if (id < primaryIndex[mid].doctorID) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }
        return -1;
    }

    void loadAvailListInMemory() {
        fstream doctorFile("doctors.txt", ios::in);

        if (!doctorFile.is_open()) {
            cerr << "Error opening file: doctors.txt\n";
            return;
        }
        if (isFileEmpty("doctors.txt")) {
            return;
        }

        string line;

        while (getline(doctorFile, line)) {
            istringstream stream(line);
            string status, size;
            getline(stream, status, '|');
            getline(stream, size, '|');
            if (status == "*") {
                int offset = static_cast<int>(doctorFile.tellg());
                AvailListNode *newNode = new AvailListNode(offset, stoi(size));
                availList.insert(newNode);
            }
        }
    }

    void addDoctor(const Doctor &doctor) {
        fstream file("doctors.txt", ios::in | ios::out);
        if (!file.is_open()) {
            cerr << "Error opening file: doctors.txt" << endl;
            return;
        }

        int lengthIndicator = static_cast<int>(doctor.id.size()) +
                              static_cast<int>(doctor.name.size()) +
                              static_cast<int>(doctor.address.size()) + 4;

        AvailListNode* node = availList.bestFit(lengthIndicator);

        string newRecord = "" ;
        int offset  ;

        if (node != nullptr) { // node is found with appropriate size
            file.seekp(node->offset, ios::beg);
            file.put(' ') ;

            file.seekp(3, ios::cur);

            newRecord +=  "|" + doctor.id + "|" + doctor.name + "|" + doctor.address + "|";

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
            availList.remove(node);

        } else {
            newRecord += " |" ;
            if (lengthIndicator < 10) {
                newRecord += '0';
            }
            newRecord += to_string(lengthIndicator) + "|" + doctor.id + "|" + doctor.name + "|" + doctor.address + "|\n";

            file.seekp(0, ios::end);
            offset = static_cast<int>(file.tellp());
            file.write(newRecord.c_str(), newRecord.size());
        }

        cout << "Doctor " << doctor.name << " is added with id " << doctor.id << endl;

        file.close();

        primaryIndex.emplace_back(doctor.id, offset);
        updatePrimaryIndexFile();
        sortIndexes();
    }


    void updateDoctorName(const string &id, string &newName) {
        int offset = binarySearchPrimaryIndex(id);

        if (offset == -1) {
            cout << "Doctor with ID: " << id << " does not exist.\n";
            return;
        }
        fstream doctorFile("doctors.txt", ios::in | ios::out);
        if (!doctorFile.is_open()) {
            cout << "Error: Could not open the file.\n";
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
            doctorFile.seekp(offset + 7, ios::beg);

            int padding = oldSize - newSize ;
            newName.append(padding , '-') ;

            doctorFile.write(newName.c_str(), oldSize);
            cout << "Doctor's name updated successfully.\n";
        } else {
            cout << "Error: New name is too long to fit in the existing record.\n";
        }

        doctorFile.close();
    }


    void deleteDoctor(const string &id) {
        int offset = binarySearchPrimaryIndex(id);
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

        doctorFile.seekp(1,ios::cur) ;

        string lengthString;
        getline(doctorFile, lengthString, '|'); // get length indicator from record
        int lengthIndicator = stoi(lengthString); // Convert the string to an integer

        cout << "Doctor with ID " << id << " has been marked as deleted.\n";

        doctorFile.close();

        AvailListNode* newNode= new AvailListNode(offset , lengthIndicator) ;
        availList.insert(newNode) ;

        // Remove the doctor from the primary index and update the file
        auto it = remove_if(primaryIndex.begin(), primaryIndex.end(), [&](const DoctorPrimaryIndex &entry) {
            return entry.doctorID == id;
        });
        primaryIndex.erase(it, primaryIndex.end());
        updatePrimaryIndexFile();
    }

    void printPrimaryIndex() {
        for (const auto &index: primaryIndex) {
            cout << index.doctorID << "-->" << index.fileOffset << endl;
        }
    }

    void printDoctorInfo(const string &id) {
        int offSet = binarySearchPrimaryIndex(id);
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

        if (line.empty()) {
            cout << "Error: Empty record at offset " << offSet << ".\n";
            return;
        }

        istringstream recordStream(line);
        string status , length, record_id, name, address;

        // Parse the record
        getline(recordStream, status, '|') ;
        getline(recordStream, length, '|') ;
        getline(recordStream, record_id, '|') ;
        getline(recordStream, name, '|') ;
        getline(recordStream, address, '|') ;

        file.close();

        string temp ;
        for(int i = 0 ; i < name.size() ; ++i){
            if(name[i] != '-')
                temp += name[i] ;
        }
        name = temp ;

        cout << "Doctor's info:\n"
             << "  ID: " << record_id << '\n'
             << "  Name: " << name << '\n'
             << "  Address: " << address << '\n';
    }

};


#endif //HEALTHCAREMANAGEMENTSYSTEM_DOCTORMANAGEMENTSYSTEM_H
