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


#ifndef __AKNEXQUERYCONTAINER_H__
#define __AKNEXQUERYCONTAINER_H__

// INCLUDES
#include <coecntrl.h>
#include <coecobs.h>
#include <AknGlobalListQuery.h>
#include <AknQueryDialog.h>
#include <aknglobalmsgquery.h>     // for global message query
#include <AknPopup.h>
#include "AknExQueryGlobalListObserver.h"

// CONSTANTS
const TInt KAknExQueryTextBufLength = 128;

#define KAknExQueryGray TRgb( 0xaaaaaa )
#define AKNEXQUERY_LABEL_ORIGIN TPoint( 0, 0 )
#define AKNEXQUERY_LABEL2_POSITION TPoint( 0, 20 )

const TInt KAknExQueryCountComponentControls = 1;
const TInt KAknExQueryNameColumnIndex = 0;
const TInt KAknExQueryNameGapColumnIndex = 1;
const TInt KAknExQueryListBoxRectPointX = 30;
const TInt KAknExQueryListBoxRectPointY = 15;
const TInt KAknExQueryListBoxRectWidth = 120;
const TInt KAknExQueryListBoxRectHeight = 120;

_LIT( KAknExQueryMbmFilePath, "\\private\\A0000232\\aknexquery.mbm" );

// FORWARD DECLARATIONS
class CEikLabel;
class CEikColumnListBox;
class CAknExQueryView;
class CAknExQueryBaseListEventHandler;

// CLASS DECLARATION

/**
* CAknExQueryContainer  container control class.
*/
class CAknExQueryContainer : public CCoeControl
    {
    public:

    private: // Enumerations

        enum TAknExQueryComponentControls
            {
            EAknExQueryComponentLabel, //0
            EAknExQueryComponentLabel2, //1 
            EAknExQueryComponentListBox //2
            };

    public: // Constructors and destructor

        /**
        * CAknExQueryContainer
        * Constructor.
        * @param aView Pointer to View class object.
        */
        CAknExQueryContainer( CAknExQueryView* aView );

        /**
        * ConstructL
        * Symbian default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL( const TRect& aRect );

        /**
        * ~CAknExQueryContainer
        * Destructor.
        */
        virtual ~CAknExQueryContainer();

    public: // New functions

        /**
        * ShowConfirmationQueryL
        * Shows confirmation query.
        * @param aResourceId for dialog.
        * @param aTextResourceId for prompt.
        * @param aToneFlag is flag for adding tone to argument of constructor.
        * @param aPromptFlag is flag for displaying prompt dynamically.
        * @param aHeaderTextAndImageFlag
        * @param aTone
        */
        void ShowConfirmationQueryL( 
            const TInt aResourceId,
            const TInt aTextResourceId = 0,
            const TBool aPromptFlag = EFalse,
            const TBool aToneFlag = EFalse,
            const TBool aHeaderTextAndImageFlag = EFalse,
			const CAknQueryDialog::TTone aTone = CAknQueryDialog::ENoTone );
 

        /**
        * ShowDataQueryL
        * Shows data query.
        * @param aQueryResourceId for dialog.
        * @param aTextResourceId for inial value.
        * @param aPromptResourceId for ptompt
        * @param aMaxLength sets up the length of the character.
        * @param aDisplayFlag
        */
        void ShowDataQueryL( 
            const TInt aQueryResourceId,
            const TInt aTextResourceId = 0,
            const TInt aPromptResourceId = 0,
            const TInt aMaxLength = 0,
            const TInt aDisplayFlag = ETrue );

        /**
        * ShowListQueryL
        * Show list query.
        * @param aResourceId
        */
        void ShowListQueryL( const TInt aResourceId );
 
        /**
        * PopoutListBoxL
        * Create handler.
        */
        void PopoutListBoxL();

        /**
        * CreateListBoxL
        * Creates listbox object.
        */
        void CreateListBoxL();

        /**
        * DeleteListBoxL
        * Deletes listbox object.
        */
        void DeleteListBoxL();

        /**
        * CreateLabelL
        * Creates label object.
        * @param aResourceId
        */
        void CreateLabelL( TInt aResourceId );

        /**
        * DeleteLabelL
        * Deletes label object.
        */
        void DeleteLabelL();

        /**
        * SetTextToLabelL
        * Set Label text.
        * @param aResourceId
        */
        void SetTextToLabelL( TInt aResourceId );

        /**
        * ShowMessageL
        * Show Message Text. 
        * @param aMessage
        */
        void ShowMessageL( const TDesC& aMessage );

        /**
        * ShowMultiLineTimeAndDurationLayoutL
        * Show multi line time and duration layout.
        * @param aResourceId for dialog
        * @param aTopResourceId for top of multi line.
        */
        void ShowMultiLineTimeAndDurationLayoutL( 
            const TInt aResourceId, 
            const TInt aTopResourceId );

        /**
        * ShowGlobalListQueryL
		* The function for the test of the following three APIs.
        * MoveSelectionUp, MoveSelectionDown, SelectItem.
        */
        void ShowGlobalListQueryL();

    public: // From CCoeControl

        /**
        * From CCoeControl, OfferKeyEventL.
        * Handles the key events.
        * @return if key-event is consumed, EKeyWasConsumed.
        *   Else EKeyWasNotConsumed
        * @param aKeyEvent Key event
        * @param aType Type of key event ( EEventKey, EEventKeyUp or
        *   EEventKeyDown )
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

    private: // From CCoeControl

        /**
        * From CCoeControl, SizeChanged.
        *
        */
        void SizeChanged();

        /**
        * From CCoeControl, CountComponentControls.
        * Returns number of component.
        * @return Number of controls
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl, ComponentControl.
        * Returns pointer to particular component.
        * @return Pointer to Control's component control, identified by index
        * @param aIndex Index of the control
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * From CCoeControl, Draw.
        * @param aRect Region of the control to be ( re )drawn.
        */
        void Draw( const TRect& aRect ) const;

    private: //New Function

        /**
        * ShowResultMessageL
        * Show Result Message.
        * @param aResult
        */
        void ShowResultMessageL( TBool aResult );

        /**
        * SetIconL
        * @param aDlg
        * @param aBitmap
        * @param aMask
        */
        void SetIconL( CAknQueryDialog* aDlg, TInt32 aBitmap, TInt32 aMask );

    private: // Data

        /**
        * iLabel
        * Owned by CAknExQueryContainer object.
        */
        CEikLabel*                          iLabel;

        /**
        * iLabelResultMessage
        * Owned by CAknExQueryContainer object.
        */
        CEikLabel*                          iLabelResultMessage;

        /**
        * iView
        * Does not own
        */
        CAknExQueryView*                    iView;

        /**
        * iListBox
        * Owned by CAknExQueryContainer object.
        */
        CEikColumnListBox*                  iListBox;

        /**
        * iHandler
        * Owned by CAknExQueryContainer object.
        */
        CAknExQueryBaseListEventHandler*    iHandler;

        /**
        * iDisplayDialog, boolean for Display Dialog
        */
        TBool                               iDisplayDialog;

        /**
        * iGlobalListQuery
        * Owned by CAknExQueryContainer object.
        */
        CAknGlobalListQuery*                iGlobalListQuery;

        /**
        * iGlobalListObserver
        * Owned by CAknExQueryContainer object.
        */
        CAknExQueryGlobalListObserver*      iGlobalListObserver;

    };

#endif // __AKNEXQUERYCONTAINER_H__

// End of File
