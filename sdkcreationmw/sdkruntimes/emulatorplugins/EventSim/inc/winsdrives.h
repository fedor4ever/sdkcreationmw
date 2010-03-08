/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class to handle wins drives
*
*/


#ifdef __WINS__

#ifndef _WINDRIVES_H_
#define _WINDRIVES_H_

// INCLUDES
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

// CONSTANTS
//none

// MACROS
//none

// DATA TYPES
//none

// FUNCTION PROTOTYPES
//none

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Provider of removable drives
*
*  @lib EcmtEventSimPlugin.lib
*  @since 
*/
class CRemovableDriveList
{

/**
*  Drive node
*
*  @lib EcmtEventSimPlugin.lib
*  @since 
*/
class Drive
{
public:
    /**
    * Constructor
    * @since 
    */  
    Drive();
    
    // Next drive
    Drive* next;
    
    // Name of the drive
    char* Name;
};

public:
    /**
    * Constructor
    * @since 
    */  
    CRemovableDriveList();
    
    /**
    * Destructor
    * @since 
    */ 
    ~CRemovableDriveList();
    
    /**
    * Get number of drives
    * @since 
    * @return int Number of drives
    */  
    int NumOfDrives();
    
    /**
    * Update removable drives
    * @since 
    * @return int
    */  
    int UpdateRemDrives();
    
    /**
    * Get removable drives
    * @since 
    * @param aDrives Removable drives 
    * @return int
    */  
    int GetRemovableDrives( char* aDrives );

private:

    /**
    * Get removable drives
    * @since 
    * @return int
    */ 
    int RemovableDrives();
    
    /**
    * Reset removable drives list
    * @since 
    * @return void
    */ 
    void DeleteList();
    
    /**
    * Add drive to removable drives list
    * @since 
    * @return int
    */ 
    int AddDrive(char* aDrive);
    
private:
    
    Drive* nListOfDrives;
    int numOfRemovableDrives;
};

#endif //_WINDRIVES_H_
#endif //__WINS__

// End of File