/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  For settings dialog.
*
*/


#ifndef __CCOMMONDIALOGSEXSETTINGSDIALOG_H__
#define __CCOMMONDIALOGSEXSETTINGSDIALOG_H__

//  INCLUDES
#include <eikdialg.h>
#include <CAknMemorySelectionDialog.h>
#include <aknsettingitemlist.h>

// CLASS DECLARATION

/**
* This class defines a settings dialog for application.
*/
class CCommonDialogsExSettingsDialog
    : public CEikDialog,
      public MEikListBoxObserver
    {

    public:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        ~CCommonDialogsExSettingsDialog();

        /**
        * Two-phased constructor.
        */
        static CCommonDialogsExSettingsDialog* NewL(
            TInt aResId, TBool aMultiDriveSupport);
    private: // Constructors

        /**
        * Default constructor. Inlined to save a few bytes.
        * Parameters to array: granularity, isnumered, 1st ordinal
        */
        inline CCommonDialogsExSettingsDialog() : iSettingItemArray( 1, EFalse, 1 ) {} 
        
    protected: // from CEikDialog

        /**
        * Tests if it is OK to exit the dialog.
        * @param aButtonId The ID of the button that was activated.
        * @return ETrue to validate and exit the dialog,
        *         EFalse to keep the dialog active.
        */
        TBool OkToExitL( TInt aButtonId );

        /**
        * Pre-layout dialog initialisation.
        */
        void PreLayoutDynInitL();

        void ProcessCommandL( TInt aCommandId );
        

    public: // From MEikListBoxObserver

        /**
        * Handles list box events.
        * This is invoked by CEikListBox to notify the observer of
        * list box events.
        * @param aListBox The originating list box.
        * @param aEventType A code for the event.
        */
        void HandleListBoxEventL( CEikListBox* aListBox,
                                  TListBoxEvent aEventType );

   private:    // data

        /**
        * Array of setting items contained in the listbox. Owned.
        */
        CAknSettingItemArray iSettingItemArray;

        /**
        * Selected memo store
        */
        CAknMemorySelectionDialog::TMemory iMemory;
        
        TInt iResourceId; // for memory selection setting page
        
        TBool iMultiDriveSupport;
        
        TDriveNumber iDriveNumber;
    };

#endif  // __CCOMMONDIALOGSEXSETTINGSDIALOG_H__
