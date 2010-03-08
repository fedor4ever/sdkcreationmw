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
* Description:  
*     Declares UI class for application.
*
*/


#ifndef LMUIREFAPPVIEW
#define LMUIREFAPPVIEW_H

// INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknappui.h>
#include <AknViewAppUi.h>
#include <aknview.h>

// FORWARD DECLARATIONS

class CLmkLandmarkSelectorDlg;
class CLmkCategorySelectorDlg; 
class CLmUiRefAppContainer;
class CPosLandmark;
class CPosLandmarkDatabase;
// CLASS DECLARATION

/**
*  CLmUiRefAppview class.
*
*/
class CLmUiRefAppView : public CAknView 
    {
    public: // Constructors and destructor
        /**
        * C++ default constructor.
        * @param aKeyProcessor 
        * @param aLmkSender
        * @return created CLmkByLmView object
        */
        static CLmUiRefAppView* NewLC();

        /**
        * Destructor.
        */
        ~CLmUiRefAppView();

    private: // constructor
        /**
        * C++ constructor
        * @return newly instantiated object
        */
        CLmUiRefAppView();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // New Functions added 
        void NewLmCmdL();        
        void ExecuteLmEditorL(CPosLandmark* aLandmark, CPosLandmarkDatabase& aDb);		
		

	public: // New Funcs	
        CAknViewAppUi* ViewAppUi();
        TPosLmItemId SelectedItem() const;       
        TInt LaunchLandmarksDialogL();
        
        // Multiple Landmarks Selector dialog
        TInt LaunchMultiLandmarkSelectorDialogL(); 
        
        // Multiple category selector dialog       
        TInt LaunchMultiCategorySelectorDialogL();
        
        // Helper method for checking whether the database is empty or not
		TBool IsLmDbEmptyL();
		
		// Single category selector dialog       
		TInt CLmUiRefAppView::LaunchSingleCategoeyDialogL();

    public: // From CAknView
        TUid Id() const;
        void HandleCommandL( TInt aCommand );
        
    private: //From CAknView
        void DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );
        void DoDeactivate();

    protected:  // From MEikMenuObserver
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    private: // Data
		/// Own: A pointer to container
		CLmUiRefAppContainer* iContainer;
		CLmUiRefAppUi* iLmUiRefAppUi;		
		TPosLmItemId  iSelectedItem;		
		CLmkLandmarkSelectorDlg *iLmkSelectDlg;
        CLmkCategorySelectorDlg *iCategorySelectDlg;		
		CPosLandmarkDatabase* ilDb;
    };

#endif // CLMKBYLMVIEW_H

// End of File

