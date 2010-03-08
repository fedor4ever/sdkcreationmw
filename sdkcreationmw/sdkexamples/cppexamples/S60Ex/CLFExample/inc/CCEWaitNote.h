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
* Description: 
*
*/



#ifndef __CCEWAITNOTE_H__
#define __CCEWAITNOTE_H__

//  INCLUDES
#include <e32base.h>
#include <aknwaitdialog.h>
#include <aknwaitnotewrapper.h>

#include "CLFExample.hrh"

// CLASS DECLARATION

/**
*  Wait note for CLF Example Application.
*  Shows a wait note to indicate the refresh of the List Model.
*  The wait note is closed, when refreshing operation has completed.
*  This wait note is used in classes CCEGroupView and CCEMusicView when
*  calling MCLFItemListModel::RefreshL().
*/
class CCEWaitNote
    : public CBase,
      public MProgressDialogCallback
    {
    public:  // Constructors and destructor

		/**
		 * Creates and returns a new instance of this class.
		 * @return Pointer to the CCEWaitNote object
		 */
	    static CCEWaitNote* NewL();

        /**
        * Destructor.
        */
        virtual ~CCEWaitNote();

    private:    // Constructors
        /**
        * C++ default constructor.
        */
        CCEWaitNote();

    protected:  // Functions from base classes

        /**
        * From MProgressDialogCallback. Gets called when the
        * dialog is dismissed/closed.
        * @param aButtonId Softkey Button Id
        */
        void DialogDismissedL( const TInt aButtonId );

    public:     // New functions

        /**
        * Close the wait note dialog.
        */
        void CloseWaitNoteL();

        /**
        * Open the wait note dialog.
        * @param aResourceId Wait note resource id
        */
        void OpenWaitNoteL( const TInt aResourceId );

    private:    // Data

        // Wait note dialog for indicating refresh operation
        // of the List Model (owned)
		CAknWaitDialog* iWaitDialog;

    };

#endif // __CCEWAITNOTE_H__

// End of File
