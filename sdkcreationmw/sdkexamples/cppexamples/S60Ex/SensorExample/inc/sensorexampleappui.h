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


#ifndef __SENSOREXAMPLEAPPUI_H__
#define __SENSOREXAMPLEAPPUI_H__

// INCLUDES
#include <aknappui.h>
#include <sensrvchannelinfo.h>


// FORWARD DECLARATIONS
class CSensorExampleAppView;
class CSensrvChannel;
class CSensorExampleDataListener;

// CLASS DECLARATION
/**
* CSensorExampleAppUi application UI class.
* Interacts with the user through the UI and request message processing
* from the handler class
*/
class CSensorExampleAppUi : public CAknAppUi
    {
    public: // Constructors and destructor

       /**
        * ConstructL.
        * 2nd phase constructor.
        */
        void ConstructL();

       /**
        * CSensorExampleAppUi.
        * C++ default constructor. This needs to be public due to
        * the way the framework constructs the AppUi
        */
        CSensorExampleAppUi();

       /**
        * ~CSensorExampleAppUi.
        * Virtual Destructor.
        */
        virtual ~CSensorExampleAppUi();

    private:  // Functions from base classes

        /**
        * From CEikAppUi, HandleCommandL.
        * Takes care of command handling.
        * @param aCommand Command to be handled.
        */
        void HandleCommandL( TInt aCommand );

       /**
        *  HandleStatusPaneSizeChange.
        *  Called by the framework when the application status pane
        *  size is changed.
        */

        void HandleStatusPaneSizeChange();

    private:

       /**
        * Open a channel.
        *
        * @param aChannel On return contains pointer to opened channel.
        * @param aChannelTypeId Channel ID for the channel to be open.
        */
        void OpenChannelL( CSensrvChannel*& aChannel,
                           TSensrvChannelTypeId aChannelTypeId );

       /**
        * Show a property query
        *
        * @param aCommand Command to be handled.
        */
        void HandleQueryL( TInt aCommand );

       /**
        * Show information note
        *
        * @param aNoteText Text to be shown on information note.
        */
        void ShowNoteL( const TDesC& aNoteText );

       /**
        * Show content of aProperty on information note
        *
        * @param aProperty Property to be shown.
        */
        void ShowPropertyNoteL( TSensrvProperty& aProperty );

        /**
        * Show property's value.
        *
        * @param aProperty Property which value is shown.
        */
        void ShowPropertyValueNoteL( TSensrvProperty& aProperty );

       /**
        * Query new value for property
        *
        * @param aProperty Property which value is changed.
        * @param aChannel Channel where aProperty belongs to.
        */
        void HandlePropertyValueQueryL( TSensrvProperty& aProperty,
                                        CSensrvChannel* aChannel );

    private: // Data

       /**
        * The application view
        * Owned by CSensorExampleAppUi
        */
        CSensorExampleAppView* iAppView;

       /**
        * Opened sensor channels.
        * Owned by CSensorExampleAppUi
        */
        CSensrvChannel* iDblTapChannel;
        CSensrvChannel* iAccelerometerChannel;
        CSensrvChannel* iOrientationChannel;

       /**
        * The listener for sensor data
        * Owned by CSensorExampleAppUi
        */
        CSensorExampleDataListener* iDataListener;
    };

#endif // __SENSOREXAMPLEAPPUI_H__

// End of File
