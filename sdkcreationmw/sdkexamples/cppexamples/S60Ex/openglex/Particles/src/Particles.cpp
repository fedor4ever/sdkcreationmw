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
#include "particles.h"

// MACROS
#define FRUSTUM_LEFT   -1.f     //left vertical clipping plane
#define FRUSTUM_RIGHT  +1.f     //right vertical clipping plane
#define FRUSTUM_BOTTOM -1.f     //bottom horizontal clipping plane
#define FRUSTUM_TOP    +1.f     //top horizontal clipping plane
#define FRUSTUM_NEAR   +3.f     //near depth clipping plane
#define FRUSTUM_FAR    +1000.f  //far depth clipping plane

// CONSTANTS

// ============================= MEMBER FUNCTIONS ==============================
// ============================= LOCAL FUNCTIONS ===============================
// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CParticles::NewL
// Symbian 2-phase constructor. Can leave.
// -----------------------------------------------------------------------------
//
CParticles* CParticles::NewL( TUint aWidth, TUint aHeight, TUint aParticleCount )
    {
    // Symbian 2-phase constructor. Calls both the default
    // C++ constructor and Symbian ConstructL methods
    CParticles* self = new (ELeave) CParticles( aWidth, aHeight, aParticleCount );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------------------------------
// CParticles::CParticles
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CParticles::CParticles( TUint aWidth, TUint aHeight, TUint aParticleCount )
    : CFiniteStateMachine()
    {
    iScreenWidth  = aWidth;
    iScreenHeight = aHeight;
    iParticleCount = aParticleCount;
    }

// ----------------------------------------------------------------------------
// CParticles::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CParticles::ConstructL( void )
    {
    }

// ----------------------------------------------------------------------------
// CParticles::~CParticles()
// Destructor.
// ----------------------------------------------------------------------------
//
CParticles::~CParticles()
    {
    }

// ----------------------------------------------------------------------------
// CParticles::AppInit
// Application initialization code.
// ----------------------------------------------------------------------------
//
void CParticles::AppInitL( void )
    {
    // Construct a texture manager that uses the application's private
    // directory as the location for all textures.
    iTextureManager = CTextureManager::NewL ( iScreenWidth, iScreenHeight,
	                                            FRUSTUM_TOP, FRUSTUM_BOTTOM, FRUSTUM_RIGHT, FRUSTUM_LEFT, FRUSTUM_NEAR,
	                                            this );
		// Create the feedback effect texture
    glGenTextures(1, &iFeedbackTextureID);

    // Initialize viewport and projection.
    SetScreenSize( iScreenWidth, iScreenHeight );

    // Set the screen background color.
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    // Enable back face culling.
    glEnable( GL_CULL_FACE );

    // Enable point sprites.
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_POINT_SPRITE_OES );

    // We don't need a depth buffer in this example
    glDisable( GL_DEPTH_TEST );
    glDepthMask( 0 );

    // Enable blending for transparency.
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    // Make the points bigger.
    glPointSize( 32 );

    // Disable lighting
    glDisable( GL_LIGHTING );


    // Have the material track the current color
    glEnable( GL_COLOR_MATERIAL );

    // Setup the camera
    iCamera.LookAt( TVector( 0.f, 30.0f, 0.f ),
                    TVector( 0.f, 0.f, -( FRUSTUM_FAR + FRUSTUM_NEAR ) / 2.f ),
                    TVector( 0.f, 1.f, 0.f ) );

    // Create particle array
    iParticleCoords = new GLfixed[iParticleCount * 3];
    iParticleColors = new GLfixed[iParticleCount * 4];

    iTextureManager->RequestToLoad(_L("particle.png"), &iParticleTexture);

    // Start to load the queued textures
    iTextureManager->DoLoadL();
    }

// ----------------------------------------------------------------------------
// CParticles::AppExit
// Application cleanup code.
// ----------------------------------------------------------------------------
//
void CParticles::AppExit( void )
    {
    delete iTextureManager;
    delete[] iParticleCoords;
    delete[] iParticleColors;
    }


// ----------------------------------------------------------------------------
// CParticles::AppCycle
// Draws and animates the objects.
// aFrame = Number of the frame to be rendered.
// aTimeSecs = Seconds elapsed since the application started running.
// aDeltaTimeSecs = Seconds elapsed since last call to AppCycle().
// ----------------------------------------------------------------------------
//
void CParticles::AppCycle( TInt /*aFrame*/,
                       TReal aTimeSecs,
                       TReal /*aDeltaTimeSecs*/ )
    {
    // Move the particles
    UpdateParticles( aTimeSecs );

    // Draw the feedback effect in the background
    DrawFeedbackEffect( aTimeSecs );

    // Draw the particles on top
    DrawParticles( aTimeSecs );

    // Update the feedback texture from the framebuffer
    UpdateFeedbackEffect();
   }

// ----------------------------------------------------------------------------
// CParticles::OnEnterState( TInt aState )
// Called by TFiniteStateMachine when the f.s.m enters a new state
// ----------------------------------------------------------------------------
//
void CParticles::OnEnterStateL( TInt /*aState*/ )
    {
    }

// ----------------------------------------------------------------------------
// CParticles::OnStartLoadingTextures()
// Called for a MTextureLoadingListener by the texture manager when texture
// loading operation starts
// ----------------------------------------------------------------------------
//
void CParticles::OnStartLoadingTexturesL()
    {
    SetStateL( ELoadingTextures );
    }

// ----------------------------------------------------------------------------
// CParticles::OnEndLoadingTextures()
// Called for a MTextureLoadingListener by the texture manager when texture
// loading operation is completed
// ----------------------------------------------------------------------------
void CParticles::OnEndLoadingTexturesL()
    {
    if ( GetState() == ELoadingTextures )
        {
        SetStateL( ERunning );
        }
    }

// ----------------------------------------------------------------------------
// CParticles::SetScreenSize
// Reacts to the dynamic screen size change during execution of this program.
// ----------------------------------------------------------------------------
//
void CParticles::SetScreenSize( TUint aWidth, TUint aHeight )
    {
    iScreenWidth  = aWidth;
    iScreenHeight = aHeight;

    // Notify the texture manager of screen size change
    if( iTextureManager )
        {
        iTextureManager->SetScreenSize( aWidth, aHeight );
        }

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

    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Initialize the feedback texture
    iFeedbackTextureWidth = nextPowerOfTwo(iScreenWidth);
    iFeedbackTextureHeight = nextPowerOfTwo(iScreenHeight);

    // Choose the smaller one to get a reasonably small square texture
    if (iFeedbackTextureWidth < iFeedbackTextureHeight)
        iFeedbackTextureHeight = iFeedbackTextureWidth;
    else
        iFeedbackTextureWidth = iFeedbackTextureHeight;

    glBindTexture(GL_TEXTURE_2D, iFeedbackTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iFeedbackTextureWidth, iFeedbackTextureHeight,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Read the initial black image
    UpdateFeedbackEffect();
    }

void CParticles::UpdateParticles(TReal aTime)
    {
    TInt i;
    TReal theta1 = 3 * aTime;
    TReal theta2 = 5 * aTime;
    TReal theta3 = 1 * aTime;

    for (i = 0; i < iParticleCount; i++)
        {
        // Update position and color
        iParticleColors[i * 4 + 0] = iParticleCoords[i * 3 + 0] = (GLfixed)(sin(theta1) * (1 << 16));
        iParticleColors[i * 4 + 1] = iParticleCoords[i * 3 + 1] = (GLfixed)(cos(theta2) * (1 << 16));
        iParticleColors[i * 4 + 2] = iParticleCoords[i * 3 + 2] = (GLfixed)(sin(theta3) * (1 << 16));
        iParticleColors[i * 4 + 0] = (1 << 16) - iParticleColors[i * 4 + 0];
        iParticleColors[i * 4 + 3] = 1 << 16;
        theta1 += 0.1f;
        theta2 += 0.2f;
        theta3 += 0.3f;
        }
    }

void CParticles::DrawParticles(TReal aTime)
    {
    // Move the viewer away from the particles
    glLoadIdentity();
    glTranslatef( 0, 0, -5 );
    glRotatef( aTime, 0, 1, 0 );

    // Enable vertex and color arrays.
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    glVertexPointer( 3, GL_FIXED, 0, iParticleCoords );
    glColorPointer( 4, GL_FIXED, 0, iParticleColors );

    // Use the particle texture
    glBindTexture( GL_TEXTURE_2D, iParticleTexture.iID );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Draw the array of particles
    glTexEnvi( GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_TRUE );
    glDrawArrays( GL_POINTS, 0, iParticleCount );
    glTexEnvi( GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_FALSE );

    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );
    }

void CParticles::DrawFeedbackEffect(TReal aTime)
    {
    // Use the feedback texture
    glBindTexture( GL_TEXTURE_2D, iFeedbackTextureID );

    // Set orthographic projection
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    glOrthof( 0, 1, 0, 1, -10, 10 );

    // Transform the texture matrix to animate the feedback effect
    glMatrixMode( GL_TEXTURE );
    glLoadIdentity();

    TReal zoom = 0.75f;
    TReal tw = iScreenWidth / (TReal)(iFeedbackTextureWidth);
    TReal th = iScreenHeight / (TReal)(iFeedbackTextureHeight);

    glTranslatef( 0.5 * tw, 0.5 * th, 0 );
    glScalef( zoom, zoom, 1 );
    glRotatef( aTime * 10, 0, 0, 1 );
    glTranslatef( -0.5 * tw, -0.5 * th, 0 );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // Draw a full screen textured quad
    glColor4f( 0.95f, 0.95f, 0.95f, 1 );

    GLfloat texCoords[] =
        {
        0,  0,
        tw, 0,
        0,  th,
        tw, th,
        };
    GLfixed vertices[] =
        {
        0,       0,
        1 << 16, 0,
        0,       1 << 16,
        1 << 16, 1 << 16,
        };

    glDisable( GL_BLEND );
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
    glVertexPointer( 2, GL_FIXED, 0, vertices );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    glEnable( GL_BLEND );

    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_TEXTURE );
    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    }

void CParticles::UpdateFeedbackEffect()
    {
    // Take a snapshot of the framebuffer
    glBindTexture( GL_TEXTURE_2D, iFeedbackTextureID );

    // Two alternative methods
    //glCopyTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 0, 0, iScreenWidth, iScreenHeight );
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, iFeedbackTextureWidth, iFeedbackTextureHeight, 0);
    }

// End of File
