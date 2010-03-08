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


#ifndef __AKNEXNOTECONTAINER_H__
#define __AKNEXNOTECONTAINER_H__


// INCLUDES
#include <coecntrl.h>
#include <aknnotedialog.h>
#include <AknNotifyStd.h>
#include <apadef.h>
#include <aknprogressdialog.h>


// FORWARD DECLARATIONS
class CEikLabel;
class CAknExNoteView;


// CLASS DECLARATION

/**
* CAknExNoteContainer container control class.
*/
class CAknExNoteContainer : public CCoeControl, MProgressDialogCallback
    {
    private: // Enumerations

        enum TAknExNoteComponentControls
            {
            EAknExNoteComponentLabel,
            EAknExNoteComponentListbox
            };

    public: // Constructors and destructor

        /**
        * CAknExNoteContainer.
        * C++ default constructor.
        * @param aView Pointer to view class object.
        */
        CAknExNoteContainer( CAknExNoteView* aView );


        /**
        * ConstructL
        * 2nd phase constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL( const TRect& aRect );


        /**
        * ~CAknExNoteContainer.
        * Virtual Destructor.
        */
        virtual ~CAknExNoteContainer();


    public: // New functions

        /**
        * SetTextToLabelL
        * @param aResourceId
        */
        void SetTextToLabelL( TInt aResourceId );


        /**
        * ShowGeneralNoteL
        * Indicates general note
        * @param aResourceId
        * @param aTimeout
        * @param aTone
        */
        void ShowGeneralNoteL( TInt aResourceId, 
                               const CAknNoteDialog::TTimeout aTimeout,
                               const CAknNoteDialog::TTone aTone );

        /**
        * ShowGeneralNoteL
        * Indicates general note.
        * @param aResourceId
        * @param aControlId
        * @param aTimeout
        * @param aTone
        * @param aPlural
        */
        void ShowGeneralNoteL( TInt aResourceId,
                               TInt aControlId,
                               const CAknNoteDialog::TTimeout aTimeout,
                               const CAknNoteDialog::TTone aTone,
                               TBool aPlural );

        /**
        * ShowDynamicNumberNoteL
        * Indicates note
        */
        void ShowDynamicNumberNoteL();

        /**
        * ShowWaitNoteL
        * Indicates wait note.
        * @param aResourceId
        * @param aControlId
        */
        void ShowWaitNoteL( TInt aResourceId, TInt aControlId );

        /**
        * ShowProgressNoteUnderSingleProcessL
        * Indicates ProgressNote.
        * @param aResourceId
        * @param aControlId
        */
        void ShowProgressNoteUnderSingleProcessL( TInt aResourceId,   
                                                  TInt aControlId );

        /**
        * ShowShowNoteL
        * Show Note
        * @param aType 
        * @param aResourceId
        */
        void ShowShowNoteL( TAknGlobalNoteType aType, 
                            TInt aResourceId );

        /**
        * CallbackIncrementProgressNoteL
        * Callback function to increment progress note
        * @param aThis
        * @return TInt Return 0 when work is done, otherwise return 1.
        */
        static TInt CallbackIncrementProgressNoteL( TAny* aThis );
        
        /**
        * UpdateProgressNote
        * The entity of ProgressCallBackL() function
        * @return TInt 0 when work is done, otherwise return 1.
        */
        TInt UpdateProgressNote();


    public: // From MProgressDialogCallback
        
        /**
        * DialogDismissedL
        * Called when/if the dialog has been dismissed.
        * @param aButtonId
        */
        void DialogDismissedL( TInt aButtonId );


    private: // New functions

        /**
        * CreateLabelL
        * Creates the label object.
        */
        void CreateLabelL();

    private: // From CCoeControl

        /**
        * OfferKeyEventL.
        * Handles the key events.
        * @param aKeyEvent Key event
        * @param aType Type of key event (EEventKey, EEventKeyUp 
        * or EEventKeyDown)
        * @return if key-event is consumed, EKeyWasConsumed. 
        * Else EKeyWasNotConsumed
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                     TEventCode aType );

        /**
        * SizeChanged
        * Called by framework when the View size is changed..
        */
        void SizeChanged();

        /**
        * CountComponentControls.
        * Returns number of component.
        * @return Number of controls
        */
        TInt CountComponentControls() const;

        /**
        * ComponentControl.
        * Returns pointer to particular component.
        * @param aIndex Index of the control
        * @return Pointer to Control's component control, identified by index
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * Draw
        * @param aRect Region of the control to be (re)drawn.
        */
        void Draw( const TRect& aRect ) const;

    private: // Data

        /**
        * iLabel
        * owned by CAknExNoteContainer object.
        */
        CEikLabel*                      iLabel;

        /**
        * iIdle
        * owned by CAknExNoteContainer object.
        */
        CIdle*                          iIdle;

        /**
        * iProgressDialog
        * Not owned by CAknExNoteContainer object.
        */
        CAknProgressDialog*             iProgressDialog;

        /**
        * iProgressInfo
        * Not owned by CAknExNoteContainer object.
        */
        CEikProgressInfo*               iProgressInfo;

        /**
        * iView
        * Not owned by CAknExNoteContainer object.
        */
        CAknExNoteView*                 iView;

        /** Private Integers for Notes behaviours **/
        TInt                            iGlobalNoteId;
        TInt                            iPermanentNoteId;
        TRequestStatus                  iStatus;
    
    };

#endif  //__AKNEXNOTECONTAINER_H__

// End of File
