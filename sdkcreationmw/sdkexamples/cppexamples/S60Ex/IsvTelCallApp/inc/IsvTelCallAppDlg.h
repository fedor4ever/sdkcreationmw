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
* Description:  A class representing a Multi page Avkon dialog
*
*/


#ifndef ISVTELCALLAPPDLG_H
#define ISVTELCALLAPPDLG_H

// INCLUDES
#include <aknform.h>
#include <eikappui.h>
#include "IsvTelCallApp.hrh"
#include "IsvTelCallAppTelephonyNotify.h"
#include "IsvTelCallAppTelephony.h"

//FOWARD DECLARATIONS
class CIsvTelCallAppContainer;

// CLASS DEFINITIONS
/**
* CIsvTelCallAppDlg: A dialog class derived from CAknForm representing a
* Multi-page UI dialog
* It implments the MIsvTelCallAppTelephonyNotify interface
*/
class CIsvTelCallAppDlg : public CAknForm, 
                      public MIsvTelCallAppTelephonyNotify
    {
    public:  // Constructor and destructor  
        /**
        * Two-phased constructor.
        */
        static CIsvTelCallAppDlg* NewL();

        /**
        * Destructor.
        */
        virtual ~CIsvTelCallAppDlg();

    public: // From CAknForm
        /**
        * Overrides the ExecuteLD from CAknForm.
        * Inturn calls the ExecuteLD of the base class. It adds the
        * functionality of setting the Title of the page on the status
        * pane.
        * @param aResourceId: Id of the form resource
        * @return Return value of the base class ExecuteLD
        */
        TInt ExecuteLD( TInt aResourceId ); 

        /**
        * An over-riding routine that captures the Green dial key and the 
        * Red hang-up key from the key-pad when they are pressed with the 
        * focus being present on the Phone-numbers in the dialog screen.
         * For other keys, it calls the base-class routine for handling.
         * @param aKeyEvent The key event.
        * @param aType The type of key event.
        * @return Indicates whether or not the key event was used by 
        * this control.
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
            TEventCode aType );
        
        /**
        * Different controls/dialog items/pages on the multi-page dialog 
        * require their own context sensitive menus to be able to choose 
        * only the applicable operations on the item. For this purpose, 
        * this function, loads menu panes that are appropriate and
        * applicable for the respective menu items.
        * @param aResourceId Resource ID identifying the menu pane to 
        * initialise.
        * @param aMenuPane The in-memory representation of the menu pane.
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
        
        /**
        * An over-riding function that handles all commands loaded by the
        * DynInitMenuPaneL. For the remaining commands like Edit & Save, 
        * it invokes the base class routine.
        * aCommandId ID of the command to respond to. 
        */
        virtual void ProcessCommandL( TInt aCommandId );
        
        /**
        * Derived from base-class. It invokes the base-class function and 
        * after the page switch is executed, it sets the appropriate title
        * for the page.
        * @param aPageId The page ID.
        */
        void PageChangedL( TInt aPageId );

    public: //New Functions

        /**
        * Sets page on the dialog
        * @param aPageId. The control Id of the page
        */
        void SetDlgPage(TInt aPageId);

        /**
        * Sets title to the status pane.
        * @param aTitlePane. This object contains Title Name.
        */
        void SetTitlePane(CAknTitlePane* aTitlePane);

        /**
        * Sets the container control.
        * @param aContainer. This object specifies container of type 
        * CIsvTelCallAppContainer
        */
        void SetContainer(CIsvTelCallAppContainer* aContainer);

        /**
        * Sets title of specified pafe to the status pane.
        * @param aPageId. specifes which page
        */
        void SetPageTitleOnPaneL( TInt aPageId );
        
        /**
        * Sets title of Application to the status pane.
        */
        void SetMainPageTitleOnPaneL();

   private:
        /**
        * Invokes the DialNewNumber ETel ISV API
        * @param aFocusControlId, Id of the control where it is focused
        */
        void MakeNewCallProcessCommandL( TInt aFocusControlId );
            
       /**
        * Invokes the GetCallStatus ETel ISV API
        * @param const CTelephony::TCallId aCallId, Id by which call is made
        */        
        void GetCallStatusL( const CTelephony::TCallId aCallId );
        
       /**
        * Invokes the Hold ETel ISV API
        * @param aPageId, Id of the page from where it got invoked
        */
        void HoldProcessCommandL( TInt aPageId );
    
       /**
        * Invokes the Resume ETel ISV API
        * @param aPageId, Id of the page from where it got invoked
        */
        void ResumeProcessCommandL( TInt aPageId );
    
       /**
        * Invokes the Hangup ETel ISV API
        * @param aPageId, Id of the page from where it got invoked
        */
        void HangupProcessCommandL( TInt aPageId );
    
       /**
        * Invokes the Swap ETel ISV API
        */
        void SwapProcessCommamndL( );
      
       /**
        * Invokes the SendDTMF ETel ISV API
        * @param aFocusControlId, Id of the control where it is focused
        */
        void SendDTMFProcessCommandL( TInt aFocusControlId );
    
    public: 
  
       /**
        * Instances of overload pure virtual functions drawn from 
        * MIsvTelCallAppTelephonyNotify
        */
        
       /** 
        * Gets invoked once command completion notification from MakeNewCall 
        * ETel ISV API is received.
        * @param aStatus, specifies SUCCESS or Error Code
        * @param const CTelephony::TCallId aCallId, refrence to Call ID. 
        * @param CTelephony::TTelNumber &aTelNum, refrence to Tel Number.
        * @param aErrMsg, contains the error message returned by ETel ISV API
        */
        void MakeNewCallApiCompleteL( TInt aStatus,
            const CTelephony::TCallId aCallId,
            CTelephony::TTelNumber& aTelNum,
            TDesC16& aErrMsg );
        
       /** 
        * Gets invoked once command completion notification from HangUp 
        * ETel ISV API is received.
        * @param aStatus, specifies SUCCESS or Error Code
        * @param const CTelephony::TCallId aCallId, refrence to Call ID. 
        * @param aErrMsg, contains the error message returned by ETel ISV API
        */
        void HangupCompleteL( TInt aStatus, TDesC16& aErrMsg );
        
       /** 
        * Gets invoked once command completion notification from Hold 
        * ETel ISV API is received.
        * @param aStatus, specifies SUCCESS or Error Code
        * @param const CTelephony::TCallId aCallId, refrence to Call ID. 
        * @param aErrMsg, contains the error message returned by ETel ISV API
        */
        void HoldCompleteL(TInt aStatus, TDesC16& aErrMsg );
        
       /** 
        * Gets invoked once command completion notification from Resume 
        * ETel ISV API is received.
        * @param aStatus, specifies SUCCESS or Error Code
        * @param const CTelephony::TCallId aCallId, refrence to Call ID. 
        * @param aErrMsg, contains the error message returned by ETel ISV API
        */
        void ResumeCompleteL( TInt aStatus, TDesC16& aErrMsg );
        
       /** 
        * Gets invoked once command completion notification from Swap ETel 
        * ISV API is received.
        * @param aStatus, specifies SUCCESS or Error Code
        * @param aErrMsg, contains the error message returned by ETel ISV API
        */
        void SwapCompleteL( TInt aStatus, TDesC16& aErrMsg);
        
        
       /** 
        * Gets invoked once command completion notification from GetCallStatus 
        * ETel ISV API is received.
        * @param aStatus, specifies SUCCESS or Error Code
        * @param const CTelephony::TCallId aCallId, refrence to Call ID. 
        * @param CTelephony::TCallStatusV1 &aCallStatus, contains status
        * @param aErrMsg, contains the error message returned by ETel ISV API
        */
        void GetCallStatusCompleteL( TInt aStatus,
            const CTelephony::TCallId aCallId,
            CTelephony::TCallStatusV1& aCallStatus,
            TDesC16& aErrMsg );

                                        
       /** 
        * Gets invoked once command completion notification from 
        * GetVoiceLineStatus ETel ISV API is received.
        * @param aStatus, specifies SUCCESS or Error Code
        * @param aErrMsg, contains the error message returned by ETel ISV API
        * @param CTelephony::TCallStatusV1 &aCallStatus, contains status.
        */
        void GetVoiceLineStatusCompleteL( TInt aStatus, 
            TDesC16& aErrMsg,
            CTelephony::TCallStatusV1& aCallStatus );
                                        
       /** 
        * Gets invoked once command completion notification from 
        * GetDataLineStatus ETel ISV API is received.
        * @param aStatus, specifies SUCCESS or Error Code
        * @param aErrMsg, contains the error message returned by ETel ISV API
        * @param CTelephony::TCallStatusV1 &aCallStatus, contains status.
        */
        void GetDataLineStatusCompleteL( TInt aStatus, 
            TDesC16& aErrMsg,
            CTelephony::TCallStatusV1& aCallStatus );
        
       /** 
        * Gets invoked once command completion notification from 
        * GetFaxLineStatus ETel ISV API is received.
        * @param aStatus, specifies SUCCESS or Error Code
        * @param aErrMsg, contains the error message returned by ETel ISV API
        * @param CTelephony::TCallStatusV1 &aCallStatus, contains status.
        */
        void GetFaxLineStatusCompleteL( TInt aStatus, 
            TDesC16& aErrMsg,
            CTelephony::TCallStatusV1& aCallStatus );
                                        
       /** 
        * Gets invoked once command completion notification from 
        * SendDTMF ETel ISV API is received.
        * @param aStatus, specifies SUCCESS or Error Code
        * @param aErrMsg, contains the error message returned by ETel ISV API
        */
        void SendDTMFApiCompleteL( TInt aStatus, TDesC16& aErrMsg );   
        
       /** 
        * Gets invoked once command completion notification from 
        * AnswerIncomingCallApiCompleteL ETel ISV API is received.
        * @param aStatus, specifies SUCCESS or Error Code
        * @param aErrMsg, contains the error message returned by ETel ISV API
        */
        void AnswerIncomingCallApiCompleteL( TInt aStatus, 
            const CTelephony::TCallId aCallId,
            TDesC16& aErrMsg );  
                   
     private:  // Constructor
        /**
        * Default constructor.
        */
        CIsvTelCallAppDlg();

        /**
        * Second-phase constructor.
        */
        void ConstructL();
    
    private:  
    	// Functions from base class
        /**
        * From CEikDialog 
        * Set default field value to member data.
        */
        void PostLayoutDynInitL();

        /**
        * From CAknForm 
        * Show save query. If user answers "No" to this query.
        * If true returns field value to the value which is before editing.
        */
        TBool QuerySaveChangesL();

        /**
        * From CAknForm 
        * Save the contents of the form.
        */
        TBool SaveFormDataL();  

        /**
        * From CAknForm 
        * Does not save the contents of the form. Updates the form from stored
        * data
        */
        void DoNotSaveFormDataL();

        /**
         * Loads strings from resource
         */
        void LoadStringsFromResource( );
        
        
      
    private:
        //Have a handle to the title pane. This will be initialised
        //by the container calls before displaying the form
        CAknTitlePane* iTitlePane;
        CIsvTelCallAppContainer* iContainer;

        //Start page Id
        TInt iPageId;

        TBool iIsAnswerInCall;

        //Edit/View mode
        TBool iIsInEditMode;

        // Pointer to CIsvTelCallAppTelephony
        // Class and those gets instantiated at second phase construction
        CIsvTelCallAppTelephony*       iTelephony;
        CIsvTelCallAppTelephony*       iAnswerInCallTelephony;
        CIsvTelCallAppTelephony*       iVoiceLineAsyncTelephony;
        CIsvTelCallAppTelephony*       iCall1StatusAsyncTelephony;
        CIsvTelCallAppTelephony*       iCall2StatusAsyncTelephony;
   
        //General Error string
        TBuf8<KMaxCmdSize> iErrString;
        //General Success string
        TBuf8<KMaxCmdSize> iSuccessString;

        //Contains command languauge arguments loaded from resource
        CTelephony::TCallId iCallId1Str;
        CTelephony::TCallId iCallId2Str;


        TBuf8<KMaxCmdSize> iDelimiterStr;
        TBuf8<KMaxCmdSize> iNewLineStr;
        
        //Contains GetCallStatus strings loaded from resource
        TBuf8<KMaxCmdSize> iUnknownStr;
        TBuf8<KMaxCmdSize> iIdleStr;
        TBuf8<KMaxCmdSize> iDiallingStr;
        TBuf8<KMaxCmdSize> iRingingStr;
        TBuf8<KMaxCmdSize> iAnsweringStr;
        TBuf8<KMaxCmdSize> iConnectingStr;
        TBuf8<KMaxCmdSize> iConnectedStr;
        TBuf8<KMaxCmdSize> iReconnectPendingStr;
        TBuf8<KMaxCmdSize> iDisconnectingStr;
        TBuf8<KMaxCmdSize> iHoldStr;
        TBuf8<KMaxCmdSize> iTransferringStr;
        TBuf8<KMaxCmdSize> iTransferAlertingStr;
        
    	//Phone 1 Operations Menu
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinPhone1;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinDtmfSeq1;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall1Status;

    	//Phone 2 Operations Menu
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinPhone2;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinDtmfSeq2;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall2Status;

    	//Call 1 status Menu
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall1Dialnum;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall1NumType;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall1NumPlan;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall1DialStatus;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall1Starttime;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall1Duration;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall1Exitcode;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall1Ciphering;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall1CallId;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall1RemStatus;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall1RemName;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall1RemNum;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall1RemNumType;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall1RemNumPlan;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall1Dir;

    	//Call 2 status Menu
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall2Dialnum;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall2NumType;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall2NumPlan;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall2DialStatus;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall2Starttime;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall2Duration;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall2Exitcode;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall2Ciphering;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall2CallId;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall2RemStatus;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall2RemName;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall2RemNum;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall2RemNumType;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall2RemNumPlan;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinCall2Dir;

    	//Line Status Menu
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinVoiceLineStatus;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinDataLineStatus;
        TBuf<EIsvTelCallAppEdwinMaxLength> iEdwinFaxLineStatus;

        //Answer Incoming Call Phone Num
        TBuf<EIsvTelCallAppEdwinMaxLength> iAnswerInCallPhNum1;
        TBuf<EIsvTelCallAppEdwinMaxLength> iAnswerInCallPhNum2;

        TBuf<EIsvTelCallAppEdwinMaxLength> iEmuApiNotSupported;
  };

#endif  // ISVTELCALLAPPDLG_H

// End of File