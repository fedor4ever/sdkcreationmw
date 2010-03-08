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
* Description: 
*
*/


#ifndef NAVISCROLLTIMER_H
#define NAVISCROLLTIMER_H


#include <e32base.h>
#include <aknnavilabel.h>
#include <aknnavi.h>
#include <aknnavide.h>

/**
*  CNaviScrollTimer class.
*  This class is used to scroll text on navigation pane.
*/
class CNaviScrollTimer : public CTimer
    {

    public:

        /**
        * Two-phased constructor.
        * @param aNavitext A string to navipane.
        */
        static CNaviScrollTimer* NewL( const TDesC& aNavitext );

        /**
        * C++ default constructor.
        */
        CNaviScrollTimer();

        /**
        * Destructor
        */
        ~CNaviScrollTimer();

    public: // New functions

        /**
        * Starts to scroll navigation pane text. Scrolling is not started if
        * given text fits to navigation pane. In this case it only adds text
        * to navigation pane.
        */
        void StartScroll();

    private:

        /**
        * Updates the Navigation pane. Every time when this function is called,
        * it moves the text to the left by one character.
        * @return ETrue if text is scrolled totally out of navigation pane,
        *               otherwise EFalse.
        */
        TBool UpdateNaviPaneL();

        /**
        * 2nd phase constructor.
        * @param aNavitext A string to navipane.
        */
        void ConstructL( const TDesC& aNavitext );


    private: // From CActive

        /**
        * After completed timer period, execution comes to RunL
        */
        void RunL();

    private: //Data

        /**
        * A pointer to default control in navigation pane.
        *
        * Not owned
        */
        CAknNavigationControlContainer* iNaviPane;

        /**
        * A pointer to navigation pane decorator.
        *
        * Owned
        */
        CAknNavigationDecorator* iDecorator;

        /**
        * Integer indicating current shown text length in navigation pane.
        */
        TInt iCount;

        /**
        * Text shown in navigation pane.
        *
        * Owned.
        */
        HBufC* iNaviText;

    };

#endif // NAVISCROLLTIMER_H

// End of File

