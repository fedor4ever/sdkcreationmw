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
* Description:  Functions of the CIsvTelInfoAppDlg class
*
*/


// INCLUDE FILES    
#include <eikmenub.h>
#include <eikcapc.h>
#include <avkon.hrh>
#include <avkon.rsg>
#include <aknnotewrappers.h>
#include <akntitle.h>
#include <etel3rdparty.h> 
#include <IsvTelInfoApp.rsg>
#include "IsvTelInfoAppContainer.h"
#include "IsvTelInfoAppTelephonyNotify.h"
#include "IsvTelInfoAppTelephony.h"
#include "IsvTelInfoAppDlg.h"
#include "IsvTelInfoAppUi.h"

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CIsvTelInfoAppDlg::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CIsvTelInfoAppDlg* CIsvTelInfoAppDlg::NewL()
    {
    CIsvTelInfoAppDlg* self = 
        new (ELeave) CIsvTelInfoAppDlg();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// CIsvTelInfoAppDlg::~CIsvTelInfoAppDlg()
// Destructor.
// ----------------------------------------------------------------------------
//
CIsvTelInfoAppDlg::~CIsvTelInfoAppDlg()
    {
    
	SetMainPageTitleOnPaneL();
	
    if ( !iIsStoreForm )
        {
        if ( iTelephony )
            {
            delete iTelephony;
            }
        }
    }
    
// ----------------------------------------------------------------------------
// CIsvTelInfoAppDlg::ExecuteLD( TInt aResourceId )
// Over-riding function from base-class. It sets the page title for the first
// page and then invokes the base-class function.
// ----------------------------------------------------------------------------
//
TInt CIsvTelInfoAppDlg::ExecuteLD( TInt aResourceId )
    {
    if ( iIsStoreForm )
        {
        User::Leave(KErrGeneral);
        return KErrGeneral;
        }
    else
        {
        TInt retCode;

        //Set page title for first page
        SetPageTitleOnPaneL(iPageId);
        
        //Call the base-class routine
        // Launches the dialog having resource ID aResourceId
        retCode = CAknForm::ExecuteLD( aResourceId );

        TryChangeFocusToL(EIsvTelInfoAppDlgCtrlIdPhoneMfr);
        
        return(retCode);
        }
    }

// ----------------------------------------------------------------------------
// CIsvTelInfoAppDlg::OfferKeyEventL()
// An over-riding routine that captures the Green dial key and the Red hang-up
// key from the key-pad when they are pressed with the focus being present 
// on the Phone-numbers in the dialog screen.
// For other keys, it calls the base-class routine for handling.
// ----------------------------------------------------------------------------
//
TKeyResponse CIsvTelInfoAppDlg::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType)
    {
    if (aType == EEventKey)
        {
        //Catch the dial button
        switch (aKeyEvent.iCode)
            {
            case EKeyYes:
            case EKeyNo:
            	{
                return EKeyWasConsumed;
            	}
            case EKeyUpArrow:
            case EKeyDownArrow:
                {
                if( iIsInEditMode )
                    {
                    return EKeyWasConsumed;
                    }
                }
                break;
            default:
                break;
            }//switch
        }//if
    return CAknForm::OfferKeyEventL(aKeyEvent, aType);
    }
    
// ----------------------------------------------------------------------------
// CIsvTelInfoAppDlg::DynInitMenuPaneL()
// Different controls/dialog items/pages on the multi-page dialog require their
// own context sensitive menus to be able to choose only the applicable
// operations on the item. For this purpose, this function, loads menu panes
// that are appropriate and applicable for the respective menu items.
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppDlg::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {

    TInt pageId = ActivePageId();

    if (aResourceId == R_AVKON_FORM_MENUPANE) 
        {
        aMenuPane->AddMenuItemsL(
            R_GENERAL_STATUS_MENU_PANE,0,1);
        aMenuPane->SetItemDimmed(EAknFormCmdEdit,TRUE);
        aMenuPane->SetItemDimmed(EAknFormCmdSave,TRUE);
        aMenuPane->SetItemDimmed(EAknFormCmdAdd,TRUE);
        aMenuPane->SetItemDimmed(EAknFormCmdLabel,TRUE);
        aMenuPane->SetItemDimmed(EAknFormCmdDelete,TRUE);
        }
    else
    	{
    	CAknForm::DynInitMenuPaneL(aResourceId, aMenuPane);	
    	}
    }

// ----------------------------------------------------------------------------
// CIsvTelInfoAppDlg::ProcessCommandL()
// An over-riding function that handles all commands loaded by the
// DynInitMenuPaneL. For the remaining commands like Edit & Save, it invokes 
// the base class routine.
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppDlg::ProcessCommandL( TInt aCommandId )
    {
    TInt pageId = ActivePageId();
    switch (pageId)
        {
        /////////////////////////////////////////////////////////////
        case EIsvTelInfoAppPageCtrlIdPagePhoneInfo:
             {
             iTelephony->TelApiWrapperGetPhoneIdL( );    
             iTelephony->TelApiWrapperGetSubscriberIdL( );
             }//case
             break; 
          	/////////////////////////////////////////////////////////////    
       	case EIsvTelInfoAppPageCtrlIdPageNwBarringStatus:
       	   	{
           	iTelephony->TelApiWrapperGetBarringStatusL( 
           	    CTelephony::EBarAllIncoming );    
           	iTelephony->TelApiWrapperGetBarringStatusL( 
           	    CTelephony::EBarAllOutgoing );    
			iTelephony->TelApiWrapperGetBarringStatusL( 
           	    CTelephony::EBarIncomingRoaming );    
           	iTelephony->TelApiWrapperGetBarringStatusL( 
           	    CTelephony::EBarOutgoingInternational );    
           	iTelephony->TelApiWrapperGetBarringStatusL( 
         	    CTelephony::EBarOutgoingInternationalExHC );    
           	}//case
           	break;                
       	/////////////////////////////////////////////////////////////    
       	case EIsvTelInfoAppPageCtrlIdPageMisc:
          	{
      		iTelephony->TelApiWrapperGetWaitingStatusL( );   
       		iTelephony->TelApiWrapperGetNwRegistrationStatusL( );
           	iTelephony->TelApiWrapperGetIdentityStatusL(
           	    CTelephony::EIdServiceCallerPresentation );    
          	iTelephony->TelApiWrapperGetIdentityStatusL(
          	    CTelephony::EIdServiceCallerRestriction );    
           	}//case
           	break;  
        ////////////////////////////////////////////////////////////////////
        case EAknFormCmdEdit:
            {
            iIsInEditMode = ETrue;
            }
            break;

       default:
          	break;
       }//pageId switch
    //Call the base class routine
    CAknForm::ProcessCommandL(aCommandId);
    }

// ----------------------------------------------------------------------------
// CIsvTelInfoAppDlg::PageChangedL()
// Derived from base-class. It invokes the base-class function and after the
// page switch is executed, it sets the appropriate title for the page.
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppDlg::PageChangedL(TInt aPageId)
    {
    //Invoke the base class routine
    CAknForm::PageChangedL(aPageId);

    //Call SetPageTitleOnPaneL to set the appropriate title on pane
    SetPageTitleOnPaneL(aPageId);
    }
       
// ----------------------------------------------------------------------------
// CIsvTelInfoAppDlg::SetDlgPageL()
// Is called from the container class to set the start page.
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppDlg::SetDlgPage(TInt aPageId)
    {
    iPageId = aPageId;
    }
           
// ----------------------------------------------------------------------------
// CIsvTelInfoAppDlg::SetTitlePane()
// It is called by the container class that creates this dialog, to set a
// pointer to the Title pane. This is required to update the name of the
// current page to on the title pane when a page change happens in the dialog.
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppDlg::SetTitlePane(CAknTitlePane* aTitlePane)
    {
    iTitlePane = aTitlePane;
    }
 
// ----------------------------------------------------------------------------
// CIsvTelInfoAppDlg::SetContainer()
// It is called by the container class that creates this dialog, to set a
// pointer to itself. The dialog class uses this pointer from some command 
// completion handlers to bring the view back to foreground.
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppDlg::SetContainer(CIsvTelInfoAppContainer* aContainer)
    {
    iContainer = aContainer;
    }

// ----------------------------------------------------------------------------
// CIsvTelInfoAppDlg::SetPageTitleOnPaneL()
// For each page, it sets the appropriate page title on the pane
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppDlg::SetPageTitleOnPaneL(TInt aPageId)
    {
    const TInt KTitlePaneBufLength = 256;
    TBuf<KTitlePaneBufLength> titleText( NULL );
    
    //Select the page title
    switch (aPageId)
        {
        case EIsvTelInfoAppPageCtrlIdPagePhoneInfo:
        	{
        	iEikonEnv->ReadResource( titleText, 
                R_ISVTELINFOAPP_TEXT_STATUS_PANE_PAGE_PHONEINFO);
        	}
            break;
        case EIsvTelInfoAppPageCtrlIdPageNwBarringStatus:
        	{
        	iEikonEnv->ReadResource( titleText, 
                R_ISVTELINFOAPP_TEXT_STATUS_PANE_PAGE_BARRINGSTATUS );
        	}
            break;
        case EIsvTelInfoAppPageCtrlIdPageMisc:
        	{
        	iEikonEnv->ReadResource(titleText, 
                R_ISVTELINFOAPP_TEXT_STATUS_PANE_PAGE_MISC);
        	}
            break;
        default:
            break;
        }
        
    //Set the appropriate title on the pane
    if (iTitlePane)
        {
        iTitlePane->SetTextL( titleText );
        }
    }

// ----------------------------------------------------------------------------
// CIsvTelInfoAppDlg::SetMainPageTitleOnPaneL()
// Sets the Application's title on the pane
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppDlg::SetMainPageTitleOnPaneL( )
    {
    const TInt KTitlePaneBufLength = 256;
    TBuf<KTitlePaneBufLength> titleText( NULL );
    
    iEikonEnv->ReadResource(titleText, 
        R_ISVTELINFOAPP_TEXT_STATUS_PANE_MAIN_PAGE);
     
     //Set the appropriate title on the pane
    if (iTitlePane)
        {
        iTitlePane->SetTextL( titleText );
        }
    }

// ----------------------------------------------------------------------------
// CIsvTelInfoAppDlg::GetPhoneIdApiCompleteL()
// Command completion routine called from the CIsvTelInfoAppTelephony class
// once the request to Get Phone ID is complete
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppDlg::GetPhoneIdApiCompleteL(TInt aStatus, 
    CTelephony::TPhoneIdV1 aPhoneId, TDesC16& aErrMsg)
    {
    if (aStatus != KErrNone)
        {
        SetEdwinTextL((TInt)EIsvTelInfoAppDlgCtrlIdPhoneMfr,
        	&aErrMsg);
        	
       	SetEdwinTextL((TInt)EIsvTelInfoAppDlgCtrlIdPhoneModel,
            &aErrMsg);
                                       
		SetEdwinTextL((TInt)EIsvTelInfoAppDlgCtrlIdPhoneSerNum,
            &aErrMsg);                                       
        }
    else    
        {
        //Update Phone Mfr to UI
        iEdwinPhoneMfr.Copy(aPhoneId.iManufacturer);
        SetEdwinTextL((TInt)EIsvTelInfoAppDlgCtrlIdPhoneMfr,
        	&iEdwinPhoneMfr);

        //Update Phone Model to UI
        iEdwinPhoneModel.Copy(aPhoneId.iModel);
        SetEdwinTextL((TInt)EIsvTelInfoAppDlgCtrlIdPhoneModel,
            &iEdwinPhoneModel);
                                      
        //Update Phone Ser num to UI
        iEdwinPhoneSerNum.Copy(aPhoneId.iSerialNumber);
        SetEdwinTextL((TInt)EIsvTelInfoAppDlgCtrlIdPhoneSerNum,
        	&iEdwinPhoneSerNum);
        }
    }

// ----------------------------------------------------------------------------
// CIsvTelInfoAppDlg::GetSubscriberIdApiCompleteL()
// Command completion routine called from the CIsvTelInfoAppTelephony class
// once the request to Get Subscriber ID is complete
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppDlg::GetSubscriberIdApiCompleteL(TInt aStatus, 
    CTelephony::TSubscriberIdV1 aSubscriberId, TDesC16& aErrMsg)
    {
    if (aStatus != KErrNone)
        {
        SetEdwinTextL((TInt)EIsvTelInfoAppDlgCtrlIdSubscriberId,
            &aErrMsg);  
        }
    else
        {
        //Update the subscriber id to UI
        iEdwinSubscriberId.Copy(aSubscriberId.iSubscriberId);
        SetEdwinTextL((TInt)EIsvTelInfoAppDlgCtrlIdSubscriberId,
        	&iEdwinSubscriberId);    
       }
    }

// ----------------------------------------------------------------------------
// CIsvTelInfoAppDlg::GetBarringStatusApiCompleteL()
// Command completion routine called from the CIsvTelInfoAppTelephony class
// once the request to Get Barring status is complete
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppDlg::GetBarringStatusApiCompleteL(
    TInt aStatus, 
    CTelephony::TCallBarringCondition aBarringCondition,
    CTelephony::TCallBarringSupplServicesV1 aCallBarringServices,
    TDesC16& aErrMsg)
    {
    if (aStatus != KErrNone)
        {
        SetEdwinTextL((TInt) 
            EIsvTelInfoAppDlgCtrlIdBarringAllIncoming, &aErrMsg);
            
        SetEdwinTextL(
            (TInt) EIsvTelInfoAppDlgCtrlIdBarringIncomingRoaming, &aErrMsg);

		SetEdwinTextL((TInt) 
            EIsvTelInfoAppDlgCtrlIdBarringAllOutgoing, &aErrMsg);

		SetEdwinTextL((TInt) 
            EIsvTelInfoAppDlgCtrlIdBarringOutgoingInternational, &aErrMsg);

		SetEdwinTextL((TInt)
            EIsvTelInfoAppDlgCtrlIdBarringOutgoingInternationalExHC, 
            &aErrMsg);
        
         return;
        }    
    else
        {
        TBuf<EIsvTelInfoAppEdwinMaxLength> barringStatusStr;
        switch (aCallBarringServices.iCallBarring)
            {
            case CTelephony::EStatusActive:
                {
                barringStatusStr.Copy(iBarStatusActive);    
                }
                break;
            case CTelephony::ENotActive:
                {
                barringStatusStr.Copy(iBarStatusNotActive);    
                }
                break;
            case CTelephony::ENotProvisioned:
                {
                barringStatusStr.Copy(iBarStatusNotProvisioned);    
                }
                break;
            case CTelephony::ENotAvailable:
                {
                barringStatusStr.Copy(iBarStatusNotAvailable);    
                }
                break;
            case CTelephony::EUnknown:
                {
                barringStatusStr.Copy(iBarStatusUnknown);    
                }
                break;
            default:
                {
                barringStatusStr.Copy(iErrString);
                }
                break;
            }
            
        iEdwinBarringStatus.Copy(barringStatusStr);
    
        switch (aBarringCondition)    
            {
            case CTelephony::EBarAllIncoming:
                {
                SetEdwinTextL((TInt) 
                    EIsvTelInfoAppDlgCtrlIdBarringAllIncoming, 
                    &iEdwinBarringStatus);
                }
                break;
            case CTelephony::EBarIncomingRoaming:
                {
                SetEdwinTextL(
                    (TInt) EIsvTelInfoAppDlgCtrlIdBarringIncomingRoaming,     
                    &iEdwinBarringStatus);
                }
                break;
            case CTelephony::EBarAllOutgoing:
                {
                SetEdwinTextL((TInt) 
                    EIsvTelInfoAppDlgCtrlIdBarringAllOutgoing, 
                    &iEdwinBarringStatus);
                }
                break;
            case CTelephony::EBarOutgoingInternational:
                {
                SetEdwinTextL((TInt) 
                    EIsvTelInfoAppDlgCtrlIdBarringOutgoingInternational, 
                    &iEdwinBarringStatus);
                }
                break;
            case CTelephony::EBarOutgoingInternationalExHC:
                {
                SetEdwinTextL((TInt)
                    EIsvTelInfoAppDlgCtrlIdBarringOutgoingInternationalExHC,  
                    &iEdwinBarringStatus);
                }
                break;
            default:
                break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CIsvTelInfoAppDlg::GetWaitingStatusApiCompleteL()
// Command completion routine called from the CIsvTelInfoAppTelephony class
// once the request to Get call waiting status is complete
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppDlg::GetWaitingStatusApiCompleteL(TInt aStatus, 
    CTelephony::TCallWaitingSupplServicesV1 aCallWaitingServices, 
    TDesC16& aErrMsg)
    {
    if (aStatus != KErrNone)
        {
        SetEdwinTextL((TInt) EIsvTelInfoAppDlgCtrlIdWaiting,
            &aErrMsg);
         return;
        }    
    else
        {
        TBuf<EIsvTelInfoAppEdwinMaxLength> waitingStatusStr;
        switch (aCallWaitingServices.iCallWaiting)
            {
            case CTelephony::EStatusActive:
                {
                waitingStatusStr.Copy(iWaitStatusActive);    
                }
                break;
            case CTelephony::ENotActive:
                {
                waitingStatusStr.Copy(iWaitStatusNotActive);    
                }
                break;
            case CTelephony::ENotProvisioned:
                {
                waitingStatusStr.Copy(iWaitStatusNotProvisioned);    
                }
                break;
            case CTelephony::ENotAvailable:
                {
                waitingStatusStr.Copy(iWaitStatusNotAvailable);    
                }
                break;
            case CTelephony::EUnknown:
                {
                waitingStatusStr.Copy(iWaitStatusUnknown);    
                }
                break;
            default:
                {
                waitingStatusStr.Copy(iErrString);
                }
                break;
            }
        iEdwinWaiting.Copy(waitingStatusStr);
        SetEdwinTextL((TInt) EIsvTelInfoAppDlgCtrlIdWaiting,
            &iEdwinWaiting);
        }  
    }

// ----------------------------------------------------------------------------
// CIsvTelInfoAppDlg::GetIdentityServiceStatusApiCompleteL()
// Command completion routine called from the CIsvTelInfoAppTelephony class
// once the request to Get Id Service status is complete
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppDlg::GetIdentityServiceStatusApiCompleteL(
    TInt aStatus, 
    CTelephony::TIdentityService aIdentityServiceType, 
    CTelephony::TIdentityServiceV1 aIdentityService, 
    TDesC16& aErrMsg)
    {
    if (aStatus != KErrNone)
        {
        SetEdwinTextL((TInt) EIsvTelInfoAppDlgCtrlIdIdPresentation,
            &aErrMsg);
            
        SetEdwinTextL((TInt) EIsvTelInfoAppDlgCtrlIdIdRestriction,
            &aErrMsg); 
        return;
        }
     else
     	{
        TBuf<EIsvTelInfoAppEdwinMaxLength> identityServiceStatusStr;
        switch (aIdentityService.iIdentityStatus)
            {
        	case CTelephony::EIdServiceActivePermanent:
                {
                identityServiceStatusStr.Copy(iIdStatusActivePerm);    
                }
                break;
                
            case CTelephony::EIdServiceActiveDefaultRestricted:
                {
                identityServiceStatusStr.Copy(
                	iIdStatusDefaultRestricted);    
                }
                break;
                
            case CTelephony::EIdServiceActiveDefaultAllowed:
                {
                identityServiceStatusStr.Copy(iIdStatusDefaultAllowed);    
                }
                break;
                
            case CTelephony::EIdServiceNotProvisioned:
                {
                identityServiceStatusStr.Copy(iIdStatusNotProvisioned);    
                }
                break;
                
            case CTelephony::EIdServiceUnknown:
                {
                identityServiceStatusStr.Copy(iIdStatusUnknown);    
                }
                break;
                
            default:
                {
                identityServiceStatusStr.Copy(iErrString);
                }
                break;
            }
            
        iEdwinIdentity.Copy(identityServiceStatusStr);
        
        switch (aIdentityServiceType)
            {
            case CTelephony::EIdServiceCallerPresentation:
                {
                SetEdwinTextL((TInt)EIsvTelInfoAppDlgCtrlIdIdPresentation,
                    &iEdwinIdentity);
                }
                break;
                
            case CTelephony::EIdServiceCallerRestriction:
                {
                SetEdwinTextL((TInt) EIsvTelInfoAppDlgCtrlIdIdRestriction,
                    &iEdwinIdentity);    
                }
                break;

            default:
                break;
            }
        }    
    }

// ----------------------------------------------------------------------------
// CIsvTelInfoAppDlg::GetNwRegistrationStatusApiCompleteL()
// Command completion routine called from the CIsvTelInfoAppTelephony class
// once the request to Get Network Registration Information is complete
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppDlg::GetNwRegistrationStatusApiCompleteL(TInt aStatus, 
    CTelephony::TNetworkRegistrationV1 aNwRegistrationStatus, TDesC16& aErrMsg)
    {
    if (aStatus != KErrNone)
        {
        SetEdwinTextL((TInt)EIsvTelInfoAppDlgCtrlIdNwRegistration,
            &aErrMsg);
        }
    else
        {
        switch (aNwRegistrationStatus.iRegStatus)
            {
            case CTelephony::ERegistrationUnknown :
                {
                iEdwinNwRegistration.Copy(iNwRegUnknown);
                }
                break;

            case CTelephony::ENotRegisteredNoService:
                {
                iEdwinNwRegistration.Copy(iNwRegNoRegNoSrvc);
                }
                break;
    
            case CTelephony::ENotRegisteredEmergencyOnly:
                {
                iEdwinNwRegistration.Copy(iNwRegNoRegEmergSrvc);
                }
                break;

            case CTelephony::ENotRegisteredSearching:
                {
                iEdwinNwRegistration.Copy(iNwRegNoRegSearching);
                }
                break;

            case CTelephony::ERegisteredBusy:
                {
                iEdwinNwRegistration.Copy(iNwRegBusy);
                }
                break;

            case CTelephony::ERegisteredOnHomeNetwork:
                {
                iEdwinNwRegistration.Copy(iNwRegHome);
                }
                break;

            case CTelephony::ERegistrationDenied:
                {
                iEdwinNwRegistration.Copy(iNwRegDenied);
                }
                break;
        
            case CTelephony::ERegisteredRoaming:
                {
                iEdwinNwRegistration.Copy(iNwRegRoaming);
                }
                break;
            
            default:
                {
                iEdwinNwRegistration.Copy(iErrString);
                }
                break;
            }

        //Update Nw Regstrn status to UI        
        SetEdwinTextL((TInt)EIsvTelInfoAppDlgCtrlIdNwRegistration,
            &iEdwinNwRegistration);
        }
    }

// ----------------------------------------------------------------------------
// CIsvTelInfoAppDlg::CIsvTelInfoAppDlg()
// Default constructor.
// ----------------------------------------------------------------------------
//
CIsvTelInfoAppDlg::CIsvTelInfoAppDlg()
    {
    }

// ----------------------------------------------------------------------------
// CIsvTelInfoAppDlg::ConstructL()
// Second-phase constructor.
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppDlg::ConstructL()
    {
    iTitlePane = NULL;

    if(!iIsStoreForm)
        {
        CAknForm::ConstructL();
        // Create the telephony object for execution from GUI
        iTelephony = CIsvTelInfoAppTelephony::NewL(*this);
        }
    //Load strings from resource
    LoadStringsFromResource();
    }

// ----------------------------------------------------------------------------
// CIsvTelInfoAppDlg::PostLayoutDynInitL()
// An over-riding routine which Initialises values to certain default fields
// Also calls the base-class routine
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppDlg::PostLayoutDynInitL()
    {
    //Call the base class routine
    CAknForm::PostLayoutDynInitL();

    //Set edit mode to false
    iIsInEditMode = EFalse;
    }

// ----------------------------------------------------------------------------
// CIsvTelInfoAppDlg::LoadStringsFromResource()
// Loads strings from resource
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppDlg::LoadStringsFromResource( )
    {
    TBuf16<KMaxCmdSize> resStr;

    //Load barring status strings from resource
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELINFOAPP_STATUS_ACTIVE);
    iBarStatusActive.Copy(resStr);    
    
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELINFOAPP_STATUS_NOTACTIVE);
    iBarStatusNotActive.Copy(resStr);
    
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELINFOAPP_STATUS_NOTPROVISIONED);
    iBarStatusNotProvisioned.Copy(resStr);
    
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELINFOAPP_STATUS_NOTAVAILABLE);
    iBarStatusNotAvailable.Copy(resStr);
    
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELINFOAPP_STATUS_UNKNOWN);
    iBarStatusUnknown.Copy(resStr);  
    
  // Network Registration  
   iCoeEnv->ReadResource(resStr, 
        R_ISVTELINFOAPP_NWREG_UNKNOWN);
    iNwRegUnknown.Copy(resStr);  
    
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELINFOAPP_NWREG_NOREGNOSRVC);
    iNwRegNoRegNoSrvc.Copy(resStr);
    
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELINFOAPP_NWREG_NOREGEMERGSRVC);
    iNwRegNoRegEmergSrvc.Copy(resStr);
    
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELINFOAPP_NWREG_NOREGSEARCHING);
    iNwRegNoRegSearching.Copy(resStr);
    
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELINFOAPP_NWREG_REGBUSY);
    iNwRegBusy.Copy(resStr);
    
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELINFOAPP_NWREG_REGHOME);
    iNwRegHome.Copy(resStr);    
                      
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELINFOAPP_NWREG_REGDENIED);
    iNwRegDenied.Copy(resStr);
    
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELINFOAPP_NWREG_REGROAMING);
    iNwRegRoaming.Copy(resStr);  

   //Load waiting status strings from resource
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELINFOAPP_STATUS_ACTIVE);
    iWaitStatusActive.Copy(resStr);  
    
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELINFOAPP_STATUS_NOTACTIVE);
    iWaitStatusNotActive.Copy(resStr);
    
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELINFOAPP_STATUS_NOTPROVISIONED);
    iWaitStatusNotProvisioned.Copy(resStr);  
    
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELINFOAPP_STATUS_NOTAVAILABLE);
    iWaitStatusNotAvailable.Copy(resStr);  
    
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELINFOAPP_STATUS_UNKNOWN);
    iWaitStatusUnknown.Copy(resStr);
    
   //Load identity service strings from resource
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELINFOAPP_IDSTATUS_ACTIVEPERM);
    iIdStatusActivePerm.Copy(resStr);     
    
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELINFOAPP_IDSTATUS_DEFAULTRESTRICTED);
    iIdStatusDefaultRestricted.Copy(resStr);
    
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELINFOAPP_IDSTATUS_DEFAULTALLOWED);
    iIdStatusDefaultAllowed.Copy(resStr);
    
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELINFOAPP_IDSTATUS_NOTPROVISIONED);
    iIdStatusNotProvisioned.Copy(resStr);  
    
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELINFOAPP_IDSTATUS_UNKNOWN);
    iIdStatusUnknown.Copy(resStr);      

   //Load general error and success strings
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELINFOAPP_TELSTR_ERRSTRING);
    iErrString.Copy(resStr);

    iCoeEnv->ReadResource(resStr, 
        R_ISVTELINFOAPP_TELSTR_SUCCESSSTRING);
    iSuccessString.Copy(resStr);

    return;
    }

//End of file
