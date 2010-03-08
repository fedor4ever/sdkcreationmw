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
* Description:  Functions of the CIsvTelCallAppDlg class
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
#include <IsvTelCallApp.rsg>
#include "IsvTelCallAppContainer.h"
#include "IsvTelCallAppTelephonyNotify.h"
#include "IsvTelCallAppTelephony.h"
#include "IsvTelCallAppDlg.h"
#include "IsvTelCallAppUi.h"
#include <EikEdwin.h>

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CIsvTelCallAppDlg* CIsvTelCallAppDlg::NewL()
    {
    CIsvTelCallAppDlg* self = 
        new (ELeave) CIsvTelCallAppDlg();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::~CIsvTelCallAppDlg()
// Destructor.
// ----------------------------------------------------------------------------
//
CIsvTelCallAppDlg::~CIsvTelCallAppDlg()
    {

	SetMainPageTitleOnPaneL();

	if (iVoiceLineAsyncTelephony)
    	{
        delete(iVoiceLineAsyncTelephony);
        }
 	if (iCall1StatusAsyncTelephony)
        {
        delete(iCall1StatusAsyncTelephony);
        }
  	if (iCall2StatusAsyncTelephony)
        {
        delete(iCall2StatusAsyncTelephony);
        }
  	if (iAnswerInCallTelephony)
        {
        delete(iAnswerInCallTelephony);
        }
    
    //Delete the CTelephony object
    if ( iTelephony->GetTelephonyPtr() )
        {
        delete ( iTelephony->GetTelephonyPtr() );
        }        
    
    //Delete the telephony pointer
    if ( iTelephony )
        {
        delete iTelephony;
        }
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::ExecuteLD( TInt aResourceId )
// Over-riding function from base-class. It sets the page title for the first
// page and then invokes the base-class function.
// ----------------------------------------------------------------------------
//
TInt CIsvTelCallAppDlg::ExecuteLD( TInt aResourceId )
    {

    TInt retCode;

    //Set page title for first page
    SetPageTitleOnPaneL(iPageId);
      
    //Call the base-class routine
    retCode = CAknForm::ExecuteLD( aResourceId );

    TryChangeFocusToL(EIsvTelCallAppDlgCtrlIdPhone1);

    return( retCode );
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::OfferKeyEventL()
// An over-riding routine that captures the Green dial key and the Red hang-up
// key from the key-pad when they are pressed with the focus being present 
// on the Phone-numbers in the dialog screen.
// For other keys, it calls the base-class routine for handling.
// ----------------------------------------------------------------------------
//
TKeyResponse CIsvTelCallAppDlg::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType)
    {
    if (aType == EEventKey)
        {
        //Catch the dial button
        switch (aKeyEvent.iCode)
            {
            case EKeyYes:
                {
                TInt focusControlId = IdOfFocusControl();
                switch (focusControlId)
                    {
                    case EIsvTelCallAppDlgCtrlIdPhone1:
                    case EIsvTelCallAppDlgCtrlIdPhone2:
                   		{
                    	ProcessCommandL(EIsvTelCallAppCmdAPITestCall);
                        return EKeyWasConsumed;	
                    	}
                    default:
                        break;
                    }
                }
                break;
            case EKeyNo:
                {
                TInt focusControlId = IdOfFocusControl();
                switch (focusControlId)
                    {
                    case EIsvTelCallAppDlgCtrlIdPhone1:
                    case EIsvTelCallAppDlgCtrlIdPhone2:
                    	{
						ProcessCommandL(EIsvTelCallAppCmdAPITestHangup);
                        return EKeyWasConsumed;                    	
                    	}
                    default:
                        break;
                    }
                }
                break;
            case EKeyUpArrow:
            case EKeyDownArrow:
                {
                if( iIsInEditMode )
                    {
                    return EKeyWasConsumed;
                    }
                }
                break;
            case EKeyOK:
                {
                if(iIsInEditMode)
                    {
                    ProcessCommandL(EAknFormCmdSave);
                    }
                else
                    {
                    ProcessCommandL(EAknFormCmdEdit);
                    }
                return EKeyWasConsumed;
                }
            default:
                break;
            }
        }
    return CAknForm::OfferKeyEventL(aKeyEvent, aType);
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::DynInitMenuPaneL()
// Different controls/dialog items/pages on the multi-page dialog require their
// own context sensitive menus to be able to choose only the applicable
// operations on the item. For this purpose, this function, loads menu panes
// that are appropriate and applicable for the respective menu items.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {

    TInt pageId = ActivePageId();
    TInt focusControlId = IdOfFocusControl();

    if (aResourceId == R_AVKON_FORM_MENUPANE)  
        {
        switch (pageId)
            {
            case EIsvTelCallAppPageCtrlIdPagePhone1:
                {
                //Phone Menu
                switch (focusControlId)    
                    {
                    case EIsvTelCallAppDlgCtrlIdPhone1:
                    	{
						//Menu for Phone-1 and Phone-2 numbers
						aMenuPane->AddMenuItemsL(R_PHONE_MENU_PANE,0,1);
                        aMenuPane->SetItemDimmed(EAknFormCmdAdd,TRUE);
                        aMenuPane->SetItemDimmed(EAknFormCmdLabel,TRUE);
                        aMenuPane->SetItemDimmed(EAknFormCmdDelete,TRUE);
                        break;
                    	}
                    case EIsvTelCallAppDlgCtrlIdDtmfSeq1:
                    	{
						//Menu for DTMF sequence
                        aMenuPane->SetItemDimmed(EAknFormCmdAdd,TRUE);
                        aMenuPane->SetItemDimmed(EAknFormCmdLabel,TRUE);
                        aMenuPane->SetItemDimmed(EAknFormCmdDelete,TRUE);
                        break;                    	
                    	}
                    case EIsvTelCallAppDlgCtrlIdCall1Status:
                    	{
						aMenuPane->AddMenuItemsL(
                                    R_GENERAL_STATUS_MENU_PANE,0,1);
                        aMenuPane->SetItemDimmed(EAknFormCmdEdit,TRUE);
                        aMenuPane->SetItemDimmed(EAknFormCmdSave,TRUE);
                        aMenuPane->SetItemDimmed(EAknFormCmdAdd,TRUE);
                        aMenuPane->SetItemDimmed(EAknFormCmdLabel,TRUE);
                        aMenuPane->SetItemDimmed(EAknFormCmdDelete,TRUE);
                    	}
                    default:
                        break;
                    }
                }
                break;
            case EIsvTelCallAppPageCtrlIdPagePhone2:
                {
                //Phone Menu
                switch (focusControlId)    
                    {
                    case EIsvTelCallAppDlgCtrlIdPhone2:
                    	{
                    	//Menu for Phone-1 and Phone-2 numbers
                        aMenuPane->AddMenuItemsL(R_PHONE_MENU_PANE,0,1);
                        aMenuPane->SetItemDimmed(EAknFormCmdAdd,TRUE);
                        aMenuPane->SetItemDimmed(EAknFormCmdLabel,TRUE);
                        aMenuPane->SetItemDimmed(EAknFormCmdDelete,TRUE);
                        break;
                    	}
                    case EIsvTelCallAppDlgCtrlIdDtmfSeq2:
    	                {
                    	//Menu for DTMF sequence
                        aMenuPane->SetItemDimmed(EAknFormCmdAdd,TRUE);
                        aMenuPane->SetItemDimmed(EAknFormCmdLabel,TRUE);
                        aMenuPane->SetItemDimmed(EAknFormCmdDelete,TRUE);
                        break;
	                    }
                    case EIsvTelCallAppDlgCtrlIdCall2Status:
	                    {
                    	aMenuPane->AddMenuItemsL(
                                    R_GENERAL_STATUS_MENU_PANE,0,1);
                        aMenuPane->SetItemDimmed(EAknFormCmdEdit,TRUE);
                        aMenuPane->SetItemDimmed(EAknFormCmdSave,TRUE);
                        aMenuPane->SetItemDimmed(EAknFormCmdAdd,TRUE);
                        aMenuPane->SetItemDimmed(EAknFormCmdLabel,TRUE);
                        aMenuPane->SetItemDimmed(EAknFormCmdDelete,TRUE);
    	                }
                    default:
                        break;
                    }
                }
                break;
            case EIsvTelCallAppPageCtrlIdPageCallLineStatus:
    	        {
            	aMenuPane->AddMenuItemsL(
                            R_GENERAL_STATUS_MENU_PANE,0,1);
                aMenuPane->SetItemDimmed(EAknFormCmdEdit,TRUE);
                aMenuPane->SetItemDimmed(EAknFormCmdSave,TRUE);
                aMenuPane->SetItemDimmed(EAknFormCmdAdd,TRUE);
                aMenuPane->SetItemDimmed(EAknFormCmdLabel,TRUE);
                aMenuPane->SetItemDimmed(EAknFormCmdDelete,TRUE);
                break;
                }
            default:
                break;
            }
        }
    else
        {
    	CAknForm::DynInitMenuPaneL(aResourceId, aMenuPane);
        }    
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::ProcessCommandL()
// An over-riding function that handles all commands loaded by the
// DynInitMenuPaneL. For the remaining commands like Edit & Save, it invokes 
// the base class routine.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::ProcessCommandL( TInt aCommandId )
    {
    
    switch (aCommandId)
        {
        ////////////////////////////////////////////////////////////////////
        case EIsvTelCallAppCmdAPITestCall:
            {
            TInt afocusControlId = IdOfFocusControl();
            MakeNewCallProcessCommandL(afocusControlId);
            }
            break;
            
        ////////////////////////////////////////////////////////////////////
        case EIsvTelCallAppCmdAPITestHangup:
            {
            //Branch to the appropriate page and control
            TInt pageId = ActivePageId();
            HangupProcessCommandL(pageId);
            }
            break;
    
        ////////////////////////////////////////////////////////////////////
        case EIsvTelCallAppCmdAPITestHold:
            {
            //Branch to the appropriate page and control
            TInt pageId = ActivePageId();
            HoldProcessCommandL(pageId);
            }
            break;
            
        ////////////////////////////////////////////////////////////////////
        case EIsvTelCallAppCmdAPITestResume:
            {
            //Branch to the appropriate page and control
            TInt pageId = ActivePageId();
            ResumeProcessCommandL(pageId);
            }
            break;
                    
        ////////////////////////////////////////////////////////////////////
        case EIsvTelCallAppCmdAPITestSwap:
            {
            SwapProcessCommamndL();
            }
            break;
                
        ////////////////////////////////////////////////////////////////////
        case EIsvTelCallAppCmdAPITestFetch:
            {
            //Branch to the appropriate page and control
            TInt pageId = ActivePageId();
            switch (pageId)
                {
                /////////////////////////////////////////////////////////////
                case EIsvTelCallAppPageCtrlIdPagePhone1:
                	{
                	//Fetch information for call status control.  
                    //This helps user to stay updated
                    //all the time. These are synchronous calls
                    GetCallStatusL(CTelephony::EISVCall1);
                	}
                    break;

                /////////////////////////////////////////////////////////////
                case EIsvTelCallAppPageCtrlIdPagePhone2:
                	{
                	//Fetch information for call status control.  
                    //This helps user to stay updated
                    //all the time. These are synchronous calls
                    GetCallStatusL(CTelephony::EISVCall2);
                	}
                    break;
            		/////////////////////////////////////////////////////////////   
           		case EIsvTelCallAppPageCtrlIdPageCallLineStatus:
	                {
                	//Fetch EIsvTelCallAppDlgCtrlIdVoiceLineStatus:
                	iTelephony->TelApiWrapperGetVoiceLineStatusL( );
     
                	//Fetch EIsvTelCallAppDlgCtrlIdDataLineStatus:
                	iTelephony->TelApiWrapperGetDataLineStatusL( );
                                                                
                	//Fetch EIsvTelCallAppDlgCtrlIdFaxLineStatus:
                	iTelephony->TelApiWrapperGetFaxLineStatusL( );
                  }                
                  break;  
               /////////////////////////////////////////////////////////////
                default:
                    break;
                }//Switch pageId
            }//Fetch Case
            break;
        
        //////////// /////////////////////////////////////////////////     
        case EIsvTelCallAppCmdAPITestSendDTMF:
            {
            TInt focusControlId = IdOfFocusControl();  
            SendDTMFProcessCommandL(focusControlId);
            }
            break;

        ////////////////////////////////////////////////////////////////////
        case EAknFormCmdEdit:
            {
            iIsInEditMode = ETrue;
            }
            break;

        ////////////////////////////////////////////////////////////////////
        default:
            break;
        }

    //Call the base class routine
    CAknForm::ProcessCommandL(aCommandId);
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::PageChangedL()
// Derived from base-class. It invokes the base-class function and after the
// page switch is executed, it sets the appropriate title for the page.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::PageChangedL(TInt aPageId)
    {
    //Invoke the base class routine
    CAknForm::PageChangedL(aPageId);

    //Call SetPageTitleOnPaneL to set the appropriate title on pane
    SetPageTitleOnPaneL(aPageId);
    }
       
// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::SetDlgPage()
// Is called from the container class to set the start page.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::SetDlgPage(TInt aPageId)
    {
    iPageId = aPageId;
    }
           
// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::SetTitlePane()
// It is called by the container class that creates this dialog, to set a
// pointer to the Title pane. This is required to update the name of the
// current page to on the title pane when a page change happens in the dialog.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::SetTitlePane(CAknTitlePane* aTitlePane)
    {
    iTitlePane = aTitlePane;
    }
 
// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::SetContainer()
// It is called by the container class that creates this dialog, to set a
// pointer to itself. The dialog class uses this pointer from some command 
// completion handlers to bring the view back to foreground.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::SetContainer(CIsvTelCallAppContainer* aContainer)
    {
    iContainer = aContainer;
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::SetPageTitleOnPaneL()
// For each page, it sets the appropriate page title on the pane
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::SetPageTitleOnPaneL(TInt aPageId)
    {
    const TInt KTitlePaneBufLength = 256;
    TBuf<KTitlePaneBufLength> titleText( NULL );
    
    //Select the page title
    switch (aPageId)
        {
        case EIsvTelCallAppPageCtrlIdPagePhone1:
        	{
        	iEikonEnv->ReadResource( titleText, 
            	R_ISVTELCALLAPP_TEXT_STATUS_PANE_PHONE1 );
            break;
        	}
        case EIsvTelCallAppPageCtrlIdPagePhone2:
        	{
        	iEikonEnv->ReadResource( titleText, 
                R_ISVTELCALLAPP_TEXT_STATUS_PANE_PHONE2 );
            break;
        	}
        case EIsvTelCallAppPageCtrlIdPageCallLineStatus:
    	    {
        	iEikonEnv->ReadResource(titleText, 
                R_ISVTELCALLAPP_TEXT_STATUS_PANE_CALL_LINE_STATUS);
            break;
	        }
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
// CIsvTelCallAppDlg::SetMainPageTitleOnPaneL()
// Sets the Application's title on the pane
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::SetMainPageTitleOnPaneL( )
    {
    const TInt KTitlePaneBufLength = 256;
    TBuf<KTitlePaneBufLength> titleText( NULL );
    
    iEikonEnv->ReadResource(titleText, 
        R_ISVTELCALLAPP_TEXT_STATUS_PANE_MAIN_PAGE);
     
     //Set the appropriate title on the pane
    if (iTitlePane)
        {
        iTitlePane->SetTextL( titleText );
        }
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::MakeNewCallProcessCommandL()
// Dials a new call by invoking the CIsvTelCallAppTelephony object
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::MakeNewCallProcessCommandL(TInt aFocusControlId)
    {
    switch (aFocusControlId)
        {
        
        case EIsvTelCallAppDlgCtrlIdPhone1:
            {
            if (ControlOrNull(EIsvTelCallAppDlgCtrlIdPhone1))
                {
                CEikEdwin* edwinText = reinterpret_cast <CEikEdwin*> (ControlOrNull(EIsvTelCallAppDlgCtrlIdPhone1));
                edwinText->GetText(iEdwinPhone1);
                //GetEdwinText(iEdwinPhone1, EIsvTelCallAppDlgCtrlIdPhone1);
                iTelephony->TelApiWrapperMakeNewCallL(CTelephony::EISVCall1, 
                    iEdwinPhone1);
                }
            }
            break;
            
        case EIsvTelCallAppDlgCtrlIdPhone2:
            {
            if (ControlOrNull(EIsvTelCallAppDlgCtrlIdPhone2))
                {
                CEikEdwin* edwinText = reinterpret_cast <CEikEdwin*> (ControlOrNull(EIsvTelCallAppDlgCtrlIdPhone1));
                edwinText->GetText(iEdwinPhone2);
                //GetEdwinText(iEdwinPhone2, EIsvTelCallAppDlgCtrlIdPhone2);
                iTelephony->TelApiWrapperMakeNewCallL(CTelephony::EISVCall2,
                    iEdwinPhone2);
                }
            }
            break;
            
        default:
            break;
        }    
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::HoldProcessCommandL()
// Puts a call on hold by invoking the CIsvTelCallAppTelephony object
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::HoldProcessCommandL(TInt aPageId)
    {
    switch (aPageId)
        {
        /////////////////////////////////////////////////////////////
        case EIsvTelCallAppPageCtrlIdPagePhone1:
            //Hold Call 1
            {
            iTelephony->TelApiWrapperHoldL( CTelephony::EISVCall1 );
            }
            break;

        /////////////////////////////////////////////////////////////
        case EIsvTelCallAppPageCtrlIdPagePhone2:
            //Hold Call 2
            {
            iTelephony->TelApiWrapperHoldL( CTelephony::EISVCall2 );
            }
            break;
        }    
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::ResumeProcessCommandL()
// Resumes a call by invoking the CIsvTelCallAppTelephony object
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::ResumeProcessCommandL(TInt aPageId)
    {
    switch (aPageId)
        {
        /////////////////////////////////////////////////////////////
        case EIsvTelCallAppPageCtrlIdPagePhone1:
            //Resume Call 1
            {
            iTelephony->TelApiWrapperResumeL( CTelephony::EISVCall1 );
            }
            break;

        /////////////////////////////////////////////////////////////
        case EIsvTelCallAppPageCtrlIdPagePhone2:
            //Resume Call 2
            {
            iTelephony->TelApiWrapperResumeL( CTelephony::EISVCall2 );
            }
            break;
        }    
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::HangupProcessCommandL()
// Hangs-up a call by invoking the CIsvTelCallAppTelephony object
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::HangupProcessCommandL(TInt aPageId)
    {
    switch (aPageId)
        {
        /////////////////////////////////////////////////////////////
        case EIsvTelCallAppPageCtrlIdPagePhone1:
            //Hangup Call 1
            {
            iTelephony->TelApiWrapperHangupL( CTelephony::EISVCall1 );
            }
            break;

        /////////////////////////////////////////////////////////////
        case EIsvTelCallAppPageCtrlIdPagePhone2:
            //Hangup Call 1
            {
            iTelephony->TelApiWrapperHangupL( CTelephony::EISVCall2 );
            }
            break;
        }    
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::SendDTMFProcessCommandL()
// Sends a DTMF string on a call by invoking the CIsvTelCallAppTelephony 
// object
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::SendDTMFProcessCommandL(TInt aFocusControlId)
    {
    TBuf<KMaxDTMFLength> edwinData;
              
    switch (aFocusControlId)
        {
        case EIsvTelCallAppDlgCtrlIdPhone1:
            {
            CEikEdwin* edwinText = reinterpret_cast <CEikEdwin*> (ControlOrNull(EIsvTelCallAppDlgCtrlIdDtmfSeq1));
            edwinText->GetText(edwinData);
            //GetEdwinText(edwinData, EIsvTelCallAppDlgCtrlIdDtmfSeq1);  
    		if (edwinData.Length() > KMaxDTMFLength)
        		{
        		TBuf<KMaxErrMsgSize> errNoteMsg;
        		iCoeEnv->ReadResource(errNoteMsg,
            		R_ISVTELCALLAPP_UIERR_DTMFTOOLONG);

        		CAknErrorNote* errorNote = new (ELeave) CAknErrorNote;
        		errorNote->ExecuteLD(errNoteMsg);
        		}
    		else
        		{                   
        		iTelephony->TelApiWrapperSendDTMFL( CTelephony::EISVCall1,
        			edwinData);    
        		}        		
            }
            break;
            
        case EIsvTelCallAppDlgCtrlIdPhone2:
            {
            CEikEdwin* edwinText = reinterpret_cast <CEikEdwin*> (ControlOrNull(EIsvTelCallAppDlgCtrlIdDtmfSeq2));
            edwinText->GetText(edwinData);
            //GetEdwinText(edwinData, EIsvTelCallAppDlgCtrlIdDtmfSeq2);    
    		if (edwinData.Length() > KMaxDTMFLength)
        		{
        		TBuf<KMaxErrMsgSize> errNoteMsg;
        		iCoeEnv->ReadResource(errNoteMsg,
            		R_ISVTELCALLAPP_UIERR_DTMFTOOLONG);

        		CAknErrorNote* errorNote = new (ELeave) CAknErrorNote;
        		errorNote->ExecuteLD(errNoteMsg);
        		}
    		else
        		{                   
        		iTelephony->TelApiWrapperSendDTMFL( CTelephony::EISVCall2,
        			edwinData);    
        		}        		
            }
            break;
        
        default:    
            break;
        }
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::SwapProcessCommamnd()
// Swaps two calls by invoking the CIsvTelCallAppTelephony object
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::SwapProcessCommamndL()
    {
    //Swap Call1 and Call2
    //Format the string with command & parameters 
    iTelephony->TelApiWrapperSwapL( );    
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::GetCallDynCapsLockStatus()
// Fetches the Call status, Dynamic Capabilities and Line Lock status on Page-1
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::GetCallStatusL(const CTelephony::TCallId aCallId)
    {
    iTelephony->TelApiWrapperGetCallStatusL( aCallId );
    }        

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::MakeNewCallApiCompleteL()
// Command completion routine called from the CIsvTelCallAppTelephony class
// once the request to dial a new call is complete.
// It also implements the function of status auto-update by calling the
// respective status update routines.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::MakeNewCallApiCompleteL(
    TInt aStatus, 
    const CTelephony::TCallId aCallId,
    CTelephony::TTelNumber& aTelNum,
    TDesC16& aErrMsg)
    {
    if (aStatus != KErrNone)
        {
        TBuf<KMaxErrMsgSize> errorNoteMsg;
        errorNoteMsg.Copy(aErrMsg);
        CAknErrorNote* errorNote = new (ELeave) CAknErrorNote;
        errorNote->ExecuteLD(errorNoteMsg);
        }
    else
        {   

        //Set the Phone number to the page where the corresponding Call ID
        //is handled. If the first call is made on the second page ie., 
        //Phone-2, since it is the first call, the Call ID returned is 1
        //and hence, in the GUI, the call will have to be set to Phone-1
        //page. Also the focus will have to be changed to the corresponding
        //page.
        switch ((TInt)aCallId)
            {
            case CTelephony::EISVCall1:
                {
                if ( aTelNum == iEdwinPhone2 )
                    {
                    //Swap Phone1 and Phone2
                    iEdwinPhone2.Copy(iEdwinPhone1);
                    }
                iEdwinPhone1.Copy(aTelNum);
                
                if (iEdwinPhone1 == iEdwinPhone2)
                    {
                    //Set Phone-2 to NULL
                    _LIT(KNullStr,"");
                    iEdwinPhone2 = KNullStr;
                    }
                }
                SetEdwinTextL((TInt)EIsvTelCallAppDlgCtrlIdPhone1,
                    &iEdwinPhone1);
                SetEdwinTextL((TInt)EIsvTelCallAppDlgCtrlIdPhone2,
                    &iEdwinPhone2);
                TryChangeFocusToL(EIsvTelCallAppDlgCtrlIdPhone1);
                break;
                
            case CTelephony::EISVCall2:
                {
                iEdwinPhone2.Copy(aTelNum);
                SetEdwinTextL((TInt)EIsvTelCallAppDlgCtrlIdPhone2,
                    &iEdwinPhone2);
                TryChangeFocusToL(EIsvTelCallAppDlgCtrlIdPhone2);
                }
                break;

            default:
                break;
            }
        //Fetch status for both the call ids
        GetCallStatusL(CTelephony::EISVCall1);
        GetCallStatusL(CTelephony::EISVCall2);        
        }
    //Bring back the view to foreground
    iContainer->BringToForeGroundL();
    }
   
// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::HangupCompleteL()
// Command completion routine called from the CIsvTelCallAppTelephony class
// once the request to Hangup a call is complete
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::HangupCompleteL( TInt aStatus, TDesC16& aErrMsg)
    {
    if (aStatus != KErrNone)
        {
        TBuf<KMaxErrMsgSize> errNoteMsg; 
        errNoteMsg.Copy(aErrMsg);
 
        CAknErrorNote* errorNote = new (ELeave) CAknErrorNote;
        errorNote->ExecuteLD(errNoteMsg);
        }
    else
        {
        //Fetch status for both the call ids
        GetCallStatusL(CTelephony::EISVCall1);
        GetCallStatusL(CTelephony::EISVCall2);        
        }

    //Bring back the view to foreground
    iContainer->BringToForeGroundL();
    }
  
// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::HoldCompleteL()
// Command completion routine called from the CIsvTelCallAppTelephony class
// once the request to place a call on hold is complete
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::HoldCompleteL( TInt aStatus, TDesC16& aErrMsg)
    {
    if (aStatus != KErrNone)
        {
        TBuf<KMaxErrMsgSize> errNoteMsg;
        errNoteMsg.Copy(aErrMsg);

        CAknErrorNote* errorNote = new (ELeave) CAknErrorNote;
        errorNote->ExecuteLD(errNoteMsg);
        }
    else
        {
        //Fetch status for both the call ids
        GetCallStatusL(CTelephony::EISVCall1);
        GetCallStatusL(CTelephony::EISVCall2);        
        }

    //Bring back the view to foreground
    iContainer->BringToForeGroundL();
    }
 
// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::ResumeCompleteL()
// Command completion routine called from the CIsvTelCallAppTelephony class
// once the request to resume a call is complete
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::ResumeCompleteL( TInt aStatus, TDesC16& aErrMsg )
    {
    if (aStatus != KErrNone)
        {
        TBuf<KMaxErrMsgSize> errNoteMsg;
        errNoteMsg.Copy(aErrMsg);
 
        CAknErrorNote* errorNote = new (ELeave) CAknErrorNote;
        errorNote->ExecuteLD(errNoteMsg);
        }
    else
        {
        //Fetch status for both the call ids
        GetCallStatusL(CTelephony::EISVCall1);
        GetCallStatusL(CTelephony::EISVCall2);        
        }
        
    //Bring back the view to foreground
    iContainer->BringToForeGroundL();
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::SwapCompleteL()
// Command completion routine called from the CIsvTelCallAppTelephony class
// once the request to swap two calls is complete
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::SwapCompleteL(
    TInt aStatus,
    TDesC16& aErrMsg)
    {
    if (aStatus != KErrNone)
        {
        TBuf<KMaxErrMsgSize> errNoteMsg;
        errNoteMsg.Copy(aErrMsg);

        CAknErrorNote* errorNote = new (ELeave) CAknErrorNote;
        errorNote->ExecuteLD(errNoteMsg);
        }
    else
        {
        //Fetch status for both the call ids
        GetCallStatusL(CTelephony::EISVCall1);
        GetCallStatusL(CTelephony::EISVCall2);        
        }

    //Bring back the view to foreground
    iContainer->BringToForeGroundL();
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::GetCallStatusCompleteL()
// Command completion routine called from the CIsvTelCallAppTelephony class
// once the request to Get Call Status is complete
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::GetCallStatusCompleteL(
    TInt aStatus, 
    const CTelephony::TCallId aCallId,
    CTelephony::TCallStatusV1& aCallStatus,
    TDesC16& aErrMsg)
    {

    TBuf<EIsvTelCallAppEdwinMaxLength> callStatusStr;

    if (aStatus != KErrNone)
        {
        callStatusStr.Copy(iErrString);
        }
    else
        {
        switch (aCallStatus.iStatus)
            {
            case CTelephony::EStatusUnknown:
                {
                callStatusStr.Copy(iUnknownStr);
                }
                break;
                
            case CTelephony::EStatusIdle:
                {
                callStatusStr.Copy(iIdleStr);
                }
                break;
                
            case CTelephony::EStatusDialling:
                {
                callStatusStr.Copy(iDiallingStr);
                }
                break;
                
            case CTelephony::EStatusRinging:
                {
                callStatusStr.Copy(iRingingStr);
                }
                break;
                
            case CTelephony::EStatusAnswering:
                {
                callStatusStr.Copy(iAnsweringStr);
                }
                break;
                
            case CTelephony::EStatusConnecting:
                {
                callStatusStr.Copy(iConnectingStr);
                }
                break;
                
            case CTelephony::EStatusConnected:
                {
                callStatusStr.Copy(iConnectedStr);
                }
                break;
                
            case CTelephony::EStatusReconnectPending:
                {
                callStatusStr.Copy(iReconnectPendingStr);
                }
                break;

            case CTelephony::EStatusDisconnecting:
                {
                callStatusStr.Copy(iDisconnectingStr);
                }
                break;

            case CTelephony::EStatusHold:
                {
                callStatusStr.Copy(iHoldStr);
                }
                break;

            case CTelephony::EStatusTransferring:
                {
                callStatusStr.Copy(iTransferringStr);
                }
                break;

            case CTelephony::EStatusTransferAlerting:
                {
                callStatusStr.Copy(iTransferAlertingStr);
                }
                break;

            default:
                {
                callStatusStr.Copy(iErrString);
                }
                break;
            }
        }
	//Update the UI
    switch (aCallId)
        {
        case CTelephony::EISVCall1:
            {
            if ( aStatus != KErrNone )
                {
                iEdwinCall1Status.Copy(aErrMsg);
                }
            else
                {
                iEdwinCall1Status.Copy(callStatusStr);
                }
            SetEdwinTextL((TInt)EIsvTelCallAppDlgCtrlIdCall1Status,
                &iEdwinCall1Status);
            }
            break;
            
        case CTelephony::EISVCall2:
            {
            if ( aStatus != KErrNone )
                {
                iEdwinCall2Status.Copy(aErrMsg);
                }
            else
                {
                iEdwinCall2Status.Copy(callStatusStr);
                }
            SetEdwinTextL((TInt)EIsvTelCallAppDlgCtrlIdCall2Status,
                &iEdwinCall2Status);
            }
            break;
            
        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::GetVoiceLineStatusCompleteL()
// Command completion routine called from the CIsvTelCallAppTelephony class
// once the request to Get Voice Line Status is complete
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::GetVoiceLineStatusCompleteL(
    TInt aStatus, 
    TDesC16& aErrMsg, 
    CTelephony::TCallStatusV1& aCallStatus)
    {
    if (aStatus != KErrNone)
        {
        iEdwinVoiceLineStatus.Copy(aErrMsg);
        SetEdwinTextL((TInt)EIsvTelCallAppDlgCtrlIdVoiceLineStatus,
            &iEdwinVoiceLineStatus);
        }
    else
        {
        TBuf<EIsvTelCallAppEdwinMaxLength> callStatusStr;
        switch (aCallStatus.iStatus)
            {
            case CTelephony::EStatusUnknown:
                {
                callStatusStr.Copy(iUnknownStr);
                }
                break;
            case CTelephony::EStatusIdle:
                {
                callStatusStr.Copy(iIdleStr);
                }
                break;
            case CTelephony::EStatusDialling:
                {
                callStatusStr.Copy(iDiallingStr);
                }
                break;
            case CTelephony::EStatusRinging:
                {
                callStatusStr.Copy(iRingingStr);
                }
                break;
            case CTelephony::EStatusAnswering:
                {
                callStatusStr.Copy(iAnsweringStr);
                }
                break;
            case CTelephony::EStatusConnecting:
                {
                callStatusStr.Copy(iConnectingStr);
                }
                break;
            case CTelephony::EStatusConnected:
                {
                callStatusStr.Copy(iConnectedStr);
                }
                break;
            case CTelephony::EStatusReconnectPending:
                {
                callStatusStr.Copy(iReconnectPendingStr);
                }
                break;
            case CTelephony::EStatusDisconnecting:
                {
                callStatusStr.Copy(iDisconnectingStr);
                }
                break;
            case CTelephony::EStatusHold:
                {
                callStatusStr.Copy(iHoldStr);
                }
                break;
            case CTelephony::EStatusTransferring:
                {
                callStatusStr.Copy(iTransferringStr);
                }
                break;
            case CTelephony::EStatusTransferAlerting:
                {
                callStatusStr.Copy(iTransferAlertingStr);
                }
                break;
            default:
                {
                callStatusStr.Copy(iErrString);
                }
                break;
            }

        //Update line status to UI
        iEdwinVoiceLineStatus.Copy(callStatusStr);
        SetEdwinTextL((TInt)EIsvTelCallAppDlgCtrlIdVoiceLineStatus,
                                           &iEdwinVoiceLineStatus);
       // Register the Call 1 and 2 Status Active Object
        iCall1StatusAsyncTelephony->
            RegisterForAsyncNotification(
            CTelephony::EOwnedCall1StatusChange);
        iCall2StatusAsyncTelephony->
            RegisterForAsyncNotification(
            CTelephony::EOwnedCall2StatusChange);

        // Invoke Answer Incoming Call Wrapper function, 
        // if voice line status is ringing
        if ( aCallStatus.iStatus  == CTelephony::EStatusRinging )
            {
            iAnswerInCallTelephony->TelApiWrapperAnswerInCallL();
            } 
		}
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::GetDataLineStatusCompleteL()
// Command completion routine called from the CIsvTelCallAppTelephony class
// once the request to Get Data Line Status is complete
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::GetDataLineStatusCompleteL(
    TInt aStatus, 
    TDesC16& aErrMsg,
    CTelephony::TCallStatusV1& aCallStatus)
    {
    if (aStatus != KErrNone)
        {
        iEdwinDataLineStatus.Copy(aErrMsg);
        SetEdwinTextL((TInt)EIsvTelCallAppDlgCtrlIdDataLineStatus,
            &iEdwinDataLineStatus);
        }
    else
        {
        TBuf<EIsvTelCallAppEdwinMaxLength> callStatusStr;
        switch (aCallStatus.iStatus)
            {
            case CTelephony::EStatusUnknown:
                {
                callStatusStr.Copy(iUnknownStr);
                }
                break;
            case CTelephony::EStatusIdle:
                {
                callStatusStr.Copy(iIdleStr);
                }
                break;
            case CTelephony::EStatusDialling:
                {
                callStatusStr.Copy(iDiallingStr);
                }
                break;
            case CTelephony::EStatusRinging:
                {
                callStatusStr.Copy(iRingingStr);
                }
                break;
            case CTelephony::EStatusAnswering:
                {
                callStatusStr.Copy(iAnsweringStr);
                }
                break;
            case CTelephony::EStatusConnecting:
                {
                callStatusStr.Copy(iConnectingStr);
                }
                break;
            case CTelephony::EStatusConnected:
                {
                callStatusStr.Copy(iConnectedStr);
                }
                break;
            case CTelephony::EStatusReconnectPending:
                {
                callStatusStr.Copy(iReconnectPendingStr);
                }
                break;
            case CTelephony::EStatusDisconnecting:
                {
                callStatusStr.Copy(iDisconnectingStr);
                }
                break;
            case CTelephony::EStatusHold:
                {
                callStatusStr.Copy(iHoldStr);
                }
                break;
            case CTelephony::EStatusTransferring:
                {
                callStatusStr.Copy(iTransferringStr);
                }
                break;
            case CTelephony::EStatusTransferAlerting:
                {
                callStatusStr.Copy(iTransferAlertingStr);
                }
                break;
            default:
                {
                callStatusStr.Copy(iErrString);
                }
                break;
            }
        //Update line status to UI
        iEdwinDataLineStatus.Copy(callStatusStr);
        SetEdwinTextL((TInt)EIsvTelCallAppDlgCtrlIdDataLineStatus,
                                           &iEdwinDataLineStatus);
        }    
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::GetFaxLineStatusCompleteL()
// Command completion routine called from the CIsvTelCallAppTelephony class
// once the request to Get Fax Line Status is complete
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::GetFaxLineStatusCompleteL(
    TInt aStatus, 
    TDesC16& aErrMsg,
    CTelephony::TCallStatusV1& aCallStatus)
    {

    if (aStatus != KErrNone)
        {
        iEdwinFaxLineStatus.Copy(aErrMsg);
        SetEdwinTextL((TInt)EIsvTelCallAppDlgCtrlIdFaxLineStatus,
            &iEdwinFaxLineStatus);
	    }
    else
        {
        TBuf<EIsvTelCallAppEdwinMaxLength> callStatusStr;
        switch (aCallStatus.iStatus)
            {
            case CTelephony::EStatusUnknown:
                {
                callStatusStr.Copy(iUnknownStr);
                }
                break;
            case CTelephony::EStatusIdle:
                {
                callStatusStr.Copy(iIdleStr);
                }
                break;
            case CTelephony::EStatusDialling:
                {
                callStatusStr.Copy(iDiallingStr);
                }
                break;
            case CTelephony::EStatusRinging:
                {
                callStatusStr.Copy(iRingingStr);
                }
                break;
            case CTelephony::EStatusAnswering:
                {
                callStatusStr.Copy(iAnsweringStr);
                }
                break;
            case CTelephony::EStatusConnecting:
                {
                callStatusStr.Copy(iConnectingStr);
                }
                break;
            case CTelephony::EStatusConnected:
                {
                callStatusStr.Copy(iConnectedStr);
                }
                break;
            case CTelephony::EStatusReconnectPending:
                {
                callStatusStr.Copy(iReconnectPendingStr);
                }
                break;
            case CTelephony::EStatusDisconnecting:
                {
                callStatusStr.Copy(iDisconnectingStr);
                }
                break;
            case CTelephony::EStatusHold:
                {
                callStatusStr.Copy(iHoldStr);
                }
                break;
            case CTelephony::EStatusTransferring:
                {
                callStatusStr.Copy(iTransferringStr);
                }
                break;
            case CTelephony::EStatusTransferAlerting:
                {
                callStatusStr.Copy(iTransferAlertingStr);
                }
                break;
            default:
                {
                callStatusStr.Copy(iErrString);
                }
                break;
            }
        //Update Line status to UI
        iEdwinFaxLineStatus.Copy(callStatusStr);
        SetEdwinTextL((TInt)EIsvTelCallAppDlgCtrlIdFaxLineStatus,
            &iEdwinFaxLineStatus);
        }    
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::SendDTMFApiCompleteL()
// Command completion routine called from the CIsvTelCallAppTelephony class
// once the request to Send a DTMF string is complete
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::SendDTMFApiCompleteL(TInt aStatus, TDesC16& aErrMsg)
    {
    
    if (aStatus != KErrNone)
        {
        TBuf<KMaxErrMsgSize> errNoteMsg;
        errNoteMsg.Copy(aErrMsg);
        CAknErrorNote* errorNote = new (ELeave) CAknErrorNote;
        errorNote->ExecuteLD(errNoteMsg);
        }
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::AnswerIncomingCallApiCompleteL()
// Command completion routine called from the CIsvTelCallAppTelephony class
// once the request to Get Id Service status is complete
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::AnswerIncomingCallApiCompleteL(
    TInt aStatus, 
    const CTelephony::TCallId aCallId,
    TDesC16& aErrMsg)
    {
    if (aStatus == KErrNone)
        {
        // Getting Call Status here, just to display on Dialog
        TInt retCode;
        retCode = iAnswerInCallTelephony->iTel->GetCallStatus( aCallId, 
            iAnswerInCallTelephony->iCallStatusPckg );

        //Find the type of error message and store in errMsgBuffer.
        TBuf16<KMaxErrMsgSize> errMsgBuffer;
        errMsgBuffer.Copy(aErrMsg);
        iAnswerInCallTelephony->GeneralErrorMessage( retCode, &errMsgBuffer );

        GetCallStatusCompleteL( retCode, 
            aCallId,
            iAnswerInCallTelephony->iCallStatus,
            errMsgBuffer);       
         
        if((TInt)aCallId == CTelephony::EISVCall1 )
            {
            //Writing Incoming Call1 on dialog
            SetEdwinTextL((TInt) EIsvTelCallAppDlgCtrlIdPhone1,
                &iAnswerInCallPhNum1);
            // Registering here to update dialog fields, based on this 
            // answered call
            iCall1StatusAsyncTelephony->RegisterForAsyncNotification(
                CTelephony::EOwnedCall1StatusChange);
            }
        else
            {
            //Writing Incoming Call2 on dialog
            SetEdwinTextL((TInt) EIsvTelCallAppDlgCtrlIdPhone2,
                &iAnswerInCallPhNum2);
            iCall2StatusAsyncTelephony->RegisterForAsyncNotification(
                CTelephony::EOwnedCall2StatusChange);
            }
        }
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::CIsvTelCallAppDlg()
// Default constructor.
// ----------------------------------------------------------------------------
//
CIsvTelCallAppDlg::CIsvTelCallAppDlg()
    {
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::ConstructL()
// Second-phase constructor.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::ConstructL()
    {
    iTitlePane = NULL;

    iIsAnswerInCall = ETrue;
    
    //Load strings from resource
    LoadStringsFromResource();
        
    CAknForm::ConstructL();

    // Create the telephony object for execution from GUI
    iTelephony = CIsvTelCallAppTelephony::NewL(*this);
    iAnswerInCallTelephony = CIsvTelCallAppTelephony::NewL(*this);
    iVoiceLineAsyncTelephony = CIsvTelCallAppTelephony::NewL(*this);
    iCall1StatusAsyncTelephony = CIsvTelCallAppTelephony::NewL(*this);
    iCall2StatusAsyncTelephony = CIsvTelCallAppTelephony::NewL(*this);
        
#ifdef __WINS__	

#else
    iTelephony->CreateTelephonyPtrL();

    iAnswerInCallTelephony->SetTelephonyPtr(iTelephony->GetTelephonyPtr());
       
    iVoiceLineAsyncTelephony->SetTelephonyPtr(
     	iTelephony->GetTelephonyPtr());
    iVoiceLineAsyncTelephony->RegisterForAsyncNotification(
        CTelephony::EVoiceLineStatusChange );
            
    iCall1StatusAsyncTelephony->SetTelephonyPtr(
       	iTelephony->GetTelephonyPtr());
        	
    iCall2StatusAsyncTelephony->SetTelephonyPtr(
      	iTelephony->GetTelephonyPtr());
#endif           	
            
    //Initilalise the delimiter string
    _LIT8(KSpaceStr," ");
    iDelimiterStr.Copy(KSpaceStr);

    //Initialise the new line string
    _LIT8(KNewLineStr,"\n");
    iNewLineStr.Format(KNewLineStr);
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::PostLayoutDynInitL()
// An over-riding routine which Initialises values to certain default fields
// Also calls the base-class routine
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::PostLayoutDynInitL()
    {
    //Call the base class routine
    CAknForm::PostLayoutDynInitL();

    //Set edit mode to false
    iIsInEditMode = EFalse;
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::QuerySaveChangesL()
// Show save query. If user answers "No" to this query.
// return field value to the value which is before editing.
// This is called Back-softkey when edit mode.
// ----------------------------------------------------------------------------
//
TBool CIsvTelCallAppDlg::QuerySaveChangesL()
    {
    TBool isAnsYes(CAknForm::QuerySaveChangesL());

    if (isAnsYes)
        {
        SaveFormDataL();
        }
    else 
        {
        // Case that answer "No" to query.
        DoNotSaveFormDataL();
        }
    return isAnsYes;
    }
       
// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::SaveFormDataL()
// Save form data.
// ----------------------------------------------------------------------------
//
TBool CIsvTelCallAppDlg::SaveFormDataL()
    {
    CEikEdwin* edwinText1 = reinterpret_cast <CEikEdwin*> (ControlOrNull(EIsvTelCallAppDlgCtrlIdPhone1));
    edwinText1->GetText(iEdwinPhone1);
    //GetEdwinText(iEdwinPhone1,          EIsvTelCallAppDlgCtrlIdPhone1);
    
    CEikEdwin* edwinText2 = reinterpret_cast <CEikEdwin*> (ControlOrNull(EIsvTelCallAppDlgCtrlIdPhone2));
    edwinText2->GetText(iEdwinPhone2);
    //GetEdwinText(iEdwinPhone2,          EIsvTelCallAppDlgCtrlIdPhone2);
    
    CEikEdwin* edwinText3 = reinterpret_cast <CEikEdwin*> (ControlOrNull(EIsvTelCallAppDlgCtrlIdDtmfSeq1));
    edwinText3->GetText(iEdwinDtmfSeq1);
    //GetEdwinText(iEdwinDtmfSeq1,        EIsvTelCallAppDlgCtrlIdDtmfSeq1);
    
    CEikEdwin* edwinText4 = reinterpret_cast <CEikEdwin*> (ControlOrNull(EIsvTelCallAppDlgCtrlIdDtmfSeq2));
    edwinText4->GetText(iEdwinDtmfSeq2);
    //GetEdwinText(iEdwinDtmfSeq2,        EIsvTelCallAppDlgCtrlIdDtmfSeq2);

    //Set edit mode to false
    iIsInEditMode = EFalse;
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::DoNotSaveFormDataL()
// Does not save form data.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::DoNotSaveFormDataL()
    {
    SetEdwinTextL(EIsvTelCallAppDlgCtrlIdPhone1,       &iEdwinPhone1);
    SetEdwinTextL(EIsvTelCallAppDlgCtrlIdPhone2,       &iEdwinPhone2);
    SetEdwinTextL(EIsvTelCallAppDlgCtrlIdDtmfSeq1,     &iEdwinDtmfSeq1);
    SetEdwinTextL(EIsvTelCallAppDlgCtrlIdDtmfSeq2,     &iEdwinDtmfSeq2);

    //Set edit mode to false
    iIsInEditMode = EFalse;
    }
     
// ----------------------------------------------------------------------------
// CIsvTelCallAppDlg::LoadStringsFromResource()
// Loads strings from resource
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppDlg::LoadStringsFromResource( )
    {
    TBuf16<KMaxCmdSize> resStr;

    //Load general error and success strings
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELCALLAPP_TELSTR_ERRSTRING);
    iErrString.Copy(resStr);

    iCoeEnv->ReadResource(resStr, 
        R_ISVTELCALLAPP_TELSTR_SUCCESSSTRING);
    iSuccessString.Copy(resStr);
    
    //Load GetCallStatus strings from resource
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELCALLAPP_UNKNOWN);
    iUnknownStr.Copy(resStr);
    
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELCALLAPP_CALLSTATUS_IDLE);
    iIdleStr.Copy(resStr);
    
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELCALLAPP_CALLSTATUS_DIALLING);
    iDiallingStr.Copy(resStr);
    
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELCALLAPP_CALLSTATUS_RINGING);
    iRingingStr.Copy(resStr);
    
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELCALLAPP_CALLSTATUS_ANSWERING);
    iAnsweringStr.Copy(resStr);
    
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELCALLAPP_CALLSTATUS_CONNECTING);
    iConnectingStr.Copy(resStr);
    
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELCALLAPP_CALLSTATUS_CONNECTED);
    iConnectedStr.Copy(resStr);
    
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELCALLAPP_CALLSTATUS_RECONNECTPENDING);
    iReconnectPendingStr.Copy(resStr);
    
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELCALLAPP_CALLSTATUS_DISCONNECTING);
    iDisconnectingStr.Copy(resStr);

    iCoeEnv->ReadResource(resStr, 
        R_ISVTELCALLAPP_CALLSTATUS_HOLD);
    iHoldStr.Copy(resStr);

    iCoeEnv->ReadResource(resStr, 
        R_ISVTELCALLAPP_CALLSTATUS_TRANSFERRING);
    iTransferringStr.Copy(resStr);

    iCoeEnv->ReadResource(resStr, 
        R_ISVTELCALLAPP_CALLSTATUS_TRANSFERALERTING);
    iTransferAlertingStr.Copy(resStr);  
    
    //Answer Incoming Call Phone Number
    iCoeEnv->ReadResource(resStr, 
        R_ISVTELCALLAPP_TELSTR_ANSWERINCALLPHONENUM1);
    iAnswerInCallPhNum1.Copy(resStr);  

    iCoeEnv->ReadResource(resStr, 
        R_ISVTELCALLAPP_TELSTR_ANSWERINCALLPHONENUM2);
    iAnswerInCallPhNum2.Copy(resStr);  

	iCoeEnv->ReadResource(iEmuApiNotSupported,
	    R_ISVTELCALLAPP_ERR_EMUAPINOTSUPPORTED);        

    return;
    }
    
    

//End of file
