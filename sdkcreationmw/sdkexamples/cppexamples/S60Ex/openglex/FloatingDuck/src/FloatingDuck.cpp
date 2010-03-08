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


// INCLUDE FILES

#include <aknnotewrappers.h>
#include <e32math.h>
#include "floatingduck.h"

// MACROS
#define FRUSTUM_LEFT   -1.f     // left vertical clipping plane
#define FRUSTUM_RIGHT  +1.f     // right vertical clipping plane
#define FRUSTUM_BOTTOM -1.f     // bottom horizontal clipping plane
#define FRUSTUM_TOP    +1.f     // top horizontal clipping plane
#define FRUSTUM_NEAR   +2.f     // near depth clipping plane
#define FRUSTUM_FAR    +1000.f  // far depth clipping plane

#define WATER_VERTICES      4
#define RIPPLE_SEGMENTS     24

// CONSTANTS
/** Vertices for the water object. */
static const GLfloat objVertexDataWater[]=
    {
    -10000.f, 0.f, -10000.f,
     10000.f, 0.f, -10000.f,
     10000.f, 0.f,  10000.f,
    -10000.f, 0.f,  10000.f
    };

/** Indices to the water object vertices. */
static const GLubyte objIndicesWater[]=
    {
    1, 0, 2, 3
    };

/** Materials for the duck object. */
static const GLfloat objDiffuseDuck[4]     = { 0.6, 0.6, 0.10, 1.0 };
static const GLfloat objAmbientDuck[4]     = { 0.5, 0.4, 0.05, 1.0 };
static const GLfloat objSpecularDuck[4]    = { 0.8, 0.8, 0.20, 1.0 };
static const GLfloat objEmissionDuck[4]    = { 0.2, 0.2, 0.00, 1.0 };

/** Materials for the underwater duck object. */
static const GLfloat objDiffuseSunkenDuck[4]     = { 0.4, 0.4, 0.60, 1.0 };
static const GLfloat objAmbientSunkenDuck[4]     = { 0.4, 0.3, 0.65, 1.0 };
static const GLfloat objSpecularSunkenDuck[4]    = { 0.6, 0.6, 0.80, 1.0 };
static const GLfloat objEmissionSunkenDuck[4]    = { 0.0, 0.0, 0.20, 1.0 };

/* Global ambient light. */
static const GLfloat globalAmbient[4]      = { 0.0, 0.0, 0.0, 1.0 };

/* Lamp parameters. */
static const GLfloat lightDiffuseLamp[4]   = { 1.0, 1.0, 0.85, 1.0 };
static const GLfloat lightAmbientLamp[4]   = { 0.4, 0.4, 0.20, 1.0 };
static const GLfloat lightPositionLamp[4]  = { 0.0, 50.0, 50.0, 0.0 };

// ============================= LOCAL FUNCTIONS ===============================


// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CFloatingDuck::NewL
// Symbian 2-phase constructor. Can leave.
// -----------------------------------------------------------------------------
//
CFloatingDuck* CFloatingDuck::NewL( GLuint aWidth, GLuint aHeight )
	{
    // Symbian 2-phase constructor. Calls both the default
    // C++ constructor and Symbian ConstructL methods
    CFloatingDuck* self = new (ELeave) CFloatingDuck( aWidth, aHeight );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
	}

// -----------------------------------------------------------------------------
// CFloatingDuck::~CFloatingDuck()
// Destructor.
// -----------------------------------------------------------------------------
//
CFloatingDuck::~CFloatingDuck()
	{
	}

// -----------------------------------------------------------------------------
// CFloatingDuck::AppInit
// Application initialization code.
// -----------------------------------------------------------------------------
//
void CFloatingDuck::AppInitL( void )
	{
    // Calculate the view frustrum based on the aspect ratio
    SetScreenSize( iScreenWidth, iScreenHeight );

    // Set the screen background color.
    glClearColor( 0.f, 0.f, 1.f, 1.f );

    // Enable required Open GL ES features
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE  );
    glEnable( GL_LIGHTING );

    // Make OpenGL ES automatically normalize all normals after tranformations.
    // This is important when making irregular transforms like scaling, or if we
    // have specified non-unit length normals.
    glEnable( GL_NORMALIZE  );

    // Enable vertex, normal and texture arrays
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );

    // Do not use perspective correction
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );

    // Set shading mode to smooth
    glShadeModel( GL_SMOOTH );

    // Set up global ambient light
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, globalAmbient );

    // Set up light source
    glEnable( GL_LIGHT0 );
    glLightfv(  GL_LIGHT0, GL_DIFFUSE,  lightDiffuseLamp  );
    glLightfv(  GL_LIGHT0, GL_AMBIENT,  lightAmbientLamp  );
    glLightfv(  GL_LIGHT0, GL_SPECULAR, lightDiffuseLamp  );
    glLightfv(  GL_LIGHT0, GL_POSITION, lightPositionLamp );

    SetStateL( ERunning );

    iRippleVertices = new GLfloat[(RIPPLE_SEGMENTS * 2) * 3];
    iRippleIndices = new GLushort[RIPPLE_SEGMENTS * 2 + 2];

    // Calculate the ripple triangle strip indices now, since they won't change.
    // The ripples are drawn as a triangle strip, which goes around a full circle.
    for (GLint i = 0; i < RIPPLE_SEGMENTS * 2 + 2; i++)
        {
        iRippleIndices[i] = i % (RIPPLE_SEGMENTS * 2);
        }

    // Create the needed vertex buffers object names
    glGenBuffers( 1, &iVertexDataBufferId );
    glGenBuffers( 1, &iNormalDataBufferId );
    glGenBuffers( 1, &iIndexDataBufferId );

    // Create the vertex buffer object for duck vertices
    glBindBuffer( GL_ARRAY_BUFFER, iVertexDataBufferId );
    glBufferData( GL_ARRAY_BUFFER,
    			  ( sizeof(GLfloat) * NUM_DUCK_VERTICES * 3 ),  // Size of the data array
    			  objVertexdataDuck,      // Vertices
    			  GL_STATIC_DRAW );	      // Hint that the object data is static (non-changing)

    // Create the vertex buffer object for duck normals
    glBindBuffer( GL_ARRAY_BUFFER, iNormalDataBufferId );
    glBufferData( GL_ARRAY_BUFFER,
    			  ( sizeof(GLbyte) * NUM_DUCK_VERTICES * 3 ),// Size of the data array
    			  objNormaldataDuck,    // Normals
    			  GL_STATIC_DRAW );		// Hint that the object data is static (non-changing)

    // Create the vertex buffer object for duck indices
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, iIndexDataBufferId );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER,
    			  ( sizeof(GLubyte) * NUM_DUCK_FACES * 3 ), // Size of the index array
    			  objFacedataDuck,		// No data pointer given (creates empty buffer)
    			  GL_STATIC_DRAW );		// Hint that the object data is static (non-changing)
	}

// -----------------------------------------------------------------------------
// CFloatingDuck::AppExit
// Application cleanup code.
// -----------------------------------------------------------------------------
//
void CFloatingDuck::AppExit( void )
	{
	glDeleteBuffers( 1, &iVertexDataBufferId );
	glDeleteBuffers( 1, &iNormalDataBufferId );
	glDeleteBuffers( 1, &iIndexDataBufferId  );
    delete[] iRippleVertices;
    delete[] iRippleIndices;
    }

// -----------------------------------------------------------------------------
// CFloatingDuck::AppCycle
// Draws and animates the objects.
// aFrame = Number of the frame to be rendered.
// aTimeSecs = Seconds elapsed since the application started running.
// aDeltaTimeSecs = Seconds elapsed since last call to AppCycle().
// -----------------------------------------------------------------------------
//
void CFloatingDuck::AppCycle( GLuint aFrame, GLfloat aTimeSecs, GLfloat /*aDeltaTimeSecs*/ )
	{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Calculate the slow up-down movement of the camera
    GLfloat slowTime = aTimeSecs / 5.f;
    GLfloat camY     = sin( slowTime ) * 50.f;

    // Setup the camera using the handy GLU Look At method implemented in the utils.cpp
	glLoadIdentity();
    gluLookAt(  0.f,    100.f + camY,     150.f, // Position
			    0.f,    0.f,       0.f,   // Look At
                0.f,    1.f,       0.f ); // Up Vector

    // Set submerged duck material
    glMaterialfv(   GL_FRONT_AND_BACK, GL_AMBIENT,   objAmbientSunkenDuck  );
    glMaterialfv(   GL_FRONT_AND_BACK, GL_DIFFUSE,   objDiffuseSunkenDuck  );
    glMaterialfv(   GL_FRONT_AND_BACK, GL_SPECULAR,  objSpecularSunkenDuck );
    glMaterialfv(   GL_FRONT_AND_BACK, GL_EMISSION,  objEmissionSunkenDuck );
    glMaterialx(    GL_FRONT_AND_BACK, GL_SHININESS,     5 << 16     );

    // Draw duck reflection by using y-scale of -1.0
    DrawDuck(  aFrame, aTimeSecs, -1.0f );

    // Draw semitransparent water surface
    DrawWater( aFrame, aTimeSecs );

    // Draw water ripples
    DrawRipples( aFrame, aTimeSecs );

    // Set duck material
    glMaterialfv(   GL_FRONT_AND_BACK, GL_AMBIENT,   objAmbientDuck  );
    glMaterialfv(   GL_FRONT_AND_BACK, GL_DIFFUSE,   objDiffuseDuck  );
    glMaterialfv(   GL_FRONT_AND_BACK, GL_SPECULAR,  objSpecularDuck );
    glMaterialfv(   GL_FRONT_AND_BACK, GL_EMISSION,  objEmissionDuck );
    glMaterialx(    GL_FRONT_AND_BACK, GL_SHININESS,     5 << 16     );

    // Draw above the water duck by using y-scale of 1.0
    DrawDuck(  aFrame, aTimeSecs, 1.0f );
	}


// -----------------------------------------------------------------------------
// CFloatingDuck::DrawDuck()
// Draws the duck. If yScale is 1.0 the duck is drawn, if the yScale is -1.0
// the reflection of the duck is drawn.
// -----------------------------------------------------------------------------
//
void CFloatingDuck::DrawDuck( GLuint /*aFrame*/, GLfloat aTimeSecs, GLfloat yScale )
    {
    // Store the model view matrix
    glPushMatrix();

    // Reflection and the light position must be mirrored
    glScalef( 1.f, yScale, 1.f );
    glLightfv(  GL_LIGHT0, GL_POSITION, lightPositionLamp );

	// Define the water clip plane that prevents the duck reflection
	// from being drawn above the water
    GLfloat coeff[4] =  { 0.f, 1.f, 0.f, 0.f };
    glClipPlanef( GL_CLIP_PLANE0, coeff );
    glEnable( GL_CLIP_PLANE0 );


    // Make the duck bob up and down
    glTranslatef( 0.f, 7.f + cos( aTimeSecs * 2.f ) * 7.f, 0.f );

    // Rotate the duck around the y-axis
    glRotatef( aTimeSecs * 3.f, 0.f, 1.f, 0.f );

    // Make the duck swing back and forth
    glRotatef( sin( aTimeSecs * 3.f) * -5.f, 0.f, 0.f, 1.f );
    glRotatef( cos( aTimeSecs ) * -5.f, 1.f, 0.f, 0.f );

    // Duck model lies on it's side, it has to be rotated to face up
    glRotatef( -90.f, 1.f, 0.f, 0.f );

	// Bind the VBOs
    glBindBuffer( GL_ARRAY_BUFFER, iVertexDataBufferId );
    glVertexPointer( 3, GL_FLOAT, 0, NULL );
    glBindBuffer( GL_ARRAY_BUFFER, iNormalDataBufferId );
    glNormalPointer( GL_BYTE,  0, NULL );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, iIndexDataBufferId );

    // Draw the duck using currently bound element array buffer
    glDrawElements( GL_TRIANGLES, NUM_DUCK_FACES * 3, GL_UNSIGNED_BYTE, NULL );

	// Unbind all VBOs
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// Disable the clipping plane
    glDisable( GL_CLIP_PLANE0 );

	// Restore model view matrix
    glPopMatrix();
    }

// -----------------------------------------------------------------------------
// CFloatingDuck::DrawWater()
// Draws the water surface
// -----------------------------------------------------------------------------
//
void CFloatingDuck::DrawWater( GLuint /*aFrame*/, GLfloat /*aTimeSecs*/ )
    {
    // Disable lighting and normals
    glDisable( GL_LIGHTING );
    glDisableClientState( GL_NORMAL_ARRAY );

    // Set array pointers for water model.
    glVertexPointer( 3, GL_FLOAT, 0, objVertexDataWater );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glNormal3f( 0.f, 1.f, 0.f );
    glColor4f(0.204, 0.796, 0.988, 0.600 );
    glDrawElements( GL_TRIANGLE_STRIP, WATER_VERTICES, GL_UNSIGNED_BYTE, objIndicesWater );
    glDisable( GL_BLEND );

    // Enable lighting and normal arrays
    glEnable( GL_LIGHTING );
    glEnableClientState( GL_NORMAL_ARRAY );
    }

// -----------------------------------------------------------------------------
// CFloatingDuck::DrawWater()
// Draws the water surface ripples
// -----------------------------------------------------------------------------
//
void CFloatingDuck::DrawRipples( GLuint /*aFrame*/, GLfloat aTimeSecs )
    {
    // Update the ripple vertex positions
    GLint i, n;
    GLfloat angle;
    const GLfloat maxRadius = 100.0;

    // Disable lighting and normals
    glDisable( GL_LIGHTING );
    glDisableClientState( GL_NORMAL_ARRAY );

    // Set array pointers for the ripple model.
    glVertexPointer( 3, GL_FLOAT, 0, iRippleVertices );

    // Speed up the animation
    aTimeSecs *= 3;

    glDisable( GL_DEPTH_TEST );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glNormal3f( 0.f, 1.f, 0.f );

    for (n = 0; n < 3; n++)
        {
        GLfloat radius = (6 * aTimeSecs + 30 * n) - maxRadius * (GLint)((6 * aTimeSecs + 30 * n) / maxRadius);

        for (i = 0, angle = 0; i < RIPPLE_SEGMENTS; i++, angle += 2 * PI / RIPPLE_SEGMENTS)
            {
            // inner ring
            iRippleVertices[(i * 2 + 0) * 3 + 0] = sin(angle) * (radius + 4 * cos(angle * 8 + aTimeSecs));
            iRippleVertices[(i * 2 + 0) * 3 + 1] = 0;
            iRippleVertices[(i * 2 + 0) * 3 + 2] = cos(angle) * (radius + 4 * cos(angle * 6 + aTimeSecs * 4));
            // outer ring
            iRippleVertices[(i * 2 + 1) * 3 + 0] = sin(angle) * (radius + 15 + 4 * sin(angle * 5 - aTimeSecs * 3));
            iRippleVertices[(i * 2 + 1) * 3 + 1] = 0;
            iRippleVertices[(i * 2 + 1) * 3 + 2] = cos(angle) * (radius + 15 + 4 * sin(angle * 7 - aTimeSecs * 2));
            }

        glColor4f(1, 1, 1, .6 * (1.0 - radius / maxRadius));
        glDrawElements( GL_TRIANGLE_STRIP, RIPPLE_SEGMENTS * 2 + 2, GL_UNSIGNED_SHORT, iRippleIndices );
        }

    glDisable( GL_BLEND );
    glEnable( GL_DEPTH_TEST );

    // Enable lighting and normal arrays
    glEnable( GL_LIGHTING );
    glEnableClientState( GL_NORMAL_ARRAY );
    }


// ----------------------------------------------------------------------
// CFloatingDuck::CFloatingDuck
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------
//
CFloatingDuck::CFloatingDuck( GLuint aWidth, GLuint aHeight )
	: CFiniteStateMachine()
	{
    iScreenWidth  = aWidth;
    iScreenHeight = aHeight;
	}

// ----------------------------------------------------------------------
// CFloatingDuck::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------
//
void CFloatingDuck::ConstructL( void )
	{
	}

// ---------------------------------------------------------
// CFloatingDuck::OnEnterState( TInt aState )
// Called by TFiniteStateMachine when the f.s.m enters a new state
// ---------------------------------------------------------
//
void CFloatingDuck::OnEnterStateL( TInt /*aState*/ )
	{
	}

// -------------------------------------------------------------------------------------------------------
// CFloatingDuck::OnStartLoadingTextures()
// Called for a MTextureLoadingListener by the texture manager when texture loading operation starts
// -------------------------------------------------------------------------------------------------------
//
void CFloatingDuck::OnStartLoadingTexturesL()
	{
    SetStateL( ELoadingTextures );
	}

// ------------------------------------------------------------------------------------------------------------
// CFloatingDuck::OnEndLoadingTextures()
// Called for a MTextureLoadingListener by the texture manager when texture loading operation is completed
// ------------------------------------------------------------------------------------------------------------
void CFloatingDuck::OnEndLoadingTexturesL()
	{
	if ( GetState() == ELoadingTextures )
		{
        SetStateL( ERunning );
		}
	}

// -----------------------------------------------------------------------------
// CFloatingDuck::SetScreenSize
// Reacts to the dynamic screen size change during execution of this program.
// -----------------------------------------------------------------------------
//
void CFloatingDuck::SetScreenSize( GLuint aWidth, GLuint aHeight )
    {
    iScreenWidth  = aWidth;
    iScreenHeight = aHeight;

    // Initialize viewport and projection.
    glViewport( 0, 0, iScreenWidth, iScreenHeight );

    // Calculate the view frustrum
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    GLfloat aspectRatio = (GLfloat)(iScreenWidth) / (GLfloat)(iScreenHeight);
    glFrustumf( FRUSTUM_LEFT * aspectRatio, FRUSTUM_RIGHT * aspectRatio,
                FRUSTUM_BOTTOM, FRUSTUM_TOP,
                FRUSTUM_NEAR, FRUSTUM_FAR );

	// Restore matrix mode
    glMatrixMode( GL_MODELVIEW );
    }

// End of File
