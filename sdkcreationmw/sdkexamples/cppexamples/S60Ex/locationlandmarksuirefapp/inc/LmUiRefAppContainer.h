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
*     Declares container control class for application.
*
*/


#ifndef CLMUIREFAPPCONTAINER_H
#define CLMUIREFAPPCONTAINER_H 

// INCLUDES
#include <coecntrl.h>
   
// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  CLmUiRefAppContainer container control class.
*  
*/
class CLmUiRefAppContainer: public CCoeControl
    {
    public: // Constructors and destructor
        

         /* Two-phased constructor
         * @param aKeyProcessor
         * @param aParent
         * @param aRect A rectangle for drawing.
         * @return The newly created object.
         */
         static CLmUiRefAppContainer* NewL(MObjectProvider* aParent,
		                           const TRect& aRect );

        /**
        * Destructor.
        */
        ~CLmUiRefAppContainer();

    public: // New functions


    public: // Functions from base classes

	void Draw(const TRect& /*aRect*/) const;

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


       private:
        /**
        * C++ default constructor.
        * @param aKeyProcessor
        * @return newly instantiated object
        */
        CLmUiRefAppContainer();   
   
        void ConstructL(const TRect& aRect);

 
    private: //data
 
        CEikLabel* iLabel;          // example label
        CEikLabel* iToDoLabel;      // example label
        
    };

#endif

// End of File
