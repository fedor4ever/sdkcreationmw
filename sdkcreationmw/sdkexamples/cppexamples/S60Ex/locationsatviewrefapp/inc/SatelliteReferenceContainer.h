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
*
*/


#ifndef SATELLITE_REFERENCE_CONTAINER_H
#define SATELLITE_REFERENCE_CONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <aknappui.h>
   
// FORWARD DECLARATIONS
class CEikLabel;        // for example labels

// CLASS DECLARATION

/**
*  CSatelliteReferenceContainer  container control class.
*  
*/
class CSatelliteReferenceContainer : public CCoeControl, MCoeControlObserver
    {
    public: // Constructors and destructor
        
        /**
        * EPOC default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(const TRect& aRect, CAknAppUi* aAppUi);

        /**
        * Destructor.
        */
        ~CSatelliteReferenceContainer();
        
        /**
        * To handle layout change event
        */
        void HandleResourceChange(TInt aType);

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
        * From ?base_class ?member_description
        */
        // event handling section
        // e.g Listbox events
        void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);
        
    private:
    	CAknAppUi* iAppUi;
    };

#endif

// End of File
