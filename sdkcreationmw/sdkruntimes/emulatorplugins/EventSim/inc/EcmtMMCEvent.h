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
* Description:  Utility class for handling MMC related events
*
*/


#ifndef _ECMTMMCEVENT_H
#define _ECMTMMCEVENT_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h> 

//CONSTANTS
//Pluging Messaging constants
_LIT8(KMMCEvent,     "MMC");
_LIT8(KBlank,        " ");
_LIT8(KMMCRemove,    "REMOVE");
_LIT8(KMMCInsert,    "INSERT");
_LIT8(KMMCCorrupted, "CORRUPTED");
_LIT8(KWINSDrives,   "WINS_DRIVES");
_LIT8(KEMUDrive,     "ED");
_LIT8(KSet,          "SET");
_LIT8(KWhat,         "WHAT");
_LIT8(KVirtualMMC,   "VIRTUAL");
_LIT8(KNotSet, 	 	 "NOT_SET");
//configuration line in epoc.ini
_LIT8( KConfiguration, "configuration" );
_LIT8( KEpocLocDrv, "_EPOC_LocDrv_1" );
_LIT8( KEpocDriveE, "_EPOC_DRIVE_E" );
_LIT8( KCrLf, "\r\n" );
//Drive letter for MMC card this (magic number is from series 60 MMC Application)
_LIT16(KMMCDrive16, "E:");
_LIT8(KMMCDrive8, "E:");
//NameOfThis module for use in e.g. panic
_LIT(KEcmtMMCEvent, "EcmtMMCEvent Plugin");

// MACROS
//none

// DATA TYPES
//none

// FUNCTION PROTOTYPES
//none

// FORWARD DECLARATIONS
class CEventSimPlugin;
class CRemovableDriveList;

// CLASS DECLARATION

/**
 *  Utility class for handling MMC related events
 *
 *  @lib EcmtEventSimPlugin.lib
 *  @since 
 *
 * MMCEvent Message
 * 
 * <mmc><blank><mmc-value>
 * Where:
 * <mmc>=MMC
 * <blank>= ' '
 * <mmc-value>= REMOVE or INSERT
 *
 * Status message
 * <mmc><blank><mmc-value>
 * Where:
 * <mmc-value>= REMOVE or INSERT or CORRUPTED
 *
 * Requesting WINS removable drives
 * <mmc><blank><wins_drives>
 * Reply message to this is 
 * <mmc><blank><wins_drives><blank><drives>
 * Where:
 * <wins_drives>=WINS_DRIVES
 * <blank>=" "
 * <drives>= e.g. ">A: D: Q: "
 *
 * Requesting and setting the Emulated Drive letter
 * Set:
 * <mmc><blank><emul_drive><blank><set><blank><drive>
 * Request:
 * <mmc><blank><emul_drive><blank><what>
 * Reply message to this is
 * <mmc><blank><emul_drive><blank><drive>
 * Where:
 * <emul_drive>=ED
 * <set>=SET
 * <what>=WHAT
 * <drive>= e.g. E: or VIRTUAL
*/

class CEcmtMMCEvent : public CBase
{
  
public:
    /**
    * CEcmtMMCEvent::NewL
    * Symbian Two-phased constructor.
    *
    */
    static CEcmtMMCEvent* NewL();
  
    /**
    * CEcmtMMCEvent::~CEcmtMMCEvent();
    * C++ destructor
    *
    */
    virtual ~CEcmtMMCEvent();

private:
  
    /**
    * CEcmtMMCEvent::CEcmtMMCEvent
    * C++ default constructor can NOT contain any code, that
    * might leave.
    *
    */  
    CEcmtMMCEvent();
  
    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();
  
public: 
  
    /**
    * Set MMC card seem to be as removed
    * @since
    */ 
    void OpenMMCDoor();
    
    /**
    * Set MMC card seem to be as inserted
    * @since
    */ 
    void CloseMMCDoor();
    
    /**
    * Get number removable drives
    * @since
    */ 
    TInt NumOfWinsRemovableDrives();
    
    /**
    * Get descriptor containing removable drives
    * @since
    * @param aBuff Contains removable drive letters 
    */ 
    void WinsRemovableDrivesL( TDes8& aBuff );
    
    /**
    * Get MMC disk status 
    * @since
    * @param aStatus MMC disk status
    */ 
    void GetMMCStatus( TDes8& aStatus ) const;
    
    /**
    * Handle all MMC events
    * @since
    * @param aParams Parameters for MMC event
    */ 
    void SetEventL( TDesC& aParams );
    
    /**
    * Send MMC Status to EPDTHandle all MMC events
    * @since
    */ 
    HBufC8* SendMMCStatusLC();


private:

    // connection to filesystem
    RFs   iFs;
    
    // List of removable drives
    CRemovableDriveList* iWinsRemDrives;
};
#endif // _ECMTMMCEVENT_H

// End of File