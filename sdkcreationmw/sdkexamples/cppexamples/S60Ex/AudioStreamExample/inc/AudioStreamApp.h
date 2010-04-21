/*
* Copyright (c) 2006 - 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Declares the main application class for AudioStream application
*
*/


#ifndef AUDIOSTREAMAPP_H
#define AUDIOSTREAMAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS
// UID of the application


const TUid KUidAudioStream = { 0xE01FF1C3 };


// CLASS DECLARATION

/**
* CAudioStreamApp application class.
* Provides factory to create concrete document object.
* 
*/
class CAudioStreamApp : public CAknApplication
    {
    
    public: // Functions from base classes
    private:

        /**
        * From CApaApplication, creates CAudioStreamDocument document object.
        * return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * From CApaApplication, returns application's UID (KUidAudioStream).
        * return The value of KUidAudioStream.
        */
        TUid AppDllUid() const;
    };

#endif

// End of File

