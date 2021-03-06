/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include "SimpleCubePixmap.h"

// MACROS
#define cameraDistance 100

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




// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSimpleCubePixmap::NewL
// 
// The usual Symbian NewL implementation. 
// Creates the object, pushes it to cleanup stack and calls ContructL.
// Returns the contructed object or leaves.
// -----------------------------------------------------------------------------
//

CSimpleCubePixmap* CSimpleCubePixmap::NewL( TUint aWidth, TUint aHeight )
    {
    /* Symbian 2-phase constructor. Calls both the default 
       C++ constructor and Symbian ConstructL methods */
    CSimpleCubePixmap* self = new (ELeave) CSimpleCubePixmap( aWidth, aHeight );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// -----------------------------------------------------------------------------
// CSimpleCubePixmap::CSimpleCubePixmap
// 
// C++ constructor. Initializes the size instance variables from arguments.
// -----------------------------------------------------------------------------
//

CSimpleCubePixmap::CSimpleCubePixmap( TUint aWidth, TUint aHeight ) : 
    iScreenWidth( aWidth ),
    iScreenHeight( aHeight )
    {
    }

// Destructor. 
CSimpleCubePixmap::~CSimpleCubePixmap()
    {
    }


// -----------------------------------------------------------------------------
// CSimpleCubePixmap::ConstructL
// 
// 2nd phase constructor.
// -----------------------------------------------------------------------------
//

void CSimpleCubePixmap::ConstructL( void )
    {
    }


// -----------------------------------------------------------------------------
// CSimpleCubePixmap::AppInit
// 
// Initializes OpenGL ES, sets the vertex and color arrays and pointers, 
// and selects the shading mode.
// -----------------------------------------------------------------------------
//

void CSimpleCubePixmap::AppInit( void )
    {
    // Set the screen background color. 
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    // Enable back face culling. 
    glEnable( GL_CULL_FACE  );

    // Initialize viewport and projection. 
    glViewport( 0, 0, iScreenWidth, iScreenHeight );
    glMatrixMode( GL_PROJECTION );

    // Calculate the view frustrum
    GLfloat aspectRatio = (GLfloat)(iScreenWidth) / (GLfloat)(iScreenHeight);
    glFrustumf( FRUSTUM_LEFT * aspectRatio, FRUSTUM_RIGHT * aspectRatio,
                FRUSTUM_BOTTOM, FRUSTUM_TOP,
                FRUSTUM_NEAR, FRUSTUM_FAR );

    glMatrixMode( GL_MODELVIEW );

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

    
    }


// -----------------------------------------------------------------------------
// CSimpleCubePixmap::AppExit
// 
// Release any allocations made in AppInit.
// -----------------------------------------------------------------------------
//
void CSimpleCubePixmap::AppExit( void )
    {
    }


// -----------------------------------------------------------------------------
// CSimpleCubePixmap::DrawBox
// 
// Draws a box with triangles.
// Scales the box to the given size using glScalef.
// -----------------------------------------------------------------------------
//

void CSimpleCubePixmap::DrawBox( GLfloat aSizeX, GLfloat aSizeY, GLfloat aSizeZ )
    {
    glScalef( aSizeX, aSizeY, aSizeZ );

    glDrawElements( GL_TRIANGLES, 12 * 3, GL_UNSIGNED_BYTE, triangles );
    }


// -----------------------------------------------------------------------------
// CSimpleCubePixmap::AppCycle
// 
// Draws and animates the objects.
// The frame number determines the amount of rotation.
// -----------------------------------------------------------------------------
//

void CSimpleCubePixmap::AppCycle( TInt aFrame )
    {

    glClear( GL_COLOR_BUFFER_BIT );

    // Animate and draw box 
    glLoadIdentity();
    glTranslatex( 0 , 0 , -cameraDistance << 16 );
    glRotatex( aFrame << 16, 1 << 16,    0   ,    0    );
    glRotatex( aFrame << 15,    0   , 1 << 16,    0    );
    glRotatex( aFrame << 14,    0   ,    0   , 1 << 16 );
    DrawBox( 15.f, 15.f, 15.f );
    }


//----------------------------------------------------------
// The following methods are called by the CSimpleCubePixmapAppUi 
// class when handling the incoming menu events.
//----------------------------------------------------------

// -----------------------------------------------------------------------------
// CSimpleCube::FlatShading
// 
// Sets the GL shading model to flat.
// -----------------------------------------------------------------------------
//

void CSimpleCubePixmap::FlatShading( void )
    {
    glShadeModel( GL_FLAT );
    }


// -----------------------------------------------------------------------------
// CSimpleCube::SmoothShading
// 
// Sets the GL shading model to smooth.
// -----------------------------------------------------------------------------
//

void CSimpleCubePixmap::SmoothShading( void )
    {
    glShadeModel( GL_SMOOTH );
    }


// -----------------------------------------------------------------------------
// CSimpleCubePixmap::SetScreenSize
// Reacts to the dynamic screen size change during execution of this program.
// -----------------------------------------------------------------------------
//
void CSimpleCubePixmap::SetScreenSize( TUint aWidth, TUint aHeight )
    {
    iScreenWidth  = aWidth;
    iScreenHeight = aHeight;

    // Set the screen background color. 
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    // Reinitialize OpenGL ES rendering. Needed because context has been changed.

    // Enable back face culling. 
    glEnable( GL_CULL_FACE  );

    // Initialize viewport and projection. 
    glViewport( 0, 0, iScreenWidth, iScreenHeight );
    glMatrixMode( GL_PROJECTION );

    GLfloat ar = (GLfloat)(iScreenHeight) / (GLfloat)(iScreenWidth);
    glFrustumf( -1.f, 1.f, -1.f*ar, 1.f*ar, 3.f, 1000.f );


    glFrustumf( -1.f, 1.f, -1.f, 1.f, 3.f, 1000.f );
    glMatrixMode( GL_MODELVIEW );

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
