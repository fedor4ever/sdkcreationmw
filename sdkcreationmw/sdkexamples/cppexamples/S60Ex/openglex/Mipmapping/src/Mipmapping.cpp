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
#include "Mipmapping.h"

// MACROS
#define FRUSTUM_LEFT   -1.f     //left vertical clipping plane
#define FRUSTUM_RIGHT  +1.f     //right vertical clipping plane
#define FRUSTUM_BOTTOM -1.f     //bottom horizontal clipping plane
#define FRUSTUM_TOP    +1.f     //top horizontal clipping plane
#define FRUSTUM_NEAR   +1.f     //near depth clipping plane
#define FRUSTUM_FAR    +1000.f  //far depth clipping plane

// CONSTANTS

// ============================= MEMBER FUNCTIONS ==============================

// -----------------------------------------------------------------------------
// CMipmapping::NewL
// Symbian 2-phase constructor. Can leave.
// -----------------------------------------------------------------------------
//
CMipmapping* CMipmapping::NewL( TUint aWidth, TUint aHeight, TUint aSegments, TUint aSlices )
	{
    // Symbian 2-phase constructor. Calls both the default
    // C++ constructor and Symbian ConstructL methods
    CMipmapping* self = new (ELeave) CMipmapping( aWidth, aHeight, aSegments, aSlices );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
	}

// ----------------------------------------------------------------------------
// CMipmapping::CMipmapping
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CMipmapping::CMipmapping( TUint aWidth, TUint aHeight, TUint aSegments, TUint aSlices )
	: CFiniteStateMachine()
	{
    iScreenWidth  = aWidth;
    iScreenHeight = aHeight;
    iSegments = aSegments;
    iSlices = aSlices;
    iFilteringMode = EMipmappedBilinearFiltering;
	}

// ----------------------------------------------------------------------------
// CMipmapping::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CMipmapping::ConstructL( void )
	{
    // Create vertex, normal and index arrays
    iVertices = new GLfixed[ 3 * iSegments * iSlices ];
    iNormals = new GLfixed[ 3 * iSegments * iSlices ];
    iTexCoords = new GLfixed[ 2 * iSegments * iSlices ];
    iIndices = new GLshort[ 2 + (iSegments - 1) * iSlices * 2 ];

    // Fill index arrays for rendering the tunnel as a triangle strip
    GLshort * i = iIndices;
    TInt segment, edge;

    *i++ = 0;
    for (segment = 0; segment < iSegments - 1; segment++)
        {
        for (edge = 0; edge < iSlices; edge++)
            {
            *i++ = segment * iSlices + (edge + 1) % iSlices;
            *i++ = segment * iSlices + (edge + 1) % iSlices + iSlices;
            }
        }
    *i++ = (iSegments - 1) * iSlices + 1;

    // Calculate the texture coordinates
    GLfixed * t = iTexCoords;

    for (segment = 0; segment < iSegments; segment++)
        {
        for (edge = 0; edge < iSlices; edge++)
            {
            *t++ = segment << 15;
            *t++ = (4 * (edge << 16) / iSlices) & ((1 << 16) - 1);
            }
        }

	}

// ----------------------------------------------------------------------------
// CMipmapping::~CMipmapping()
// Destructor.
// ----------------------------------------------------------------------------
//
CMipmapping::~CMipmapping()
	{
    delete[] iVertices;  iVertices = 0;
    delete[] iNormals;   iNormals = 0;
    delete[] iTexCoords; iTexCoords = 0;
    delete[] iIndices;   iIndices = 0;
	}

// ----------------------------------------------------------------------------
// CMipmapping::AppInit
// Application initialization code.
// ----------------------------------------------------------------------------
//
void CMipmapping::AppInitL( void )
	{
    // Construct a texture manager that uses the application's private
    // directory as the location for all textures.
    iTextureManager = CTextureManager::NewL ( iScreenWidth, iScreenHeight,
	                                          FRUSTUM_TOP, FRUSTUM_BOTTOM, FRUSTUM_RIGHT, FRUSTUM_LEFT, FRUSTUM_NEAR,
	                                          this );

    // Initialize viewport and projection.
    SetScreenSize( iScreenWidth, iScreenHeight );

    // Set the screen background color.
    glClearColor( 0.0f, 0.3f, 0.0f, 1.0f );

    // Set required OpenGL state
    glEnable( GL_CULL_FACE  );
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_DEPTH_TEST );

    // Disable lighting
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );

    static const GLfloat lightDiffuse[4]   = { 1.0, 1.0, 1.0, 1.0 };
    static const GLfloat lightAmbient[4]   = { 0.4, 0.4, 0.4, 1.0 };
    static const GLfloat lightPosition[4]  = { 0, 10, 10, 0 };
    glLightfv(  GL_LIGHT0, GL_DIFFUSE,  lightDiffuse  );
    glLightfv(  GL_LIGHT0, GL_AMBIENT,  lightAmbient  );
    glLightfv(  GL_LIGHT0, GL_SPECULAR, lightDiffuse  );
    glLightfv(  GL_LIGHT0, GL_POSITION, lightPosition );

    // Request perspective correct rendering
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

    // Enable vertex, normal and texture arrays
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glVertexPointer( 3, GL_FIXED, 0, iVertices );
    glNormalPointer( GL_FIXED, 0, iNormals );
    glTexCoordPointer( 2, GL_FIXED, 0, iTexCoords );

#if defined(MIPMAPPING_SPLIT_SCREEN)
    glEnable(GL_SCISSOR_TEST);
#endif

    // Push the textures into the loading queue
    iTextureManager->RequestToLoad(_L("texture.jpg"), &iTexture, NULL, NULL, ETrue );

    // Start to load the queued textures
    iTextureManager->DoLoadL();
	}

// ----------------------------------------------------------------------------
// CMipmapping::AppExit
// Application cleanup code.
// ----------------------------------------------------------------------------
//
void CMipmapping::AppExit( void )
	{
    delete iTextureManager;
	}


// ----------------------------------------------------------------------------
// CMipmapping::AppCycle
// Draws and animates the objects.
// aFrame = Number of the frame to be rendered.
// aTimeSecs = Seconds elapsed since the application started running.
// aDeltaTimeSecs = Seconds elapsed since last call to AppCycle().
// ----------------------------------------------------------------------------
//
void CMipmapping::AppCycle( TInt /*aFrame*/,
                       TReal aTimeSecs,
                       TReal /*aDeltaTimeSecs*/ )
	{
#if defined(MIPMAPPING_SPLIT_SCREEN)
    // Clear the entire screen
    glScissor(0, 0, iScreenWidth, iScreenHeight);
#endif

	// Clear the color and depth buffers
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Pan the texture toward the viewer
    glMatrixMode( GL_TEXTURE );
    glLoadIdentity();
    glTranslatex( (TInt)((1 << 14) * aTimeSecs) & ((1 << 16) - 1), 0, 0);
    glMatrixMode( GL_MODELVIEW );

    // Roll the camera around the z-axis
    glLoadIdentity();
    glRotatef( 16 * aTimeSecs, 0, 0, 1 );

    // Move the tunnel vertices
    UpdateTunnel ( aTimeSecs );

    // Set up texturing
    glBindTexture( GL_TEXTURE_2D, iTexture.iID );

    switch (iFilteringMode)
        {
        case EMipmappedNearestNeighbourFiltering:
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST );
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
            break;
        case EMipmappedBilinearFiltering:
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            break;
        case EMipmappedTrilinearFiltering:
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            break;
        case ENearestNeighbourFiltering:
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
            break;
        case EBilinearFiltering:
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            break;
        }

#if defined(MIPMAPPING_SPLIT_SCREEN)
    // Draw the left side of the screen with mipmapping.
    glScissor(0, 0, iScreenWidth / 2 - 1, iScreenHeight);
#endif

    glDrawElements( GL_TRIANGLE_STRIP, 2 + (iSegments - 1) * iSlices * 2, GL_UNSIGNED_SHORT, iIndices);

#if defined(MIPMAPPING_SPLIT_SCREEN)
    // Draw the right side of the screen without mipmapping.
    glScissor(iScreenWidth / 2, 0, iScreenWidth / 2, iScreenHeight);

    switch (iFilteringMode)
        {
        case EMipmappedNearestNeighbourFiltering:
        case ENearestNeighbourFiltering:
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
            break;
        case EMipmappedTrilinearFiltering:
        case EMipmappedBilinearFiltering:
        case EBilinearFiltering:
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            break;
        }

    glDrawElements( GL_TRIANGLE_STRIP, 2 + (iSegments - 1) * iSlices * 2, GL_UNSIGNED_SHORT, iIndices);
#endif

	}

// ----------------------------------------------------------------------------
// CMipmapping::OnEnterState( TInt aState )
// Called by TFiniteStateMachine when the f.s.m enters a new state
// ----------------------------------------------------------------------------
//
void CMipmapping::OnEnterStateL( TInt /*aState*/ )
	{
	}

// ----------------------------------------------------------------------------
// CMipmapping::OnStartLoadingTextures()
// Called for a MTextureLoadingListener by the texture manager when texture
// loading operation starts
// ----------------------------------------------------------------------------
//
void CMipmapping::OnStartLoadingTexturesL()
	{
    SetStateL( ELoadingTextures );
	}

// ----------------------------------------------------------------------------
// CMipmapping::OnEndLoadingTextures()
// Called for a MTextureLoadingListener by the texture manager when texture
// loading operation is completed
// ----------------------------------------------------------------------------
void CMipmapping::OnEndLoadingTexturesL()
	{
	if ( GetState() == ELoadingTextures )
		{
        SetStateL( ERunning );
		}
	}

// ----------------------------------------------------------------------------
// CMipmapping::SetScreenSize
// Reacts to the dynamic screen size change during execution of this program.
// ----------------------------------------------------------------------------
//
void CMipmapping::SetScreenSize( TUint aWidth, TUint aHeight )
    {
    iScreenWidth  = aWidth;
    iScreenHeight = aHeight;

    // Notify the texture manager of screen size change
    iTextureManager->SetScreenSize( aWidth, aHeight );

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


void CMipmapping::UpdateTunnel( TReal aTime )
    {
    GLfixed * vertex = iVertices;
    GLfixed * normal = iNormals;
    TInt segment, edge;
    TReal time = 0, phase = aTime;
    TReal timeStep = 0.1;                   // Time units per segment
    const TReal timeAcceleration = 0.03;    // Acceleration of the time step
    const TReal phaseStep = 0.01;           // Phase units per segment
    TReal pos[3], radius;
    GLfixed origin[3] = {0, 0, 0};

    for (segment = 0; segment < iSegments; segment++)
        {
        // Evaluate the tunnel at this position
        TunnelFunction(time, phase, pos, radius);
        if (segment == 0)
            {
            origin[0] = RealToFixed(pos[0]);
            origin[1] = RealToFixed(pos[1]);
            origin[2] = RealToFixed(pos[2]);
            }

        // Build a ring of vertices at this position. Since the tunnel's
        // deformations are quite negligible, the ring is aligned to the
        // XY-plane.
        TReal angle;
        for (edge = 0, angle = 0; edge < iSlices; edge++, angle += 2 * KPi / iSlices)
            {
            TReal c, s;
            Math::Sin(s, angle);
            Math::Cos(c, angle);
            *vertex++ = RealToFixed(pos[0] + c * radius) - origin[0];
            *vertex++ = RealToFixed(pos[1] + s * radius) - origin[1];
            *vertex++ = RealToFixed(pos[2]) - origin[2];
            *normal++ = RealToFixed(c);
            *normal++ = RealToFixed(s);
            *normal++ = 0;
            }
        // Update our position along the tunnel
        time += timeStep;
        timeStep += timeAcceleration;
        phase += phaseStep;
        }
    }

void CMipmapping::TunnelFunction( TReal aTime, TReal aPhase, TReal aPos[3], TReal& aRadius ) const
    {
    // Calculate a wavy parametric curve
    Math::Sin(aPos[0], aTime + aPhase);
    Math::Cos(aPos[1], aTime + aPhase);
    aPos[1] *= 2;
    aPos[2] = -aTime * 16;
    aRadius = 4;
    }

GLfixed CMipmapping::RealToFixed( TReal aReal )
    {
    return (GLfixed)(aReal * (1 << 16));
    }

void CMipmapping::SetFilteringMode( CMipmapping::EFilteringMode aMode )
    {
    iFilteringMode = aMode;
    }

// End of File
