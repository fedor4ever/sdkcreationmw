/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __VIEWTEMPLATE_H__
#define __VIEWTEMPLATE_H__

#include <aknview.h>
#include <aknutils.h>
#include "Listbox.hrh"

/**
 * CViewTemplate is a template class for creating a view for Avkon 
 * View-switching architecture.
 * CViewTemplate is used as view classes for CListboxNumberContainer
 * and CListboxGraphicContainer containers.
 *
 * CViewTemplate sets context specific options menu for view's MSK by giving 
 * the menubar resource id to CViewTemplate::NewL(). Setting is done with 
 * CEikMenuBar::SetContextMenuTitleResourceId() function call, which is 
 * required for the MSK functionality. If menu is not given, MSK does not have
 * menu.
 *
 * This template class requires that container class implements following 
 * functions :
 * - ConstructL() for container construction and for passing rectangle 
 *   for drawing.
 * - AddItemL() for adding new item to listbox.
 * - RemoveItemL() for removing item from listbox 
 * - ItemCount() for getting the listbox item count 
 *
 * @param T container class used by this view
 */

template <class T>
class CViewTemplate : public CAknView
    {
    public: // New methods

        /**
        * Two-phased constructor.
        * @param aViewResource Resource id of the view
        * @param aMenubarResource Resource id of the view menubar
        * @param aViewId Unique id of the view
        * @return a pointer to the created instance of CViewTemplate.
        */
        static CViewTemplate* NewL( TInt aViewResource,
                                    TInt aMenubarResource, TUid aViewId );

        /**
        * Two-phased constructor.
        * @param aViewResource Resource id of the view
        * @param aMenubarResource Resource id of the view menubar
        * @param aViewId Unique id of the view
        * @return A pointer to the created instance of CViewTemplate.
        */
        static CViewTemplate* NewLC( TInt aViewResource,
                                     TInt aMenubarResource, TUid aViewId );

        /**
        * Virtual Destructor.
        */
        virtual ~CViewTemplate();

    public: // Functions from base classes from CAknView

        /**
        * @return Id Uid value
        */
        TUid Id() const;

        /**
        * HandleCommandL
        * From CAknView, takes care of command handling.
        * @param aCommand Command to be handled
        */
        void HandleCommandL( TInt aCommand );
        
        /**
        * From CAknExView, activate an AknView.
        * @param aPrevViewId The id of the previous view
        * @param aCustomMessageId message identifier
        * @param aCustomMessage custom message provided when the view is changed
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );

        /**
        * From AknView, deactivate an AknView
        * Remove the container class instance from the App UI's stack and
        * deletes the instance
        */
        void DoDeactivate();
        
        /**
        * From MEikMenuObserver, DynInitMenuPaneL.
        * This function is called by the EIKON framework just before it
        * displays a menu pane.
        * @param aResourceId The ID of command which defined in resource file.
        * @param aMenuPane Pointer to menu-pane object.
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    public:
       
        /**
        * Called by HandleResourceChangeL() from CListboxAppUi when layout 
        * is changed.
        * @param aType Type of resources that have changed
        */
        void HandleSizeChange( TInt aType );

        /**
        * With this function is possible to dimm selected menuitem from options 
        * menu or from its submenus. 
        * @param aMenuItemId Menu item id
        */
        void MenuItemToDimm( TInt aMenuItemId );
 
    private: 

        /**
        * 2nd phase constructor.
        */
        void ConstructL( TInt aViewResource );

        /**
        * C++ default constructor.
        * @param aViewId Unique id of the view
        * @param aMenubarResource Resource id of the view menubar
        */
        CViewTemplate( TUid aViewId, TInt aMenubarResource );
 
        
    private: // Data

        /**
        * Container for this view.
        *
        * Owned.
        */
        T* iContainer;

        /** 
        * View Identifier 
        */
        TUid iIdentifier;
        
        /** 
        * MSK menubar resource ID
        */
        TInt iMenubarResource;
        
        /**
        * Identifier for dimmed menu item
        */
        TInt iItemToDimm;
    };
    

// -----------------------------------------------------------------------------
// C++ default constructor.
// -----------------------------------------------------------------------------
//
template <class T> CViewTemplate<T>::CViewTemplate( TUid aViewId, 
                                                    TInt aMenubarResource ) : 
    iIdentifier( aViewId ),
    iMenubarResource( aMenubarResource ),
    iItemToDimm( 0 )
    {        
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
template <class T> CViewTemplate<T>::~CViewTemplate()
    {
    if( iContainer ) 
        delete iContainer;
    }
  
// -----------------------------------------------------------------------------
// CViewTemplate::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
template <class T> CViewTemplate<T>* CViewTemplate<T>::NewL( 
    TInt aViewResource, TInt aMenubarResource, TUid aViewId )
    {
    CViewTemplate<T>* self = CViewTemplate<T>::NewLC( aViewResource, 
            aMenubarResource, aViewId );
    CleanupStack::Pop( self );
    return self;    
    }
    
// -----------------------------------------------------------------------------
// CViewTemplate::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
template <class T> CViewTemplate<T>* CViewTemplate<T>::NewLC( 
    TInt aViewResource, TInt aMenubarResource, TUid aViewId )
    {
    CViewTemplate<T>* self = new ( ELeave ) CViewTemplate<T>( aViewId, 
                                                              aMenubarResource );
    CleanupStack::PushL( self );
    self->ConstructL( aViewResource );
    return self;    
    }
    
// -----------------------------------------------------------------------------
// CViewTemplate::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
template <class T> void CViewTemplate<T>::ConstructL( TInt aViewResource )
    {
    BaseConstructL( aViewResource );        
    }

// -----------------------------------------------------------------------------
// CViewTemplate::Id()
// Returns View's ID.
// -----------------------------------------------------------------------------
//
template <class T> TUid CViewTemplate<T>::Id() const
    {
    return iIdentifier;
    }

// -----------------------------------------------------------------------------
// CViewTemplate::DoActivateL()
// Activate view
// -----------------------------------------------------------------------------
//
template <class T> void CViewTemplate<T>::DoActivateL( 
        const TVwsViewId& /*aPrevViewId*/,
        TUid /*aCustomMessageId*/,
        const TDesC8& /*aCustomMessage*/)
    {
    iContainer = new ( ELeave ) T();
    iContainer->SetMopParent( this );
    iContainer->ConstructL( ClientRect() );
    
    
    if( iMenubarResource > 0 )
        {
        CEikMenuBar* menuBar = MenuBar();
        menuBar->SetContextMenuTitleResourceId( iMenubarResource ); 
        }
        
    AppUi()->AddToStackL( *this, iContainer );
         
    iContainer->MakeVisible( ETrue );
    }

// -----------------------------------------------------------------------------
// CViewTemplate::DoDeactivate()
// DeActivate view
// -----------------------------------------------------------------------------
//
template <class T> void CViewTemplate<T>::DoDeactivate()
    {
    if ( iContainer )
        {
        // Remove container form controllStack
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CViewTemplate::HandleCommandL()
// Takes care of Command handling.
// -----------------------------------------------------------------------------
//
template <class T> void CViewTemplate<T>::HandleCommandL( TInt aCommand )
   {
    switch( aCommand )
        {
        case EListboxCommandAddItem:        
            iContainer->AddItemL();            
            break;
    	case EListboxCommandRemoveItem:
    		iContainer->RemoveItemL();
    	    break;    	
    	default:    
            AppUi()->HandleCommandL( aCommand );
        }
    }

// -----------------------------------------------------------------------------
// CViewTemplate::HandleSizeChange()
// Called by HandleResourceChangeL() from CListboxAppUi when layout is 
// changed.
// -----------------------------------------------------------------------------
//
template <class T> void CViewTemplate<T>::HandleSizeChange( TInt aType )
    {
    if( iContainer )
        {
        iContainer->HandleResourceChange( aType );
        
        if( aType==KEikDynamicLayoutVariantSwitch )
            {        
            TRect rect;
            AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
            iContainer->SetRect(rect);
            }
        }         
    }
 
// ---------------------------------------------------------------------------
// CViewTemplate::MenuItemToDimm()
// With this function is possible to dimm selected menuitem from options 
// menu or from its submenus.
// ---------------------------------------------------------------------------
//
template <class T> void CViewTemplate<T>::MenuItemToDimm( TInt aMenuItemId )
    {
    iItemToDimm = aMenuItemId;    
    } 
 
// ---------------------------------------------------------------------------
// CViewTemplate::DynInitMenuPaneL()
// This function is called by the EIKON framework just before it displays a 
// menu pane.
// ---------------------------------------------------------------------------
//
template <class T> void CViewTemplate<T>::DynInitMenuPaneL( TInt /*aResourceId*/, 
                                           CEikMenuPane* aMenuPane )
    {            
    // Hide user selected menu item
    TInt index;
    if( aMenuPane->MenuItemExists (iItemToDimm, index) )
        {
        aMenuPane->SetItemDimmed( iItemToDimm, ETrue );    
        }
        
    // From options menu and MSK menu we remove always "Remove item" item
    // if there is no items to remove
    if( aMenuPane->MenuItemExists (EListboxCommandRemoveItem, index) )
        {
        // Hide "Remove item" menu item if listbox does not contain any items
        if( iContainer->ItemCount() < 1 )
            aMenuPane->SetItemDimmed( EListboxCommandRemoveItem, ETrue );   
        }
    }                
    

#endif // __VIEWTEMPLATE_H__