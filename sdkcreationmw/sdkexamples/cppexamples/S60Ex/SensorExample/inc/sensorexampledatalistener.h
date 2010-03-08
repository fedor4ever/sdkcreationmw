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


#ifndef SENSOREXAMPLEDATALISTENER_H
#define SENSOREXAMPLEDATALISTENER_H

#include <sensrvdatalistener.h>

// FORWARD DECLARATIONS
class CSensorExampleAppView;

// CLASS DECLARATION

/**
* Data listener implementation
* Callback interface to indicate when new data is available.
*/
class CSensorExampleDataListener : public CBase,
                                   public MSensrvDataListener
    {
public:

   /**
    * Two-phase constructor
    */
    static CSensorExampleDataListener* NewL( CSensorExampleAppView* aAppView );

   /**
    * Destructor.
    */
    virtual ~CSensorExampleDataListener();

public://From MSensorDataListener

    void DataReceived( CSensrvChannel& aChannel,
                       TInt aCount,
                       TInt aDataLost );

    void DataError( CSensrvChannel& aChannel,
                    TSensrvErrorSeverity aError );
                    
    void GetDataListenerInterfaceL( TUid aInterfaceUid, 
                                    TAny*& aInterface );

private:

   /**
    * C++ constructor
    */
    CSensorExampleDataListener( CSensorExampleAppView* aAppView );

private://data

    CSensorExampleAppView* iAppView; //Not own

    };

#endif //SENSOREXAMPLEDATALISTENER
