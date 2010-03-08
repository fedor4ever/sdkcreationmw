/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
#include "noughtsandcrossesappui.h"
#include "noughtsandcrossesdocument.h"
#include "noughtsandcrossesinformationandsettings.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNoughtsAndCrossesDocument* CNoughtsAndCrossesDocument::NewL(
    CEikApplication& aApp)
    {
    CNoughtsAndCrossesDocument* self = new (ELeave) CNoughtsAndCrossesDocument (aApp);
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesDocument::CNoughtsAndCrossesDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CNoughtsAndCrossesDocument::CNoughtsAndCrossesDocument(CEikApplication& aApp) :
    CAknDocument(aApp)    
    {
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNoughtsAndCrossesDocument::ConstructL()
    {
    iSettings = new (ELeave) CNoughtsAndCrossesInformationAndSettings;

    CDictionaryStore* iniFile = Application()->OpenIniFileLC (CEikonEnv::Static()->FsSession());
	
    if (iniFile->IsPresentL (Application()->AppDllUid()))
        {
        RDictionaryReadStream stream;
        stream.OpenLC (*iniFile, Application()->AppDllUid());	
        stream >> *iSettings;
        CleanupStack::PopAndDestroy();
        }

    CleanupStack::PopAndDestroy (iniFile);

    if (iSettings->HumanTypeCross())
        {
        iEngine = CNoughtsAndCrossesEngine::NewL (CNoughtsAndCrossesEngine::ECross);
        }
    else
        {
        iEngine = CNoughtsAndCrossesEngine::NewL (CNoughtsAndCrossesEngine::ENought);
        }

    if (!iSettings->HumanPlayFirst())
        {
        MakeComputerMove();
        }
    }

// ---------------------------------------------------------------------------
// CNoughtsAndCrossesDocument::~CNoughtsAndCrossesDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CNoughtsAndCrossesDocument::~CNoughtsAndCrossesDocument()
    {
    delete iEngine;
    delete iSettings;
    }

// ---------------------------------------------------------------------------
// CNoughtsAndCrossesDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CNoughtsAndCrossesDocument::CreateAppUiL()
    {
    return new (ELeave) CNoughtsAndCrossesAppUi;
    }

// ---------------------------------------------------------------------------
// CNoughtsAndCrossesDocument::SetObserver()
// 
// ---------------------------------------------------------------------------
//
void CNoughtsAndCrossesDocument::SetObserver (
    CNoughtsAndCrossesEngine::MObserver* aObserver)
    {
    iEngine->SetObserver (aObserver);
    }

// ---------------------------------------------------------------------------
// CNoughtsAndCrossesDocument::NewGame()
// 
// ---------------------------------------------------------------------------
//
void CNoughtsAndCrossesDocument::NewGame()
    {
    if (iSettings->HumanTypeCross())
        {
        iEngine->StartNewGame (CNoughtsAndCrossesEngine::ECross);
        }
    else
        {
        iEngine->StartNewGame (CNoughtsAndCrossesEngine::ENought);
        }

    if (!iSettings->HumanPlayFirst())
        {
        MakeComputerMove();
        }
    iEngine->SetFirstMove(EFalse);
    }

// ---------------------------------------------------------------------------
// CNoughtsAndCrossesDocument::CanMove()
// 
// ---------------------------------------------------------------------------
//
TBool CNoughtsAndCrossesDocument::CanMove() const
    {
    return iEngine->CanMove();
    }

// ---------------------------------------------------------------------------
// CNoughtsAndCrossesDocument::MakeHumanMoveL()
// 
// ---------------------------------------------------------------------------
//
TBool CNoughtsAndCrossesDocument::MakeHumanMoveL(TInt aRow, TInt aColumn)
    {
    return iEngine->MakeHumanMoveL(aRow, aColumn);
    }

// ---------------------------------------------------------------------------
// CNoughtsAndCrossesDocument::MakeComputerMove()
// 
// ---------------------------------------------------------------------------
//
TBool CNoughtsAndCrossesDocument::MakeComputerMove()
    {
    return iEngine->MakeComputerMove();
    }

// ---------------------------------------------------------------------------
// CNoughtsAndCrossesDocument::InformationAndSettings()
// 
// ---------------------------------------------------------------------------
//
CNoughtsAndCrossesInformationAndSettings& 
    CNoughtsAndCrossesDocument::InformationAndSettings()
    {
    return *iSettings;
    }

// ---------------------------------------------------------------------------
// CNoughtsAndCrossesDocument::InformationAndSettings()
// 
// ---------------------------------------------------------------------------
//
const CNoughtsAndCrossesInformationAndSettings& 
    CNoughtsAndCrossesDocument::InformationAndSettings() const
    {
    return *iSettings;
    }

// ---------------------------------------------------------------------------
// CNoughtsAndCrossesDocument::SaveSettingsAndInformationL()
// 
// ---------------------------------------------------------------------------
//
void CNoughtsAndCrossesDocument::SaveSettingsAndInformationL()
    {
    CDictionaryStore* iniFile = Application()->OpenIniFileLC (CEikonEnv::Static()->FsSession());
	
    iniFile->RemoveL (Application()->AppDllUid());

    RDictionaryWriteStream stream;

    stream.AssignLC (*iniFile, Application()->AppDllUid());
    stream << *iSettings;
    stream.CommitL();
    CleanupStack::PopAndDestroy(); // stream

    iniFile->Commit();

    CleanupStack::PopAndDestroy (iniFile);
    }

// ---------------------------------------------------------------------------
// CNoughtsAndCrossesDocument::Board()
// 
// ---------------------------------------------------------------------------
//
const CNoughtsAndCrossesDocument::TBoard& CNoughtsAndCrossesDocument::Board() const
    {
    return iEngine->Board();
    }

// End of File
