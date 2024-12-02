#include <bits/stdc++.h>
#include "Doctor.h"
#include "DoctorManagementSystem.h"
#include "Appointment.h"
#include "AppointmentManagementSystem.h"
using namespace std;

void checkContinue() {
    char cont = '0';
    while (tolower(cont) != 'y')
    {
        cout << "Press 'y' or 'Y' to continue: ";
        cin >> cont;
    }
}

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

int main() {

    cout << "Welcome to Your Health Care Management System\n";
    int choice = -1;

    DoctorManagementSystem sys;
    AppointmentManagementSystem appoSys;

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
             "0) Exit\n"
             "Enter a choice: ";
        cin >> choice;

        if (choice == 1) {
            Doctor doctor;
            string name, address;
            cout << "Enter doctor name: ";
            cin.ignore() ;
            getline(cin, name);

            cout << "Enter doctor address: ";
            getline(cin, address);
            trim(name) ;
            trim(address) ;
            doctor.name = name;
            doctor.address = address;

            sys.addDoctor(doctor);
            checkContinue();
        }
        else if (choice == 2) {
            Appointment appointment;
            string date, doctorID;
            cout << "Enter the date: ";
            cin.ignore() ;
            getline(cin, date);

            cout << "Enter doctor ID: ";
            getline(cin, doctorID);
            trim(date) ;
            trim(doctorID) ;
            appointment.date = date;
            appointment.doctorID = doctorID;

            appoSys.addAppointment(appointment);
            checkContinue();
        }
        else if (choice == 3) {
            // Code for updating doctor name
            string id;
            cout << "Please enter the Doctor's ID you want to change his name: ";
            cin >> id;

            string newName;
            cout << "Please enter Doctor's new name: ";
            cin.ignore(); // Clear input buffer
            getline(cin, newName);
            trim(newName) ;

            sys.updateDoctorName(id , newName);
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
            trim(newDate) ;

            appoSys.updateAppointmentDate(id , newDate);
            checkContinue();
        }
        else if (choice == 5) {
            string id;
            cout << "Please enter the Appointment's ID you want to delete: ";
            cin >> id;
            appoSys.deleteAppointment(id);
            checkContinue();
        }
        else if (choice == 6) {
            string id;
            cout << "Please enter the Doctor's ID you want to delete: ";
            cin >> id;
            sys.deleteDoctor(id);
            checkContinue();
        }
        else if (choice == 7) {
            string id;
            cout << "Please enter the Doctor's ID you want to search on: ";
            cin >> id;
            sys.printDoctorInfo(id);
            checkContinue();
        }
        else if (choice == 8) {
            string id;
            cout << "Please enter the Appointment's ID you want to search on: ";
            cin >> id;
            appoSys.printAppointmentInfo(id);
            checkContinue();
        }
        else if (choice == 9) {
            // Code for writing query
        }
        else if (choice == 0) {
            // Exit condition
        }
        else {
            cout << "Enter a valid choice\n";
        }
    }

    cout << "End of program\n";
    return 0;
}
