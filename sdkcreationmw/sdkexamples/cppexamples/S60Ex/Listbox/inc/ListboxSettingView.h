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


#ifndef __LISTBOXSETTINGVIEW_H__
#define __LISTBOXSETTINGVIEW_H__


#include <aknview.h>

class CListboxSettingList;


/**
*  CListBoxSettingView class
*  This class is a view class for setting list container (CListboxSettingList).
*/
class CListboxSettingView : public CAknView
    {
    
    public:
    
        /**
        * Two-phased constructor.
        */
        static CListboxSettingView* NewL();
    
    public: //New functions
    
        /**
        * Called by HandleResourceChangeL() from CListboxAppUi when layout 
        * is changed.
        *
        * @param aType Type of resources that have changed
        */
        void HandleSizeChange( TInt aType );
    
    private:
    
        /**
        * From CAknView.
        * Returns views id.
        *
        * @return Id for this view.
        */
        TUid Id() const;
    
        /**
        * From CAknView.
        * Handles commands
        *
        * @param aCommand A command to be handled.
        */
        void HandleCommandL( TInt aCommand );
    
        /**
        * From CAknView.
        * Activates view
        *
        * @param aPrevViewId Specifies the view previously active.
        * @param aCustomMessageId Specifies the message type.
        * @param aCustomMessage The activation message.
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );
    
        /**
        * From CAknView.
        * Deactivates view.
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
    
    private:
    
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * C++ default constructor.
        */
        CListboxSettingView();
        
        /**
        * Destructor.
        */
        ~CListboxSettingView();
    
    private: //Data
    
        /**
        * Container.
        * Own.
        */
        CListboxSettingList* iContainer;

    };

#endif // __LISTBOXSETTINGVIEW_H__
