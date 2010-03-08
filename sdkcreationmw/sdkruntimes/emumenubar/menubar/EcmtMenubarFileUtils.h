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


#ifndef _ECMT_MENUBAR_FILE_UTILS_H_
#define _ECMT_MENUBAR_FILE_UTILS_H_

#include <f32file.h>
#include "EcmtMenubarUtils.h"

/**
 * Collection of static file-related utilities. Included from Symbian part
 * of EcmtMenubar. 
 */
class EcmtMenubarFileUtils: public EcmtMenubarUtils
{
public:

    /**
     * Extracts the file part from the path.
     */
    static TPtrC ExtractFilePart(const TDesC& aFileName);

    /**
     * Returns file extension WITHOUT the dot, for example "txt" for "foo.txt"
     * If the file name has no extension, returns empty descriptor.
     */
    static TPtrC FileExt(const TDesC& aFileName);

    /**
     * Reads a file from the external (i.e. Windows) file system into memory.
     * Returns NULL if file does not exist, I/O error occurs or in case 
     * of out-of-memory condition.
     */
    static HBufC8* ReadExtFile(const TDesC& aExtFile);

    /**
     * Reads a file from the external (i.e. Windows) file system into memory.
     * Leaves if file does not exist, I/O error occurs or in case of 
     * out-of-memory condition. Pushes the returned data to the cleanup
     * stack.
     */
    static HBufC8* ReadExtFileLC(const TDesC& aExtFile);

    /**
     * Copies file from the external (i.e. Windows) file system into the
     * emulator's Symbian file system. Returns Symbian-style error code,
     * KErrNone on success.
     */
    static TInt CopyExtFileToEpocFs(const TDesC& aExtFile, RFs& aFs, 
                                    const TDesC& aEpocFile);

    /**
     * Copies file from the external (i.e. Windows) file system into the
     * Symbian temporary folder. Leaves on failure. Returns Symbian path
     * to the created file. The return value is zero terminated.
     */
    static HBufC* CopyExtFileToEpocTempDirL(const TDesC& aExtFile, RFs& aFs);

    /**
     * Copies file from the external (i.e. Windows) file system into the
     * Symbian temporary folder. Leaves on failure. Returns Symbian path
     * to the created file. The return value is pushed to the cleanup stack.
     * The returned string is zero terminated.
     */
    static HBufC* CopyExtFileToEpocTempDirLC(const TDesC& aExtFile, RFs& aFs);

    /**
     * Extract JAR name from JAD file
     */
    static HBufC* JarFileNameFromJadL(const TDesC& aFileName);

private:

    /**
     * Closes a Windows handle. Used for cleanup.
     */
    static void CleanupCloseHandle(TAny* aPtr);

    /**
     * Deallocates pointer with MEM_Free. Used for cleanup.
     */
    static void CleanupMemFree(TAny* aPtr);

    /**
     * Destroys Prop object. Used for cleanup.
     */
    static void CleanupDestroyProp(TAny* aPtr);
};
    
#endif	// _ECMT_MENUBAR_FILE_UTILS_H_

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
