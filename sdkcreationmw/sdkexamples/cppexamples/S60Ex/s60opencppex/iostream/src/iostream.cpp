/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


//  Include Files  

#include "iostream.h"
#include <fstream>
#include <iostream>
#include <string>

// This is a GCCE toolchain workaround needed when compiling with GCCE
// and using main() entry point
#ifdef __GCCE__
#include <staticlibinit_gcce.h>
#endif

using namespace std;

int entering()
{
	char FileName[20];
	char EmployeeName[40], Address[50], City[20], State[32], ZIPCode[10];

    cout << "Enter the Following pieces of information\n";
	cout << "Empl Name: "; cin >> ws;
    cin.getline(EmployeeName, 40);
    cout << "Address:   "; cin >> ws;
    cin.getline(Address, 50);
    cout << "City:      "; cin >> ws;
    cin.getline(City, 20);
    cout << "State:     "; cin >> ws;
    cin.getline(State, 32);
    cout << "ZIP Code:  "; cin >> ws;
    cin.getline(ZIPCode, 10);

    cout << "\nEnter the name of the file you want to create: ";
    cin >> FileName;
    
    ifstream EmplRecords(FileName);
     	
     if(EmplRecords.is_open())
     {
     	cout<<"file name already exits\n";	
     }
     else
     {
   	 ofstream EmplRecords(FileName, ios::out);
     EmplRecords << EmployeeName << "\n" << Address << "\n" << City << "\n" << State << "\n" << ZIPCode;
     }
     
     EmplRecords.close();
	return 0;
}

int reading()
{
	char FileName[20];
	char EmployeeName[40], Address[50], City[20], State[32], ZIPCode[10];

	cout << "Enter the name of the file you want to open: ";
    cin >> FileName;
    ifstream EmplRecords(FileName);
	if(EmplRecords.is_open())
	{
    EmplRecords.getline(EmployeeName, 40, '\n');
    EmplRecords.getline(Address, 50);
    EmplRecords.getline(City, 20);
    EmplRecords.getline(State, 32);
    EmplRecords.getline(ZIPCode, 10);

    cout << "\n -=- Employee Information -=-";
    cout << "\nEmpl Name: " << EmployeeName;
    cout << "\nAddress:   " << Address;
    cout << "\nCity:      " << City;
    cout << "\nState:     " << State;
    cout << "\nZIP Code:  " << ZIPCode;
	}
	else
	{
		cout<<"file not found\n";
	}
	
	EmplRecords.close();
    return 0;
}


int main()
{
	char select;
	char opt;
	while(1)
	{

	cout<<"---=----WELCOME TO NOKIA----=---\n ";
	cout<<"================================\n";
	cout<<"1. To create the profile of new Employees\n";
	cout<<"2. To see the details of Employee\n";
	cout<<"3. To exit\n";
	
	cout<<"select an option\n";
	cin>>select;
		
	switch(select)
	{
		case '1' :
							entering();
							break;
		case '2' : 			reading();
							break;
							
		case '3' :           exit(0);					
		
		default :   cout<<"wrong option\n";
				
	}
	
		cout<<"\nWant to continue "<<"press y";
		cout<<"\nOR Press any other key to exit\n";
		cin>>opt;
		
		if( opt=='Y' || opt=='y')
				continue;
		else
			break;
	}
	
	return 0;
}
