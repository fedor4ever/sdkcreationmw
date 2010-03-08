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



#ifndef __AKNEXPBARCONTAINER_H__
#define __AKNEXPBARCONTAINER_H__


// INCLUDES
#include <coecntrl.h>
#include <eikon.hrh>
#include "AknExPbar.hrh"
#include "AknExPbartimermodel.h"


// CONSTANTS
const TInt KInitIncrement = 0;
const TInt KStepBackValueView = -8;
const TInt KTimerModelHundredths = 10;
const TInt KTimerModelIncrement = 1;
const TBool KTimerModelRunning = ETrue;

const TInt KProgressBarResetValue = 0;
const TInt KAknExPbarBlack = 0xffffff;

const TInt KProgressBar0Position_X = 4;
const TInt KProgressBar0Position_Y = 30; 
const TInt KProgressBar1Position_X = 4;
const TInt KProgressBar1Position_Y = 70;

const TInt KProgressBarHeight = 10;
const TInt KProgressBarSplitsInBlock = 0;

const TInt KProgressBar1EmptyLeftSpace = 15; 

const TInt KProgressBar0FinalValue = 15; 
const TInt KProgressBar1FinalValue = 40; 

const TInt KLengthOfTempString = 100;
const TInt KNumberOfBars = 2;


// FORWARD DECLARATIONS
class CEikProgressInfo;
class CAknTitlePane;
class CAknExPbarTimer;
struct TAknExPbarTimerModel;

// CLASS DECLARATION
/**
*  CAknExPbarContainer  container control class.
*  
*/
class CAknExPbarContainer : public CCoeControl, MCoeControlObserver
    {
    public: // Constructors and destructor
        
        /**
        * CAknExPbarContainer
        * C++ default constructor.
        */
        CAknExPbarContainer();    
            
        /**
        * NewL.
        * Two-phased constructor.
        * Construct a CAknExPbarContainer using two phase construction,
        * and return a pointer to the created object
        * @param aRect the rectangle this view will be drawn to
        * @return A pointer to the created instance of CAknExPbarContainer
        */
        static CAknExPbarContainer* NewL( const TRect& aRect );

        /**
        * NewLC.
        * Two-phased constructor.
        * Construct a CAknExPbarContainer using two phase construction,
        * and return a pointer to the created object
        * @param aRect the rectangle this view will be drawn to
        * @return A pointer to the created instance of CAknExPbarContainer
        */
        static CAknExPbarContainer* NewLC( const TRect& aRect );
            
        /**
        * ConstructL
        * 2nd phase constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL( const TRect& aRect );

        /**
        * ~CAknExPbarContainer.
        * Virtual Destructor.
        */
        virtual ~CAknExPbarContainer();


    public: // New functions

        /**
        * IncrementBarsAndDraw
        * Increase progress bar length
        * @param aIncrement value The increment value.
        */
        void IncrementBarsAndDraw( TInt aIncrement );


        /**
        * ReConstructProgressBarsL
        * Deletes old progressbars and creates new ones. Called  from 
        * CAknExPbarAppUi::HandleResourceChangeL() when display layout 
        * is changed.
        */        
        void ReConstructProgressBarsL();


    private: // Functions from base classes

       /**
        * From CoeControl,CountComponentControls.
        * @return number of controls inside this container.
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl,ComponentControl.
        * @param aIndex number of the control.
        * @return Pointer of controls inside this container.
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * From CCoeControl,Draw.
        * @param aRect Specified area for drawing
        */
        void Draw( const TRect& aRect ) const;
        
        
    private : // Functions from base classes

        /**
        * From CCoeControl, HandleControlEventL
        * Handles an event of type aEventType
        * @param aControl Pointer to component control
        * @param aEventType Event code
        */
        void HandleControlEventL( CCoeControl* aControl, 
                                  TCoeEvent aEventType );


    private: // New functions

        /**
        * ConstructTimerModel
        * Constructs Timer's Model
        */
        void ConstructTimerModel();

        /**
        * ConstructTimerL
        * Constructs Timer itself
        */
        void ConstructTimerL();
        
        /**
        * CreateProgressBarsL
        * Creates progress bars
        */
        void CreateProgressBarsL();

    public: // New functions

         /**
        * SetFinalValue
        * Set Final Value. Changes final values between progress bar 1 and 2.
        */
        void SetFinalValue();

        /**
        * ShowInfo
        * Shows progressbars Info
        * @param aBarNo progressbars number
        */
        void ShowInfo( TInt aBarNo );

    private: // Data
        
        /**
        * iPBar 
        * owned by CAknExPbarContainer object.
        */
        CEikProgressInfo* iPBar[ KNumberOfBars ]; 
        
        /** 
        * Indicates are progressbar final values twisted so that final value 
        * for progressbar 1 is set for progressbar 0 and otherwise. If ETrue, 
        * values are twisted. 
        */
        TBool               iFinalValuesTwisted;
        
        /**
        * iTimer, pointer to timer class.
        * Owned by CAknExPbarContainer object.
        */    
        CAknExPbarTimer*     iTimer;

        /** iTimerModel, struct to TAknExPbarTimerModel **/
        TAknExPbarTimerModel iTimerModel; 
        
    };

#endif // __AKNEXPBARCONTAINER_H__

// End of File
