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
*     Declares document for application.
*
*/


#ifndef LMUIREFAPPDOCUMENT_H
#define LMUIREFAPPDOCUMENT_H 

// INCLUDES
#include <akndoc.h>
#include <Epos_Landmarks.h>
   
// CONSTANTS

// FORWARD DECLARATIONS
class  CEikAppUi;

class CPosLandmarkDatabase;

// CLASS DECLARATION

/**
*  CLmUiRefAppDocument application class.
*/
class CLmUiRefAppDocument: public CAknDocument
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CLmUiRefAppDocument* NewL(CEikApplication& aApp);

        /**
        * Destructor.
        */
        virtual ~CLmUiRefAppDocument();

    public: // New functions

       /**
        * Returns a reference to landmarks database.
        * @return database reference, not initialized necessarily
        */
    CPosLandmarkDatabase& LmDb() const;

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * EPOC default constructor.
        */
         CLmUiRefAppDocument(CEikApplication& aApp);
        void ConstructL();

    private:

        /**
        * From CEikDocument, create LmUiRefAppUi "App UI" object.
        */
        CEikAppUi* CreateAppUiL();

    private: //Data
    	///Own:

    	CPosLandmarkDatabase* iDb;
    };

#endif

// End of File

