/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class to handle drives that are used by MMC events
*
*/


// INCLUDES
#include "winsdrives.h"

// ============================= LOCAL FUNCTIONS ===============================
// none

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRemovableDriveList::Drive::Drive
// Constructor
// -----------------------------------------------------------------------------
//
CRemovableDriveList::Drive::Drive()
{
	next = NULL;
	Name = NULL;
}

// -----------------------------------------------------------------------------
// CRemovableDriveList::CRemovableDriveList
// Constructor
// -----------------------------------------------------------------------------
//
CRemovableDriveList::CRemovableDriveList()
{
	numOfRemovableDrives = 0;
	nListOfDrives = NULL;
	RemovableDrives();
}

// -----------------------------------------------------------------------------
// CRemovableDriveList::~CRemovableDriveList
// Destructor
// -----------------------------------------------------------------------------
//
CRemovableDriveList::~CRemovableDriveList()
{
	// delete list
	DeleteList();
}

// -----------------------------------------------------------------------------
// CRemovableDriveList::DeleteList
// Delete list of removable drives
// -----------------------------------------------------------------------------
//
void CRemovableDriveList::DeleteList()
{
    numOfRemovableDrives = 0;
    while (nListOfDrives)
    {
        Drive* tmp = nListOfDrives->next; // store the next node pointer 
        delete [] nListOfDrives->Name;
        delete nListOfDrives;
        nListOfDrives = tmp;
    }
}

// -----------------------------------------------------------------------------
// CRemovableDriveList::AddDrive
// Add removable drive to list
// -----------------------------------------------------------------------------
//
int CRemovableDriveList::AddDrive( char* aDrive )
{
    Drive* newNode = new Drive();
    // Name of new node
    newNode->Name = new char[sizeof(char) * strlen( aDrive ) + 1];
    strcpy( newNode->Name, aDrive );
    if ( nListOfDrives == NULL )
    {
        nListOfDrives = newNode;
    }
    else
    {
        // add new element to the list 
        Drive* drive = nListOfDrives;
        while (drive->next) drive = drive->next;
        drive->next = newNode;
    }
    numOfRemovableDrives++;
    return 0;
}

// -----------------------------------------------------------------------------
// CRemovableDriveList::GetRemovableDrives
// Get removable drives
// -----------------------------------------------------------------------------
//
int CRemovableDriveList::GetRemovableDrives( char* aDrives )
{
    // copy drive names into a one string
    Drive* drive = nListOfDrives;
    aDrives[0] = 0;
    while (drive)
    {
        strcat( aDrives, drive->Name );
        drive = drive->next;
    }
    return strlen( aDrives );
}

// -----------------------------------------------------------------------------
// CRemovableDriveList::UpdateRemDrives
// Update removable drives
// -----------------------------------------------------------------------------
//
int CRemovableDriveList::UpdateRemDrives()
{
    // delete old list
    DeleteList();
    // update removable drives
    return RemovableDrives();
}

// -----------------------------------------------------------------------------
// CRemovableDriveList::NumOfDrives
// Get number of removable drives
// -----------------------------------------------------------------------------
//
int CRemovableDriveList::NumOfDrives()
{
	return numOfRemovableDrives;
}


// -----------------------------------------------------------------------------
// CRemovableDriveList::RemovableDrives
// 
// -----------------------------------------------------------------------------
//
int CRemovableDriveList::RemovableDrives()
{
	char szRoot[] = "X:\\"; //( ):\ needed
	char cDrive;
	DWORD dwDrivesOnSystem = GetLogicalDrives();
	UINT  uDriveType;

	// Check that there are drives
 	if(0 != dwDrivesOnSystem )
	{
    for ( cDrive = 'A'; cDrive <= 'Z'; cDrive++, dwDrivesOnSystem >>= 1 )
        {
        if ( !( dwDrivesOnSystem & 1 ) )
            continue;

        // Get the type for the next drive, and check dwFlags to determine
        // if we should show it in the list.

		szRoot[0] = cDrive;
        uDriveType = GetDriveTypeA ( szRoot );

        switch ( uDriveType )
            {
		    case DRIVE_REMOTE:
            case DRIVE_NO_ROOT_DIR:
            case DRIVE_UNKNOWN:
            case DRIVE_FIXED:
			case DRIVE_RAMDISK:
			case DRIVE_CDROM:
				break; // We are not interested of this type of drives
            case DRIVE_REMOVABLE:
                 // this is what we are interested of 
				AddDrive( szRoot );
				break;

            default:;
				// this is unreachable
            }
		}
	}
	return 0;

}
// End of File  
