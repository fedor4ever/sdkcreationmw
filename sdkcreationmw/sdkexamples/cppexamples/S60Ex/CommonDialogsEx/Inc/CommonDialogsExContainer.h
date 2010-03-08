/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares CommonDialogsEx container.
*
*/


#ifndef CCOMMONDIALOGSEXCONTAINER_H
#define CCOMMONDIALOGSEXCONTAINER_H

// INCLUDES
#include <coecntrl.h>

// FORWARD DECLARATIONS
class CEikLabel;        // for example labels

// CLASS DECLARATION

/**
*  CCommonDialogsExContainer  container control class.
*
*/
class CCommonDialogsExContainer : public CCoeControl, MCoeControlObserver
    {
    public: // Constructors and destructor

        /**
        * EPOC default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(const TRect& aRect);

        /**
        * Destructor.
        */
        ~CCommonDialogsExContainer();

    public: // New functions
        /**
         * Show text message on screen by using label
         */
        void CCommonDialogsExContainer::ShowTextL(
                    const TDesC& aTextMain, const TDesC& aTextSub);

    public: // Functions from base classes

    private: // Functions from base classes

       /**
        * From CoeControl,SizeChanged.
        */
        void SizeChanged();

       /**
        * From CoeControl,CountComponentControls.
        */
        TInt CountComponentControls() const;

       /**
        * From CCoeControl,ComponentControl.
        */
        CCoeControl* ComponentControl(TInt aIndex) const;

       /**
        * From CCoeControl,Draw.
        */
        void Draw(const TRect& aRect) const;

       /**
        * From MCoeControlObserver
        */

        void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);
        
	    /**
        * From CCoeControl, HandleResourceChange.
        * Make dynamic resolution change smoother.
        * @param aType A message UID value.
        */
        void HandleResourceChange( TInt aType );
        
    private: //data

        CEikLabel* iLabel;          // example label
        CEikLabel* iToDoLabel;      // example label
    };

#endif

// End of File
