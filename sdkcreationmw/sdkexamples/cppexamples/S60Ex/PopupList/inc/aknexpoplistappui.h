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


#ifndef __AKNEXPOPLISTAPPUI_H__
#define __AKNEXPOPLISTAPPUI_H__


// INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknviewappui.h>
#include <akntabgrp.h>
#include <aknnavide.h>


// CLASS DECLARATION

/**
* Application UI class.
* Provides support for the following features:
* - EIKON control architecture
* - view architecture
* - status pane
*/
class CAknExPopListAppUi : public CAknViewAppUi
    {
    public: // Constructors and destructor

        /**
        * ConstructL
        * 2nd phase constructor.
        */   
        void ConstructL();

        /**
        * ~CAknExNoteAppUi
        * Virtual Destructor.
        */     
        virtual ~CAknExPopListAppUi();

    private:    // From CEikAppUi

        /**
        * HandleCommandL.
        * Handles the commands.
        * @param aCommand command to be handled
        */
        void HandleCommandL( TInt aCommand );

        /**
        * From CEikAppUi, HandleResourceChangeL.
        * Handles resource changes.
        * @param aType Resource change type to be handled
        */
        void HandleResourceChangeL( TInt aType );

    };

#endif // __AKNEXPOPLISTAPPUI_H__

// End of File
