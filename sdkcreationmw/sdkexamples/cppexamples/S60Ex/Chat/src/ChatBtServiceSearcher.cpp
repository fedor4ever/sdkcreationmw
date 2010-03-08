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


// INCLUDE FILES
#include <StringLoader.h>
#include <ChatEx.rsg>
#include "ChatBtServiceSearcher.h"
#include "ChatBtServiceSearcher.pan"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CChatBtServiceSearcher::CChatBtServiceSearcher()
// Constructor.
// ----------------------------------------------------------------------------
//
CChatBtServiceSearcher::CChatBtServiceSearcher( MLog& aLog )
    : iIsDeviceSelectorConnected( EFalse ),
    iLog( aLog )
    {
    }

// ----------------------------------------------------------------------------
// CChatBtServiceSearcher::~CChatBtServiceSearcher()
// Destructor.
// ----------------------------------------------------------------------------
//
CChatBtServiceSearcher::~CChatBtServiceSearcher()
    {
    if ( iIsDeviceSelectorConnected )
        {
        iDeviceSelector.CancelNotifier( KDeviceSelectionNotifierUid );
        iDeviceSelector.Close();
        }
    delete iSdpSearchPattern;
    iSdpSearchPattern = NULL;

    delete iAgent;
    iAgent = NULL;
    }

// ----------------------------------------------------------------------------
// CChatBtServiceSearcher::SelectDeviceByDiscoveryL()
// Select a device.
// ----------------------------------------------------------------------------
//
void CChatBtServiceSearcher::SelectDeviceByDiscoveryL( 
    TRequestStatus& aObserverRequestStatus )
    {
    if ( ! iIsDeviceSelectorConnected )
        {
        User::LeaveIfError( iDeviceSelector.Connect() );
        iIsDeviceSelectorConnected = ETrue;
        }


    iSelectionFilter().SetUUID( ServiceClass() );

    iDeviceSelector.StartNotifierAndGetResponse( 
        aObserverRequestStatus, 
        KDeviceSelectionNotifierUid, 
        iSelectionFilter, 
        iResponse );
    }

// ----------------------------------------------------------------------------
// CChatBtServiceSearcher::FindServiceL()
// Find a service on the specified device.
// ----------------------------------------------------------------------------
//
void CChatBtServiceSearcher::FindServiceL( TRequestStatus& aObserverRequestStatus )
    {
    if ( !iResponse().IsValidBDAddr() )
        {
        User::Leave( KErrNotFound );
        }
    iHasFoundService = EFalse;

    // delete any existing agent and search pattern
    delete iSdpSearchPattern;
    iSdpSearchPattern = NULL;

    delete iAgent;
    iAgent = NULL;

    iAgent = CSdpAgent::NewL( *this, BTDevAddr() );

    iSdpSearchPattern = CSdpSearchPattern::NewL();
   
    iSdpSearchPattern->AddL( ServiceClass() );
    // return code is the position in the list that the UUID is inserted at 
    // and is intentionally ignored
  
    iAgent->SetRecordFilterL( *iSdpSearchPattern );

    iStatusObserver = &aObserverRequestStatus;

    iAgent->NextRecordRequestL();
    }

// ----------------------------------------------------------------------------
// CChatBtServiceSearcher::NextRecordRequestComplete()
// Process the result of the next record request.
// ----------------------------------------------------------------------------
//
void CChatBtServiceSearcher::NextRecordRequestComplete( 
    TInt aError, 
    TSdpServRecordHandle aHandle, 
    TInt aTotalRecordsCount )
    {
    TRAPD( error, 
        NextRecordRequestCompleteL( aError, aHandle, aTotalRecordsCount ) );

    if ( error != KErrNone ) 
        {
        Panic( EChatBtServiceSearcherNextRecordRequestComplete );
        }
    }

// ----------------------------------------------------------------------------
// CChatBtServiceSearcher::NextRecordRequestCompleteL()
// Process the result of the next record request.
// ----------------------------------------------------------------------------
//
void CChatBtServiceSearcher::NextRecordRequestCompleteL( 
    TInt aError, 
    TSdpServRecordHandle aHandle, 
    TInt aTotalRecordsCount )
    {
    if ( aError == KErrEof )
        {
        Finished();
        return;
        }

    if ( aError != KErrNone )
        {
        iLog.LogL( KErrNRRCErr, aError );
        Finished( aError );
        return;
        }

    if ( aTotalRecordsCount == 0 )
        {
        HBufC* errNRRCNoRecords = StringLoader
            ::LoadLC ( R_CHAT_ERR_NRRC_NO_RECORDS );
        iLog.LogL( *errNRRCNoRecords );
        CleanupStack::PopAndDestroy ( errNRRCNoRecords );
        Finished( KErrNotFound );
        return;
        }

    //  Request its attributes
    iAgent->AttributeRequestL( aHandle, KSdpAttrIdProtocolDescriptorList );
    }

// ----------------------------------------------------------------------------
// CChatBtServiceSearcher::AttributeRequestResult()
// Process the next attribute requested.
// ----------------------------------------------------------------------------
//
void CChatBtServiceSearcher::AttributeRequestResult( 
    TSdpServRecordHandle aHandle, 
    TSdpAttributeID aAttrID, 
    CSdpAttrValue* aAttrValue )
    {
    TRAPD( error, AttributeRequestResultL( aHandle, aAttrID, aAttrValue ) );
    if ( error != KErrNone )
        {
        Panic( EChatBtServiceSearcherAttributeRequestResult );
        }
    // Delete obsolete local atribute pointer. 
    delete aAttrValue;
    aAttrValue = NULL;
    }

// ----------------------------------------------------------------------------
// CChatBtServiceSearcher::AttributeRequestResultL()
// Process the next attribute requested.
// ----------------------------------------------------------------------------
//
void CChatBtServiceSearcher::AttributeRequestResultL( 
    TSdpServRecordHandle /*aHandle*/, 
    TSdpAttributeID aAttrID, 
    CSdpAttrValue* aAttrValue )
    {
    __ASSERT_ALWAYS( aAttrID == KSdpAttrIdProtocolDescriptorList, 
                     User::Leave( KErrNotFound ) );
    TChatSdpAttributeParser parser( ProtocolList(), *this );

    // Validate the attribute value, and extract the RFCOMM channel
    aAttrValue->AcceptVisitorL( parser );

    if ( parser.HasFinished() )
        {
        // Found a suitable record so change state
        iHasFoundService = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CChatBtServiceSearcher::AttributeRequestComplete()
// Process the attribute request completion.
// -----------------------------------------------------------------------------
//
void CChatBtServiceSearcher::AttributeRequestComplete( TSdpServRecordHandle aHandle,
    TInt aError )
    {
    TRAPD( error, AttributeRequestCompleteL( aHandle, aError ) );
    if ( error != KErrNone )
        {
        Panic( EChatBtServiceSearcherAttributeRequestComplete );
        }
    }

// ----------------------------------------------------------------------------
// CChatBtServiceSearcher::AttributeRequestCompleteL()
// Process the attribute request completion.
// ----------------------------------------------------------------------------
//
void CChatBtServiceSearcher::AttributeRequestCompleteL( 
    TSdpServRecordHandle /*aHandle*/,
    TInt aError )
    {
    if ( aError != KErrNone )
        {
        HBufC* errCantGetAttribute = StringLoader
            ::LoadLC ( R_CHAT_ERR_CANT_GET_ATTRIBUTE );
        iLog.LogL( *errCantGetAttribute, aError );
        CleanupStack::PopAndDestroy ( errCantGetAttribute );
        }
    else if ( !HasFinishedSearching() )
        {
        // have not found a suitable record so request another
        iAgent->NextRecordRequestL();
        }
    else
        {
        HBufC* errAttrReqCom = StringLoader
            ::LoadLC ( R_CHAT_ERR_ATTR_REQ_COM );
        iLog.LogL( *errAttrReqCom );
        CleanupStack::PopAndDestroy ( errAttrReqCom );
        Finished();
        }
    }

// ----------------------------------------------------------------------------
// CChatBtServiceSearcher::Finished()
// The search has finished and notify the observer 
// that the process is complete.
// ----------------------------------------------------------------------------
//
void CChatBtServiceSearcher::Finished( TInt aError /* default = KErrNone */ )
    {
    if ( aError == KErrNone && !HasFoundService() )
        {
        aError = KErrNotFound;
        }
    User::RequestComplete( iStatusObserver, aError );
    }

// ----------------------------------------------------------------------------
// CChatBtServiceSearcher::HasFinishedSearching()
// Is the instance still wanting to search.
// ----------------------------------------------------------------------------
//
TBool CChatBtServiceSearcher::HasFinishedSearching() const
    {
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CChatBtServiceSearcher::BTDevAddr()
// Returns the bluetooth device address.
// ----------------------------------------------------------------------------
//
const TBTDevAddr& CChatBtServiceSearcher::BTDevAddr()
    {
    return iResponse().BDAddr();
    }

// ----------------------------------------------------------------------------
// CChatBtServiceSearcher::ResponseParams()
// Returns information about the device selected by the user.
// ----------------------------------------------------------------------------
//
const TBTDeviceResponseParams& CChatBtServiceSearcher::ResponseParams()
    {
    return iResponse();
    }

// ----------------------------------------------------------------------------
// CChatBtServiceSearcher::HasFoundService()
// True if a service has been found.
// ----------------------------------------------------------------------------
//
TBool CChatBtServiceSearcher::HasFoundService() const
    {
    return iHasFoundService;
    }

// End of File
