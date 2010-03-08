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


#ifndef __HELPFULFORM_H__
#define __HELPFULFORM_H__

// INCLUDES
#include <AknForm.h>

/**
*  CHelpfulForm
*  CHelpfulForm implements context sensitive help for
*  itself and the controls it contains
*/
class CHelpfulForm : public CAknForm
    {
        public: // Constructors and Destructors.

            /**
            * NewL()
            * Construct a CHelpfulForm for the AVKON application 
            * using two phase construction, and return a pointer to 
            * the created object
            * @param aMenuResource Resource of menu to be added to form menu 
            * (with Help option)
            * @return A pointer to the created instance of CHelpfulForm
            */
            static CHelpfulForm* NewL( TInt aMenuResource );

            /**
            * NewLC()
            * Construct a CHelpfulForm for the AVKON application 
            * using two phase construction, and return a pointer to 
            * the created object
            * @param aMenuResource Resource of menu to be added to form 
            * menu (with Help option)
            * @return A pointer to the created instance of CHelpfulForm
            */
            static CHelpfulForm* NewLC( TInt aMenuResource );

        public: // from AknForm
            /**
            *  ProcessCommandL()
            *  Handle user menu commands
            *  @param aCommandId The command identifier to process
            */    
            void ProcessCommandL( TInt aCommandId );

            /**
            *  GetHelpContext()
            *  Identify the help context so that the framework can look up
            *  the corresponding help topic
            *  @param aContext Returns the help context
            */    
            void GetHelpContext( TCoeHelpContext& aContext ) const;

            /**
            *  DynInitMenuPaneL()
            *  Remove the unused default form options
            *  @param aResourceId The id of the menu pane
            *  @param aMenuPane The menu pane itself
            */    
            void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );


        private:    // Constructor

            /**
            *  ConstructL()
            *  Perform the second phase construction of a CHelpfulForm object
            *  @param aMenuResource The menu resource to be used for the form
            */
            void ConstructL( TInt aMenuResource );

    };

#endif // __HELPFULFORM_H__

// End of File
