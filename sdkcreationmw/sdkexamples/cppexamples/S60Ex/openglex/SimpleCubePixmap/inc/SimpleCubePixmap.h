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


#ifndef SIMPLECUBEPIXMAP_H
#define SIMPLECUBEPIXMAP_H

//  INCLUDES
#include <e32base.h> // for CBase definition
#include <GLES/gl.h> // OpenGL ES header file

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
class CSimpleCubePixmap : public CBase
    {
    public:  // Constructors and destructor

        /**
         * Factory method for creating a new CSimpleCubePixmap object.
         */
        static CSimpleCubePixmap* NewL( TUint aWidth, TUint aHeight);

        /**
         * Destructor. Does nothing.
         */
        virtual ~CSimpleCubePixmap();

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
         * Draws a 3D box. Scales the box to the given size using glScalef.
         * @param  aSizeX X-size of the box.
         * @param  aSizeY Y-size of the box.
         * @param  aSizeZ Z-size of the box.
         */
        void DrawBox( GLfloat aSizeX, GLfloat aSizeY, GLfloat aSizeZ );

        /**
         * Renders one frame.
         * @param aFrame Number of the frame to be rendered.
         */
        void AppCycle( TInt aFrame );

        /**
         * Sets the shading mode to flat.
         */
        void FlatShading( void );

        /**
         * Sets the shading to smooth (gradient).
         */
        void SmoothShading( void );

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
        CSimpleCubePixmap( TUint aWidth, TUint aHeight);

        /**
         * Second phase contructor. Does nothing.
         */
        void ConstructL( void );

    private: // Data

        /** Width of the screen */
        TUint iScreenWidth;

        /** Height of the screen */
        TUint iScreenHeight;
    };

#endif // SIMPLECUBEPIXMAP_H

// End of File
