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
* Description:  Declares the output container class.
*
*/


#ifndef DATAMOBILITYAPPOUTPUTCONTAINER_H
#define DATAMOBILITYAPPOUTPUTCONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <eiklbo.h>
#include "datamobilityprinter.h"

// CONSTANTS
static const TInt KTextXMargin = 10;
static const TInt KTextMinY = 20;
static const TInt KTextDistanceBetweenLines = 20;

// CLASS DECLARATION

/**
*  CDataMobilityAppOutputContainer  container control class.
*
*/
class CDataMobilityAppOutputContainer : public CCoeControl, public MDataMobilityPrinter
    {
    public: // Constructors and destructor

        /**
        * Symbian OS default constructor.
        * @param aRect Frame rectangle for container.
        * @param aObjectToStart Identify an example object to start.
        */
        void ConstructL( const TRect& aRect, TInt aObjectToStart );

        /**
        * Destructor.
        */
        ~CDataMobilityAppOutputContainer();

    public: // Functions from MDataMobilityPrinter interface

        /**
        * Print a line of text in the output view.
        * @param aLine Line of text to be printed.
        */
        void Print( const TDesC& aLine );

    private: // Functions from base classes

        /**
        * From CCoeControl,Draw.
        * Draw a control, called by window server.
        * @param aRect The region of the control to be redrawn. Co-ordinates
        *              are relative to the control's origin (top left corner).
        */
        void Draw( const TRect& aRect ) const;

        /**
        * From CCoeControl,OfferKeyEventL
        * Called by framework when the view layout is changed.
        * @param aType The type of resource change.
        */
        virtual void HandleResourceChange(TInt aType);

    private: //data

        CDesCArrayFlat*     iLinesToPrint;
        TBool               iAllowedToPrint;
        CBase*              iApplication;
    };

#endif // DATAMOBILITYAPPOUTPUTCONTAINER_H

// End of File
