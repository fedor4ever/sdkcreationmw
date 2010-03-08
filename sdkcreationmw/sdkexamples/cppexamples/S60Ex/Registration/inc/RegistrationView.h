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


#ifndef REGISTRATIONVIEW_H
#define REGISTRATIONVIEW_H

// INCLUDE FILES
#include <aknview.h>
#include <avkon.hrh>
#include <avkon.rsg>
#include <akntitle.h>
#include <aknviewappui.h>

#include <Registration.rsg>
#include "Registration.hrh"
#include "RegistrationItemdata.h"

// CONSTANTS
 // UID of View.
const TUid KViewId = { 1 };
const TInt KMaxEmailLength = 40;

// FORWARD DECLARATIONS
class CRegistrationListbox;

// CLASS DECLARATION
/**
* CRegistrationView view class.
*/
class CRegistrationView : public CAknView
    {
    public: // Constructors and destructor
        /**
        * NewL
        * Two-phased constructor.
        * Create a CRegistrationView object, which will draw itself to aRect.
        * @param aRect The rectangle this view will be drawn to.
        * @return a pointer to the created instance of CRegistrationView.
        */
        static CRegistrationView* NewL();

        /**
        * NewLC
        * Two-phased constructor.
        * Create a CRegistrationView object, which will draw itself to aRect.
        * @param aRect Rectangle this view will be drawn to.
        * @return A pointer to the created instance of CRegistrationView.
        */
        static CRegistrationView* NewLC();

        /**
        * ~CRegistrationView
        * Virtual Destructor.
        */
        virtual ~CRegistrationView();        

    public: // New function
        /**
        * SizeChanged
        * Creates Listbox
        * Called by AppUi when the view size is changed.
        */
        void SizeChanged();
      
        /**
        * ValidateInput
        * Checks validity of input.
        * @return ETrue if input is valid and EFalse if not valid
        */
        TBool ValidateInput();     
        
    public: // From CAknView
        /**
        * Id
        * Returns the ID of view.
        * @return The ID of view.
        */
        TUid Id() const;

        /**
        * HandleCommandL.
        * handles the commands. if the command is command which is require to
        * display outline-screen, the command is reported to container class.
        * @param aCommand Command to be handled.
        */
        void HandleCommandL( TInt aCommand );
        
    private: // Constructors and destructor    
        /**
        * CRegistrationView
        * C++ default constructor.
        */
        CRegistrationView();
        
        /**
        * ConstructL
        * 2nd phase constructor.
        * Perform the second phase construction of a CRegistrationView object.
        */
        void ConstructL();
      
    private: // From CAknView   
        /**
        * DoActivateL.
        * Creates the Container class object.
        * @param aPrevViewId aPrevViewId is not used.
        * @param aCustomMessageId aCustomMessageId is not used.
        * @param aCustomMessage aCustomMessage is not used.
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );

        /**
        * DoDeactivate.
        * Deletes the Container class object.
        */
        void DoDeactivate();

    private: // Data    
         
        CRegistrationListbox*       iListbox;
        TRegistrationItemdata       iItemData;
    };

#endif // REGISTRATIONVIEW_H

// End of File
