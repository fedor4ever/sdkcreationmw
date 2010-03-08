/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef SIMPLELIGHT_H
#define SIMPLELIGHT_H

// INCLUDES
#include <e32base.h> // for CBase definition
#include <GLES/gl.h> // OpenGL ES header file

/** Enumerated value that is used to switch between duck and head models. */
typedef enum { EDuckModel, EHeadModel } TUsedModel;

// MACROS
#define FRUSTUM_LEFT   -1.f     //left vertical clipping plane
#define FRUSTUM_RIGHT   1.f     //right vertical clipping plane
#define FRUSTUM_BOTTOM -1.f     //bottom horizontal clipping plane
#define FRUSTUM_TOP     1.f     //top horizontal clipping plane
#define FRUSTUM_NEAR    3.f     //near depth clipping plane
#define FRUSTUM_FAR  1000.f     //far depth clipping plane

// CLASS DECLARATION

/**
 * Class that does the actual OpenGL ES rendering.
 */
class CSimpleLight : public CBase
    {
    public:  // Constructors and destructor

        /**
         * Factory method for creating a new CSimpleCube object.
         */
        static CSimpleLight* NewL( TUint aWidth, TUint aHeight);

        /**
         * Destructor. Does nothing.
         */
        virtual ~CSimpleLight();

    public:  // New functions

        /**
         * Initializes OpenGL ES, sets the vertex and color
         * arrays and pointers. Also selects the shading mode.
         */
        void AppInit( void );

        /**
         * Called upon application exit. Does nothing.
         */
        void AppExit( void );

        /**
         * Renders one frame.
         * @param aFrame Number of the frame to be rendered.
         */
        void AppCycle( TInt aFrame );

        /**
         * Toggles lighting enabled/disabled.
         */
        void ToggleLighting( void );

        /**
         * Toggles lamp enabled/disabled.
         */
        void ToggleLamp( void );

        /**
         * Toggles spot light enabled/disabled.
         */
        void ToggleSpot( void );

        /**
         * Changes the current model to duck object.
         */
        void DuckModel( void );

        /**
         * Changes the current model to head object.
         */
        void HeadModel( void );

        /**
         * Notifies that the screen size has dynamically changed during execution of
         * this program. Resets the viewport to this new size.
         * @param aWidth New width of the screen.
         * @param aHeight New height of the screen.
         */
        void SetScreenSize( TUint aWidth, TUint aHeight );

    protected: // New functions

        /**
         * Standard constructor that must never Leave.
         * Stores the given screen width and height.
         * @param aWidth Width of the screen.
         * @param aHeight Height of the screen.
         */
        CSimpleLight( TUint aWidth, TUint aHeight );

        /**
         * Second phase contructor. Does nothing.
         */
        void ConstructL( void );

        /**
         * Helper function that is used to make the duck 'quak'.
         * Calculates the sine for the given angle. Returns 0 if any
         * errors occur while calling the Math::Sin() method.
         * @param aRad Radian angle whose sine is to be calculated.
         * @return The sine of the given angle or 0 if error occured while calculating the sine.
         */
        GLfloat sin( GLfloat aRad );

    public: // Data

        /** Whether or not the lighting is enabled. Used when building the menu for the application. */
        TBool iLightingEnabled;

        /** Whether or not the lamp is enabled. Used when building the menu for the application. */
        TBool iLampEnabled;

        /** Whether or not the spotlight is enabled. Used when building the menu for the application. */
        TBool iSpotEnabled;

    private: // Data

        /** Width of the screen */
        TUint iScreenWidth;

        /** Height of the screen */
        TUint iScreenHeight;

        /** Enumerated value for switching between the HEAD and DUCK model. */
        TUsedModel iObjectModel;
    };

#endif // SIMPLELIGHT_H
