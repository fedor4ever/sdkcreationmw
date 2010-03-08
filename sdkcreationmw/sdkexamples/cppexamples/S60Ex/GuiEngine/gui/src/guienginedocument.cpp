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



// INCLUDE FILES
#include "GuiEngineAppUi.h"
#include "GuiEngineDocument.h"
#include "GuiEngineApplication.h"



// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CGuiEngineDocument::CGuiEngineDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CGuiEngineDocument::CGuiEngineDocument( CEikApplication& aApp )
: CEikDocument( aApp )
    {
    //  No implementation required
    }

// -----------------------------------------------------------------------------
// CGuiEngineDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGuiEngineDocument* CGuiEngineDocument::NewL( CEikApplication& aApp )
    {
    CGuiEngineDocument* self = NewLC( aApp );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CGuiEngineDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGuiEngineDocument* CGuiEngineDocument::NewLC( CEikApplication& aApp )
    {
    CGuiEngineDocument* self = new ( ELeave ) CGuiEngineDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CGuiEngineDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGuiEngineDocument::ConstructL()
    {
    iModel = CShapeListManager::NewL();
    }

// -----------------------------------------------------------------------------
// CGuiEngineDocument::~CGuiEngineDocument()
// Destructor.
// -----------------------------------------------------------------------------
//
CGuiEngineDocument::~CGuiEngineDocument()
    {
    delete iModel;
    iModel = NULL;
    }

// -----------------------------------------------------------------------------
// CGuiEngineDocument::CreateAppUiL()
// Create the application user interface, and return a pointer to it;
// the framework takes ownership of this object
// -----------------------------------------------------------------------------
//
CEikAppUi* CGuiEngineDocument::CreateAppUiL()
    {
    return ( static_cast<CEikAppUi*>( new ( ELeave ) CGuiEngineAppUi( this ) ) );
    }

// -----------------------------------------------------------------------------
// CGuiEngineDocument::StoreL()
// Store model information
// -----------------------------------------------------------------------------
//
void CGuiEngineDocument::StoreL( CStreamStore& aStore,
                                 CStreamDictionary& aStreamDic ) const
    {

    //  Get the model to save itself to the store
    TStreamId modelStreamId = iModel->StoreL( aStore );

    //  Add an entry into the dictionary for the model data
    aStreamDic.AssignL( static_cast<CGuiEngineApplication*>
    						( Application() )->EngineDllUid(), modelStreamId );

    // Now get the Ui to store itself to the store
    TStreamId uiStreamId = static_cast<CGuiEngineAppUi*>
                           ( iAppUi )->StoreL( aStore );

    //  Add an entry for it to the dictionary
    aStreamDic.AssignL( static_cast<CGuiEngineApplication*>
                            ( Application() )->AppDllUid(), uiStreamId );
    }


// -----------------------------------------------------------------------------
// CGuiEngineDocument::RestoreL()
// Restore the model from the appropriate store stream.
// -----------------------------------------------------------------------------
//
void CGuiEngineDocument::RestoreL( const CStreamStore& aStore,
                                   const CStreamDictionary& aStreamDic )
    {

    // Do not set this as the document's model yet.
    // That way, if the restore fails, we have still
    // got the old model.

    CShapeListManager* newModel = CShapeListManager::NewL();
    CleanupStack::PushL( newModel );

    //  Find the stream id for the model's data
    TStreamId modelStreamId = aStreamDic.At( static_cast<CGuiEngineApplication*>
    							( Application() )->EngineDllUid() );

    //  Restore the model from that stream
    newModel->RestoreL( aStore, modelStreamId );

    //  Now get rid of the old model and set the new one as current
    delete iModel;

    iModel = newModel;
    CleanupStack::Pop( newModel );

    //  Just restored so can't have changed
    SetChanged( EFalse );

    // Now restore UI data
    TStreamId uiStreamId;

    uiStreamId = aStreamDic.At( static_cast<CGuiEngineApplication*>
                                ( Application() )->AppDllUid() );

    ( static_cast<CGuiEngineAppUi*>( iAppUi ) )->RestoreL( aStore, uiStreamId );
    }

// -----------------------------------------------------------------------------
// CGuiEngineDocument::Model()
// The model
// -----------------------------------------------------------------------------
//
CShapeListManager* CGuiEngineDocument::Model()
    {
    return iModel;
    }

// End of File
