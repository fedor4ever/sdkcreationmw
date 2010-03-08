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



#ifndef __AKNEXPBARTIMER_H__
#define __AKNEXPBARTIMER_H__


// INCLUDES
#include <aknappui.h>

// CONSTANTS
const TInt KIntervalBaseTime = 10000;


// FORWARD DECLARATIONS
class  CAknExPbarContainer;
struct TAknExPbarTimerModel;


// CLASS DECLARATION
/**
* Timer class.
*/
class CAknExPbarTimer : public CTimer
    {

    public:  // Constructors and destructor
                     
        /**
        * CAknExPbarTimer.
        * C++ default constructor.
        */
        CAknExPbarTimer();

        /**
        * NewL
        * Two-phased constructor.
        * Construct a CAknExPbarTimer.
        * Using two phase construction, and return a pointer to
        * the created object.
        * @param aModel pointer to TAknExPbarTimerModel.
        * @param aContainer pointer to CAknExPbarContainer
        * @return A pointer to the created instance of CAknExPbarTimer.
        */
        static CAknExPbarTimer* NewL( TAknExPbarTimerModel* aModel, 
                                      CAknExPbarContainer *aContainer );

        /**
        * NewLC
        * Two-phased constructor.
        * Construct a CAknExPbarTimer.
        * Using two phase construction, and return a pointer to
        * the created object.
        * @param aModel pointer to TAknExPbarTimerModel.
        * @param aContainer pointer to CAknExPbarContainer
        * @return A pointer to the created instance of CAknExPbarTimer.
        */
        static CAknExPbarTimer* NewLC( TAknExPbarTimerModel* aModel, 
                                       CAknExPbarContainer *aContainer );

         /**
        * ConstructL
        * 2nd phase constructor.
        * @param aModel pointer to TAknExPbarTimerModel.
        * @param aContainer pointer to CAknExPbarContainer
        */
        void ConstructL( TAknExPbarTimerModel* aModel, 
                         CAknExPbarContainer*  aContainer );


        /**
        * ~CAknExPbarTimer
        * Virtual Destructor.
        */
        virtual ~CAknExPbarTimer();


    private: // from CActive
        
        /**
        * RunL
        * Definition of what to do when timer event occurred.
        */
        void RunL();


    private: // New function
        
        /**
        * Queue
        * Set timer event interval.
        */
        void Queue();


    private:  // Data

        /**
        * iModel
        * Pointer to Timer model
        * owned by CAknExPbarTimer object.
        */
        TAknExPbarTimerModel* iModel;      
        
        /**
        * iContainer
        * Pointer to Container class
        * owned by CAknExPbarTimer object.
        */
        CAknExPbarContainer*  iContainer; 
    };

#endif // __AKNEXPBARTIMER_H__

// End of File
