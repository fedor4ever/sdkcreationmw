/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __GUIENGINEAPPLICATION_H__
#define __GUIENGINEAPPLICATION_H__


// INCLUDES
#include <aknapp.h>


// CONSTANTS
// UID for the application; this should correspond to the uid defined in the mmp file
const TUid KUidGuiEngineUi =  { 0xA00001F5 };
const TUid KUidEngineDll = { 0xA00001F6 };


// CLASS DECLARATION

/**
* CGuiEngineApplication
* An instance of CGuiEngineApplication is the application part of the CKON
* application framework for the GuiEngine example application
*/

class CGuiEngineApplication : public CAknApplication
    {
    public: // from CAknApplication

    public:  // from CAknApplication

        /**
        * From CApaApplication,
        * Return the application UID value
        * @return The Uid
        */
        TUid AppDllUid() const;

  	public:

  		/**
  		* Returns the Uid of the application engine. Needed
  		* when storing and receiving the model data..
  		* @return The Uid of the application engine.
  		*/
  		TUid EngineDllUid() const;


    protected:  // from CAknApplication

        /**
        * From CApaApplication, CreateDocumentL.
        * Create a CApaDocument object and return a pointer to it
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
    };

#endif // __GUIENGINEAPPLICATION_H__


// End of File
