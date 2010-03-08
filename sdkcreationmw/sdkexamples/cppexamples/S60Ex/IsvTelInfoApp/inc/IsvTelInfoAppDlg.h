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


#ifndef ISVTELINFOAPPDLG_H
#define ISVTELINFOAPPDLG_H

// INCLUDES
#include <aknform.h>
#include <eikappui.h>
#include "IsvTelInfoApp.hrh"
#include "IsvTelInfoAppTelephonyNotify.h"
#include "IsvTelInfoAppTelephony.h"

//FOWARD DECLARATIONS
class CIsvTelInfoAppContainer;

// CONSTANTS
const TInt KDefaultAutomationCount = 1;
const TInt KMaxUILogFileName = 200;

class CIsvTelInfoAppAutomationActive;

// CLASS DEFINITIONS
/**
* CIsvTelInfoAppDlg: A dialog class derived from CAknForm representing a
* Multi-page UI dialog
* It implments the MIsvTelInfoAppTelephonyNotify interface
*/
class CIsvTelInfoAppDlg : public CAknForm, 
                           public MIsvTelInfoAppTelephonyNotify
    {
    public:  // Constructor and destructor  
        /**
        * Two-phased constructor.
        */
        static CIsvTelInfoAppDlg* NewL();

        /**
        * Destructor.
        */
        virtual ~CIsvTelInfoAppDlg();

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
        * CIsvTelInfoAppContainer
        */
        void SetContainer(CIsvTelInfoAppContainer* aContainer);

        /**
        * Sets title of specified page to the status pane.
        * @param aPageId. specifes which page
        */
        void SetPageTitleOnPaneL(TInt aPageId);

        /**
        * Sets title of Application to the status pane.
        */
        void SetMainPageTitleOnPaneL();

   public:
       /** 
        * Gets invoked once command completion notification from GetPhoneId 
        * ETel ISV API is received.
        * @param aStatus, specifies SUCCESS or Error Code
        * @param aPhoneId Contains mobile phone identity.
        * @param aErrMsg, contains the error message returned by ETel ISV API
        */
        void GetPhoneIdApiCompleteL(TInt aStatus, 
            CTelephony::TPhoneIdV1 aPhoneId, TDesC16& aErrMsg);
        
       /** 
        * Gets invoked once command completion notification from 
        * GetSubscriberId ETel ISV API is received.
        * @param aStatus, specifies SUCCESS or Error Code
        * @param aSubscriberId Contains Subscriber Id
        * @param aErrMsg, contains the error message returned by ETel ISV API
        */
        void GetSubscriberIdApiCompleteL(TInt aStatus, 
            CTelephony::TSubscriberIdV1 aSubscriberId, TDesC16& aErrMsg);
        
       /** 
        * Gets invoked once command completion notification from 
        * GetNwRegistrationStatus ETel ISV API is received.
        * @param aStatus, specifies SUCCESS or Error Code
        * @param aNwRegistrationStatus Contains current network 
        * registration status.
        * @param aErrMsg, contains the error message returned by ETel ISV API
        */
        void GetNwRegistrationStatusApiCompleteL(TInt aStatus, 
            CTelephony::TNetworkRegistrationV1 aNwRegistrationStatus, 
            TDesC16& aErrMsg);
        
       /** 
        * Gets invoked once command completion notification from 
        * GetBarringStatus ETel ISV API is received.
        * @param aStatus, specifies SUCCESS or Error Code
        * @param CTelephony::TCallBarringCondition aBarringCondition,
        * contains barring condition   
        * @param aCallBarringServices Contains information about the 
        * call barring service. 
        * @param aErrMsg, contains the error message returned by ETel ISV API
        */
        void GetBarringStatusApiCompleteL( TInt aStatus,
            CTelephony::TCallBarringCondition aBarringCondition, 
            CTelephony::TCallBarringSupplServicesV1 aCallBarringServices,
            TDesC16& aErrMsg );     
                                        
       /** 
        * Gets invoked once command completion notification from 
        * GetWaitingStatus ETel ISV API is received.
        * @param aStatus, specifies SUCCESS or Error Code
        * @param aCallWaitingServices Contains information about the 
        * call waiting service.
        * @param aErrMsg, contains the error message returned by ETel ISV API
        */
        void GetWaitingStatusApiCompleteL( TInt aStatus, 
            CTelephony::TCallWaitingSupplServicesV1 aCallWaitingServices,
            TDesC16& aErrMsg );
        
       /** 
        * Gets invoked once command completion notification from 
        * GetIdentityServiceStatus ETel ISV API is received.
        * @param aStatus, specifies SUCCESS or Error Code
        * @param CTelephony::TIdentityService aIdentityServiceType,
        * contains identity serivce type        
        * @param aIdentityService Conatins call identity service status.
        * @param aErrMsg, contains the error message returned by ETel ISV API
        */
        void GetIdentityServiceStatusApiCompleteL( TInt aStatus, 
            CTelephony::TIdentityService aIdentityServiceType, 
            CTelephony::TIdentityServiceV1 aIdentityService,
            TDesC16& aErrMsg );         

     private:  // Constructor
        /**
        * Default constructor.
        */
        CIsvTelInfoAppDlg();

        /**
        * Second-phase constructor.
        */
        void ConstructL();
    
        /**
        * Default Initialiser
        */
        void DefaultInitialiseMembers();

    private:  // Functions from base class
        /**
        * From CEikDialog 
        * Set default field value to member data.
        */
        void PostLayoutDynInitL();

        /**
         * Loads strings from resource
         */
        void LoadStringsFromResource( );
        
    private:
        //Have a handle to the title pane. This will be initialised
        //by the container calls before displaying the form
        CAknTitlePane* iTitlePane;
        CIsvTelInfoAppContainer* iContainer;

        //A form to store persistent information
        CIsvTelInfoAppDlg* iStoreForm;

        //Is it a StoreForm or a running form
        TBool iIsStoreForm;

        //Start page Id
        TInt iPageId;

       //Edit/View mode
        TBool iIsInEditMode;

        // Pointer to CIsvTelInfoAppTelephony
        // Class and those gets instantiated at second phase construction
        CIsvTelInfoAppTelephony* iTelephony;
        
        //General Error string
        TBuf8<KMaxCmdSize> iErrString;
        //General Success string
        TBuf8<KMaxCmdSize> iSuccessString;
        
        //Contains network registration strings loaded from resource
        TBuf8<KMaxCmdSize> iNwRegUnknown;
        TBuf8<KMaxCmdSize> iNwRegNoRegNoSrvc;
        TBuf8<KMaxCmdSize> iNwRegNoRegEmergSrvc;
        TBuf8<KMaxCmdSize> iNwRegNoRegSearching;
        TBuf8<KMaxCmdSize> iNwRegBusy;
        TBuf8<KMaxCmdSize> iNwRegHome;
        TBuf8<KMaxCmdSize> iNwRegDenied;
        TBuf8<KMaxCmdSize> iNwRegRoaming;
        
        //Contains barring status strings loaded from resource
        TBuf8<KMaxCmdSize> iBarStatusActive;
        TBuf8<KMaxCmdSize> iBarStatusNotActive;
        TBuf8<KMaxCmdSize> iBarStatusNotProvisioned;
        TBuf8<KMaxCmdSize> iBarStatusNotAvailable;
        TBuf8<KMaxCmdSize> iBarStatusUnknown;
        
        //Contains waiting status strings loaded from resource
        
        TBuf8<KMaxCmdSize> iWaitStatusActive;
        TBuf8<KMaxCmdSize> iWaitStatusNotActive;
        TBuf8<KMaxCmdSize> iWaitStatusNotProvisioned;
        TBuf8<KMaxCmdSize> iWaitStatusNotAvailable;
        TBuf8<KMaxCmdSize> iWaitStatusUnknown;
        
        //Contains identity service strings loaded from resource
        
        TBuf8<KMaxCmdSize> iIdStatusActivePerm;
        TBuf8<KMaxCmdSize> iIdStatusDefaultRestricted;
        TBuf8<KMaxCmdSize> iIdStatusDefaultAllowed;
        TBuf8<KMaxCmdSize> iIdStatusNotProvisioned;
        TBuf8<KMaxCmdSize> iIdStatusUnknown;
            
        //General Status Menu
        TBuf<EIsvTelInfoAppEdwinMaxLength> iEdwinPhoneMfr;
        TBuf<EIsvTelInfoAppEdwinMaxLength> iEdwinPhoneModel;
        TBuf<EIsvTelInfoAppEdwinMaxLength> iEdwinPhoneSerNum;
        TBuf<EIsvTelInfoAppEdwinMaxLength> iEdwinSubscriberId;

        TBuf<EIsvTelInfoAppEdwinMaxLength> iEdwinBarringStatus;
        TBuf<EIsvTelInfoAppEdwinMaxLength> iEdwinBarringCondition;
        TBuf<EIsvTelInfoAppEdwinMaxLength> iEdwinWaiting;
        TBuf<EIsvTelInfoAppEdwinMaxLength> iEdwinIdentity;
        TBuf<EIsvTelInfoAppEdwinMaxLength> iEdwinNwRegistration;

   };
   
#endif  // ISVTELINFOAPPDLG_H

// End of File
