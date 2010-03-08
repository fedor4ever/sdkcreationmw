/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __CSASYNCDOCUMENT_H__
#define __CSASYNCDOCUMENT_H__

// INCLUDE FILES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CCSAsyncAppUi;
class CEikApplication;
class CCSAsyncRequestHandler;

// CLASS DECLARATION
/**
* CCSAsyncDocument
*  An instance of class CCSAsyncDocument is the Document part of the AVKON
*  application framework for the CSAsync example application
*/
class CCSAsyncDocument : public CAknDocument
    {
    public: // Constructors and destructors

        /**
        * NewL.
        * Two-phased constructor.
        * Constructs a CCSAsyncDocument for the AVKON application aApp 
        * using two phase construction, and returns a pointer to the 
        * created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CCSAsyncDocument.
        */
        static CCSAsyncDocument* NewL( CEikApplication& aApp );

        /**
        * NewLC.
        * Two-phased constructor.
        * Constructs a CCSAsyncDocument for the AVKON application aApp 
        * using two phase construction, and returns a pointer to the 
        * created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CCSAsyncDocument.
        */
        static CCSAsyncDocument* NewLC( CEikApplication& aApp );

        /**
        * ~CCSAsyncDocument.
        * Destructor.
        * Destroys the object and release all memory objects.
        */
        virtual ~CCSAsyncDocument();

    public: // New functions

        /**
        * UpdateTime.
        * Asks the time server to update this documents locally stored time.
        */
        void UpdateTime();

        /**
        * StopClock.
        * Stops the clock.
        */
        void StopClock();

        /**
        * Time.
        * Returns the currently stored time.
        * @return The time.
        */
        TTime Time() const;

        /**
        * ClockActive.
        * Checks whether the clock has been started.
        * @return ETrue if the clock has been started,
                  EFalse if the clock is stopped
        */
        TBool ClockActive() const;

    public: // Functions from base classes

        /**
        * From CAknDocument, CreateAppUiL.
        * Creates a CCSAsyncAppUi object and returns a pointer to it.
        * @return A pointer to the created instance of the AppUi created.
        */
        CEikAppUi* CreateAppUiL();

    private: // Constructors and destructors

        /**
        * CCSAsyncDocument.
        * C++ default constructor.
        * Perform the first phase of two phase construction.
        * @param aApp Application creating this document.
        */
        CCSAsyncDocument( CEikApplication& aApp );

        /**
        * ConstructL.
        * Performs the second phase construction of a CCSAsyncDocument object.
        */
        void ConstructL();

    private: // Data

        /**
        * iHandler, Active object for handling asynchronous requests.
        * Owned by CCSAsyncDocument object.
        */
        CCSAsyncRequestHandler* iHandler;
    };


#endif // __CSASYNCDOCUMENT_H__


// End of File
