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



#ifndef __NOUGHTSANDCROSSESDOCUMENT_H__
#define __NOUGHTSANDCROSSESDOCUMENT_H__

// INCLUDES
#include <akndoc.h>
#include "noughtsandcrossesengine.h"

   
// FORWARD DECLARATIONS
class CEikAppUi;
class CNoughtsAndCrossesInformationAndSettings;


// CLASS DECLARATION

/**
* CNoughtsAndCrossesDocument application class.
* An instance of class CNoughtsAndCrossesDocument is the Document part of the
* AVKON application framework for the NAC example application.
*/
class CNoughtsAndCrossesDocument : public CAknDocument
    {
    public: // Types
        typedef CNoughtsAndCrossesEngine::TBoard TBoard;

    public: // Constructors and destructor

        /**
        * NewL.
        * Two-phased constructor.
        * Construct a CNoughtsAndCrossesDocument for the AVKON application aApp
        * using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CNoughtsAndCrossesDocument.
        */
        static CNoughtsAndCrossesDocument* NewL(CEikApplication& aApp);

        /**
        * ~CNoughtsAndCrossesDocument
        * Virtual Destructor.
        */
        virtual ~CNoughtsAndCrossesDocument();

    public: // New functions

        /**
        * SetObserver.
        * @param aObserver.
        */
        void SetObserver(CNoughtsAndCrossesEngine::MObserver* aObserver);

        /**
        * NewGame.
        */
        void NewGame();

        /**
        * CanMove.
        * @return.
        */
        TBool CanMove() const;

        /**
        * MakeHumanMoveL.
        * @param aRow.
        * @param aColumn.
        * @return.
        */
        TBool MakeHumanMoveL(TInt aRow, TInt aColumn);

        /**
        * MakeComputerMove.
        * @return.
        */
        TBool MakeComputerMove();

        /**
        * InformationAndSettings.
        * @return.
        */
        CNoughtsAndCrossesInformationAndSettings& InformationAndSettings();

        /**
        * InformationAndSettings.
        * @return.
        */
        const CNoughtsAndCrossesInformationAndSettings& InformationAndSettings() const;

        /**
        * SaveSettingsAndInformationL.
        */ 
        void SaveSettingsAndInformationL();

        /**
        * Board.
        * @return.
        */
        const TBoard& Board() const;

    private: // Functions from base classes

        /**
        * CreateAppUiL
        * From CEikDocument, CreateAppUiL.
        * Create a CNoughtsAndCrossesAppUi object and return a pointer to it.
        * The object returned is owned by the Uikon framework.
        * @return Pointer to created instance of AppUi.
        */
        CEikAppUi* CreateAppUiL();

    private: // Constructors

        /**
        * CNoughtsAndCrossesDocument.
        * C++ default constructor.
        * @param aApp Application creating this document.
        */
        CNoughtsAndCrossesDocument(CEikApplication& aApp);

        /**
        * ConstructL
        * 2nd phase constructor.
        */
        void ConstructL();

    private: // Data
        CNoughtsAndCrossesInformationAndSettings* iSettings;
        CNoughtsAndCrossesEngine* iEngine;
		
    };

#endif // __NOUGHTSANDCROSSESDOCUMENT_H__

// End of File
