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


#ifndef __AKNEXPOPFIELDBASEDIALOG_H__
#define __AKNEXPOPFIELDBASEDIALOG_H__

// INCLUDES
#include <aknform.h>

// CLASS DECLARATION

/**
* CAknExPopFieldDialog
*/
class CAknExPopFieldBaseDialog : public CAknForm
    {
    public: // destructor

        /**
        * CAknExPopFieldBaseDialog
        * Virtual Destructor.
        */
        virtual ~CAknExPopFieldBaseDialog();

    protected: // Constructors.

        /**
        * CAknExPopFieldBaseDialog
        * C++ default constructor.
        */
        CAknExPopFieldBaseDialog();


    public: // From CAknDialog

        /**
        * From CAknDialog, DynInitMenuPaneL.
        * Dims "Label", "Add" and "Delete" menu items.
        * @param aResourceId 
        * @param aMenuPane Pointer to menu-pane object.
        */
        virtual void DynInitMenuPaneL( TInt aResourceId, 
                                       CEikMenuPane* aMenuPane );

        /**
        * From CAknDialog, ProcessCommandL.
        * @param aCommandId
        */
        virtual void ProcessCommandL( TInt aCommandId );


    protected: // From CAknDialog

        /**
        * From CAknDialog, OkToExitL.
        * @param aButtonId
        * @return TBool ok to Exit?
        */
        virtual TBool OkToExitL( TInt aButtonId );


    private: // New functions

        /**
        * Configuration values into Popup Fields.
        * This is pure virtual function.
        */
        virtual void ConfigurePopfieldValueL() = 0;

    private: // From CEikDialog

        /**
        * From CEikDialog, PreLayoutDynInitL.
        * Initialises the dialog's controls before the dialog is sized 
        * and layed out.
        */
        void PreLayoutDynInitL();

        /**
        * From CEikDialog, PostLayoutDynInitL.
        * Initialises the dialog's controls after the dialog has been sized
        * but before it has been activated.
        */
        void PostLayoutDynInitL();

    };

#endif  //__AKNEXPOPFIELDBASEDIALOG_H__

// End of File
