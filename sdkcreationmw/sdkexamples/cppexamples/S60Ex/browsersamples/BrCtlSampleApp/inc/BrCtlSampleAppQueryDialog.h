/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Display a Query Dialog
*
*/


#ifndef BRCTLSAMPLEAPPQUERYDIALOG_H
#define BRCTLSAMPLEAPPQUERYDIALOG_H


//  INCLUDES

#include <eikdialg.h>
#include <aknmultilinequerycontrol.h>
#include <aknquerydialog.h>

// CLASS DECLARATION

/**
*  CBrCtlSampleAppQueryDialog class.
*  This class is used to query the user for text information
*/
class CBrCtlSampleAppQueryDialog : public CAknTextQueryDialog
    {
    public:  // Constructors and destructor
    
        /**
        * C++ default constructor.
	    * @param aDefInput Input into the dialog
	    * @param aRetBuf Return buffer
        */
	    CBrCtlSampleAppQueryDialog( TDes& aDefInput, HBufC*& aRetBuf );

        /**
        * Destructor.
        */
        virtual ~CBrCtlSampleAppQueryDialog() {};

    private:	// from CEikDialog

	    /**
	    * Fills the edwins with default values.
	    */
        void  PreLayoutDynInitL();

	    /**
	    * Set the password if the user pressed Ok.
	    * @param aKeyCode The ID of the pushed button.
	    * @return May the dialog exit?
	    */
        TBool OkToExitL( TInt aKeycode );

        // MAknQueryControlObserver
        TBool HandleQueryEditorStateEventL(CAknQueryControl* aQueryControl, TQueryControlEvent aEventType, TQueryValidationStatus aStatus);

    private:	// Data

        HBufC*& iRetBuf;
    };

#endif // BRCTLSAMPLEAPPQUERYDIALOG_H

// End of File

