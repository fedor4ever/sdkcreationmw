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


#ifndef BILLBOARDCONTAINER_H
#define BILLBOARDCONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <akndef.h>
#include <GLES\egl.h>
#include "billboard.h"
#include "Billboardinput.h"

#include <aknappui.h>

// CLASS DECLARATION

/**
 * Container control class that handles the OpenGL ES initialization and deinitializations.
 * Also uses the CBillboard class to do the actual OpenGL ES rendering.
 */
class CBillboardContainer : public CCoeControl, MCoeControlObserver
    {
    public: // Constructors and destructor

        /**
         * Second phase constructor that can call methods that may leave.
         * Initializes the OpenGL ES for rendering to the window surface.
         * @param aRect Screen rectangle for container.
         */
        void ConstructL(const TRect& aRect, CAknAppUi* aAppUi);

        /**
         * Destructor. Destroys the CPeriodic, CBillboard and uninitializes OpenGL ES.
         */
        virtual ~CBillboardContainer();

    public: // New functions

        /**
         * Callback function for the CPeriodic. Calculates the current frame, keeps the background
         * light from turning off and orders the CBillboard to do the rendering for each frame.
         *@param aInstance Pointer to this instance of CBillboardContainer.
         */
        static TInt DrawCallBack( TAny* aInstance );

        /**
         * Returns the current system time.
         * @return System time.
         */
        static TTime GetTimeTick();

    private: // Functions from base classes

        /**
         * Handles joystick movements and other key presses.
         * @param aKeyEvent Key event.
         * @param aType Key event type.
         * @return EKeyWasConsumed if the key event was handled or EKeyWasNotConsumed if the key event was not handled.
         */
		TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,TEventCode aType );

        /**
         * Method from MCoeControlObserver that handles an event from the observed control.
         * Does nothing in this implementation.
         * @param aControl   Control changing its state.
         * @param aEventType Type of the control event.
         */
        void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);

        /**
         * Method from CoeControl that gets called when the display size changes.
         * If OpenGL has been initialized, notifies the renderer class that the screen
         * size has changed.
         */
        void SizeChanged();

         /**
          * Handles a change to the control's resources. This method
          * reacts to the KEikDynamicLayoutVariantSwitch event (that notifies of
          * screen size change) by calling the SetExtentToWholeScreen() again so that
          * this control fills the new screen size. This will then trigger a call to the
          * SizeChanged() method.
          * @param aType Message UID value, only KEikDynamicLayoutVariantSwitch is handled by this method.
          */
        void HandleResourceChange(TInt aType);

        /**
         * Method from CoeControl. Does nothing in this implementation.
         */
        TInt CountComponentControls() const;

        /**
         * Method from CCoeControl. Does nothing in this implementation.
         */
        CCoeControl* ComponentControl(TInt aIndex) const;

        /**
         * Method from CCoeControl. Does nothing in this implementation.
         * All rendering is done in the DrawCallBack() method.
         */
        void Draw(const TRect& aRect) const;

    private: //data

        /** Display where the OpenGL ES window surface resides. */
        EGLDisplay  iEglDisplay;
        /** Window surface where the OpenGL ES rendering is blitted to. */
        EGLSurface  iEglSurface;
        /** OpenGL ES rendering context. */
        EGLContext  iEglContext;
        /** Active object that is the timing source for the animation. */
        CPeriodic*  iPeriodic;
        /**
         * Flag that indicates if OpenGL ES has been initialized or not.
         * Used to check if SizeChanged() can react to incoming notifications.
         */
        TBool iOpenGlInitialized;

        /** Time (int ticks) when the example started running (rendering frames). */
        TUint iStartTimeTicks;
        /** Time (in seconds) when the previous frame was rendered. */
        GLfloat iLastFrameTimeSecs;
        /** Counts the number of frames rendered. */
        TUint iFrame;
        /** Input handler that maps keycodes to inputs and stores the current state for each key. */
        CBillboardInput* iInputHandler;

        /** Application UI class for querying the client rectangle size. */ 
        CAknAppUi* iAppUi;

    public:  //data

        /** Used in DrawCallBack() method to do the actual OpenGL ES rendering.  */
        CBillboard* iBillboard;
    };

#endif

// End of File
