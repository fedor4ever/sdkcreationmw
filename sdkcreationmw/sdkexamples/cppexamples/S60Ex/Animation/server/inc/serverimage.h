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


#ifndef __SERVERIMAGE_H__
#define __SERVERIMAGE_H__

// INCLUDES
#include <w32adll.h>
#include "servertimeoutnotify.h"

// CONSTANTS

// Pixels the animation moves each timer event
static const TInt KStepSize = 1;

// Request a reset of the bouncing square
static const TInt KAnimationReset  = 1;
static const TInt KResolutionChange = 2;

// The timer timeout resolution is different in the WINS and MARM platforms.
// Under wins it is 1/10s, under marm it is 1/64s, also on both platforms it
// takes a different time to draw the object.
#ifdef __WINS__
static const TInt KTimerPeriod = 10000;
#else
static const TInt KTimerPeriod = 150000;
#endif


// FORWARD DECLARATIONS
class CTimeOutTimer;

// CLASS DECLARATION

/**
* CImage.
* An instance of class CImage encapsulates the functionality
* of the bouncing square on the screen.
*/
class CImage : public CWindowAnim, public MTimeoutNotify
    {

    public:  // Constructors and destructors

        /**
        * CImage()
        * Construct the object, forced to be public.
        */
        CImage();

        /**
        * ~CImage()
        * Virtual destructor.
        */
        virtual ~CImage();

    public:  // From CWindowAnim

        /**
        * ConstructL()
        * Complete animation server object construction
        * (called by windows server).
        * @param aArgs any arguments required during construction
        * (passed from client through window server)
        * @param aHasFocus true if the animation has window server focus.
        */
        void ConstructL( TAny* aArgs, TBool aHasFocus );

        /**
        * CommandReplyL()
        * Perform aCommand with optional additional arguments aArgs and
        * return a reply (called by windows server).
        * Note! This method can return an error, so any actions that can leave
        * should use this method and not Command ().
        * This command is not buffered and so requests from the client side are
        * performed immediately.  For these reason these functions should
        * not take a long time.
        * @param aCommand the command to perform
        * @param aArgs any additional arguments required.
        */
        TInt CommandReplyL( TInt aCommand, TAny* aArgs );

        /**
        * Command()
        * Perform aCommand with optional additional arguments aArgs.
        * Note! Because there is no way to return an error from this method, it
        * is important that these commands should not be able to fail or leave.
        * This command is buffered and so may have been requested by the client
        * side an unknown period ago.
        * @param aCommand the command to perform
        * @param aArgs any additional arguments required
        */
        void Command( TInt aCommand, TAny* aArgs );

        /**
        * Redraw()
        * Redraw the animation object, called by the Windows Server.
        */
        void Redraw();

        /**
        * Animate()
        * Animate the object, normally called by the Windows Server
        * at a fixed period
        */
        void Animate( TDateTime* aDateTime );

        /**
        * FocusChanged()
        * Handle the event when the focus changes.
        * @param aState TRUE if the Animation Server has focus
        */
        void FocusChanged( TBool aState );

        /**
        * OfferRawEvent()
        * For handling raw events. Not used in this example so return false.
        */
        TBool OfferRawEvent( const TRawEvent &aRawEvent );

    public: // From MTimeoutNotify

        /**
        * TimedOut()
        * Handle timer time out
        */
        void TimedOut();

    private: // Data

        /**
        * iServerTimer
        * Animation timer object
        * Owned by CImage object.
        */
        CTimeOutTimer* iServerTimer;

        /**
        * iNewPoint
        * The square's current position
        */
        TPoint iNewPoint;

        /**
        * iLastPoint
        * The square's last position
        */
        TPoint iLastPoint;

        /**
        * iSize
        * The square's size
        */
        TSize iSize;

        /**
        * iScreenSize
        * The screen size
        */
        TRect iScreenSize;

        /**
        * iAnimationReset
        * Should the animation be reset back to the beginning
        */
        TBool iAnimationReset;

        /**
        * iDrawn
        * Has the animation been drawn
        */
        TBool iDrawn;

        /**
        * iDeltaX
        * x coordinate step
        */
        TBool iDeltaX;

        /**
        * iDeltaY
        * y coordinate step
        */
        TBool iDeltaY;
    };

#endif // __SERVERIMAGE_H__

// End of File
