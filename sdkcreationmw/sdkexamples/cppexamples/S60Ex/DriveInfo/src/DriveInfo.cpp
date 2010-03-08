/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
/** \file DriveInfo.cpp */

//
// Copyright (C) Symbian Software Ltd 2000-2006.  All rights reserved.
//

#include <f32file.h>
#include <bacline.h>
#include "CommonFramework.h"


LOCAL_D RFs fsSession;

// example functions

void PrintAllDrivesVolInfo();
void PrintDriveVolInfo( TPtrC &drive );

// utility functions

void PrintDriveInfo( TInt driveNumber );
void FormatDriveInfo(TDes& aBuffer, const TDriveInfo aDriveInfo);
void FormatVolumeInfo(TDes& aBuffer, const TVolumeInfo aVolumeInfo);

namespace
{
_LIT(KMessage,"Drive Info\n");
_LIT(KValidDriveMsg,"\nValid drives as names (and as numbers) are:\n");
_LIT(KDriveCharAndNum,"%c(%d) ");
_LIT(KNewLine,"\n");
_LIT(KAvailDriveMsg,"\nUsing DriveList(), available drives are:\n");
_LIT(KDriveAtts,"    %c: %02x\n");
} // empty namespace

void WaitForKey()
{
    _LIT(KPressAnyKey,"\n[Press any key to continue]\n");
    console->Printf(KPressAnyKey);
    console->Getch();
}

/** \fn void doExampleL()
 *
 * main-function of the DriveInfo console application
 *
 * checks commandline arguments and then displays drive information either
 * for all available drives or the ones specified in commandline
 */
LOCAL_C void doExampleL()
{
    CCommandLineArguments *cmdline = CCommandLineArguments::NewLC();

    // Connect to file server
    User::LeaveIfError(fsSession.Connect()); // Start session
    CleanupClosePushL( fsSession );

    TInt count = cmdline->Count();
    if (count > 1)
    {
        // skip commandline argument 0, because that is the name of the exe
        for (TInt i=1; i<count; ++i)
        {
            TPtrC arg = cmdline->Arg(i);
            PrintDriveVolInfo(arg);
        }
    }
    else // show all
    {
        PrintAllDrivesVolInfo();
    }

    CleanupStack::PopAndDestroy(2, cmdline); // cmdline, fsSession
}

/** \fn void PrintAllDrivesVolInfo()
 *
 * shows all valid drive identifications and available drives in current
 * device. Then drive and volume information are shown for each available
 * drive with the help of PrintDriveInfo()
 */
void PrintAllDrivesVolInfo()
{
    console->Printf(KMessage);

    // Print the valid drives as characters and as numbers.
    // Then print the drive list (list of available drives), and
    // information about each drive in the list using Drive().
    // Finally, print volume information.

    console->Printf(KValidDriveMsg);

    TInt driveNumber=EDriveA;
    TChar driveLetter;
    for (; driveNumber<=EDriveZ; ++driveNumber)
    {
        if (fsSession.IsValidDrive(driveNumber))
        {
            fsSession.DriveToChar(driveNumber,driveLetter);
            fsSession.CharToDrive(driveLetter, driveNumber);
            console->Printf(KDriveCharAndNum, TUint(driveLetter), driveNumber);
        }
    }
    console->Printf(KNewLine);

    // A TDriveList (the list of available drives), is an array of
    // 26 bytes. Each byte with a non zero value signifies that the
    // corresponding drive is available.
    //
    TDriveList drivelist;
    User::LeaveIfError(fsSession.DriveList(drivelist));
    console->Printf(KAvailDriveMsg);
    for (driveNumber=EDriveA; driveNumber<=EDriveZ; ++driveNumber)
    {
        // if drive-list entry non-zero, drive is available
        if (drivelist[driveNumber])
        {
            User::LeaveIfError(fsSession.DriveToChar(driveNumber,driveLetter));
            // The following line prints the drive letter followed by the hex value
            // of the integer indicating that drive's attributes
            console->Printf(KDriveAtts,TUint(driveLetter), drivelist[driveNumber]);
        }
    }
    console->Printf(KNewLine);
    WaitForKey();

    for (driveNumber=EDriveA; driveNumber<=EDriveZ; ++driveNumber)
    {
        PrintDriveInfo( driveNumber );
    }
}

#define ASCII_ZERO '0'

/** \fn void PrintDriveVolInfo( TPtrC &drive )
 *
 * Parses given commandline argument and checks, whether specified drive is
 * a valid one. Then drive and volume information are shown for the
 * drive with the help of PrintDriveInfo()
 * \param drive commandline argument
 */
void PrintDriveVolInfo( TPtrC &drive )
{
    _LIT(KInvalidDrive,"Drive %c is not available!\n");
    _LIT(KInvalidArgument,"\nInvalid argument: %S (not a drive letter or number!)\n\n");

    TInt driveNumber = 0;
    TChar driveLetter;
    if ((drive.Length() == 1) && (TChar(drive[0]).IsAlpha()))
    {
        driveLetter = drive[0];
        fsSession.CharToDrive(driveLetter, driveNumber);
    }
    else
    {
        // assumption: user gave a number as parameter.
        // no fancy parsing; just assume the parameter is ascii-number
        for (TInt i = 0; i<drive.Length(); ++i)
        {
            if (! TChar(drive[i]).IsDigit())
            {
                console->Printf(KInvalidArgument, &drive);
                return;
            }
            driveNumber *= 10;
            driveNumber += drive[i] - ASCII_ZERO;
        }
    }

    TDriveList drivelist;
    User::LeaveIfError(fsSession.DriveList(drivelist));

    // if drive-list entry non-zero, drive is available
    if (!drivelist[driveNumber])
    {
        console->Printf(KInvalidDrive);
        return;
    }

    PrintDriveInfo( driveNumber );
}

/** \fn void PrintDriveInfo( TInt driveNumber )
 *
 * shows drive and volume information of the specified drive
 * \param driveNumber drive specified as number
 */
void PrintDriveInfo( TInt driveNumber )
{
    _LIT(KDriveInfo,"\nDrive information for drive %c:\n%S");
    _LIT(KVolInfo,"\nVolume information for drive %c:\n%S");

    TChar driveLetter;

    // Print information about the drive
    TBuf<200> buffer;
    TDriveInfo driveInfo;
    fsSession.Drive(driveInfo, driveNumber);

    // test whether drive is available. If not, return
    if (driveInfo.iDriveAtt == KDriveAbsent)
    {
        return;
    }

    FormatDriveInfo(buffer, driveInfo);
    User::LeaveIfError(fsSession.DriveToChar(driveNumber, driveLetter));
    console->Printf(KDriveInfo, TUint(driveLetter), &buffer);
    buffer.Zero();
    WaitForKey();

    // Print volume information
    TVolumeInfo volumeInfo;
    TInt err=fsSession.Volume(volumeInfo, driveNumber);

    // Volume() returns KErrNotReady if no volume present.
    // In this case, check next drive number
    if (err != KErrNotReady)
    {
        buffer.Zero();
        FormatVolumeInfo(buffer, volumeInfo);
        console->Printf(KVolInfo, TUint(driveLetter), &buffer);
        WaitForKey();
    }
}

/** \fn void FormatDriveInfo(TDes& aBuffer, const TDriveInfo aDriveInfo)
 *
 * Interprets passed drive information to user readable form
 * \param aBuffer       the buffer into which formatted text is written
 * \param aDriveInfo   drive's information
 */
void FormatDriveInfo(TDes& aBuffer, const TDriveInfo aDriveInfo)
{
    // Append battery, media and drive information to aBuffer
    // Define descriptor constants using the _LIT macro

    _LIT(KFormatString,"iType=%02x\niBattery=%02x\niDriveAtt=%02x\niMediaAtt=%02x\n");
    _LIT(KBatLow,"Battery low\n");
    _LIT(KBatGood,"Battery good\n");
    _LIT(KBatNotSupported,"Battery not supported\n");
    _LIT(KNotPresent,"No media present\n");
    _LIT(KFloppy,"Media is floppy disk\n");
    _LIT(KHard,"Media is hard disk\n");
    _LIT(KCDROM,"Media is CD-ROM\n");
    _LIT(KRam,"Media is RAM\n");
    _LIT(KFlash,"Media is flash\n");
    _LIT(KRom,"Media is ROM\n");
    _LIT(KRemote,"Media is remote\n");
    _LIT(KNANDFlash,"Media is NAND flash\n");
    _LIT(KUnknown,"Media unknown\n");
    _LIT(KDriveAtts,"Drive attributes:");
    _LIT(KLocal," local");
    _LIT(KROMDrive," ROM");
    _LIT(KRedirected," redirected");
    _LIT(KSubstituted," substituted");
    _LIT(KInternal," internal");
    _LIT(KRemovable," removable");
    _LIT(KMediaAtts,"\nMedia attributes:");
    _LIT(KDynamic," dynamic");
    _LIT(KDual," dual-density");
    _LIT(KFormattable," formattable");
    _LIT(KLockable," lockable");
    _LIT(KLocked," locked");
    _LIT(KHasPassword," has password");
    _LIT(KWriteProtected," write-protected");
    _LIT(KNewLine,"\n");

//    aBuffer.AppendFormat(KFormatString, TInt(aDriveInfo.iType),
//        TInt(aDriveInfo.iBattery), TInt(aDriveInfo.iDriveAtt), TInt(aDriveInfo.iMediaAtt));

    aBuffer.AppendFormat(KFormatString, aDriveInfo.iType, aDriveInfo.iBattery,
                         aDriveInfo.iDriveAtt, aDriveInfo.iMediaAtt);
    switch (aDriveInfo.iType)
    {
        case EMediaNotPresent:
            aBuffer.Append(KNotPresent);
            break;
        case EMediaFloppy:
            aBuffer.Append(KFloppy);
            break;
        case EMediaHardDisk:
            aBuffer.Append(KHard);
            break;
        case EMediaCdRom:
            aBuffer.Append(KCDROM);
            break;
        case EMediaRam:
            aBuffer.Append(KRam);
            break;
        case EMediaFlash:
            aBuffer.Append(KFlash);
            break;
        case EMediaRom:
            aBuffer.Append(KRom);
            break;
        case EMediaRemote:
            aBuffer.Append(KRemote);
            break;
        case EMediaNANDFlash:
            aBuffer.Append(KNANDFlash);
            break;
        default:
            aBuffer.Append(KUnknown);
    }

    switch (aDriveInfo.iBattery)
    {
        case EBatLow:
            aBuffer.Append(KBatLow);
            break;
        case EBatGood:
            aBuffer.Append(KBatGood);
            break;
        default:
            aBuffer.Append(KBatNotSupported);
    }

    aBuffer.Append(KDriveAtts);
    if (aDriveInfo.iDriveAtt & KDriveAttLocal)
        aBuffer.Append(KLocal);
    if (aDriveInfo.iDriveAtt & KDriveAttRom)
        aBuffer.Append(KROMDrive);
    if (aDriveInfo.iDriveAtt & KDriveAttRedirected)
        aBuffer.Append(KRedirected);
    if (aDriveInfo.iDriveAtt & KDriveAttSubsted)
        aBuffer.Append(KSubstituted);
    if (aDriveInfo.iDriveAtt & KDriveAttInternal)
        aBuffer.Append(KInternal);
    if (aDriveInfo.iDriveAtt & KDriveAttRemovable)
        aBuffer.Append(KRemovable);

    aBuffer.Append(KMediaAtts);
    if (aDriveInfo.iMediaAtt & KMediaAttVariableSize)
        aBuffer.Append(KDynamic);
    if (aDriveInfo.iMediaAtt & KMediaAttDualDensity)
        aBuffer.Append(KDual);
    if (aDriveInfo.iMediaAtt & KMediaAttFormattable)
        aBuffer.Append(KFormattable);
    if (aDriveInfo.iMediaAtt & KMediaAttWriteProtected)
        aBuffer.Append(KWriteProtected);
    if (aDriveInfo.iMediaAtt & KMediaAttLockable)
        aBuffer.Append(KLockable);
    if (aDriveInfo.iMediaAtt & KMediaAttLocked)
        aBuffer.Append(KLocked);
    if (aDriveInfo.iMediaAtt & KMediaAttHasPassword)
        aBuffer.Append(KHasPassword);
    aBuffer.Append(KNewLine);
}

/** \fn void FormatVolumeInfo(TDes& aBuffer, const TVolumeInfo aVolumeInfo)
 *
 * Interprets passed volume information to user readable form
 * \param aBuffer       the buffer into which formatted text is written
 * \param aVolumeInfo   drive's volume information
 */
void FormatVolumeInfo(TDes& aBuffer, const TVolumeInfo aVolumeInfo)
{
    // Append volume information to aBuffer
    _LIT(KUID,      "Unique ID  : %08x\n");
    _LIT(KSize,     "Size       : %Ld bytes\n");
    _LIT(KFree,     "Free space : %Ld bytes\n");
    _LIT(KVolName,  "Volume name: %S\n");
    aBuffer.AppendFormat(KUID, aVolumeInfo.iUniqueID);
    aBuffer.AppendFormat(KSize, aVolumeInfo.iSize);
    aBuffer.AppendFormat(KFree, aVolumeInfo.iFree);
    aBuffer.AppendFormat(KVolName, &aVolumeInfo.iName);
}
