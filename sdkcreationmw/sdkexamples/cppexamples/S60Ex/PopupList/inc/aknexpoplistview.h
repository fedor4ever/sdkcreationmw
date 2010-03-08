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


#ifndef __AKNEXPOPLISTVIEW_H__
#define __AKNEXPOPLISTVIEW_H__


// INCLUDES
#include <aknview.h>
#include <AknIconUtils.h>

// CONSTANTS
const TUid KViewId = { 1 };
const TInt KMaxIcons = 10;
const TInt KMaxTitleLength = 32;
const TInt KMessageLength = 64;
const TInt KIndexArraySize = 5;
const TInt KInitPos = 0;

_LIT( KInitPosString,"0" );

// path and name of mbm file
_LIT( KPathAppMbm, "AknExPopList.mbm" );

// FORWARD DECLARATIONS
class CAknExPopListContainer;
class CIconFileProvider;

// CLASS DECLARATION
/**
*  CAknExPopListView view class.
*/
class CAknExPopListView : public CAknView
    {
    public: // Constructors and destructor


        /**
        * ConstructL
        * 2nd phase constructor.
        */
        void ConstructL();


        /**
        * ~CAknExPopListView
        * Virtual Destructor.
        */
        virtual ~CAknExPopListView();

    public: // Functions from base classes
        
        
        /**
        * Id
        * From CAknView
        * @return Uid Uid value
        */
        TUid Id() const;

        /**
        * HandleCommandL
        * From CAknView, HandleCommandL.
        * Takes care of command handling.
        * @param aCommand Command to be handled.
        */
        void HandleCommandL( TInt aCommand );

        /**
        * HandleViewRectChange
        * From CAknView, handles client rectangle is changed.
        */
        void HandleViewRectChange();


    private:

        /**
        * DoActivateL
        * From AknView, Creates the container class object.
        * @param aPrevViewId Previeous view id.
        * @param aCustomMessageId messge id.
        * @param aCustomMessage message.
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage);

        /**
        * DoDeactivate
        * From AknView, Deletes the container class object.
        */
        void DoDeactivate();


    private: // New funcitons

        /**
        * AutoTestL
        * Show all kinds of popup-windows except list_single_number_pane,
        * list_single_heading_pane and list_single_number_heading_pane
        * automatically.
        */
        void AutoTestL();

        /**
        * ShowSingleItemPopupListL
        * Show "List pane for single item" popup list.
        */
        void ShowSingleItemPopupListL();

        /**
        * ShowSingleItemPopupListWithHeadingAndGraphicsL
        * Show "List pane for single item with heading and graphic" popup list.
        */
        void ShowSingleItemPopupListWithGraphicsL();

        /**
        * ShowDoubleItemPopupListWithGraphicsL
        * Show "List pane for Double item with large image" popup list.
        */
        void ShowDoubleItemPopupListWithGraphicsL();

    private: // Data

        /**
        * iContainer
        * Owned by CAknExPopListView object.
        */
        CAknExPopListContainer*     iContainer;
        
        /**
        * iFSSession, a handle to a file server session.
        */
        RFs iFsSession;
	      
	      
	    CIconFileProvider* iIconProvider;
    };

#endif  // __AKNEXPOPLISTVIEW_H__

// End of File
