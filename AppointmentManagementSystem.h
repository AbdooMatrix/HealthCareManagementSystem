#ifndef HEALTHCAREMANAGEMENTSYSTEM_APPOINTMENTMANAGEMENTSYSTEM_H
#define HEALTHCAREMANAGEMENTSYSTEM_APPOINTMENTMANAGEMENTSYSTEM_H

#include "DoctorManagementSystem.h"
#include "AvailList.h"
#include "PrimaryIndex.h"
#include "SecondaryIndex.h"

using namespace std;

// Class representing an appointment in the healthcare management system
class Appointment {
public:
    // Default constructor to initialize member variables
    Appointment() {
        id = "";     // Initialize the appointment ID to an empty string
        date = "";   // Initialize the date to an empty string
        doctorID = ""; // Initialize the doctor ID to an empty string
    }

    // Attributes of the Appointment class
    string id;       // Primary Key - Unique identifier for an appointment
    string date;     // Secondary Key - Date of the appointment, used for searching/sorting
    string doctorID; // Foreign Key - Links this appointment to a specific doctor
};




// The `AppointmentManagementSystem` class is responsible for managing appointments,
// including adding, updating, deleting, and searching for appointments using primary
// and secondary indexing techniques.
class AppointmentManagementSystem {
private:
    PrimaryIndex appointmentPrimaryIndex;  // Manages primary index for appointment IDs.
    AvailList appointmentAvailList;        // Manages available space in the file.
    PrimaryIndex doctorPrimaryIndex;       // Manages primary index for doctor IDs.
    SecondaryIndex appointmentSecondaryIndex; // Manages secondary index for appointments.

public:
    // Constructor: Initializes file names for the indexes and avail list.
    AppointmentManagementSystem() {
        appointmentPrimaryIndex.setPrimaryIndexFileName("AppointmentPrimaryIndex.txt");
        appointmentAvailList.setAvailListFileName("AppointmentAvailList.txt");
        appointmentSecondaryIndex.setSecondaryIndexAndLabelIdListFileNames(
                "AppointmentSecondaryIndex.txt", "AppointmentLabelIdList.txt");
        doctorPrimaryIndex.setPrimaryIndexFileName("DoctorPrimaryIndex.txt");
    }

    // Provides access to the primary index for appointments.
    PrimaryIndex &getAppointmentPrimaryIndex() {
        return appointmentPrimaryIndex;
    }

    // Provides access to the secondary index for appointments.
    SecondaryIndex &getAppointmentSecondaryIndex() {
        return appointmentSecondaryIndex;
    }

    // Adds a new appointment to the system.
    void addAppointment(Appointment &appointment) {


        // Verify that the doctor exists in the doctor index.
        if (doctorPrimaryIndex.binarySearchPrimaryIndex(appointment.doctorID) == -1) {
            cout << "Error: Doctor ID " << appointment.doctorID
                 << " does not exist. Cannot add appointment.\n";
            return;
        }

        // Generate a new appointment ID.
        appointment.id = appointmentPrimaryIndex.getNewId();

        // Open the appointments file for reading and writing.
        fstream file("appointments.txt", ios::in | ios::out);
        if (!file.is_open()) {
            cerr << "Error: Could not open appointments.txt\n";
            return;
        }

        // Calculate the size of the new record.
        int recordSize = appointment.id.size() + appointment.date.size() +
                         appointment.doctorID.size() + 4;

        // Attempt to find a suitable space in the available list.
        AvailListNode *availableNode = appointmentAvailList.bestFit(recordSize);

        string newRecord = "";
        int offset; // Offset where the record will be written.

        if (availableNode != nullptr) {
            // If suitable space is found, write the record in that space.
            file.seekp(availableNode->offset, ios::beg);
            file.put(' '); // Mark the record as active.
            file.seekp(3, ios::cur);

            newRecord += "|" + appointment.id + "|" + appointment.date + "|" + appointment.doctorID + "|";
            int padding = availableNode->size - newRecord.size();

            if (padding >= 0) {
                newRecord.append(padding, '-'); // Add padding if required.
            } else {
                cerr << "Error: Record size exceeds available space.\n";
                file.close();
                return;
            }

            newRecord += '\n';
            file.write(newRecord.c_str(), availableNode->size);
            offset = availableNode->offset;

            // Remove the node from the avail list.
            appointmentAvailList.remove(availableNode);

        } else {
            // If no suitable space is found, append the record at the end of the file.
            newRecord += " |";
            if (recordSize < 10) {
                newRecord += '0';
            }
            newRecord += to_string(recordSize) + "|" + appointment.id + "|" +
                         appointment.date + "|" + appointment.doctorID + "|\n";

            file.seekp(0, ios::end);
            offset = static_cast<int>(file.tellp());
            file.write(newRecord.c_str(), newRecord.size());
        }

        cout << "Appointment with ID " << stoi(appointment.id) << " has been added.\n";

        file.close();

        // Update the primary and secondary indexes.
        appointmentPrimaryIndex.addPrimaryNode(appointment.id, offset);
        appointmentSecondaryIndex.addPrimaryKeyToSecondaryNode(appointment.doctorID, appointment.id);
    }

    // Updates the date of an appointment identified by its ID.
    void updateAppointmentDate(const string &appointmentID, string &newDate) {
        int offset = appointmentPrimaryIndex.binarySearchPrimaryIndex(appointmentID);

        if (offset == -1) {
            cout << "Error: Appointment with ID " << appointmentID << " does not exist.\n";
            return;
        }

        fstream appointmentFile("appointments.txt", ios::in | ios::out | ios::binary);
        if (!appointmentFile.is_open()) {
            cout << "Error: Could not open appointments.txt\n";
            return;
        }

        // Locate the record by its offset.
        appointmentFile.seekg(offset, ios::beg);

        // Read and parse the existing record.
        string line;
        getline(appointmentFile, line);

        istringstream recordStream(line);
        string status, recordLength, id, oldDate, doctorID;

        if (!(getline(recordStream, status, '|') &&
              getline(recordStream, recordLength, '|') &&
              getline(recordStream, id, '|') &&
              getline(recordStream, oldDate, '|') &&
              getline(recordStream, doctorID, '|'))) {
            cout << "Error: Could not parse the appointment record.\n";
            appointmentFile.close();
            return;
        }

        int oldSize = oldDate.size();
        int newSize = newDate.size();

        if (newSize <= oldSize) {
            // Overwrite the date with padding if necessary.
            int writeOffset = offset + status.size() + 1 + recordLength.size() + 1 + id.size() + 1;
            appointmentFile.seekp(writeOffset, ios::beg);

            newDate.append(oldSize - newSize, '-'); // Pad with '-' if needed.
            appointmentFile.write(newDate.c_str(), oldSize);

            cout << "Appointment date updated successfully.\n";
        } else {
            cout << "Error: New date is too long to fit in the existing record.\n";
        }

        appointmentFile.close();
    }

    void deleteAppointment(const string &id) {
        // Locate the appointment using the primary index
        int offset = appointmentPrimaryIndex.binarySearchPrimaryIndex(id);
        if (offset == -1) {
            // If the ID is not found in the primary index, print a message and return
            cout << "Appointment with ID " << id << " not found.\n";
            return;
        }

        // Open the appointments file for reading and writing
        fstream appointmentFile("appointments.txt", ios::in | ios::out);
        if (!appointmentFile.is_open()) {
            cerr << "Error opening file: appointments.txt\n";
            return;
        }

        // Move to the correct offset in the file
        appointmentFile.seekp(offset, ios::beg);

        // Mark the record as deleted by overwriting the status byte with '*'
        appointmentFile.put('*');

        // Move back and read the rest of the record
        appointmentFile.seekg(-1, ios::cur);
        string line;
        getline(appointmentFile, line); // Read the record from the file

        // Parse the record fields
        istringstream recordStream(line);
        string status, recordLen, record_id, date, doctorID;

        getline(recordStream, status, '|');
        getline(recordStream, recordLen, '|');
        getline(recordStream, record_id, '|');
        getline(recordStream, date, '|');
        getline(recordStream, doctorID, '|');

        // Convert the record length from string to integer
        int lengthIndicator = stoi(recordLen);

        // Print a confirmation message
        cout << "Appointment with ID " << stoi(id) << " has been marked as deleted.\n";

        // Close the file after marking the record as deleted
        appointmentFile.close();

        // Add the deleted record's space to the available list
        AvailListNode *newNode = new AvailListNode(offset, lengthIndicator);
        appointmentAvailList.insert(newNode);

        // Remove the appointment from the primary and secondary indexes
        appointmentPrimaryIndex.removePrimaryNode(id);
        appointmentSecondaryIndex.removePrimaryKeyFromSecondaryNode(doctorID, id);
    }

    vector<string> searchAppointmentsByDoctorID(const string &doctorID) {
        // Using the appointment’s secondary index to search by doctor ID
        vector<string> appointmentIds = appointmentSecondaryIndex.getPrimaryKeysBySecondaryKey(doctorID);
        return appointmentIds;
    }

    void printAppointmentById(const string &id, int choice) {
        int offset = appointmentPrimaryIndex.binarySearchPrimaryIndex(id);
        if (offset == -1) {
            cout << "Appointment not found. The ID \"" << id << "\" is invalid.\n";
            return;
        }

        fstream file("appointments.txt", ios::in);
        if (!file.is_open()) {
            cout << "Error opening file: appointments.txt\n";
            return;
        }

        file.seekg(offset, ios::beg);
        string line;
        getline(file, line);  // Read the record from the file.

        if (line.empty()) {
            cout << "Error: Empty record at offset " << offset << ".\n";
            file.close();
            return;
        }

        istringstream recordStream(line);
        string status, length, appointmentID, date, doctorID;

        // Parse the record fields
        getline(recordStream, status, '|');
        getline(recordStream, length, '|');
        getline(recordStream, appointmentID, '|');
        getline(recordStream, date, '|');
        getline(recordStream, doctorID, '|');

        file.close();

        // Remove padding characters from the date
        string tempDate;
        for (char c : date) {
            if (c != '-')
                tempDate += c;
        }
        date = tempDate;

        switch (choice) {
            case 0:  // Print all appointment information
                cout << "Appointment ID: " << stoi(appointmentID)
                     << " | Date: " << date
                     << " | Doctor ID: " << stoi(doctorID) << '\n';
                break;
            case 1:  // Print only Appointment ID
                cout << "Appointment ID: " << stoi(appointmentID) << '\n';
                break;
            case 2:  // Print only Date
                cout << "Date: " << date << '\n';
                break;
            case 3:  // Print only Doctor ID
                cout << "Doctor ID: " << stoi(doctorID) << '\n';
                break;
            default:  // Default to printing all info
                cout << "Appointment Details:\n"
                     << "  ID: " << appointmentID << '\n'
                     << "  Date: " << date << '\n'
                     << "  Doctor ID: " << stoi(doctorID) << '\n';
                break;
        }
    }


    static void printAllAppointments(int choice) {
        ifstream file("appointments.txt", ios::in);
        if (!file) {
            cerr << "Error opening appointments file.\n";
            return;
        }

        string line;
        while (getline(file, line)) {
            if (line[0] != '*') {
                istringstream recordStream(line);
                string status, length, appointmentID, date, doctorID;

                // Parse the record
                getline(recordStream, status, '|');
                getline(recordStream, length, '|');
                getline(recordStream, appointmentID, '|');
                getline(recordStream, date, '|');
                getline(recordStream, doctorID, '|');

                // Remove padding characters from date
                date.erase(remove(date.begin(), date.end(), '-'), date.end());

                if (choice == 0) {
                    cout << "ID: " << stoi(appointmentID) << " | Date: " << date << " | Doctor ID: " << stoi(doctorID) << '\n';
                }
                else if (choice == 1) {
                    cout << "ID: " << stoi(appointmentID) << '\n';
                }
                else if (choice == 2) {
                    cout << "Date: " << date << '\n';
                }
                else if (choice == 3) {
                    cout << "Doctor ID: " << stoi(doctorID) << '\n';
                }
            }
        }
        file.close();
    }

};

#endif // HEALTHCAREMANAGEMENTSYSTEM_APPOINTMENTMANAGEMENTSYSTEM_H
