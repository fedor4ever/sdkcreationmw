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


#ifndef __AKNEXPOPLISTCONTAINER_H__
#define __AKNEXPOPLISTCONTAINER_H__


// INCLUDES
#include <aknview.h>
 
// CONSTANTS
const TInt KNumberOfControls = 0;
const TInt KRgbColorGray = 0xaaaaaa;


// CLASS DECLARATION

/**
*  CAknExPopListContainer  container control class.
*/
class CAknExPopListContainer : public CCoeControl, MCoeControlObserver
    {
    public: // Constructors and destructor
        
        /**
        * ConstructL
        * 2nd phase constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(const TRect& aRect);


        /**
        * ~CAknExPopListContainer.
        * Virtual Destructor.
        */
        virtual ~CAknExPopListContainer();


    private: // Functions from base classes

        /**
        * From CoeControl,SizeChanged.
        */
        void SizeChanged();

        /**
        * From CoeControl,CountComponentControls.
        * @return number of controls inside this container.
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl,ComponentControl.
        * @param Index number of the control.
        * @return Pointer of controls inside this container.
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * Draw
        * @param Specified area for drawing
        */
        void Draw(const TRect& aRect) const;

        /**
        * From CCoeControl, HandleControlEventL
        * Handles an event of type aEventType
        * @param aControl Pointer to component control
        * @param aEventType Event code
        */
        void HandleControlEventL( CCoeControl* aControl,
                                  TCoeEvent aEventType );
        
    };

#endif  // __AKNEXPOPLISTCONTAINER_H__

// End of File
