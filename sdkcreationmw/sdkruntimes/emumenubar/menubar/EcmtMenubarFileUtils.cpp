/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Menubar utility finctions
*
*/


#include "EcmtMenubarFileUtils.h"
#include "EcmtWin32.h"
#include <emulator.h>
#include <utf.h>

// Allow for macro expansion
#define __S(x) _S(x)

// --------------------------------------------------------------------------
// Opens a Windows file. Leaves if something is wrong.
// --------------------------------------------------------------------------
static HANDLE OpenExtFileReadOnlyL(const TDesC& aExtFile)
{
    // Open the file. Make sure that the file name is NULL terminated.
    HBufC* name = HBufC::NewL(aExtFile.Length()+1);
    *name = aExtFile;
    HANDLE hFile = CreateFile(name->Des().PtrZ(), GENERIC_READ,
                              FILE_SHARE_READ, NULL, OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, 0);
    delete name;

    if (hFile == INVALID_HANDLE_VALUE)
    {
        User::Leave(Emulator::LastError());
    }
    return hFile;
}

// --------------------------------------------------------------------------
// Copies Windows file to a Symbian file
// --------------------------------------------------------------------------
static TInt CopyExtFileToEpoc(HANDLE aSrc, RFile& aDest)
{
    TInt err = KErrNone;
    TUint bytesLeft = GetFileSize(aSrc, NULL);
    while (bytesLeft && !err)
    {
        ULONG len = 0;
        TUint8 data[1024];
        ULONG bytesToRead = MIN(bytesLeft, sizeof(data));
        if (ReadFile(aSrc, data, bytesToRead, &len, NULL))
        {
            bytesLeft -= len;
            err = aDest.Write(TPtrC8(data, len));
        } else {
            err = KErrGeneral;
        }
    }
    return err;
}

// --------------------------------------------------------------------------
// EcmtMenubarFileUtils::ExtractFilePart
//
// Extracts the file part from the path.
// --------------------------------------------------------------------------
TPtrC EcmtMenubarFileUtils::ExtractFilePart(const TDesC& aFileName)
{
    TInt lastSlash = aFileName.LocateReverse('/');
    TInt lastBackSlash = aFileName.LocateReverse('\\');
    TInt lastSeparator = Max(lastSlash,lastBackSlash);
    if (lastSeparator >= 0)
    {
        return aFileName.Mid(lastSeparator+1);
    }
    else
    {
        return TPtrC(aFileName);
    }
}

// --------------------------------------------------------------------------
// EcmtMenubarFileUtils::FileExt
//
// Returns file extension WITHOUT the dot., for example "txt" for "foo.txt"
// If the file name has no extension, returns empty descriptor.
// --------------------------------------------------------------------------
TPtrC EcmtMenubarFileUtils::FileExt(const TDesC& aFileName)
{
    TInt dotPos = aFileName.LocateReverse('.');
    TInt extLen = (dotPos >= 0) ? (aFileName.Length() - dotPos - 1) : 0;
    return aFileName.Right(extLen);
}

// --------------------------------------------------------------------------
// EcmtMenubarFileUtils::ReadExtFile
//
// Reads a file from the external (i.e. Windows) file system into memory.
// Returns NULL if file does not exist, I/O error occurs or in case
// of out-of-memory condition.
// --------------------------------------------------------------------------
HBufC8* EcmtMenubarFileUtils::ReadExtFile(const TDesC& aExtFileName)
{
    HBufC8* contents = NULL;
    TRAP_IGNORE(contents = ReadExtFileLC(aExtFileName);
        CleanupStack::Pop(contents));
    return contents;
}

// --------------------------------------------------------------------------
// EcmtMenubarFileUtils::ReadExtFileLC
//
// Reads a file from the external (i.e. Windows) file system into memory.
// Leaves if file does not exist, I/O error occurs or in case of
// out-of-memory condition. Pushes the returned data to the cleanup
// stack.
// --------------------------------------------------------------------------
HBufC8* EcmtMenubarFileUtils::ReadExtFileLC(const TDesC& aExtFile)
{
    // Open the file
    HANDLE hFile = OpenExtFileReadOnlyL(aExtFile);

    // Allocate the buffer of exactl the right size
    TUint size = GetFileSize(hFile, NULL);
    HBufC8* contents = HBufC8::NewLC(size);
    ULONG len = 0;
    TPtr8 contentsPtr(contents->Des());
    contentsPtr.SetLength(size);

    // Read the file
    if (!ReadFile(hFile, &contentsPtr[0], size, &len, NULL))
    {
        User::Leave(Emulator::LastError());
    }

    // Close the file and return leaving data on the cleanup stack
    CloseHandle(hFile);
    contentsPtr.SetLength(len);
    return contents;
}

// --------------------------------------------------------------------------
// EcmtMenubarFileUtils::CopyExtFileToSymbianFs
//
// Copies file from the external (i.e. Windows) file system into the
// emulator's Symbian file system. Returns Symbian-style error code,
// KErrNone on success.
// --------------------------------------------------------------------------
TInt EcmtMenubarFileUtils::CopyExtFileToEpocFs(const TDesC& aExtFile,
                                               RFs& aFs,
                                               const TDesC& aEpocFile)
{
    HANDLE hFile;
    TRAPD(err, hFile = OpenExtFileReadOnlyL(aExtFile));
    if (hFile != INVALID_HANDLE_VALUE)
    {
        // Create a file in the Symbian filesystem.
        RFile f;
        err = f.Replace(aFs, aEpocFile, EFileWrite);
        if (err == KErrNone) {
            err = CopyExtFileToEpoc(hFile, f);
            f.Close();
        }

        // Close Windows file
        CloseHandle(hFile);
    }
    return err;
}

// --------------------------------------------------------------------------
// EcmtMenubarFileUtils::CopyExtFileToEpocTempDirLC
//
// Copies file from the external (i.e. Windows) file system into the
// Symbian temporary folder. Leaves on failure. Returns Symbian path
// to the created file. The return value is pushed to the cleanup stack.
// The returned string is zero terminated.
// --------------------------------------------------------------------------
HBufC* EcmtMenubarFileUtils::CopyExtFileToEpocTempDirLC(const TDesC& aExtFile,
                                                       RFs& aFs)
{
    HBufC* fname = CopyExtFileToEpocTempDirL(aExtFile, aFs);
    CleanupStack::PushL(fname);
    return fname;
}

// --------------------------------------------------------------------------
// EcmtMenubarFileUtils::CopyExtFileToEpocTempDirL
//
// Copies file from the external (i.e. Windows) file system into the
// Symbian temporary folder. Leaves on failure. Returns Symbian path
// to the created file. The returned string is zero terminated.
// --------------------------------------------------------------------------
HBufC* EcmtMenubarFileUtils::CopyExtFileToEpocTempDirL(const TDesC& aExtFile,
                                                       RFs& aFs)
{
    // Open the Windows file. Make sure that the file name is NULL terminated.
    HBufC* fname = HBufC::NewL(aExtFile.Length()+1);
    *fname = aExtFile;
    HANDLE hFile = Emulator::CreateFile(fname->Des().PtrZ(), GENERIC_READ,
                                        FILE_SHARE_READ, NULL, OPEN_EXISTING,
                                        FILE_ATTRIBUTE_NORMAL, 0);
    TInt err = KErrNotFound;
    if (hFile == INVALID_HANDLE_VALUE)
    {
        err = Emulator::LastError();
    }

    delete fname;
    fname = NULL;

    if (hFile != INVALID_HANDLE_VALUE)
    {
        CleanupStack::PushL(TCleanupItem(CleanupCloseHandle,hFile));
        TPtrC tempDir(__S(SDK_TEMP_DIR));
        err = aFs.MkDirAll(tempDir);
        if (err == KErrAlreadyExists) err = KErrNone;
        if (err == KErrNone)
        {
            // Allocate buffer for the file name
            TPtrC filePart(ExtractFilePart(aExtFile));
			// Alloc space for filename and path. 2 is for zeroterminate and
			// for modifying the file name (See below: if(err==KErrInUse) )
            fname = HBufC::NewLC(tempDir.Length() + filePart.Length() + 2);

            // Format the file name
            TPtr fnamePtr(fname->Des());
            fnamePtr.Copy(tempDir);
            fnamePtr.Append(filePart);
            fnamePtr.ZeroTerminate();

            // Create a new file in the Symbian temporary directory.
            RFile f;

            err = aFs.Delete(fnamePtr);

			// If file is already open, modify file name by adding a space
			// mark before the file name. This has to be done because file with
			// the same file name can be opened from different locations in a row.
			// In this situation S60 has reserved the first file, and it can not
			// be deleted. This is why we have to create a temporary file with
			// different file name and write into that. These files from the SDK
			// temp directory are going to be deleted on emulator startup.
				if( err == KErrInUse  )
					{
					_LIT(KFileSep," ");
					fnamePtr.Insert( tempDir.Length(), KFileSep() );
					fnamePtr.ZeroTerminate();
					err = aFs.Delete( fnamePtr );
					}

				err = f.Create(aFs, fnamePtr, EFileWrite);
	      User::LeaveIfError(err);
	
	      // Copy the data
	      CleanupClosePushL(f);
	      User::LeaveIfError(CopyExtFileToEpoc(hFile, f));
	
	      // Pop items off the cleanup stack and close handles
	      CleanupStack::PopAndDestroy(&f);
	      CleanupStack::Pop(fname);
	      CleanupStack::PopAndDestroy(hFile);
	      return fname;
	      }
    }
    else
    {
        RDebug::Printf("Ecmt menubar: opening Windows-file failed: %i", err);
    }
    User::Leave(err);
    return NULL; // NOT REACHED
}

// --------------------------------------------------------------------------
// EcmtMenubarFileUtils::JarFNameFromJadL
//
// Given Windows file JAD file name, return Windows JAR file name, NULL
// if JAD contains HTTP URL to the jar file, or any kind of failure
// --------------------------------------------------------------------------
HBufC* EcmtMenubarFileUtils::JarFileNameFromJadL(const TDesC& aFileName)
{
    Prop prop;
    HBufC* jarFileName = NULL;
    if (PROP_Init(&prop))
    {
        CleanupStack::PushL(TCleanupItem(CleanupDestroyProp,&prop));
        char* fname = STRING_ToMultiByteN(aFileName.Ptr(),aFileName.Length());
        User::LeaveIfNull(fname);
        CleanupStack::PushL(TCleanupItem(CleanupMemFree,fname));

        // Parse JAD file
        if (PROP_Load(&prop, fname, NULL))
        {
            Str jarURL = PROP_Get(&prop, "MIDlet-Jar-URL");
            if (!STRING_StartsWith(jarURL, "http://"))
            {
                // not a HTTP URL
                Str filePart = FILE_FilePart(fname);
                int jadLen = strlen(filePart);
                int jarLen = strlen(jarURL);
                int jarPathLen = (filePart - fname) + jarLen;
                if (jarLen > jadLen)
                {
                    // Reallocate buffer to make root for jar name
                    int newsize = strlen(fname) + (jarLen - jadLen) + 1;
                    fname = (char*)MEM_Realloc(fname, newsize);
                    User::LeaveIfNull(fname);

                    // Update pointer on the cleanup stack
                    CleanupStack::Pop(); // pop old fname
                    CleanupStack::PushL(TCleanupItem(CleanupMemFree,fname));
                }

                int pos = filePart - fname;
                strcpy(fname + pos, jarURL);
                if (FILE_Exist(fname))
                {
                    // Determine the length of the Unicode string
                    TPtr nothing(NULL,0);
                    TPtrC8 fnamePtr((TText8*)fname);
                    TInt unicodeLength = CnvUtfConverter::
                        ConvertToUnicodeFromUtf8(nothing, fnamePtr);

                    // Convert to Unicode
                    jarFileName = HBufC::NewL(unicodeLength+1);
                    TPtr des(jarFileName->Des());
                    CnvUtfConverter::ConvertToUnicodeFromUtf8(des, fnamePtr);
                    des.ZeroTerminate();
                }
            }
        }
        CleanupStack::PopAndDestroy(2);  // CleanupMemFree, CleanupDestroyProp
    }
    return jarFileName;
}

// --------------------------------------------------------------------------
// EcmtMenubarFileUtils::CleanupCloseHandle
// Cleanup support
// --------------------------------------------------------------------------
void EcmtMenubarFileUtils::CleanupCloseHandle(TAny* aPtr)
{
    CloseHandle(aPtr);
}

// --------------------------------------------------------------------------
// EcmtMenubarFileUtils::CleanupDestroyProp
// Cleanup support.
// --------------------------------------------------------------------------
void EcmtMenubarFileUtils::CleanupDestroyProp(TAny* aPtr)
{
    PROP_Destroy((Prop*)aPtr);
}

// --------------------------------------------------------------------------
// EcmtMenubarFileUtils::CleanupMemFree
// Cleanup support
// --------------------------------------------------------------------------
void EcmtMenubarFileUtils::CleanupMemFree(TAny* aPtr)
{
    MEM_Free(aPtr);
}

/**
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
