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


#ifndef HIERARCHY_H
#define HIERARCHY_H

// INCLUDES
#include <e32base.h> // for CBase definition
#include <GLES/gl.h> // OpenGL ES header file
#include "Glutils.h" // Misc GLU and GLUT methods

// MACROS
#define ROTATE_ELBOW   1        // Rotation mode for elbow rotation.
#define ROTATE_FINGERS 2        // Rotation mode for finger rotation.

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
class CHierarchy : public CBase
    {
    public:  // Constructors and destructor

        /**
         * Factory method for creating a new CSimpleCube object.
         */
        static CHierarchy* NewL( TUint aWidth, TUint aHeight);

        /**
         * Destructor. Does nothing.
         */
        virtual ~CHierarchy();

    public: // New functions

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
         * Draws a 3D box.
         */
        void DrawBox( void );

        /**
         * Draw a coordinate system
         */
        void DrawCoordSystem( void );

        /**
         * Renders one frame.
         * @param aFrame Number of the frame to be rendered.
         */
        void AppCycle( TInt aFrame );

        /**
         * Toggle object "up" rotation on and off based on the given value.
         * @param aStart True if object should be rotated up every frame.
         *               False if object shouldn't be rotated up every frame.
         */
        void RotateObjectUp( TBool aStart );

        /**
         * Toggle object "down" rotation on and off based on the given value.
         * @param aStart True if object should be rotated down every frame.
         *               False if object shouldn't be rotated down every frame.
         */
        void RotateObjectDown( TBool aStart );

        /**
         * Toggles the rotation mode between rotating the arm or the fingers
         */
        void SwitchRotateMode( void );

        /**
         * Notifies that the screen size has dynamically changed during execution of
         * this program. Resets the viewport to this new size.
         * @param aWidth New width of the screen.
         * @param aHeight New height of the screen.
         */
        void SetScreenSize( TUint aWidth, TUint aHeight );

    protected:

        /**
         * Standard constructor that must never Leave.
         * Stores the given screen width and height.
         * @param aWidth Width of the screen.
         * @param aHeight Height of the screen.
         */
        CHierarchy( TUint aWidth, TUint aHeight);

        /**
         * Second phase contructor. Does nothing.
         */
        void ConstructL( void );

    private:  // Data

        /** Width of the screen */
        TUint iScreenWidth;

        /** Height of the screen */
        TUint iScreenHeight;

        /** The current rotation mode (ROTATE_ELBOW or ROTATE_FINGERS).  */
        TInt   iRotateMode;

        /** Whether object should be rotated up during each rendered frame or not. */
        TBool  iRotateUpEnabled;

        /** Whether object should be rotated down during each rendered frame or not. */
        TBool   iRotateDownEnabled;

        /** Current rotation angle of the elbow. */
        TReal32 iElbowRotAngle;

        /** Current rotation angle of the fingers. */
        TReal32 iFingersRotAngle;
    };

#endif

// End of File
