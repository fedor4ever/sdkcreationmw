/*
* Copyright (c) 2007, 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include <sensrvchannel.h>
#include <sensrvtappingsensor.h>
#include "sensorexampledatalistener.h"
#include "SensorExampleAppView.h"


// CLASS DECLARATION

// -----------------------------------------------------------------------------
// CSensorExampleDataListener::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSensorExampleDataListener* CSensorExampleDataListener::NewL( CSensorExampleAppView* aAppView )
    {
    return new ( ELeave ) CSensorExampleDataListener( aAppView );
    }

// -----------------------------------------------------------------------------
// CSensorExampleDataListener::CSensorExampleDataListener()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CSensorExampleDataListener::CSensorExampleDataListener( CSensorExampleAppView* aAppView )
    : iAppView( aAppView )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CSensorExampleDataListener::~CSensorExampleDataListener()
// Destructor.
// -----------------------------------------------------------------------------
//
CSensorExampleDataListener::~CSensorExampleDataListener()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CSensorExampleDataListener::DataReceived
// New data is available in the receiving buffer.
// -----------------------------------------------------------------------------
//
void CSensorExampleDataListener::DataReceived( CSensrvChannel& aChannel,
                                               TInt /*aCount*/,
                                               TInt /*aDataLost*/ )
    {
    TSensrvChannelInfo info = aChannel.GetChannelInfo();

    if( info.iChannelType == KSensrvChannelTypeIdAccelerometerDoubleTappingData )
        {
        TSensrvTappingData tappingData;
        TPckg<TSensrvTappingData> tappingPackage( tappingData );
        aChannel.GetData( tappingPackage );
        iAppView->TappingEvent( tappingData.iDirection );
        }

    else if( info.iChannelType == KSensrvChannelTypeIdAccelerometerXYZAxisData )
        {
        TSensrvAccelerometerAxisData accData;
        TPckg<TSensrvAccelerometerAxisData> accPackage( accData );
        aChannel.GetData( accPackage );
        iAppView->AccelerometerData( accData.iAxisX, accData.iAxisY );
        }

    else if( info.iChannelType == KSensrvChannelTypeIdOrientationData )
        {
        TSensrvOrientationData accData;
        TPckg<TSensrvOrientationData> accPackage( accData );
        aChannel.GetData( accPackage );
        iAppView->OrientationEvent( accData.iDeviceOrientation );
        }
    }

// -----------------------------------------------------------------------------
// CSensorExampleDataListener::DataReceived
// Data listening failed.
// -----------------------------------------------------------------------------
//
void CSensorExampleDataListener::DataError( CSensrvChannel& /*aChannel*/,
                                            TSensrvErrorSeverity /*aError*/ )
    {
    }

// -----------------------------------------------------------------------------
// CSensorExampleDataListener::GetDataListenerInterfaceL
// Get supported interface.
// -----------------------------------------------------------------------------
//
void CSensorExampleDataListener::GetDataListenerInterfaceL( 
    TUid /*aInterfaceUid*/, TAny*& aInterface )
    {
    // Set interface as NULL to indicate only default interface currently supported.
    // If new interface(s) is being introduced and application wants to support that
    // aInterfaceUid must be checked and aInterface set accordingly.
    aInterface = NULL;
    }
