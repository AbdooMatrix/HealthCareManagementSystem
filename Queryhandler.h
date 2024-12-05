#ifndef QUERYHANDLER_H
#define QUERYHANDLER_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include "DoctorManagementSystem.h"
#include "AppointmentManagementSystem.h"
#include "PrimaryIndex.h"

using namespace std;

class QueryHandler {
public:
    QueryHandler(DoctorManagementSystem &doctorSys, AppointmentManagementSystem &appointmentSys)
            : doctorSystem(doctorSys), appointmentSystem(appointmentSys) {}

    void handleUserQuery() {
        cout << "Enter your query: ";
        cin.ignore();
        string query;
        getline(cin, query);

        trim(query);
        transform(query.begin(), query.end(), query.begin(), ::tolower);

        if (query.substr(0, 6) != "select" || query.find("from") == string::npos) {
            cout << "Invalid query format. Please use: SELECT <fields> FROM <table> WHERE <condition>;\n";
            return;
        }

        size_t selectPos = query.find("select");
        size_t fromPos = query.find("from");
        size_t wherePos = query.find("where");

        string fields = query.substr(selectPos + 6, fromPos - (selectPos + 6));
        string table = query.substr(fromPos + 4, (wherePos == string::npos ? query.size() : wherePos) - (fromPos + 4));
        string condition = (wherePos != string::npos) ? query.substr(wherePos + 5) : "";

        trim(fields);
        trim(table);
        trim(condition);

        if (table == "doctors") {
            handleDoctorQuery(fields, condition);
        } else if (table == "appointments") {
            handleAppointmentQuery(fields, condition);
        } else {
            cout << "Invalid table name. Only 'doctors' and 'appointments' are supported.\n";
        }
    }

private:
    DoctorManagementSystem &doctorSystem;
    AppointmentManagementSystem &appointmentSystem;

    void trim(string &str) {
        if (str.empty()) {
            return; // Handle empty string case
        }

        int i = 0, j = str.size() - 1;

        // Move i forward until the first non-space character
        while (i <= j && str[i] == ' ') {
            i++;
        }

        // Move j backward until the last non-space character
        while (j >= i && str[j] == ' ') {
            j--;
        }

        // Update the string only if trimming is required
        if (i != 0 || (j != str.size() - 1)) {
            str = str.substr(i, j - i + 1);
        }
    }

    bool parseCondition(const string &condition, string &key, string &value) {
        size_t eqPos = condition.find('=');
        if (eqPos == string::npos) return false;

        key = condition.substr(0, eqPos);
        value = condition.substr(eqPos + 1);
        trim(key);
        trim(value);

        if (!value.empty() && value.front() == '\'' && value.back() == '\'') {
            value = value.substr(1, value.size() - 2);
        }
        return true;
    }

    void handleDoctorQuery(const string &fields, const string &condition) {
        if (condition.empty()) {
            handleDoctorNoCondition(fields);
            return;
        }

        string key, value;
        if (!parseCondition(condition, key, value)) {
            cout << "Invalid WHERE condition. Use the format: <key>=<value>.\n";
            return;
        }

        if (key == "id") {
            handleDoctorById(fields, value);
        } else if (key == "name") {
            handleDoctorByName(fields, value);
        } else {
            cout << "Invalid WHERE condition. Valid keys for Doctor are 'ID' or 'Name'.\n";
        }
    }

    void handleDoctorNoCondition(const string &fields) {
        if (fields == "*" || fields == "all") {
            doctorSystem.printAllDoctors(0);
        } else if (fields == "id") {
            doctorSystem.printAllDoctors(1);
        } else if (fields == "name") {
            doctorSystem.printAllDoctors(2);
        } else if (fields == "address") {
            doctorSystem.printAllDoctors(3);
        } else {
            cout << "Invalid field in SELECT query for Doctor.\n";
        }
    }

    void handleDoctorById(const string &fields, const string &id) {
        if (fields == "*" || fields == "all") {
            doctorSystem.printDoctorById(id, 0);  // Assuming this prints all doctor info
        } else if (fields == "id") {
            doctorSystem.printDoctorById(id, 1);  // Print only ID
        } else if (fields == "name") {
            doctorSystem.printDoctorById(id, 2);  // Print only Name
        } else if (fields == "address") {
            doctorSystem.printDoctorById(id, 3);  // Print only Address
        } else {
            cout << "Invalid field for Doctor: " << fields << ".\n";
        }
    }

    void handleDoctorByName(const string &fields, const string &name) {
        vector<string> doctorIds = doctorSystem.searchDoctorsByName(name);
        if (doctorIds.empty()) {
            cout << "No doctors found with name: " << name << ".\n";
            return;
        }

        for (const string &doctorId : doctorIds) {
            if (fields == "*" || fields == "all") {
                doctorSystem.printDoctorByName(doctorId, 0);
            } else if (fields == "id") {
                doctorSystem.printDoctorByName(doctorId, 1);
            } else if (fields == "name") {
                doctorSystem.printDoctorByName(doctorId, 2);
            } else if (fields == "address") {
                doctorSystem.printDoctorByName(doctorId, 3);
            } else {
                cout << "Invalid field for Doctor: " << fields << ".\n";
            }
        }
    }

    void handleAppointmentQuery(const string &fields, const string &condition) {
        if (condition.empty()) {
            handleAppointmentNoCondition(fields);
            return;
        }

        string key, value;
        if (!parseCondition(condition, key, value)) {
            cout << "Invalid WHERE condition for Appointment.\n";
            return;
        }

        if (key == "id") {
            handleAppointmentById(fields, value);
        } else if (key == "name") {
            handleAppointmentByDoctorId(fields, value);
        } else {
            cout << "Invalid WHERE condition. Valid keys for Appointment are 'id'.\n";
        }
    }

    void handleAppointmentNoCondition(const string &fields) {
        if (fields == "*" || fields == "all") {
            appointmentSystem.printAllAppointments(0);
        } else if (fields == "id") {
            appointmentSystem.printAllAppointments(1);
        } else if (fields == "date") {
            appointmentSystem.printAllAppointments(2);
        } else if (fields == "doctor_id") {
            appointmentSystem.printAllAppointments(3);
        } else {
            cout << "Invalid field in SELECT query for Appointment.\n";
        }
    }

    void handleAppointmentById(const string &fields, const string &id) {
        PrimaryIndex appointmentPrimaryIndex = appointmentSystem.getAppointmentPrimaryIndex();
        int offset = appointmentPrimaryIndex.binarySearchPrimaryIndex(id);
        if (offset == -1) {
            cout << "Appointment with ID " << id << " not found.\n";
            return;
        }

        if (fields == "*" || fields == "all") {
            appointmentSystem.printAppointmentById(id, 0);
        } else if (fields == "id") {
            appointmentSystem.printAppointmentById(id, 1);
        } else if (fields == "date") {
            appointmentSystem.printAppointmentById(id, 2);
        } else if (fields == "doctor id") {
            appointmentSystem.printAppointmentById(id, 3);
        } else {
            cout << "Invalid field for Appointment: " << fields << ".\n";
        }
    }

    void handleAppointmentByDoctorId(const string &fields, const string &doctorId) {
        vector<string> appointmentIds = appointmentSystem.searchAppointmentsByDoctorID(doctorId); // Assuming a method to search by doctor ID
        if (appointmentIds.empty()) {
            cout << "No appointments found for Doctor ID: " << doctorId << ".\n";
            return;
        }

        for (const string &appointmentId : appointmentIds) {
            if (fields == "*" || fields == "all") {
                appointmentSystem.printAppointmentById(appointmentId, 0);
            } else if (fields == "id") {
                appointmentSystem.printAppointmentById(appointmentId, 1);
            } else if (fields == "date") {
                appointmentSystem.printAppointmentById(appointmentId, 2);
            } else if (fields == "doctor_id") {
                appointmentSystem.printAppointmentById(appointmentId, 3);
            } else {
                cout << "Invalid field for Appointment: " << fields << ".\n";
            }
        }
    }
};

#endif // QUERYHANDLER_H
