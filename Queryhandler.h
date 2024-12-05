#ifndef QUERYHANDLER_H
#define QUERYHANDLER_H

#include <bits/stdc++.h>
#include <cstring>
#include "DoctorManagementSystem.h"
#include "AppointmentManagementSystem.h"
#include "PrimaryIndex.h"
#include "SecondaryIndex.h"
using namespace std ;

void trim(string &str) {
    if (str.empty()) {
        return; // Handle empty string case
    }

    int i = 0, j = str.size() - 1;

    // Move `i` forward until the first non-space character
    while (i <= j && str[i] == ' ') {
        i++;
    }

    // Move `j` backward until the last non-space character
    while (j >= i && str[j] == ' ') {
        j--;
    }

    // Update the string only if trimming is required
    if (i != 0 || (j != str.size() - 1) ) {
        str = str.substr(i, j - i + 1);
    }
}

bool parseCondition(const string& condition, string& key, string& value) {
    size_t eqPos = condition.find('=');
    if (eqPos == string::npos) {
        return false;
    }

    key = condition.substr(0, eqPos);
    value = condition.substr(eqPos + 1);

    trim(key);
    trim(value);

    // Remove quotes from value if present
    if (!value.empty() && value.front() == '\'' && value.back() == '\'') {
        value = value.substr(1, value.size() - 2);
    }
    return true;
}

void handleDoctorNoCondition(const string& fields, DoctorManagementSystem& doctorSystem) {
    if (fields == "*" || fields == "all") {
        // If the query requests all fields, print all doctor information
        doctorSystem.printAllDoctors();
         } else if (fields == "name" || fields == "Name") {
             // If the query requests only the 'name' field
             doctorSystem.printDoctorAllNames();
         } else if (fields == "id" || fields == "ID") {
             // If the query requests only the 'doctor id' field
             doctorSystem.printDoctorAllIds();
        }else if (fields == "address" || fields == "Address") {
            // If the query requests only the 'address' field
            doctorSystem.printDoctorAllAddresses();
    }else {
        cout << "Invalid field in SELECT query. Valid fields are '*', 'name', 'id','address'.\n";
    }
}

void handleDoctorByName(const string& fields, const string& name, DoctorManagementSystem& doctorSystem) {
    vector<string> doctorIds = doctorSystem.searchByName(name);
    if (doctorIds.empty()) {
        cout << "No doctors found with name: " << name << ".\n";
        return;
    }

    for (const string& doctorId : doctorIds) {
        if (fields == "*" || fields == "all") {
            doctorSystem.printDoctor(doctorId);  // Print all details of the doctor
        } else if (fields == "id" || fields == "ID") {
            doctorSystem.printDoctorId(doctorId);  // Print only the ID
        } else if (fields == "name" || fields == "Name") {
            doctorSystem.printDoctorName(doctorId);  // Correctly print only the Doctor Name
        } else if (fields == "address" || fields == "Address") {
            doctorSystem.printDoctorAddress(doctorId);  // Print only the Doctor Address
        } else {
            cout << "Invalid field for Doctor: " << fields << ".\n";
        }
    }
    }

void handleDoctorById(const string& fields, const string& id, DoctorManagementSystem& doctorSystem) {
    PrimaryIndex doctorprimaryindex = doctorSystem.getDoctorPrimaryIndex();
    int offset = doctorprimaryindex.binarySearchPrimaryIndex(id);
    if (offset == -1) {
        cout << "Doctor with ID " << id << " not found.\n";
        return;
    }

    if (fields == "*" || fields == "all") {
        doctorSystem.printDoctor(id);
        } else if (fields == "id" || fields == "ID") {
             doctorSystem.printDoctorId(id);
         } else if (fields == "name" || fields == "Name") {
             doctorSystem.printDoctorName(id);
         } else if (fields == "address" || fields == "Address") {
             doctorSystem.printDoctorAddress(id);
        } else {
            cout << "Invalid field for Doctor: " << fields << ".\n";
        }
    }

void handleAppointmentNoCondition(const string& fields, AppointmentManagementSystem& appointmentSystem) {
    if (fields == "*" || fields == "all") {
        appointmentSystem.printAllAppointments();
    } else if (fields == "id" || fields == "ID") {
        appointmentSystem.printAppointmentAllIds();
    } else if (fields == "date" || fields == "Date") {
        appointmentSystem.printAppointmentAllDates();
    } else if (fields == "doctor id" || fields == "doctor ID" || fields == "Doctor ID" || fields == "Doctor id" ) {
        appointmentSystem.printAppointmentAllDates();
    }else {
        cout << "Invalid field in SELECT query. Valid fields are '*', 'appointment id', 'date', 'doctor id'.\n";
    }
}

void handleAppointmentById(const string& fields, const string& id, AppointmentManagementSystem& appointmentSystem) {
    PrimaryIndex appointmentprimaryindex = appointmentSystem.getAppointmentPrimaryIndex();
    int offset = appointmentprimaryindex.binarySearchPrimaryIndex(id);
    if (offset == -1) {
        cout << "Appointment with ID " << id << " not found.\n";
        return;
    }

    if (fields == "*" || fields == "all") {
        appointmentSystem.printAppointment(id);
    } else if (fields == "id" || fields == "ID") {
        appointmentSystem.printAppointmentId(id);
    } else if (fields == "date" || fields == "Date") {
        appointmentSystem.printAppointmentDate(id);
    } else if (fields == "doctor id" || fields == "doctor ID" || fields == "Doctor ID" || fields == "Doctor id") {
        appointmentSystem.printAppointmentDoctorid(id);
    } else {
        cout << "Invalid field for Appointment: " << fields << ".\n";
    }
}

void handleAppointmentByDate(const string& fields, const string& date, AppointmentManagementSystem& appointmentSystem) {
    vector<string> appointmentIds = appointmentSystem.searchByDate(date);
    if (appointmentIds.empty()) {
        cout << "No appointments found for date: " << date << ".\n";
        return;
    }

    for (const string& appointmentId : appointmentIds) {
        if (fields == "*" || fields == "all") {
            appointmentSystem.printAppointment(appointmentId);
        } else if (fields == "id" || fields == "ID") {
            appointmentSystem.printAppointmentId(appointmentId);
        } else if (fields == "date" || fields == "Date") {
            appointmentSystem.printAppointmentDate(appointmentId);
        } else if (fields == "doctor id" || fields == "doctor ID" || fields == "Doctor ID" || fields == "Doctor id") {
            appointmentSystem.printAppointmentDoctorid(appointmentId);
        } else {
            cout << "Invalid field for Appointment: " << fields << ".\n";
        }
    }
}

void handleAppointmentQuery(const string& fields, const string& condition, AppointmentManagementSystem& appointmentSystem) {
    if (condition.empty()) {
        handleAppointmentNoCondition(fields, appointmentSystem);
        return;
    }
    string key, value;
    if (!parseCondition(condition, key, value)) {
        cout << "Invalid WHERE condition. Use the format: <key>=<value>.\n";
        return;
    }

    if (key == "id" || key == "ID") {
        handleAppointmentById(fields, value, appointmentSystem);
    } else if (key == "date" || key == "Date") {
        handleAppointmentByDate(fields, value, appointmentSystem);
    } else {
        cout << "Invalid WHERE condition. Valid keys for Appointment are 'id' or 'Date'.\n";
    }
}

    void handleDoctorQuery(const string& fields, const string& condition, DoctorManagementSystem& doctorSystem) {

        if (condition.empty()) {
            handleDoctorNoCondition(fields, doctorSystem);
            return;
        }

        string key, value;
        if (!parseCondition(condition, key, value)) {
            cout << "Invalid WHERE condition. Use the format: <key>=<value>.\n";
            return;
        }

        if (key == "id" || key == "ID") {
            handleDoctorById(fields, value, doctorSystem);
        } else if (key == "name" || key == "Name") {
            handleDoctorByName(fields, value, doctorSystem);
        } else {
            cout << "Invalid WHERE condition. Valid keys for Doctor are 'ID' or 'Name'.\n";
        }
    }

    void handleUserQuery(DoctorManagementSystem& doctorSystem, AppointmentManagementSystem& appointmentSystem) {
        cout << "Enter your query: ";
        cin.ignore(); // Clear input buffer
        string query;
        getline(cin, query);
        // Trim and validate query format
        trim(query);
        transform(query.begin(), query.end(), query.begin(),::tolower);
        if (query.substr(0, 6) != "select" || query.find("from") == string::npos) {
            cout << "Invalid query format. Please use: SELECT <fields> FROM <table> WHERE <condition>;\n";
            return;
        }

        // Parse query
        size_t selectPos = query.find("select");
        size_t fromPos = query.find("from");
        size_t wherePos = query.find("where");

        string fields = query.substr(selectPos + 6, fromPos - (selectPos + 6));
        string table = query.substr(fromPos + 4, (wherePos == string::npos ? query.size() : wherePos) - (fromPos + 4));
        string condition = (wherePos != string::npos) ? query.substr(wherePos + 5) : ""; // Remove semicolon

        trim(fields);
        trim(table);
        trim(condition);

        if (table == "doctors" || table == "Doctors") {
            handleDoctorQuery(fields, condition, doctorSystem);
        } else if (table == "appointments" || table == "Appointments") {
            handleAppointmentQuery(fields, condition, appointmentSystem);
        } else {
            cout << "Invalid table name. Only 'doctors' and 'appointments' are supported.\n";
        }
    }



#endif //QUERYHANDLER_H
