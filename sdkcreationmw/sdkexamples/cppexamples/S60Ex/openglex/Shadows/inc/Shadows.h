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


#ifndef SHADOWS_H
#define SHADOWS_H


// INCLUDES
#include <e32base.h> // for CBase definition
#include <GLES/gl.h> // OpenGL ES header file
#include "glutils.h" // Misc GLU and GLUT functions

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
class CShadows : public CBase
    {
    public:  // Constructors and destructor

        /**
         * Factory method for creating a new CShadows object.
         */
        static CShadows* NewL( TUint aWidth, TUint aHeight);

        /**
         * Destructor. Does nothing.
         */
        virtual ~CShadows();

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
         * Draws the ground plane.
         */
        void DrawPlane( void );

        /**
         * Draws the lamp.
         */
        void DrawLamp( void );

        /**
         * Draws the shadows.
         */
        void DrawShadowObject( void );

        /**
         * Draw the object.
         */
        void DrawObject( void );

        /**
         * Calculate proper shadow matrix.
         */
        void CalculateProjectionMatrix( void );

        /**
         * Renders one frame.
         * @param aFrame Number of the frame to be rendered.
         */
        void AppCycle( TInt aFrame );

        /**
         * Rotates light to left
         */
        void RotateLightLeft( void );

        /**
         * Rotates light to right
         */
        void RotateLightRight( void );

        /**
         * Notifies that the screen size has dynamically changed during execution of
         * this program. Resets the viewport to this new size.
         * @param aWidth New width of the screen.
         * @param aHeight New height of the screen.
         */
        void SetScreenSize( TUint aWidth, TUint aHeight );

    protected:  // New functions

        /**
         * Standard constructor that must never Leave.
         * Stores the given screen width and height.
         * @param aWidth Width of the screen.
         * @param aHeight Height of the screen.
         */
        CShadows( TUint aWidth, TUint aHeight);

        /**
         * Second phase contructor. Does nothing.
         */
        void ConstructL( void );

    private:  // Data

        /** Width of the screen */
        TUint iScreenWidth;

        /** Height of the screen */
        TUint iScreenHeight;


        /** Radius of the light. */
        GLfloat iLightRadius;

        /** Angle of the light. */
        GLfloat iLightAngle;


        /** X coordinate of the light */
        GLfloat iLightPosX;

        /** Y coordinate of the light */
        GLfloat iLightPosY;

        /** Z coordinate of the light */
        GLfloat iLightPosZ;


        /** Light position. */
        GLfloat iLightPos[4];

        /** Shadow projection matrix. */
        GLfloat iShadowMat[16];


        /** Whether the light is rotated left each frame or not. */
        TBool iIsLightRotatedLeft;

        /** Whether the light is rotated right each frame or not. */
        TBool iIsLightRotatedRight;

    };


#endif
//  End of File
