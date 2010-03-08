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
*
*/

// INCLUDE FILES
#include <E32def.h>
#include <avkon.hrh>
#include <AknViewAppUi.h>
#include <AknsUtils.h>
#include <stringloader.h>
#include <aknnotewrappers.h>
#include <eikmenup.h>
#include <EPos_CPosLandmarkDatabase.h>
#include <lmuirefapp.rsg>
#include "lmuirefapp.hrh"
#include "CLmkLandmarkSelectorDlg.h"
#include "CLmkCategorySelectorDlg.h"
#include "LmUiRefAppDocument.h"
#include "LmUiRefAppContainer.h"
#include "LmUiRefAppUi.h"
#include "LmUiRefAppView.h"
#include "CLmkEditorDlg.h"
#include "TLmkItemIdDbCombiInfo.h"		//For multi db support, combiinfo struct

#define LMK_PARAMS (CLmkEditorDlg::ELmkOnlyName | CLmkEditorDlg::ELmkCategory | CLmkEditorDlg::ELmkStreet | CLmkEditorDlg::ELmkCity | CLmkEditorDlg::ELmkLatitude| CLmkEditorDlg::ELmkLongitude)

#define LMK_PARAMS_DEFAULT CLmkEditorDlg::ELmkAll

// ================= MEMBER FUNCTIONS =======================
//-------------------------------------------------------------------------
// CLmUiRefAppView::NewLC
// Two-phased constructor.
//-------------------------------------------------------------------------
//
CLmUiRefAppView* CLmUiRefAppView::NewLC() 
    {
    CLmUiRefAppView* self =
        new( ELeave ) CLmUiRefAppView();
    CleanupStack::PushL( self );	
    self->ConstructL();      
    return self;
    }

//---------------------------------------------------------------------------
// CLmUiRefAppView::CLmUiRefAppView
// C++ default constructor can NOT contain any code, that
// might leave.
//---------------------------------------------------------------------------
//
CLmUiRefAppView::CLmUiRefAppView()
    {
    }

// --------------------------------------------------------------------------
// CLmUiRefAppView::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CLmUiRefAppView::ConstructL()
    {
    BaseConstructL(R_LMUIREFAPP_VIEW);   
    }

// --------------------------------------------------------------------------
// CLmUiRefAppView::~CLmUiRefAppView
// ------------------------------------------------------------------------
//
CLmUiRefAppView::~CLmUiRefAppView()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }
    delete iContainer; 
    }

// --------------------------------------------------------------------------
// TUid CLmUiRefAppView::Id
// --------------------------------------------------------------------------
//
TUid CLmUiRefAppView::Id() const
    {
    return TUid::Uid(ELmUiRefAppView);
    }

// --------------------------------------------------------------------------
// CLmUiRefAppView::HandleCommandL
// --------------------------------------------------------------------------
//
void CLmUiRefAppView::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        // these all are same: exit
        case EAknSoftkeyExit:
        case EAknSoftkeyBack:
			{
			AppUi()->HandleCommandL(EEikCmdExit);
			break;            
			}
  	    case ELmUiRefAppCmdDeleteLm:
			{ 
			if (IsLmDbEmptyL()) 
			    {
			    // Display some Error Note here
			    iContainer->DrawNow();
			    return;  	  	
			    }
			if (LaunchLandmarksDialogL() == 0) 
			    {
			    // If Selector is cancelled, return to MAIN view
			    iContainer->DrawNow();
			    return;
			    }

			// Display Note 
			CEikonEnv* env = CEikonEnv::Static();
			HBufC* noteText = NULL;
			noteText 
			   = StringLoader::LoadLC( R_LMUIREFAPP_DELETE_LANDMARK, env );
			CAknQueryDialog* query =
			          new( ELeave ) CAknQueryDialog( *noteText ); 
			TInt ret = query->ExecuteLD( R_LMUIREFAPP_QUERY);
			if (!(ret)) 
			    {
			    CleanupStack::PopAndDestroy(noteText); //noteText
			    iContainer->DrawNow();
			    return;
			    } 
			TPosLmItemId delLmItem = SelectedItem();
			ilDb->RemoveLandmarkL(delLmItem);
			delete ilDb;
			CleanupStack::PopAndDestroy(noteText); //noteText    
			break;
			}

    	case ELmUiRefAppCmdNewLm :
			{
			//if to be used later on or commented because of an Error.
			NewLmCmdL();             
			break;
			}

    	case ELmUiRefAppCmdViewLm:
			{
			/* Launch LM Selector only when Landmarks 
			* Database has any entries 
			*/
			if (IsLmDbEmptyL()) 
			    {
			    // Display some Error Note here
			    iContainer->DrawNow();
			    return;  	  	
			    }
			if (LaunchLandmarksDialogL() == 0) 
			    {
			    // If Selector is cancelled, return to MAIN view
			    iContainer->DrawNow();
			    return;
			    }

			CLmkEditorDlg::TLmkEditorParams editParams; 
			editParams.iAttributes = CLmkEditorDlg::ELmkAll;
			editParams.iEditorMode = CLmkEditorDlg::ELmkViewer;
			TPosLmItemId lmItem = SelectedItem();	 
			CLmkEditorDlg *dlg = CLmkEditorDlg::NewL(*ilDb, 
			                                            lmItem, 
			                                            editParams);
			dlg->ExecuteLD(); 
			iContainer->DrawNow();
			delete ilDb;
			break;
			}

    	case ELmUiRefAppCmdEditLm:
			{
			if (IsLmDbEmptyL()) 
			    {
			    // Display some Error Note here
			    iContainer->DrawNow();
			    return;  	  	
				}
			if (LaunchLandmarksDialogL() == 0) 
				{
				// If Selector is cancelled, return to MAIN view
				iContainer->DrawNow();		   
			    return;
				}
			CPosLandmark* landmarkedi = NULL;
			ExecuteLmEditorL(landmarkedi, *ilDb);
			// Refresh
			iContainer->DrawNow();			  
			delete ilDb;
			break;
            }               
		case ELmUiRefAppCmdMultiLmSelector:		
            {
            if ( IsLmDbEmptyL() ) 
            	{
            	// Display some Error Note here
            	iContainer->DrawNow();
            	return;  	  	
            	}
            	
            if ( LaunchMultiLandmarkSelectorDialogL() == 0 ) 
            	{
            	// If Selector is cancelled, return to MAIN view
            	iContainer->DrawNow();		   
            	return;
            	}	
            break;     			
            }				
		case ELmUiRefAppCmdMultiCatSelector:		
    		{
    		if ( IsLmDbEmptyL() ) 
    			{
    			// Display some Error Note here
    			iContainer->DrawNow();
    			return;  	  	
    			}
    			
    		if ( LaunchMultiCategorySelectorDialogL() == 0 ) 
    			{
    			// If Selector is cancelled, return to MAIN view
    			iContainer->DrawNow();		   
    			return;
    			}	
    		break;     			
    		}		
		case ELmUiRefAppCmdSingleCatSelector:		
    		{
    		if ( IsLmDbEmptyL() ) 
    			{
    			// Display some Error Note here
    			iContainer->DrawNow();
    			return;  	  	
    			}
    			
    		if ( LaunchSingleCategoeyDialogL() == 0 ) 
    			{
    			// If Selector is cancelled, return to MAIN view
    			iContainer->DrawNow();		   
    			return;
    			}
    		// Refresh view if needed
			iContainer->DrawNow();			  
			delete ilDb;
			break;
			}
        default: 
    	   break;
    	}
    }

// --------------------------------------------------------------------------
// CLmUiRefAppView::NewLmCmdL()
// --------------------------------------------------------------------------
//
void CLmUiRefAppView::NewLmCmdL()
    {
    CPosLandmark* landmark = CPosLandmark::NewL();    
    CleanupStack::PushL( landmark );       
	CLmUiRefAppUi* lmUiAppUi = static_cast<CLmUiRefAppUi*>( ViewAppUi() ); 
	CPosLandmarkDatabase& iaDb = (lmUiAppUi->Document().LmDb());             
    ExecuteLmEditorL( landmark, iaDb );
    CleanupStack::PopAndDestroy( landmark );
    }

// -----------------------------------------------------------------------------
// CLmUiRefAppView::ExecuteLmEditorL() 
// -----------------------------------------------------------------------------
void CLmUiRefAppView::ExecuteLmEditorL(
    CPosLandmark* aLandmark, CPosLandmarkDatabase& aDb)
    {
	CLmkEditorDlg::TLmkEditorParams editParams;
    editParams.iAttributes = LMK_PARAMS_DEFAULT; 
    editParams.iEditorMode = CLmkEditorDlg::ELmkEditor;
	TPosLmItemId lmItem = SelectedItem();
	if (aLandmark)
	    {
	    CLmkEditorDlg *dlg = CLmkEditorDlg::NewL(aDb, *aLandmark, editParams);
        dlg->ExecuteLD();
        }
	else 
        {	 
		CLmkEditorDlg *dlg = CLmkEditorDlg::NewL(aDb, lmItem, editParams);
		dlg->ExecuteLD();
        }
    }
    
// --------------------------------------------------------------------------
// CLmUiRefAppView::DoActivateL
// --------------------------------------------------------------------------
//
void CLmUiRefAppView::DoActivateL(
    const TVwsViewId& /*aPrevViewId*/,
    TUid /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/ )
    {
    if ( !iContainer )
        {
        iContainer = CLmUiRefAppContainer::NewL(this, ClientRect() );
    	AppUi()->AddToStackL( *this, iContainer ); 
        }
    }

// --------------------------------------------------------------------------
// CLmUiRefAppView::DoDeactivate
// --------------------------------------------------------------------------
//
void CLmUiRefAppView::DoDeactivate()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }
    delete iContainer;
    iContainer = NULL;
    }

// --------------------------------------------------------------------------
// CLmUiRefAppView::LaunchLandmarksDialogL()
// --------------------------------------------------------------------------
//
TInt CLmUiRefAppView::LaunchLandmarksDialogL()
    {   
    // Launch LMK Selector
	iLmkSelectDlg = CLmkLandmarkSelectorDlg::NewL();
	iLmkSelectDlg->SetMopParent(this);    
    TLmkItemIdDbCombiInfo selItem;    
	TInt retVal = iLmkSelectDlg->ExecuteLD( selItem );		 
    if (retVal !=0) 
        {
        iSelectedItem = selItem.GetItemId();       
        ilDb = selItem.GetLmDb(); 
        }     	
   return retVal;     
   }

// --------------------------------------------------------------------------
// CLmUiRefAppView::ViewAppUi()
// --------------------------------------------------------------------------
//
CAknViewAppUi* CLmUiRefAppView::ViewAppUi()
    {
    return static_cast<CAknViewAppUi*>( iAvkonAppUi );
    }

// --------------------------------------------------------------------------
// CLmUiRefAppView::SelectedItem()
// --------------------------------------------------------------------------
//
TPosLmItemId CLmUiRefAppView::SelectedItem() const
    {
    return iSelectedItem;
    }
    
// --------------------------------------------------------------------------
// CLmUiRefAppView::IsLmDbEmptyL()
// --------------------------------------------------------------------------
//
TBool CLmUiRefAppView::IsLmDbEmptyL()
    {
	CPosLmItemIterator* itemIterator = NULL;CLmUiRefAppUi* lmUiAppUi = static_cast<CLmUiRefAppUi*>( ViewAppUi() ); 
	CPosLandmarkDatabase& iaDb = lmUiAppUi->Document().LmDb();
	
	itemIterator = iaDb.LandmarkIteratorL();

	CleanupStack::PushL(itemIterator);	
	itemIterator->Reset();	   
	TUint nrOfItems = itemIterator->NumOfItemsL();
	CleanupStack::PopAndDestroy(itemIterator);
	if (nrOfItems == 0)
	    {
	    return ETrue;
	    }
	return EFalse;
    }

// ---------------------------------------------------------
// CLmkByLmView::DynInitMenuPaneL
// ---------------------------------------------------------
//
void CLmUiRefAppView::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {    
    switch ( aResourceId )
        {        
        case R_LMUIREFAPP_MENU1:
            {
  		    if (IsLmDbEmptyL())
		        {
                // Show NEW LM Menu Item
                aMenuPane->SetItemDimmed( ELmUiRefAppCmdEditLm, ETrue );
                aMenuPane->SetItemDimmed( ELmUiRefAppCmdViewLm, ETrue );
                aMenuPane->SetItemDimmed( ELmUiRefAppCmdDeleteLm, ETrue );
                }
		    break;
            }
        default:
            {
            break;
            }
        }    
    }    
// --------------------------------------------------------------------------
// CLmUiRefAppView::LaunchMultiLandmarkSelectorDialogL()
// --------------------------------------------------------------------------
//
TInt CLmUiRefAppView::LaunchMultiLandmarkSelectorDialogL()
	{   
	// Launch LMK Selector
	iLmkSelectDlg = CLmkLandmarkSelectorDlg::NewL();
	iLmkSelectDlg->SetMopParent(this);
	RArray<TLmkItemIdDbCombiInfo> selItemArray;    	
	TInt retVal = iLmkSelectDlg->ExecuteLD( selItemArray );  	
	TInt count = selItemArray.Count();	
	if ( count > 0 ) 
		{
		CPosLandmarkDatabase* pDb =   selItemArray[0].GetLmDb();
		delete( pDb ); 			
		}	
	selItemArray.Close();
	return retVal;	 
	}
	
//--------------------------------------------------------------------------
// CLmUiRefAppView::LaunchLandmarksDialogL()
// --------------------------------------------------------------------------
//
TInt CLmUiRefAppView::LaunchMultiCategorySelectorDialogL()
	{   
	// Launch multiple Category Selector
	iCategorySelectDlg = CLmkCategorySelectorDlg::NewL();
	iCategorySelectDlg->SetMopParent(this);
	RArray<TLmkItemIdDbCombiInfo> selItemArray; 	
	TInt retVal = iCategorySelectDlg->ExecuteLD( selItemArray );  	
	TInt count = selItemArray.Count();	
	if ( count > 0 ) 
		{
		CPosLandmarkDatabase* pDb =   selItemArray[0].GetLmDb();
		delete( pDb ); 			
		}			
	selItemArray.Close();
	return retVal;	 
	}
	
// --------------------------------------------------------------------------
// CLmUiRefAppView::LaunchSingleCategoeyDialogL()
// --------------------------------------------------------------------------
//
TInt CLmUiRefAppView::LaunchSingleCategoeyDialogL()
    {   
    // Launch single category selector
	iCategorySelectDlg = CLmkCategorySelectorDlg::NewL();
	iCategorySelectDlg->SetMopParent(this);    
    TLmkItemIdDbCombiInfo selItem;    
	TInt retVal = 	iCategorySelectDlg->ExecuteLD( selItem );		 
    if (retVal !=0) 
        {
        iSelectedItem = selItem.GetItemId();       
        ilDb = selItem.GetLmDb(); 
        }     	
   return retVal;     
   }
	
	
// End of File 
