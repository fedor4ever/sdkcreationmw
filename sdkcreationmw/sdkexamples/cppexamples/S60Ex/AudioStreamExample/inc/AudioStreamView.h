/*
* Copyright (c) 2006 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares the container controller for AudioStream application
*
*/


#ifndef AUDIOSTREAMVIEW_H
#define AUDIOSTREAMVIEW_H

// INCLUDES
#include <coecntrl.h>
#include <eiklabel.h>		// for example label control
#include "AudioStreamEngine.h"
   
// FORWARD DECLARATIONS
class CAudioStreamEngine;

// CLASS DECLARATION

/**
*  CAudioStreamView  container control class.
*  
*/
class CAudioStreamView : public CCoeControl, MCoeControlObserver
{
public: 

/*!
 * NewL()
 * 
 * discussion Create new CAudioStreamView object, being able to draw 
 *    itself into aRect
 *
 * param aRect the rectangle this view will be drawn to
 * param aEngine the engine which core logic is used by this simple view
 * return a pointer to the created instance of CAudioStreamView
 */
    static CAudioStreamView* NewL(const TRect& aRect, 
		CAudioStreamEngine* aEngine);

/*!
 * NewLC()
 * 
 * discussion Create new CAudioStreamView object, being able to draw
 *    itself into aRect
 *
 * param aRect the rectangle this view will be drawn to
 * return a pointer to the created instance of CAudioStreamView 
 *    which has also been pushed to cleanup stack
 */
    static CAudioStreamView* NewLC(const TRect& aRect, 
		CAudioStreamEngine* aEngine);


/*!
 * ~CAudioStreamView()
 *
 * discussion Destroy the object and release all memory objects
 */
	~CAudioStreamView();
        
public: // New functions

/*!
 * ShowMessageL()
 *
 * discussion Displays application messages for user
 *
 * param aMsg text to be displayed
 */
	void ShowMessageL(const TDesC& /* aMsg */);

public: // Functions from base classes

private: // Basic two-phase EPOC constructors

/*!
 * ConstructL()
 *
 * discussion Perform the second phase construction of a CAudioStreamView
 *    object
 *
 * param aRect Frame rectangle for container.
 */
    void ConstructL(const TRect& aRect, CAudioStreamEngine* aEngine);
 
/*!
 * CAudioStreamView()
 *
 * discussion Perform the first phase of two phase construction 
 */
    CAudioStreamView();

private: // Functions from base classes

   /**
    * From CoeControl,SizeChanged.
    */
    void SizeChanged();

   /**
    * From CoeControl,HandleResourceChange.
    */
	void HandleResourceChange(TInt aType);

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
	* Acts upon changes in the hosted control's state. 
	*
	* param	aControl the control changing its state
	* param	aEventType the type of control event 
	*/
    void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);
    
private: // New functions
    
        
private: // data members

	// Reference for engine object
	CAudioStreamEngine* iEngine;		
	// Label displaying the application messages to user
	CEikLabel* iLabel;
	
};

#endif

// End of File


