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



#ifndef __AKNEXPBARAPPUI_H__
#define __AKNEXPBARAPPUI_H__


// INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknviewappui.h>


// FORWARD DECLARATIONS
class CAknExPbarContainer;


// CLASS DECLARATION
/**
* Application UI class.
* Provides support for the following features:
* - EIKON control architecture
* - view architecture
* - status pane
* 
*/
class CAknExPbarAppUi : public CAknAppUi
    {
    public:  // Constructors and destructor

        /**
        * CAknExPbarAppUi
        * C++ default constructor.
        */
        CAknExPbarAppUi();

        /**
        * ConstructL
        * 2nd phase constructor.
        */      
        void ConstructL();

        /**
        * ~CAknExPbarAppUi
        * Virtual Destructor.
        */    
        virtual ~CAknExPbarAppUi();


    private:  // From CEikAppUi

        /**
        * HandleCommandL.
        * Takes care of command handling.
        * @param aCommand Command to be handled.
        */
        void HandleCommandL( TInt aCommand );
                                    
        /**
        * HandleResourceChangeL()
        * Called by framework when layout is changed.
        * @param aType The type of resources that have changed.
        */
        virtual void HandleResourceChangeL( TInt aType );
                                

    private: //Data
        
        /**
        * iContainer, The application view
        * Not owned by CAknExPbarAppUi object.
        */        
        CAknExPbarContainer* iContainer;

    };

#endif // __AKNEXPBARAPPUI_H__

// End of File
