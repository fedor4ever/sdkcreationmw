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
#include "Hierarchy.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

/**
 * Vertice coordinates for the cube.
 * This is used to draw every part of the robot hand.
 */
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

/**
 * Indices for drawing the triangles.
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


/** Colors for the first arm part of the robot's hand. */
static const GLubyte colorArm1[8 * 4] =
    {
    /* Dark gray */
    150, 150, 150,255,
    150, 150, 150,255,
    150, 150, 150,255,
    150, 150, 150,255,

    150, 150, 150,255,
    150, 150, 150,255,
    150, 150, 150,255,
    150, 150, 150,255
    };

/** Colors for the second arm part of the robot's hand. */
static const GLubyte colorArm2[8 * 4] =
    {
    /* Light gray */
    200, 200, 200,255,
    200, 200, 200,255,
    200, 200, 200,255,
    200, 200, 200,255,

    200, 200, 200,255,
    200, 200, 200,255,
    200, 200, 200,255,
    200, 200, 200,255
    };

/** Colors for the first finger on the robot's hand. */
static const GLubyte colorFinger1[8 * 4] =
    {
    /* Red */
    255,   0,   0,255,
    255,   0,   0,255,
    255,   0,   0,255,
    255,   0,   0,255,

    255,   0,   0,255,
    255,   0,   0,255,
    255,   0,   0,255,
    255,   0,   0,255
    };

/** Colors for the second finger on the robot's hand. */
static const GLubyte colorFinger2[8 * 4] =
    {
    /* White */
    255, 255, 255,255,
    255, 255, 255,255,
    255, 255, 255,255,
    255, 255, 255,255,

    255, 255, 255,255,
    255, 255, 255,255,
    255, 255, 255,255,
    255, 255, 255,255
    };


/**
 * Used to create a helper, which simply draws the coordinate system.
 * This is very useful if your transformations are not correct.
 * Kind of graphical debugging.
 */
static const GLubyte axisInd[3 * 2] =
    {
    0,1,
    0,2,
    0,3
    };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHierarchy::CHierarchy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHierarchy::CHierarchy( TUint aWidth, TUint aHeight )
:iScreenWidth(aWidth), iScreenHeight(aHeight)
    {
    }


// -----------------------------------------------------------------------------
// CHierarchy::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHierarchy::ConstructL( void )
    {
    }


// -----------------------------------------------------------------------------
// CHierarchy::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHierarchy* CHierarchy::NewL( TUint aWidth, TUint aHeight )
    {
    /* Symbian 2-phase constructor. Calls both the default
       C++ constructor and Symbian ConstructL methods */
    CHierarchy* self = new (ELeave) CHierarchy( aWidth, aHeight );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor.
CHierarchy::~CHierarchy()
    {
    }


// -----------------------------------------------------------------------------
// CHierarchy::AppInit
// Initialize OpenGL ES, set the vertex and color arrays and pointers,
// and select the shading mode.
// -----------------------------------------------------------------------------
//
void CHierarchy::AppInit( void )
    {
    // Initialize viewport and projection.
    SetScreenSize( iScreenWidth, iScreenHeight );

    /* Set the screen background color. */
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    /* Enable depth test and back face culling. */
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE  );

    /* Enable vertex and color arrays. */
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );

    /* Set the initial shading mode */
    glShadeModel( GL_FLAT );

    /* Do not use perspective correction */
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );

    /* Rotate mode switch */
    iRotateMode      = ROTATE_ELBOW;
    iElbowRotAngle   = 0.0;
    iFingersRotAngle = 0.0;
    iRotateUpEnabled        = EFalse;
    iRotateDownEnabled      = EFalse;
    }


// -----------------------------------------------------------------------------
// CHierarchy::AppExit
// Release any allocations made in AppInit.
// -----------------------------------------------------------------------------
//
void CHierarchy::AppExit( void )
    {
    }


// -----------------------------------------------------------------------------
// CHierarchy::DrawCoordSystem
// -----------------------------------------------------------------------------
//
void CHierarchy::DrawCoordSystem( void )
    {
    glDrawElements( GL_LINES, 3 * 2, GL_UNSIGNED_BYTE, axisInd );
    }

// -----------------------------------------------------------------------------
// CHierarchy::DrawBox
// Draw a box. Used to draw every part of the robot hand.
// -----------------------------------------------------------------------------
//
void CHierarchy::DrawBox( void )
    {
    glDrawElements( GL_TRIANGLES, 12 * 3, GL_UNSIGNED_BYTE, triangles );
    }


// -----------------------------------------------------------------------------
// CHierarchy::AppCycle
// Draws and animates the objects
// -----------------------------------------------------------------------------
//
void CHierarchy::AppCycle( TInt /*aFrame*/ )
    {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glLoadIdentity();

    /* Update rotate angles*/
    if ( iRotateUpEnabled )
        {
        if ( iRotateMode == ROTATE_ELBOW )
            {
            iElbowRotAngle += 1.0;
            if ( iElbowRotAngle > 90.0 )
                {
                iElbowRotAngle = 90.0;
                }
            }
        else if ( iRotateMode == ROTATE_FINGERS )
            {
            iFingersRotAngle += 1.0;
            if ( iFingersRotAngle > 80.0 )
                {
                iFingersRotAngle = 80.0;
                }
            }
        }

    if ( iRotateDownEnabled )
        {
        if ( iRotateMode == ROTATE_ELBOW )
            {
            iElbowRotAngle -= 1.0;
            if ( iElbowRotAngle < 0.0 )
                {
                iElbowRotAngle = 0.0;
                }
            }
        else if ( iRotateMode == ROTATE_FINGERS )
            {
            iFingersRotAngle -= 1.0;
            if ( iFingersRotAngle < 0.0 )
                {
                iFingersRotAngle = 0.0;
                }
            }
        }

    // Move Camera
    gluLookAt(   40.f, 10.f, 150.f,  // Camera location
                  50.f,    10.f, 0.f,  // Look at point
                   0.f,    1.f, 0.f ); // Up vector

    // Rotate the arm
    glRotatef( -100.f, 0., 1., 0. );
    glRotatef(   60.f, 0., 0., 1. );

    // Draw Arm
    glVertexPointer( 3, GL_BYTE, 0, vertices );
    glColorPointer( 4, GL_UNSIGNED_BYTE, 0, colorArm1 );
    glTranslatef( 0.0, 0.0, -30.0 );
    glPushMatrix();
    glScalef( 6.0, 6.0, 15.0 );
    glTranslatef( 0.0, 0.0, 1.0 );
    DrawBox();
    glPopMatrix();

    // Draw Hand
    glVertexPointer( 3, GL_BYTE, 0, vertices );
    glColorPointer( 4, GL_UNSIGNED_BYTE, 0, colorArm2 );
    glTranslatef( 6.0, 0.0, 0.0 );   // move coordisystem back to middle
    glRotatef( -iElbowRotAngle, 0.0, 1.0, 0.0 ); // rotate object
    glTranslatef( -6.0, 0.0, 0.0 );  // move coordsystem to top of the box
    glTranslatef( 0.0, 0.0, -30.0 ); // Move origin to the other end of cube
    glPushMatrix();
    glScalef( 6.0, 6.0, 15. );       // Scale the vertices
    glTranslatef( 0.0, 0.0, 1.0 );   // Translate origin to the side of the cube
    DrawBox();
    glPopMatrix();

    // Draw upper finger
    glPushMatrix(); // Store the current matrix because we have to draw
                       // another finger to the same joint, i.e., arm 2. joint

    // Red part
    glVertexPointer( 3, GL_BYTE, 0, vertices );
    glColorPointer( 4, GL_UNSIGNED_BYTE, 0, colorFinger1 );
    glTranslatef( 4.5, 0.0, 0.0 );   // move origin near the top of the arm 2. joint
    glTranslatef( 1.5, 0.0, 0.0 );   // move origin to the edge of the finger
    glRotatef( float(-10.0 - iFingersRotAngle), 0.0, 1.0, 0.0 ); // rotate 1st joint
    glTranslatef( -1.5, 0.0, 0.0 );  // move origin to the middle of finger
    glTranslatef( 0.0, 0.0, -10.0 ); // move origin to the other end if box
    glPushMatrix();
    glScalef( 1.5, 1.5, 5. );
    glTranslatef( 0.0, 0.0, 1.0 );
    DrawBox();
    glPopMatrix();

    // White part
    glVertexPointer( 3, GL_BYTE, 0, vertices );
    glColorPointer( 4, GL_UNSIGNED_BYTE, 0, colorFinger2 );
    glTranslatef( -1.5, 0.0, 0.0 );
    glRotatef( float(80.0 - iFingersRotAngle), 0.0, 1.0, 0.0 );
    glTranslatef( 1.5, 0.0, 0.0 );
    glTranslatef( 0.0, 0.0, -6.0 );
    glPushMatrix();
    glScalef( 1.5, 1.5, 3. );
    glTranslatef( 0.0, 0.0, 1.0 );
    DrawBox();
    glPopMatrix();

    glPopMatrix(); // Pop back to the 'old' matrix in order to draw the second finger

    // Draw lower finger
    glPushMatrix();

    // Red part
    glVertexPointer( 3, GL_BYTE, 0, vertices );
    glColorPointer( 4, GL_UNSIGNED_BYTE, 0, colorFinger1 );
    glTranslatef( -4.5, 0.0, 0.0 );  // move origin near the bottom of the arm 2. joint
    glTranslatef( -1.5, 0.0, 0.0 );  // move origin to the edge of the finger
    glRotatef( float(10.0 + iFingersRotAngle), 0.0, 1.0, 0.0 ); // rotate 1st joint
    glTranslatef( 1.5, 0.0, 0.0 );   // move origin to the middle of finger
    glTranslatef( 0.0, 0.0, -10.0 ); // move origin to the other end if box
    glPushMatrix();
    glScalef( 1.5, 1.5, 5. );
    glTranslatef( 0.0, 0.0, 1.0 );
    DrawBox();
    glPopMatrix();

    // White part
    glVertexPointer( 3, GL_BYTE, 0, vertices );
    glColorPointer( 4, GL_UNSIGNED_BYTE, 0, colorFinger2 );
    glTranslatef( 1.5, 0.0, 0.0 );
    glRotatef( float(-80.0 + iFingersRotAngle), 0.0, 1.0, 0.0 );
    glTranslatef( -1.5, 0.0, 0.0 );
    glTranslatef( 0.0, 0.0, -6.0 );
    glPushMatrix();
    glScalef( 1.5, 1.5, 3. );
    glTranslatef( 0.0, 0.0, 1.0 );
    DrawBox();
    glPopMatrix();

    glPopMatrix();


    }


//----------------------------------------------------------
// The following methods are called by the CHierarchyAppUi
// class when handling the incoming key events.
//----------------------------------------------------------

// -----------------------------------------------------------------------------
// CHierarchy::SwitchRotateMode
// Switch the rotation mode between rotating the arm or fingers
// -----------------------------------------------------------------------------
//
void CHierarchy::SwitchRotateMode( void )
    {
    if ( iRotateMode == ROTATE_ELBOW )
        {
        iRotateMode = ROTATE_FINGERS;
        }
    else
        {
        iRotateMode = ROTATE_ELBOW;
        }
    }

// -----------------------------------------------------------------------------
// CHierarchy::RotateObjectUp
// Rotates object up
// -----------------------------------------------------------------------------
//
void CHierarchy::RotateObjectUp( TBool aStart )
    {
    if( aStart )
        {
        iRotateUpEnabled = ETrue;
        iRotateDownEnabled = EFalse;
        } else
        {
        iRotateUpEnabled = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CHierarchy::RotateObjectDown
// Rotates object down
// -----------------------------------------------------------------------------
//
void CHierarchy::RotateObjectDown( TBool aStart )
    {
    if( aStart )
        {
        iRotateDownEnabled = ETrue;
        iRotateUpEnabled = EFalse;
        } else
        {
        iRotateDownEnabled = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CHierarchy::SetScreenSize
// Reacts to the dynamic screen size change during execution of this program.
// -----------------------------------------------------------------------------
//
void CHierarchy::SetScreenSize( TUint aWidth, TUint aHeight )
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

