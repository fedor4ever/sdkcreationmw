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

#include "bumpmap.h"

// MACROS
#define FRUSTUM_LEFT   -1.f     //left vertical clipping plane
#define FRUSTUM_RIGHT  +1.f     //right vertical clipping plane
#define FRUSTUM_BOTTOM -1.f     //bottom horizontal clipping plane
#define FRUSTUM_TOP    +1.f     //top horizontal clipping plane
#define FRUSTUM_NEAR   +2.f     //near depth clipping plane
#define FRUSTUM_FAR    +1000.f  //far depth clipping plane

// Size of the bumpmapped rectangle
#define BUMPMAP_SIZE            75.f
// Bumpmapped rectangle's z coordinate
#define BUMPMAP_Z_COORD         0.f
// Number of vertices per side in the drawn bumpmapped rectangle has.
#define NUM_VERTICES_PER_SIDE   7

// CONSTANTS
/**
 * Fixed tangent matrix that works for this demo.
 * For info on how to calculate the tangent matrix in other cases see:
 * http://www.blacksmith-studios.dk/projects/downloads/tangent_matrix_derivation.php
 * and
 * http://www.codesampler.com/oglsrc/oglsrc_4.htm#ogl_dot3_bump_mapping
 */
static const GLfloat tangentMatrix[] =
	{
	1.f, 0.f, 0.f, 0.f,
	0.f,-1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    0.f, 0.f, 0.f, 1.f
    };



// ============================= MEMBER FUNCTIONS ==============================

// -----------------------------------------------------------------------------
// CBumpMap::NewL
// Symbian 2-phase constructor. Can leave.
// -----------------------------------------------------------------------------
//
CBumpMap* CBumpMap::NewL( GLuint aWidth, GLuint aHeight )
	{
    // Symbian 2-phase constructor. Calls both the default
    // C++ constructor and Symbian ConstructL methods
    CBumpMap* self = new (ELeave) CBumpMap( aWidth, aHeight );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
	}

// ----------------------------------------------------------------------------
// CBumpMap::CBumpMap
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CBumpMap::CBumpMap( GLuint aWidth, GLuint aHeight )
	: CFiniteStateMachine()
	{
    iScreenWidth    = aWidth;
    iScreenHeight   = aHeight;
    iTextureManager = NULL;
	}

// ----------------------------------------------------------------------------
// CBumpMap::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CBumpMap::ConstructL( void )
	{
	}

// ----------------------------------------------------------------------------
// CBumpMap::~CBumpMap()
// Destructor.
// ----------------------------------------------------------------------------
//
CBumpMap::~CBumpMap()
	{
	}

// ----------------------------------------------------------------------------
// CBumpMap::AppInit
// Application initialization code.
// ----------------------------------------------------------------------------
//
void CBumpMap::AppInitL( void )
	{
    // Construct a texture manager that uses the application's private
	// directory as the source location for all textures.
    iTextureManager = CTextureManager::NewL ( iScreenWidth,  iScreenWidth,
                                              FRUSTUM_TOP,   FRUSTUM_BOTTOM,
                                              FRUSTUM_RIGHT, FRUSTUM_LEFT,
                                              FRUSTUM_NEAR,  this );

    // Initialize viewport and projection.
    SetScreenSize( iScreenWidth, iScreenHeight );

    // Reset texture matrix
    glMatrixMode( GL_TEXTURE );
    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );

    // Set the screen background color.
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    // Enable needed features
    glEnable( GL_CULL_FACE  );
    glEnable( GL_TEXTURE_2D );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_LIGHTING );

    // Enable vertex arrays and texture arrays.
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    // Do not use perspective correction
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GenerateBumpmapVerticesL();

    iLightSource = CFireFly::NewL( -45.f, 45.f,         // Min and max X-coordinates
                                   -45.f, 45.f,         // Min and max Y-coordinates
                                     7.f, 25.f,         // Min and max Z-coordinates
                                    iTextureManager );  // Texture manager used to load any textures

    // Push the textures into the loading queue.
	_LIT( KBumpMapName,    "bumpmap.jpg"   );
    iTextureManager->RequestToLoad( KBumpMapName,     &iBumpmapTexture, false );
	_LIT( KTextureMapName, "texmap.jpg" );
    iTextureManager->RequestToLoad( KTextureMapName,  &iTexture, false );

    // Start to load the textures.
    iTextureManager->DoLoadL();
	}

// ----------------------------------------------------------------------------
// CBumpMap::AppExit
// Application cleanup code.
// ----------------------------------------------------------------------------
//
void CBumpMap::AppExit( void )
	{
    delete[] iBumpmapVertices;
    delete[] iBumpmapColors;
    delete[] iBumpmapTexCoords;
    delete[] iBumpmapIndices;

    delete iLightSource;

    delete iTextureManager;
	}


// ----------------------------------------------------------------------------
// CBumpMap::AppCycle
// Draws and animates the objects.
// aFrame = Number of the frame to be rendered.
// aTimeSecs = Seconds elapsed since the application started running.
// aDeltaTimeSecs = Seconds elapsed since last call to AppCycle().
// ----------------------------------------------------------------------------
//
void CBumpMap::AppCycle( GLuint aFrame,
                         GLfloat aTimeSecs,
                         GLfloat aDeltaTimeSecs )
	{
	// Clear the color and depth buffers
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glLoadIdentity();
    gluLookAt(  0.f,  0.f,     150.f,   // Position
                0.f,  0.f,       0.f,   // Look At
                0.f,  1.f,       0.f ); // Up Vector

    // Position the lightsource
    iLightSource->DoMovement( aFrame, aTimeSecs, aDeltaTimeSecs );

    // Draw the bumpmapping
    DrawBumpmappedRect();

    // Draw the lightsource as firefly
    iLightSource->Draw();
	}

// ----------------------------------------------------------------------------
// CBumpMap::OnEnterState( TInt aState )
// Called by TFiniteStateMachine when the f.s.m enters a new state
// ----------------------------------------------------------------------------
//
void CBumpMap::OnEnterStateL( TInt /*aState*/ )
	{
	}

// ----------------------------------------------------------------------------
// CBumpMap::OnStartLoadingTextures()
// Called for a MTextureLoadingListener by the texture manager when texture
// loading operation starts
// ----------------------------------------------------------------------------
//
void CBumpMap::OnStartLoadingTexturesL()
	{
    SetStateL( ELoadingTextures );
	}

// ----------------------------------------------------------------------------
// CBumpMap::OnEndLoadingTextures()
// Called for a MTextureLoadingListener by the texture manager when texture
// loading operation is completed
// ----------------------------------------------------------------------------
//
void CBumpMap::OnEndLoadingTexturesL()
	{
	if ( GetState() == ELoadingTextures )
		{
        SetStateL( ERunning );
		}
	}

// ----------------------------------------------------------------------------
// CBumpMap::SetScreenSize
// Reacts to the dynamic screen size change during execution of this program.
// ----------------------------------------------------------------------------
//
void CBumpMap::SetScreenSize( GLuint aWidth, GLuint aHeight )
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
    glFrustumf( FRUSTUM_LEFT * aspectRatio,
                FRUSTUM_RIGHT * aspectRatio,
                FRUSTUM_BOTTOM,
                FRUSTUM_TOP,
                FRUSTUM_NEAR,
                FRUSTUM_FAR );
    glMatrixMode( GL_MODELVIEW );
    }

// ----------------------------------------------------------------------------
// CBumpMap::DrawBumpmappedRect
// Draws the bumpmapped rectangle.
// ----------------------------------------------------------------------------
//
void CBumpMap::DrawBumpmappedRect()
    {
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    glDisable( GL_BLEND );

    // dot3 bump mapping computes (N.L) per pixel.  we do this by storing the N
    // vector in a texture (known as the dot3 bump map) and then L is computed
    // per vertex and is stored at each vertex as a color.  This color is
    // interpolated between vertices by OpenGL when the shade model is set to GL_SMOOTH.
    // N.L is computed by the texture combiner and then modulated with the base texture.
    //
    // Note that L vector must be rotated into tangent space (aka texture space) so
    // that it is in the same space as N.

    // UNIT 0
    // Configure the first texture combiner to calculate the dot product of
    // bumpmap normal vector N (stored in the bumpmapping texture) and
    // light angle vector L (stored as the PRIMARY_COLOR).
    glClientActiveTexture(GL_TEXTURE0);
    glActiveTexture( GL_TEXTURE0 );
    glEnable( GL_TEXTURE_2D );
    // Use first loaded texture as bumpmap
    glBindTexture( GL_TEXTURE_2D, iBumpmapTexture.iID );
    // Configure the texture combiner to do the DOT3 bumpmapping calculation
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE );
    glTexEnvf( GL_TEXTURE_ENV, GL_COMBINE_RGB,  GL_DOT3_RGB );
    // The first colour for each fragment operation is gotten from the bumpmap texture
    glTexEnvf( GL_TEXTURE_ENV, GL_SRC0_RGB,  GL_TEXTURE );
    glTexEnvf( GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR );
    // The second colour for each fragment operation is gotten from the primary colour
    glTexEnvf( GL_TEXTURE_ENV, GL_SRC1_RGB,  GL_PRIMARY_COLOR );
    glTexEnvf( GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR );
    glTexCoordPointer( 2, GL_FLOAT, 0, iBumpmapTexCoords );

    // UNIT 1
    // Configure the second texture combiner to modulate the base texture
    // by N.L
    glClientActiveTexture(GL_TEXTURE1);
    glActiveTexture( GL_TEXTURE1 );
    glEnable( GL_TEXTURE_2D );
    // Use second loaded texture as the "traditional" texture map
    glBindTexture( GL_TEXTURE_2D, iTexture.iID );
    // Configure the texture combiner to modulate the colour with the texture
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE );
    glTexEnvf( GL_TEXTURE_ENV, GL_COMBINE_RGB,  GL_MODULATE );
    // The first colour for each fragment operation is gotten from the previous texture combiners output
    glTexEnvf( GL_TEXTURE_ENV, GL_SRC0_RGB,  GL_PREVIOUS );
    glTexEnvf( GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR );
    // The second colour for each fragment operation is gotten from the texture bitmap
    glTexEnvf( GL_TEXTURE_ENV, GL_SRC1_RGB,  GL_TEXTURE );
    glTexEnvf( GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR );
    glTexCoordPointer( 2, GL_FLOAT, 0, iBumpmapTexCoords );

    GLfloat lightX = iLightSource->iPosition.iX;
    GLfloat lightY = iLightSource->iPosition.iY;
    GLfloat lightZ = iLightSource->iPosition.iZ;
    TVector vectVert2Light;
    for( TInt y = 0; y < NUM_VERTICES_PER_SIDE; y++ )
        {
        for( TInt x = 0; x < NUM_VERTICES_PER_SIDE; x++ )
            {
            // Compute L and store as vertex color
            // Calculate the vector from vertice to light source
            TInt idx = x + y * NUM_VERTICES_PER_SIDE;

            vectVert2Light.iX = lightX - iBumpmapVertices[idx * 3    ];
            vectVert2Light.iY = lightY - iBumpmapVertices[idx * 3 + 1];
            vectVert2Light.iZ = lightZ - iBumpmapVertices[idx * 3 + 2];

            // Normalize the vector
            vectVert2Light.Normalize();

            // Convert vector into tangent space
            vectVert2Light.MultMatrix4x4( tangentMatrix );

            // Scale and bias light vector values from range [-0.5, 0.5] to range [0.0, 1.0]
            vectVert2Light.iX = vectVert2Light.iX * 0.5 + 0.5;
            vectVert2Light.iY = vectVert2Light.iY * 0.5 + 0.5;
            vectVert2Light.iZ = vectVert2Light.iZ * 0.5 + 0.5;

            // Store the light vector as vertex color
            iBumpmapColors[ idx * 4     ] = vectVert2Light.iX;
            iBumpmapColors[ idx * 4 + 1 ] = vectVert2Light.iY;
            iBumpmapColors[ idx * 4 + 2 ] = vectVert2Light.iZ;
            }
        }

    // Setup the array pointers and draw the bumpmapped rectangle
    glVertexPointer(   3, GL_FLOAT, 0, iBumpmapVertices );
    glColorPointer(    4, GL_FLOAT, 0, iBumpmapColors );
    glNormal3f( 0.f, 0.f, 1.f );
    glDrawElements( GL_TRIANGLES, (NUM_VERTICES_PER_SIDE - 1) * (NUM_VERTICES_PER_SIDE - 1) * 6, GL_UNSIGNED_BYTE, iBumpmapIndices );

    // reset texture unit 1 state
    glActiveTexture( GL_TEXTURE1 );
    glMatrixMode( GL_TEXTURE );
    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );
    glDisable( GL_TEXTURE_2D );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    // reset texture unit 0 state
    glActiveTexture( GL_TEXTURE0 );
    glMatrixMode( GL_TEXTURE );
    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );
    glDisable( GL_TEXTURE_2D );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    }

// ----------------------------------------------------------------------------
// CBumpMap::GenerateBumpmapVertices
// Generates the vertices for drawing the bumpmapped rectangle to the screen.
// ----------------------------------------------------------------------------
//
void CBumpMap::GenerateBumpmapVerticesL()
    {
    iBumpmapVertices  = new GLfloat[ NUM_VERTICES_PER_SIDE * NUM_VERTICES_PER_SIDE * 3];
    iBumpmapTexCoords = new GLfloat[ NUM_VERTICES_PER_SIDE * NUM_VERTICES_PER_SIDE * 2];

    iBumpmapColors    = new GLfloat[ NUM_VERTICES_PER_SIDE * NUM_VERTICES_PER_SIDE * 4];
    iBumpmapIndices   = new GLubyte[ (NUM_VERTICES_PER_SIDE - 1) * (NUM_VERTICES_PER_SIDE - 1) * 6];

    // Generate the texture coordinates, vertices, colors and normals
    GLfloat leftBottom   = - BUMPMAP_SIZE / 2.f;
    GLfloat divisionSize = BUMPMAP_SIZE / (GLfloat) ( NUM_VERTICES_PER_SIDE - 1 );
    TInt idx = 0;
    for( TInt y = 0; y < NUM_VERTICES_PER_SIDE; y++ )
        {
        for( TInt x = 0; x < NUM_VERTICES_PER_SIDE; x++ )
            {
            iBumpmapVertices[idx * 3    ] = leftBottom + x * divisionSize;
            iBumpmapVertices[idx * 3 + 1] = leftBottom + y * divisionSize;
            iBumpmapVertices[idx * 3 + 2] = BUMPMAP_Z_COORD;

            iBumpmapTexCoords[idx * 2    ] = (GLfloat) x / (GLfloat) ( NUM_VERTICES_PER_SIDE - 1 );
            iBumpmapTexCoords[idx * 2 + 1] = 1.f - (GLfloat) y / (GLfloat) ( NUM_VERTICES_PER_SIDE - 1 );

            iBumpmapColors[idx * 4    ] = 1.f;
            iBumpmapColors[idx * 4 + 1] = 1.f;
            iBumpmapColors[idx * 4 + 2] = 0.f;
            iBumpmapColors[idx * 4 + 3] = 1.f;

            idx++;
            }
        }

    // Generate the indices that are used to draw the
    // triangles that in turn form the rectangle
    idx = 0;
    for( TInt y = 0; y < NUM_VERTICES_PER_SIDE - 1; y++ )
        {
        for( TInt x = 0; x < NUM_VERTICES_PER_SIDE - 1; x++ )
            {
            // First triangle of the quad
            iBumpmapIndices[idx * 6     ] =    x       +   y       * NUM_VERTICES_PER_SIDE;
            iBumpmapIndices[idx * 6  + 2] =    x       + ( y + 1 ) * NUM_VERTICES_PER_SIDE;
            iBumpmapIndices[idx * 6  + 1] =  ( x + 1 ) + ( y + 1 ) * NUM_VERTICES_PER_SIDE;
            // Second triangle of the quad
            iBumpmapIndices[idx * 6  + 3] =    x       +   y       * NUM_VERTICES_PER_SIDE;
            iBumpmapIndices[idx * 6  + 5] =  ( x + 1 ) + ( y + 1 ) * NUM_VERTICES_PER_SIDE;
            iBumpmapIndices[idx * 6  + 4] =  ( x + 1 ) +   y       * NUM_VERTICES_PER_SIDE;

            idx++;
            }
        }
    }

// End of File
