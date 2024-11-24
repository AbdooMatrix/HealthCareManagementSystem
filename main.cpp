#include <bits/stdc++.h>
#include"Doctor.h"
#include <cstring>
#include"doctorFunctions.h"
#include"appointmentFunctions.h"
using namespace std;




int main() {
    cout << "Welcome to Your Health Care Management System\n" ;
    int choice = -1;


    while(choice != 0){
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
                "enter a choice :";
        cin >> choice ;

        Doctor doctor ;
        string name , address ;


        switch (choice) {
            case 1:
                cin.ignore() ;

                cout << "enter doctor name :" ;
                getline(cin , name) ;

                cout << "enter doctor address :" ;
                getline(cin , address) ;

                char n[30] ;
                char a[30] ;
                for(int i = 0 ; i < name.length() ; ++i){n[i] = name[i] ;}
                for(int i = 0 ; i < address.length() ; ++i){a[i] = address[i] ;}
                doctor.setName(n) ;
                doctor.setAddresss(a) ;
                AddDoctor(doctor) ;
                break;
            case 2:
                break;
            case 3:

                break;
            case 4:

                break;
            case 5:

                break;
            case 6:

                break;
            case 7:

                break;
            case 8:

                break;
            case 9:

                break;
            case 0:
                break;
            default :
                cout << "please enter a valid choice \n" ;
                break ;
        }
    }

    cout << "end of program\n" ;
    return 0;
}
