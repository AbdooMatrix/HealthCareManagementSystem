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

    void printAppointmentInfo(const string &id) {
        // Locate the appointment using the primary index
        int offset = appointmentPrimaryIndex.binarySearchPrimaryIndex(id);
        if (offset == -1) {
            // If the appointment is not found, print an error message and return
            cout << "Appointment not found. The ID \"" << id << "\" is invalid.\n";
            return;
        }

        // Open the appointments file for reading
        fstream file("appointments.txt", ios::in);
        if (!file.is_open()) {
            cerr << "Error opening file: appointments.txt\n";
            return;
        }

        // Move to the correct offset in the file
        file.seekg(offset, ios::beg);

        // Read the record from the file
        string line;
        getline(file, line);

        // Parse the record fields
        istringstream recordStream(line);
        string status, length, appointmentID, date, doctorID;

        getline(recordStream, status, '|');
        getline(recordStream, length, '|');
        getline(recordStream, appointmentID, '|');
        getline(recordStream, date, '|');
        getline(recordStream, doctorID, '|');

        // Remove padding characters ('-') from the date
        string temp;
        for (char c : date) {
            if (c != '-')
                temp += c;
        }
        date = temp;

        // Print the appointment information
        cout << "Appointment ID: " << appointmentID << endl;
        cout << "Appointment Date: " << date << endl;
        cout << "Doctor ID: " << stoi(doctorID) << endl;

        // Close the file
        file.close();
    }

    void searchAppointmentsByDoctorID(const string &doctorID) {
        // Retrieve all appointment IDs associated with the given doctor ID
        vector<string> appointmentIDs = appointmentSecondaryIndex.getPrimaryKeysBySecondaryKey(doctorID);

        if (appointmentIDs.empty()) {
            // If no appointments are found, print a message and return
            cout << "No appointments found for Doctor ID: " << doctorID << endl;
            return;
        }

        // Iterate through all appointment IDs and print their details
        for (const string &appointmentID : appointmentIDs) {
            printAppointmentInfo(appointmentID);
        }
    }


    vector<string> searchByDate(const string &date) {
        // Retrieve all appointment IDs associated with a specific date using the secondary index
        // The secondary index maps dates to primary keys (appointment IDs)
        vector<string> appointmentIds = appointmentSecondaryIndex.getPrimaryKeysBySecondaryKey(date);

        // Return the list of appointment IDs for the given date
        return appointmentIds;
    }

    void printAppointment(const string &id) {
        // Locate the appointment's offset in the file using the primary index
        int offSet = appointmentPrimaryIndex.binarySearchPrimaryIndex(id);
        if (offSet == -1) {
            // If the ID is not found in the primary index, print an error message and return
            cout << "Appointment not found. The ID \"" << stoi(id) << "\" is invalid.\n";
            return;
        }

        // Open the appointments file for reading
        fstream file("appointments.txt", ios::in);
        if (!file.is_open()) {
            // Print an error message if the file cannot be opened
            cout << "Error opening file.\n";
            return;
        }

        // Move to the correct offset in the file
        file.seekg(offSet, ios::beg);

        // Read the record at the specified offset
        string line;
        getline(file, line); // Read the record from the file.

        // Print the record details to the console
        cout << line << endl;

        if (line.empty()) {
            // If the record is empty, print a warning message
            cout << "Error: Empty record at offset " << offSet << ".\n";
            return;
        }

        // Close the file after reading
        file.close();
    }

    void printAllAppointments() {
        // Open the appointments file for reading
        ifstream file("appointments.txt", ios::in);
        if (!file) {
            // Print an error message if the file cannot be opened
            cerr << "Error opening appointments file.\n";
            return;
        }

        // Read and print all records line by line
        string line;
        while (getline(file, line)) {
            cout << line << endl;
        }

        // Close the file after reading all records
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

    void printAppointmentId(const string &appointmentId) {
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

    void printAppointmentDate(const string &appointmentId) {
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

    void printAppointmentDoctorid(const string &appointmentId) {
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


