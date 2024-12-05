#include <bits/stdc++.h>
#include "DoctorManagementSystem.h"
#include "AppointmentManagementSystem.h"
#include "QueryHandler.h"

using namespace std;

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
    if (i != 0 || (j != str.size() - 1) ) {
        str = str.substr(i, j - i + 1);
    }
}

void checkContinue() {
    char cont = '0';
    while (tolower(cont) != 'y') {
        cout << "Press 'y' or 'Y' to continue: ";
        cin >> cont;
    }
}

string padInt(int x) {
    // Pad the ID with leading zeros to make it 2 characters long
    string paddedInt = to_string(x);
    if (paddedInt.length() < 2) {
        paddedInt.insert(0, 1, '0');
    }
    return paddedInt;
}

int main() {

    cout << "Welcome to Your Health Care Management System\n";
    int choice = -1;

    DoctorManagementSystem doctorSystem;
    AppointmentManagementSystem appointmentSystem;
    QueryHandler queryHandler(doctorSystem, appointmentSystem);

    while (choice != 0) {
        cout <<
             "1) Add New Doctor\n"
             "2) Add New Appointment\n"
             "3) Update Doctor Name (Doctor ID)\n"
             "4) Update Appointment Date (Appointment ID)\n"
             "5) Delete Appointment (Appointment ID)\n"
             "6) Delete Doctor (Doctor ID)\n"
             "7) Print Doctor Info (Doctor ID)\n"
             "8) Print Appointment Info (Appointment ID)\n"
             "9) Write Query\n"
             "10) Print all doctors\n"
             "0) Exit\n"
             "Enter a choice: ";
        cin >> choice;

        if (choice == 1) {
            Doctor doctor;
            string name, address;
            cout << "Enter doctor name: ";
            cin.ignore();
            getline(cin, name);

            cout << "Enter doctor address: ";
            getline(cin, address);
            trim(name);
            trim(address);
            doctor.name = name;
            doctor.address = address;

            doctorSystem.addDoctor(doctor);
            checkContinue();

        }
        else if (choice == 2) {
            Appointment appointment;
            string date;
            int doctorID;
            cout << "Enter the date: ";
            cin.ignore();
            getline(cin, date);

            cout << "Enter doctor ID: ";
            cin >> doctorID;
            string paddedDoctorId = padInt(doctorID);
            trim(date);
            appointment.date = date;
            appointment.doctorID = paddedDoctorId;

            appointmentSystem.addAppointment(appointment);
            checkContinue();
        }
        else if (choice == 3) {
            // Code for updating doctor name
            int id;
            cout << "Please enter the Doctor's ID you want to change his name: ";
            cin >> id;

            // Pad the ID with leading zeros to make it 2 characters long
            string paddedId = padInt(id);

            string newName;
            cout << "Please enter Doctor's new name: ";
            cin.ignore(); // Clear input buffer
            getline(cin, newName);

            // Assume trim is a utility function you defined elsewhere
            trim(newName);

            doctorSystem.updateDoctorName(paddedId, newName);
            checkContinue();
        }
        else if (choice == 4) {
            string id;
            cout << "Please enter the Appointment's ID you want to change his date: ";
            cin >> id;

            string newDate;
            cout << "Please enter new date: ";
            cin.ignore(); // Clear input buffer
            getline(cin, newDate);
            trim(newDate);

            appointmentSystem.updateAppointmentDate(id, newDate);
            checkContinue();
        }
        else if (choice == 5) {
            int id;
            cout << "Please enter the Appointment's ID you want to delete: ";
            cin >> id;

            // Pad the ID with leading zeros to make it 2 characters long
            string paddedId = padInt(id);

            appointmentSystem.deleteAppointment(paddedId);
            checkContinue();
        }
        else if (choice == 6) {
            int id;
            cout << "Please enter the Doctor's ID you want to delete: ";
            cin >> id;

            // Pad the ID with leading zeros to make it 2 characters long
            string paddedId = padInt(id);

            doctorSystem.deleteDoctor(paddedId);
            checkContinue();
        }
        else if (choice == 7) {
            int id;
            cout << "Please enter the Doctor's ID you want to search on: ";
            cin >> id;

            // Pad the ID with leading zeros to make it 2 characters long
            string paddedId = padInt(id);

            doctorSystem.printDoctorById(paddedId, 4);
            checkContinue();
        }
        else if (choice == 8) {
            int id;
            cout << "Please enter the Appointment's ID you want to search on: ";
            cin >> id;

            string paddedId = padInt(id);

            appointmentSystem.printAppointmentById(paddedId, 4);
            checkContinue();
        }
        else if (choice == 9) {
            cout << "Query Example: SELECT * FROM Doctors WHERE ID = '1';\n";
            queryHandler.handleUserQuery();
            checkContinue();
        }
        else if (choice == 10) {
            doctorSystem.printAllDoctors(0);
            checkContinue();
        }
        else {
            cout << "Enter a valid choice\n";
        }
    }

    cout << "End of program\n";
    return 0;
}
