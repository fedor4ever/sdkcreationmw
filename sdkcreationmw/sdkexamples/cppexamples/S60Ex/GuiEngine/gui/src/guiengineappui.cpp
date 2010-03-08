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
#include <eiktbar.h>
#include <eikenv.h>
#include <w32adll.h>
#include <avkon.hrh>

#include "GuiEngine.pan"
#include "GuiEngineDocument.h"
#include "GuiEngineAppUi.h"
#include "GuiEngineAppView.h"
#include "GuiEngine.hrh"


// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CGuiEngineAppUi::CGuiEngineAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CGuiEngineAppUi::CGuiEngineAppUi( CGuiEngineDocument* aDocument )
: iDocument( aDocument )
    {
    //  No implementation required
    }

// -----------------------------------------------------------------------------
// CGuiEngineAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGuiEngineAppUi::ConstructL()
    {

    // Initialise app UI with standard value.
    BaseConstructL( EAknEnableSkin );

    iAppView = CGuiEngineAppView::NewL(ClientRect(), iDocument);

    // Enable the virtual cursor
    iEikonEnv->VirtualCursor().SetCursorStateL( TEikVirtualCursor::EOn,*iEikonEnv );

    AddToStackL( iAppView );

    }

// -----------------------------------------------------------------------------
// CGuiEngineAppUi::~CGuiEngineAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CGuiEngineAppUi::~CGuiEngineAppUi()
    {
    if ( iAppView )
        {
        iEikonEnv->RemoveFromStack( iAppView );
        delete iAppView;
        iAppView = NULL;
        }

    }

// -----------------------------------------------------------------------------
// CGuiEngineAppUi::HandleCommandL()
// Handles the commands.
// -----------------------------------------------------------------------------
//
void CGuiEngineAppUi::HandleCommandL( TInt aCommand )
    {

    switch ( aCommand )
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            SaveL();
            Exit();
            break;

        case KClear:
            iAppView->Clear();
            break;

        case KSelectBrushCircle:
            iAppView->SetBrushShapeType( CGuiEngineAppView::ECircle );
            break;

        case KSelectBrushRectangle:
            iAppView->SetBrushShapeType( CGuiEngineAppView::ERectangle );
            break;

        default:
            break;
        }

    }

// -----------------------------------------------------------------------------
// CGuiEngineAppUi::HandleModelChangeL()
// Handles Model Changes
// -----------------------------------------------------------------------------
//
void CGuiEngineAppUi::HandleModelChangeL()
    {
    // Redraw the application view based on the new model.
    iAppView->DrawNow();

    // Update the file name in the task list to show the current file.
    iEikonEnv->UpdateTaskNameL();
    }

// -----------------------------------------------------------------------------
// CGuiEngineAppUi::StoreL()
// Stores to a stream
// -----------------------------------------------------------------------------
//
TStreamId CGuiEngineAppUi::StoreL( CStreamStore& aStore ) const
    {
    RStoreWriteStream stream;
    TStreamId id = stream.CreateLC( aStore );

    // Write the data to the stream
    iAppView->ExternalizeL( stream );
    stream.CommitL();
    CleanupStack::PopAndDestroy();

    // Return the stream ID
    return id;
    }

// -----------------------------------------------------------------------------
// CGuiEngineAppUi::RestoreL()
// Restore from a stream
// -----------------------------------------------------------------------------
//
void CGuiEngineAppUi::RestoreL( const CStreamStore& aStore,
                                TStreamId aStreamId )
    {
    RStoreReadStream stream;
    stream.OpenLC( aStore,aStreamId );

    //  Read in the data
    iAppView->InternalizeL( stream );

    CleanupStack::PopAndDestroy();
    }


// End of File
