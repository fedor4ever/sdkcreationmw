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



// INCLUDE FILES
#include <e32std.h>
#include "SimpleCube.h"

// CONSTANTS

/** Vertice coordinates for the cube. */
static const GLbyte vertices[8 * 3] =
     {
     -1,  1,  1,
      1,  1,  1,
      1, -1,  1,
     -1, -1,  1,

     -1,  1, -1,
      1,  1, -1,
      1, -1, -1,
     -1, -1, -1
     };

/** Colors for vertices (Red, Green, Blue, Alpha). */
static const GLubyte colors[8 * 4] =
    {
    0  ,255,  0,255,
    0  ,  0,255,255,
    0  ,255,  0,255,
    255,  0,  0,255,

    0  ,  0,255,255,
    255,  0,  0,255,
    0  ,  0,255,255,
    0  ,255,  0,255
    };


/**
 * Indices for drawing the triangles.
 * The color of the triangle is determined by
 * the color of the last vertex of the triangle.
 */
static const GLubyte triangles[12 * 3] =
    {
    /* front */
    1,0,3,
    1,3,2,

    /* right */
    2,6,5,
    2,5,1,

    /* back */
    7,4,5,
    7,5,6,

    /* left */
    0,4,7,
    0,7,3,

    /* top */
    5,4,0,
    5,0,1,

    /* bottom */
    3,7,6,
    3,6,2
    };

/** First set of indices for drawing the triangle fans. */
static const GLubyte fanOne[6 * 3] =
    {
    1,0,3,
    1,3,2,
    1,2,6,
    1,6,5,
    1,5,4,
    1,4,0
    };


/** Second set of indices for drawing the triangle fans. */
static const GLubyte fanTwo[6 * 3] =
    {
    7,4,5,
    7,5,6,
    7,6,2,
    7,2,3,
    7,3,0,
    7,0,4
    };



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSimpleCube::NewL
//
// The usual Symbian NewL implementation.
// Creates the object, pushes it to cleanup stack and calls ContructL.
// Returns the contructed object or leaves.
// -----------------------------------------------------------------------------
//

CSimpleCube* CSimpleCube::NewL( TUint aWidth, TUint aHeight )
    {
    /* Symbian 2-phase constructor. Calls both the default
       C++ constructor and Symbian ConstructL methods */
    CSimpleCube* self = new (ELeave) CSimpleCube( aWidth, aHeight );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CSimpleCube::CSimpleCube
//
// C++ constructor. Initializes the size instance variables from arguments.
// -----------------------------------------------------------------------------
//

CSimpleCube::CSimpleCube( TUint aWidth, TUint aHeight ) :
    iScreenWidth( aWidth ),
    iScreenHeight( aHeight )
    {
    }

// -----------------------------------------------------------------------------
// CSimpleCube::~CSimpleCube
//
// C++ destructor.
// -----------------------------------------------------------------------------
//

CSimpleCube::~CSimpleCube()
    {
    }

// -----------------------------------------------------------------------------
// CSimpleCube::ConstructL
//
// 2nd phase constructor.
// -----------------------------------------------------------------------------
//

void CSimpleCube::ConstructL( void )
    {
    }

// -----------------------------------------------------------------------------
// CSimpleCube::AppInit
//
// Initializes OpenGL ES, sets the vertex and color arrays and pointers,
// and selects the shading mode.
// -----------------------------------------------------------------------------
//

void CSimpleCube::AppInit( void )
    {
    // Initialize viewport and projection.
		SetScreenSize( iScreenWidth, iScreenHeight );

    // Set the screen background color.
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    // Enable back face culling.
    glEnable( GL_CULL_FACE  );

    // Enable vertex arrays.
    glEnableClientState( GL_VERTEX_ARRAY );

    // Set array pointers.
    glVertexPointer( 3, GL_BYTE, 0, vertices );

    // Enable color arrays.
    glEnableClientState( GL_COLOR_ARRAY );

    // Set color pointers.
    glColorPointer( 4, GL_UNSIGNED_BYTE, 0, colors );

    // Set the initial shading mode
    glShadeModel( GL_FLAT );

    // Do not use perspective correction
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );

    // Set the initial drawing mode
    iDrawingMode = ETriangles;
    }

// -----------------------------------------------------------------------------
// CSimpleCube::AppExit
//
// Release any allocations made in AppInit.
// -----------------------------------------------------------------------------
//

void CSimpleCube::AppExit( void )
    {
    }

// -----------------------------------------------------------------------------
// CSimpleCube::DrawBox
//
// Draws a box with triangles or triangle fans depending on the current rendering mode.
// Scales the box to the given size using glScalef.
// -----------------------------------------------------------------------------
//

void CSimpleCube::DrawBox( GLfloat aSizeX, GLfloat aSizeY, GLfloat aSizeZ )
    {
    glScalef( aSizeX, aSizeY, aSizeZ );

    if ( iDrawingMode == ETriangles )
        {
        glDrawElements( GL_TRIANGLES, 12 * 3, GL_UNSIGNED_BYTE, triangles );
        }
    else if ( iDrawingMode == ETriangleFans )
        {
        glDrawElements( GL_TRIANGLE_FAN, 6 * 3, GL_UNSIGNED_BYTE, fanOne );
        glDrawElements( GL_TRIANGLE_FAN, 6 * 3, GL_UNSIGNED_BYTE, fanTwo );
        }
    }

// -----------------------------------------------------------------------------
// CSimpleCube::AppCycle
//
// Draws and animates the objects.
// The frame number determines the amount of rotation.
// -----------------------------------------------------------------------------
//

void CSimpleCube::AppCycle( TInt aFrame )
    {
    const GLint cameraDistance = 100;

    glClear( GL_COLOR_BUFFER_BIT );

    /* Animate and draw box */
    glLoadIdentity();
    glTranslatex( 0 , 0 , -cameraDistance << 16 );
    glRotatex( aFrame << 16, 1 << 16,    0   ,    0    );
    glRotatex( aFrame << 15,    0   , 1 << 16,    0    );
    glRotatex( aFrame << 14,    0   ,    0   , 1 << 16 );
    DrawBox( 15.f, 15.f, 15.f );
    }

//----------------------------------------------------------
// The following methods are called by the CSimpleCubeAppUi
// class when handling the incoming menu events.
//----------------------------------------------------------

// -----------------------------------------------------------------------------
// CSimpleCube::FlatShading
//
// Sets the GL shading model to flat.
// -----------------------------------------------------------------------------
//

void CSimpleCube::FlatShading( void )
    {
    glShadeModel( GL_FLAT );
    }

// -----------------------------------------------------------------------------
// CSimpleCube::SmoothShading
//
// Sets the GL shading model to smooth.
// -----------------------------------------------------------------------------
//

void CSimpleCube::SmoothShading( void )
    {
    glShadeModel( GL_SMOOTH );
    }

// -----------------------------------------------------------------------------
// CSimpleCube::TriangleMode
//
// Sets the rendering mode to triangles.
// -----------------------------------------------------------------------------
//

void CSimpleCube::TriangleMode( void )
    {
    iDrawingMode = ETriangles;
    }

// -----------------------------------------------------------------------------
// CSimpleCube::TriangleFanMode
//
// Sets the rendering mode to triangle fans.
// -----------------------------------------------------------------------------
//

void CSimpleCube::TriangleFanMode( void )
    {
    iDrawingMode = ETriangleFans;
    }

// -----------------------------------------------------------------------------
// CSimpleCube::SetScreenSize
// Reacts to the dynamic screen size change during execution of this program.
// -----------------------------------------------------------------------------
//
void CSimpleCube::SetScreenSize( TUint aWidth, TUint aHeight )
    {
    iScreenWidth  = aWidth;
    iScreenHeight = aHeight;

    // Reinitialize viewport and projection.
    glViewport( 0, 0, iScreenWidth, iScreenHeight );

    // Recalculate the view frustrum
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    GLfloat aspectRatio = (GLfloat)(iScreenWidth) / (GLfloat)(iScreenHeight);
    glFrustumf( FRUSTUM_LEFT * aspectRatio, FRUSTUM_RIGHT * aspectRatio,
                FRUSTUM_BOTTOM, FRUSTUM_TOP,
                FRUSTUM_NEAR, FRUSTUM_FAR );
    glMatrixMode( GL_MODELVIEW );
    }

// End of File

