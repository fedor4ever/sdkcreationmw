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


// INCLUDES
#include <e32std.h>
#include <e32def.h>

#include "PickSelect.h"

// MACROS
#define cameraDistance 100

/** First alpha color value for object selection. */
#define BOX_ALPHA_ONE 22
/** Second alpha color value for object selection. */
#define BOX_ALPHA_TWO 33


// LOCAL CONSTANTS
/** Four diagonal line segments, making an 'X' or cross. */
static const GLshort cross[8 * 2] =
    {
    200,200,
    800,800,

    -200,200,
    -800,800,

    -200,-200,
    -800,-800,

    200,-200,
    800,-800
    };

/** Indices for 4 line segments (BTW, colors with current color, not array). */
static const GLubyte crossIndices[4 * 2] =
    {
    0,1,
    2,3,
    4,5,
    6,7
    };


/** Vertex coordinates for the cube. */
static const GLbyte vertices[24 * 3] =
    {
    /* top */
    -1,  1,  1,
    1,  1,  1,
    1, -1,  1,
    -1, -1,  1,

    /* front */
    1,  1,  1,
    1,  1, -1,
    1, -1, -1,
    1, -1,  1,

    /* right */
    -1,  1,  1,
    -1,  1, -1,
    1,  1, -1,
    1,  1,  1,

    /* left */
    1, -1,  1,
    1, -1, -1,
    -1, -1, -1,
    -1, -1,  1,

    /* back */
    -1, -1,  1,
    -1, -1, -1,
    -1,  1, -1,
    -1,  1,  1,

    /* bottom */
    -1,  1, -1,
    1,  1, -1,
    1, -1, -1,
    -1, -1, -1
    };


/**
 * Indices for drawing the triangles Note: these triangles are flat shaded,
 * and the color comes from the last vertex of a triangle. Alpha is used
 * to give an 'index' for picking a triangle, zero alpha is reserved for background.
 */
static const GLubyte triangles[12 * 3] =
    {
    /* top */
    1,0,3,    // alpha 1
    1,3,2,    // alpha 2

    /* front */
    5,4,7,    // alpha 3
    5,7,6,    // alpha 4

    /* right */
    9,8,11,   // alpha 5
    9,11,10,  // alpha 6

    /* left */
    13,12,15, // alpha 7
    13,15,14, // alpha 8

    /* back */
    17,16,19, // alpha 9
    17,19,18, // alpha 10

    /* bottom */
    21,22,23, // alpha 11
    21,23,20  // alpha 12
    };


/**
 * Colors for vertices (Red, Green, Blue, Alpha) Alpha denotes a triangle index,
 * 255 is reserved for debugging (no triangle should get alpha == 255).
 */
static const GLubyte colors[24 * 4] =
    {
    /* top */
    255,   0,   0, 255,
    255,   0,   0, 255,
    200,   0, 200, 2,
    200,   0,   0, 1,

    /* front */
    0  ,   0, 255, 255,
    0  ,   0, 255, 255,
    0  , 200, 200, 4,
    0  ,   0, 200, 3,

    /* right */
    0  , 255,   0, 255,
    0  , 255,   0, 255,
    200, 200,   0, 6,
    0  , 200,   0, 5,

    /* left */
    255, 160,   0, 255,
    255, 160,   0, 255,
    160,   0, 200, 8,
    200, 160,   0, 7,

    /* back */
    120, 120,   0, 255,
    120, 120,   0, 255,
    10 , 120,   0, 10,
    120, 120,   0, 9,

    /* bottom */
    120,   0, 120, 12,
    120,   0,   0, 255,
    120,   0,   0, 255,
    120,   0,   0, 11
    };


/**
 * Define colors for vertices (Red, Green, Blue, Alpha) Alpha denotes a
 * triangle index, 255 is reserved for debugging (no triangle should get
 * alpha == 255)
 */
static const GLubyte colorsBox1[24 * 4] =
    {
    /* top */
    255,   0,   0, 255,
    255,   0,   0, 255,
    200,   0, 200, BOX_ALPHA_ONE,
    200,   0,   0, BOX_ALPHA_ONE,

    /* front */
    0  ,   0, 255, 255,
    0  ,   0, 255, 255,
    0  , 200, 200, BOX_ALPHA_ONE,
    0  ,   0, 200, BOX_ALPHA_ONE,

    /* right */
    0  , 255,   0, 255,
    0  , 255,   0, 255,
    200, 200,   0, BOX_ALPHA_ONE,
    0  , 200,   0, BOX_ALPHA_ONE,

    /* left */
    255, 160,   0, 255,
    255, 160,   0, 255,
    160,   0, 200, BOX_ALPHA_ONE,
    200, 160,   0, BOX_ALPHA_ONE,

    /* back */
    120, 120,   0, 255,
    120, 120,   0, 255,
    10 , 120,   0, BOX_ALPHA_ONE,
    120, 120,   0, BOX_ALPHA_ONE,

    /* bottom */
    120,   0, 120, BOX_ALPHA_ONE,
    120,   0,   0, 255,
    120,   0,   0, 255,
    120,   0,   0, BOX_ALPHA_ONE
    };

/**
 * Colors for vertices (Red, Green, Blue, Alpha) Alpha denotes a
 * triangle index, 255 is reserved for debugging (no triangle should
 * get alpha == 255).
 */
static const GLubyte colorsBox2[24 * 4] =
    {
    /* top */
    255,   0,   0, 255,
    255,   0,   0, 255,
    200,   0, 200, BOX_ALPHA_TWO,
    200,   0,   0, BOX_ALPHA_TWO,

    /* front */
    0  ,   0, 255, 255,
    0  ,   0, 255, 255,
    0  , 200, 200, BOX_ALPHA_TWO,
    0  ,   0, 200, BOX_ALPHA_TWO,

    /* right */
    0  , 255,   0, 255,
    0  , 255,   0, 255,
    200, 200,   0, BOX_ALPHA_TWO,
    0  , 200,   0, BOX_ALPHA_TWO,

    /* left */
    255, 160,   0, 255,
    255, 160,   0, 255,
    160,   0, 200, BOX_ALPHA_TWO,
    200, 160,   0, BOX_ALPHA_TWO,

    /* back */
    120, 120,   0, 255,
    120, 120,   0, 255,
    10 , 120,   0, BOX_ALPHA_TWO,
    120, 120,   0, BOX_ALPHA_TWO,

    /* bottom */
    120,   0, 120, BOX_ALPHA_TWO,
    120,   0,   0, 255,
    120,   0,   0, 255,
    120,   0,   0, BOX_ALPHA_TWO
    };


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPickSelect::CPickSelect
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPickSelect::CPickSelect( TUint aWidth, TUint aHeight )
:iScreenWidth(aWidth), iScreenHeight(aHeight)
    {
    // Recalculate aspect ratio
    if ( iScreenHeight != 0 )
        {
        iAspectRatio = (GLfloat)iScreenWidth / (GLfloat)iScreenHeight;
        }
    else
        {
        iAspectRatio = 1.0;
        }
    }


// -----------------------------------------------------------------------------
// CPickSelect::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPickSelect::ConstructL( void )
    {
    }


// -----------------------------------------------------------------------------
// CPickSelect::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPickSelect* CPickSelect::NewL( TUint aWidth, TUint aHeight )
    {
    /* Symbian 2-phase constructor. Calls both the default
       C++ constructor and Symbian ConstructL methods */
    CPickSelect* self = new (ELeave) CPickSelect( aWidth, aHeight );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor.
CPickSelect::~CPickSelect()
    {
    }


// -----------------------------------------------------------------------------
// CPickSelect::AppInit
// Initialize OpenGL ES, set the vertex and color arrays and pointers,
//  and select the shading mode.
// -----------------------------------------------------------------------------
//
void CPickSelect::AppInit( void )
    {
    // Initialize viewport.
    SetScreenSize( iScreenWidth, iScreenHeight );

    // Set the screen background color.
    glClearColor( 0.f, 0.f, 0.f, 0.f );

    glEnable( GL_DEPTH_TEST );   // z-compare enabled
    glEnable( GL_CULL_FACE  );   // backface culling enabled


    // Enable vertex and color arrays.
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );

    glLineWidth( 1.0 );

    // Set the initial shading mode
    glShadeModel( GL_FLAT );

    iSelectionMode = POLYGON_SELECTION;
    iCubeRotX      = GLfloat( 0.0 );
    iCubeRotY      = GLfloat( 0.0 );
    iCursorPosX    = GLint( 30 );
    iCursorPosY    = GLint( 30 );

    iMoveUp       = EFalse;
    iMoveDown     = EFalse;
    iMoveLeft     = EFalse;
    iMoveRight    = EFalse;
    iSelectionIdx = 0;

    }


// -----------------------------------------------------------------------------
// CPickSelect::AppExit
// Release any allocations made in AppInit, reset NGL if necessary.
// This is called if you want to use the same instance again, without
// creating a new instance.
// -----------------------------------------------------------------------------
//
void CPickSelect::AppExit( void )
{
}


// -----------------------------------------------------------------------------
// CPickSelect::AppCycle
// Draws and animates the objects
// -----------------------------------------------------------------------------
//
void CPickSelect::AppCycle( TInt aFrame )
    {

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    if ( iSelectionMode == POLYGON_SELECTION )
        {
        DrawSingleBox( aFrame );
        }
    else if ( iSelectionMode == OBJECT_SELECTION )
        {
        DrawMultipleBoxes( aFrame );
        }

    // Update the cursor position and redraw it
    UpdateCursorPosition();
    DrawCursor();
    }


// -----------------------------------------------------------------------------
// CPickSelect::UpdateCursorPosition
// Update cursor position data.
// Make sure that the cursor stays inside the screen, because
// the viewport size is 208 x 208. See DrawCursor for more comments.
// -----------------------------------------------------------------------------
//
void CPickSelect::UpdateCursorPosition( void )
    {
    // Check if cursor should move in up/down direction
    if ( iMoveUp )
        {
        iCursorPosY += 2;
        if ( iCursorPosY > iScreenWidth )
            {
            iCursorPosY = iScreenWidth;
            }
        }
    else if ( iMoveDown )
        {
        iCursorPosY -= 2;
        if ( iCursorPosY < 0 )
            {
            iCursorPosY = 0;
            }
        }

    // Check if cursor should move in left/right direction
    if ( iMoveLeft )
        {
        iCursorPosX -= 2;
        if ( iCursorPosX < 0 )
            {
            iCursorPosX = 0;
            }
        }
    else if ( iMoveRight )
        {
        iCursorPosX += 2;
        if ( iCursorPosX > iScreenHeight )
            {
            iCursorPosX = iScreenHeight;
            }
        }
    }



// -----------------------------------------------------------------------------
// CPickSelect::DrawCursor
// Draw the selector cursor.
// -----------------------------------------------------------------------------
//
void CPickSelect::DrawCursor( void )
    {
    // Setup orthogonal projection for drawing the cursor to the screen
    glViewport( 0, 0, iScreenWidth, iScreenHeight );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrthof( 0.f, (float) iScreenWidth, 0.f, (float) iScreenHeight, -1, 1 ); // set the same size as viewport

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslatex( iCursorPosX << 16 , iCursorPosY << 16, 0 );
    glScalex( 65536 / 100, 65536 / 100, 0); // Scale down the geometry

    glVertexPointer( 2, GL_SHORT, 0, cross );
    glDisableClientState( GL_COLOR_ARRAY ); // disable color array
    glColor4f( 0.f, 1.f, 1.f, 1.f );        // use current color for drawing
    glDrawElements( GL_LINES, 8, GL_UNSIGNED_BYTE, crossIndices );
    glEnableClientState( GL_COLOR_ARRAY );  // re-enable color array
    // Switch back to original Viewport definitions
    glViewport( 0, 0, iScreenWidth, iScreenHeight );

    }


// -----------------------------------------------------------------------------
// CPickSelect::DrawSingleBox
// Draw a single cube and white lines around the selected polygon
// if one is selected.
// -----------------------------------------------------------------------------
//
void CPickSelect::DrawSingleBox( TInt aFrame )
    {
      // Set the projection and modelview matrices
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glFrustumf( FRUSTUM_LEFT * iAspectRatio, FRUSTUM_RIGHT * iAspectRatio,
                FRUSTUM_BOTTOM, FRUSTUM_TOP,
                FRUSTUM_NEAR, FRUSTUM_FAR );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslatex( 0 , 0 , -cameraDistance << 16 );
    glRotatex( aFrame << 16, 1 << 16,    0   ,    0    );
    glRotatex( aFrame << 15,    0   , 1 << 16,    0    );
    glRotatex( aFrame << 14,    0   ,    0   , 1 << 16 );

    // draw box
    glVertexPointer( 3, GL_BYTE, 0, vertices );
    glColorPointer( 4, GL_UNSIGNED_BYTE, 0, colors );
    glPushMatrix();
    glScalex( 10 << 16, 10 << 16, 10 << 16 );
    glDrawElements( GL_TRIANGLES, 12 * 3, GL_UNSIGNED_BYTE, triangles );


    /* draw white lines around the polygon selected. Zero is for background
       and 255 is for debugging purposes only. */
    if ( iSelectionIdx != 0 && iSelectionIdx != 255 )
        {
        // gather indices of the selected polygon
        for ( int i = 0; i < 3; i++ )
            {
            iSelectedPoly[i] = triangles[ 3*(iSelectionIdx - 1) + i ];
            }

        glDisable( GL_DEPTH_TEST ); // draw on top of the existing graphics
        glDisableClientState( GL_COLOR_ARRAY ); // disable color array
        glColor4f( 1.f, 1.f, 1.f, 1.f ); // use white for the selected polygon
        glDrawElements( GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, iSelectedPoly );
        glEnableClientState( GL_COLOR_ARRAY ); // re-enable color array
        glEnable( GL_DEPTH_TEST );
        }

    glPopMatrix();
    }


// -----------------------------------------------------------------------------
// CPickSelect::DrawMultipleBoxes
// Draw two rotating boxes.
// The selected cube, if there is one, is draw in wireframe mode.
// -----------------------------------------------------------------------------
//
void CPickSelect::DrawMultipleBoxes( TInt aFrame )
    {
    // Set the projection and modelview matrices
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glFrustumf( FRUSTUM_LEFT * iAspectRatio, FRUSTUM_RIGHT * iAspectRatio,
                FRUSTUM_BOTTOM, FRUSTUM_TOP,
                FRUSTUM_NEAR, FRUSTUM_FAR );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslatex( 0 , 0 , -cameraDistance << 16 );

    // Set the correct vertex pointer
    glVertexPointer( 3, GL_BYTE, 0, vertices );

    /* Check whether or not the first box was selected.
       If it was, draw it completely white, otherwise with filled polygons */
    glEnableClientState( GL_COLOR_ARRAY ); // enable color array
    glColor4f( 1.f, 1.f, 1.f, 1.f );       // set color to white
    if ( iSelectionIdx == BOX_ALPHA_ONE )
        {
        glDisableClientState( GL_COLOR_ARRAY ); // disable color array
        }

    // draw box1
    glColorPointer( 4, GL_UNSIGNED_BYTE, 0, colorsBox1 );
    glPushMatrix();
    glTranslatex( 12 << 16, 0, 0 );
    glRotatex( 40 << 16,       1 << 16,    0   , 1 << 16 );
    glRotatex( 2*aFrame << 16, 1 << 16,    0   ,    0    );
    glRotatex( 2*aFrame << 15,    0   , 1 << 16,    0    );
    glRotatex( 2*aFrame << 14,    0   ,    0   , 1 << 16 );
    glScalex( 8 << 16, 8 << 16, 8 << 16 );
    glDrawElements( GL_TRIANGLES, 12 * 3, GL_UNSIGNED_BYTE, triangles );
    glPopMatrix();

    /* Check whether or not the second box was selected.
       If it was, draw it completely white, otherwise with filled polygons */
    glEnableClientState( GL_COLOR_ARRAY ); // enable color array
    if ( iSelectionIdx == BOX_ALPHA_TWO )
        {
        glDisableClientState( GL_COLOR_ARRAY ); // disable color array
        }

    // draw box2
    glColorPointer( 4, GL_UNSIGNED_BYTE, 0, colorsBox2 );
    glPushMatrix();
    glTranslatex( -12 << 16, 0, 0 );
    glRotatex( 2*aFrame << 16, 1 << 16,    0   ,    0    );
    glRotatex( 2*aFrame << 15,    0   , 1 << 16,    0    );
    glRotatex( 2*aFrame << 14,    0   ,    0   , 1 << 16 );
    glScalex( 8 << 16, 8 << 16, 8 << 16 );
    glDrawElements( GL_TRIANGLES, 12 * 3, GL_UNSIGNED_BYTE, triangles );
    glPopMatrix();

    // By default we want to use color arrays.
    glEnableClientState( GL_COLOR_ARRAY ); // enable color array
    }


//----------------------------------------------------------
// The following methods are called by the CPickSelectAppUi
// class when handling the incoming menu events.
//----------------------------------------------------------


// -----------------------------------------------------------------------------
// CPickSelect::MoveUp
// -----------------------------------------------------------------------------
//
void CPickSelect::MoveUp( TBool move )
    {
    iMoveUp = move;
    }

// -----------------------------------------------------------------------------
// CPickSelect::MoveDown
// -----------------------------------------------------------------------------
//
void CPickSelect::MoveDown( TBool move )
    {
    iMoveDown = move;
    }

// -----------------------------------------------------------------------------
// CPickSelect::MoveLeft
// -----------------------------------------------------------------------------
//
void CPickSelect::MoveLeft( TBool move )
    {
    iMoveLeft = move;
    }

// -----------------------------------------------------------------------------
// CPickSelect::MoveRight
// -----------------------------------------------------------------------------
//
void CPickSelect::MoveRight( TBool move )
    {
    iMoveRight = move;
    }

// -----------------------------------------------------------------------------
// CPickSelect::SelectObject
// Uses the cursor position to get the current pixel's alpha value.
// -----------------------------------------------------------------------------
//
void CPickSelect::SelectObject( void )
    {
    /* Read the cursor's position pixel value using glReadPixels. Then get the
       3rd byte value, which is the alpha value of the pixel, and store it to
       iSelectionIdx that is used to check if a polygon or an object is chosen.*/
    GLuint aPixel;
    // At the moment GL_RGBA/GL_UNSIGNED_BYTE is the only supported combo.
    glReadPixels( iCursorPosX, iCursorPosY, 1, 1, GL_RGBA,
                  GL_UNSIGNED_BYTE, &aPixel );

    iSelectionIdx = aPixel >> 24; // get the alpha value of the pixel
    }

// -----------------------------------------------------------------------------
// CPickSelect::SetPolygonSelection
// -----------------------------------------------------------------------------
//
void CPickSelect::SetPolygonSelection( void )
    {
    iSelectionMode = POLYGON_SELECTION;
    iSelectionIdx = 0;
    }

// -----------------------------------------------------------------------------
// CPickSelect::SetObjectSelection
// -----------------------------------------------------------------------------
//
void CPickSelect::SetObjectSelection( void )
    {
    iSelectionMode = OBJECT_SELECTION;
    iSelectionIdx = 0;
    }

// -----------------------------------------------------------------------------
// CPickSelect::SetScreenSize
// Reacts to the dynamic screen size change during execution of this program.
// -----------------------------------------------------------------------------
//
void CPickSelect::SetScreenSize( TUint aWidth, TUint aHeight )
    {
    iScreenWidth  = aWidth;
    iScreenHeight = aHeight;

    // Reinitialize viewport and projection.
    glViewport( 0, 0, iScreenWidth, iScreenHeight );

    // Recalculate aspect ratio
    if ( iScreenHeight != 0 )
        {
        iAspectRatio = (GLfloat)iScreenWidth / (GLfloat)iScreenHeight;
        }
    else
        {
        iAspectRatio = 1.0;
        }

    // Make sure cursor stays on the screen after resolution changes
    if ( iCursorPosX > iScreenHeight )
        {
        iCursorPosX = iScreenHeight;
        }
    if ( iCursorPosY > iScreenWidth )
        {
        iCursorPosY = iScreenWidth;
        }
    }
