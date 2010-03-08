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


#ifndef MESSAGINGCONTAINER_H
#define MESSAGINGCONTAINER_H

// INCLUDES
#include <coecntrl.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  CMessagingContainer  container control class.
*  
*/
class CMessagingContainer : public CCoeControl, MCoeControlObserver
    {
    public: // Constructors and destructor
		/**
		* NewL.
		* Two-phased constructor.
		* Construct a CMessagingContainer using two phase construction,
		* and return a pointer to the created object
		* @return A pointer to the created instance of CMessagingContainer
		*/
		static CMessagingContainer* NewL( const TRect& aRect );

		/**
		* NewLC.
		* Two-phased constructor.
		* Construct a CMessagingContainer using two phase construction,
		* and return a pointer to the created object
		* @return A pointer to the created instance of CMessagingContainer
		*/
		static CMessagingContainer* NewLC( const TRect& aRect );    
        
        /**
        * Destructor.
        */
        ~CMessagingContainer();
	
	private:
		/**
        * Symbian default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL( const TRect& aRect );

    public: // New functions
    
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
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * From CCoeControl,Draw.
        */
        void Draw( const TRect& aRect ) const;

		/**
		* From MCoeControlObserver
		* Acts upon changes in the hosted control's state. 
		*
		* @param	aControl	The control changing its state
		* @param	aEventType	The type of control event 
		*/
        void HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType );
        
    private: //data
        
    };

#endif

// End of File