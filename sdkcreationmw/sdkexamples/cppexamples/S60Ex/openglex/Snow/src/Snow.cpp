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

#include "snow.h"
#include <aknnotewrappers.h>
#include <e32math.h>

// MACROS

// CONSTANTS

/** Vertice coordinates for the ground plane. */
static const GLbyte planeVertices[4 * 3] =
	{
    +1,0,-1,
    +1,0,+1,
    -1,0,+1,
    -1,0,-1
	};

/** Texture coordinates for ground plane. */
static const GLbyte planeTextureCoords[4 * 2] =
	{
      0, 10,
      0,  0,
     10,  0,
     10, 10
	};


/** Indices for drawing the triangles of the ground plane. */
static const GLubyte planeTriangles[2 * 3] =
	{
    0,2,1,
    0,3,2
	};


/** Vertice coordinates for tree. */
static const GLbyte treeVertices[4 * 3] =
	{
    +1, 0,0,
    +1,+1,0,
    -1,+1,0,
    -1, 0,0
	};

/** Texture coordinates for tree. */
static const GLbyte treeTextureCoords[4 * 2] =
	{
    0, 1,
    0, 0,
    1, 0,
    1, 1
	};

/** Indices for drawing the tree. */
static const GLubyte treeTriangles[2 * 3] =
	{
    0,1,2,
    0,2,3
	};

/** Indices for drawing the clouds. */
static const GLubyte cloudTriangles [] =
	{
    0,1,2,
    0,2,3
	};

/** Vertice coordinates for snow. */
static const GLbyte snowVertices[4 * 3] =
	{
    +2, 0,0,
    +2,+2,0,
    -2,+2,0,
    -2, 0,0
	};

/** Texture coordinates for snow. */
static const GLbyte snowTextureCoords[4 * 2] =
	{
    0, 1,
    0, 0,
    1, 0,
    1, 1
	};

/* Indices for drawing the snow. */
static const GLubyte snowTriangles[2 * 3] =
	{
    0,1,2,
    0,2,3
	};


// LOCAL CONSTANTS AND MACROS

// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSnow::NewL
// Symbian 2-phase constructor. Can leave.
// -----------------------------------------------------------------------------
//
CSnow* CSnow::NewL( TUint aWidth, TUint aHeight )
	{
    /* Symbian 2-phase constructor. Calls both the default
       C++ constructor and Symbian ConstructL methods */
    CSnow* self = new (ELeave) CSnow( aWidth, aHeight );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
	}

// Destructor.
CSnow::~CSnow()
	{
	}

// -----------------------------------------------------------------------------
// CSnow::GetSnowfall
// Gets the particle engine.
// Returns:		The owned particle engine pointer
// -----------------------------------------------------------------------------
//
CSnowfall* CSnow::GetSnowfall()
	{
	return iSnowfall;
	}

// -----------------------------------------------------------------------------
// CSnow::AppInit
// Application initialization code.
// -----------------------------------------------------------------------------
//
void CSnow::AppInitL( void )
	{
    // Construct a texture manager that uses the application's private
    // directory as the source location for all textures.
    iTextureManager = CTextureManager::NewL ( iScreenWidth, iScreenHeight,
                                              FRUSTUM_TOP, FRUSTUM_BOTTOM, FRUSTUM_RIGHT, FRUSTUM_LEFT, FRUSTUM_NEAR,
                                              this );

    // Initialize viewport and projection.
		SetScreenSize( iScreenWidth, iScreenHeight );

    /* Set the screen background color. */
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    /* Enable back face culling. */
    glEnable( GL_CULL_FACE  );
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_DEPTH_TEST );

    //Enable alpha test for color keying.
    glEnable( GL_ALPHA_TEST );
    glAlphaFunc( GL_GREATER, 0.0f );

    /*Disable lighting */
    glDisable( GL_LIGHTING );

    // Calculate cloud vertices based on the aspect ratio
    GLfloat aspectRatio = (GLfloat)(iScreenWidth) / (GLfloat)(iScreenHeight);
    iCloudVertices[ 0 ] = FRUSTUM_RIGHT * aspectRatio;
    iCloudVertices[ 1 ] = 0.f;
    iCloudVertices[ 2 ] = -FRUSTUM_NEAR;

    iCloudVertices[ 3 ] = FRUSTUM_RIGHT * aspectRatio;
    iCloudVertices[ 4 ] = FRUSTUM_TOP;
    iCloudVertices[ 5 ] = -FRUSTUM_NEAR;

    iCloudVertices[ 6 ] = FRUSTUM_LEFT * aspectRatio;
    iCloudVertices[ 7 ] = FRUSTUM_TOP;
    iCloudVertices[ 8 ] = -FRUSTUM_NEAR;

    iCloudVertices[ 9 ] = FRUSTUM_LEFT * aspectRatio;
    iCloudVertices[ 10 ] = 0.f;
    iCloudVertices[ 11 ] = -FRUSTUM_NEAR;

    glMatrixMode( GL_MODELVIEW );

    /* Enable vertex arrays. */
    glEnableClientState( GL_VERTEX_ARRAY );
    /* Enable texture arrays. */
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

    iCloudTextureCoords[0] = 0; iCloudTextureCoords[1] = 1;
    iCloudTextureCoords[2] = 0; iCloudTextureCoords[3] = 0;
    iCloudTextureCoords[4] = 1; iCloudTextureCoords[5] = 0;
    iCloudTextureCoords[6] = 1; iCloudTextureCoords[7] = 1;

    iCamera.LookAt( TVector( 0, 30.0f, 0 ),
                    TVector(0, 0, -(FRUSTUM_FAR+FRUSTUM_NEAR)/2),
                    TVector(0, 1, 0));

    iSnowfall = CSnowfall::NewL(
        700,                                                       // Number of particles
        TVector( 0, 80, -( FRUSTUM_FAR + FRUSTUM_NEAR ) / 2 ),     // Center position
        120.f,                                                     // Area width
        ( FRUSTUM_FAR - FRUSTUM_NEAR ),                            // Area height
        0,                                                         // Ground level
        iTextureManager );                                         // Texture manager

    // Defining a color key interval for the tree texture.
    TUint8 MinColor[] = { 50, 0, 50 };
    TUint8 MaxColor[] = { 255, 75, 255 };

    // Pushing the textures into the loading queue.
	_LIT(KSnowyTreeName, "snowy_tree.gif");
	_LIT(KSnowyGroundName, "snowy_ground.jpg");
	_LIT(KCloudName, "cloud.jpg");
    iTextureManager->RequestToLoad(KSnowyTreeName, &iTreeTexture, MinColor, MaxColor );
    iTextureManager->RequestToLoad(KSnowyGroundName, &iGroundTexture );
    iTextureManager->RequestToLoad(KCloudName, &iCloudTexture );

    //Start to load the textures.
    iTextureManager->DoLoadL();
	}

// -----------------------------------------------------------------------------
// CSnow::AppExit
// Application cleanup code.
// -----------------------------------------------------------------------------
//
void CSnow::AppExit( void )
	{
    delete iSnowfall;
    delete iTextureManager;
	}

// -----------------------------------------------------------------------------
// CSnow::DrawPlane
// Draws the ground plane.
// -----------------------------------------------------------------------------
//
void CSnow::DrawPlane()
	{
    glBindTexture(  GL_TEXTURE_2D, iGroundTexture.iID );
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glLoadIdentity();
    T3DModel::MakeWorldViewMatrix(iCamera, TVector(0.0f, 0.0f, 0.0f));
    glScalef( 1000, 0, 1000 );


    glVertexPointer( 3, GL_BYTE, 0, planeVertices );
    glTexCoordPointer( 2, GL_BYTE, 0, planeTextureCoords );
    glDrawElements( GL_TRIANGLES, 2 * 3, GL_UNSIGNED_BYTE, planeTriangles );
	}

// -----------------------------------------------------------------------------
// CSnow::DrawCloud
// Draws the clouds.
// -----------------------------------------------------------------------------
//
void CSnow::DrawCloud(GLfloat aDeltaTimeSecs )
	{
    iTextureOffset += (GLfloat)(0.01f*aDeltaTimeSecs);

    iCloudTextureCoords[0] = iCloudTextureCoords[2] = iTextureOffset;
    iCloudTextureCoords[4] = iCloudTextureCoords[6] = iTextureOffset + 0.4f;

    glDisable( GL_DEPTH_TEST );

    glLoadIdentity();

    glBindTexture(  GL_TEXTURE_2D, iCloudTexture.iID );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glVertexPointer( 3, GL_FLOAT, 0, iCloudVertices );
    glTexCoordPointer( 2, GL_FLOAT, 0, iCloudTextureCoords );
    glDrawElements( GL_TRIANGLES, 2 * 3, GL_UNSIGNED_BYTE, cloudTriangles );

    glEnable( GL_DEPTH_TEST );
	}

// -----------------------------------------------------------------------------
// CSnow::DrawTree
// Draws a tree.
// -----------------------------------------------------------------------------
//
void CSnow::DrawTree( GLfloat aX, GLfloat aZ)
	{
    glLoadIdentity();

    //Turns the tree so that it always faces the camera.
    T3DModel::MakeBillboardWorldViewMatrix(iCamera, TVector(aX, 0.0f, aZ));
    glScalef( 10, 30, 10 );

    glBindTexture(  GL_TEXTURE_2D, iTreeTexture.iID );
    glTexParameterf( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    glVertexPointer( 3, GL_BYTE, 0, treeVertices );
    glTexCoordPointer( 2, GL_BYTE, 0, treeTextureCoords );
    glDrawElements( GL_TRIANGLES, 2 * 3, GL_UNSIGNED_BYTE, treeTriangles );
	}

// ----------------------------------------------------------------------------
// C#Name#::AppCycle
// Draws and animates the objects.
// aFrame = Number of the frame to be rendered.
// aTimeSecs = Seconds elapsed since the application started running.
// aDeltaTimeSecs = Seconds elapsed since last call to AppCycle().
// ----------------------------------------------------------------------------
//
void CSnow::AppCycle( GLuint /*aFrame*/, GLfloat /*aTimeSecs*/, GLfloat aDeltaTimeSecs )
	{
    glClear( GL_DEPTH_BUFFER_BIT );

    /* Draws the clouds, and the ground plane */
    glDisable( GL_DEPTH_TEST );
    DrawCloud( aDeltaTimeSecs );
    DrawPlane();

    const TReal32 treeStepX = 30.0f;
    const TReal32 treeStepZ = 100.0f;

    //Draw the trees.
    DrawTree( -treeStepX, -1*treeStepZ);
    DrawTree( +treeStepX, -1*treeStepZ);
    DrawTree( -treeStepX, -2*treeStepZ);
    DrawTree( +treeStepX, -2*treeStepZ);

    glEnable( GL_DEPTH_TEST );

    /*Update and render the particle engine */
    iSnowfall->UpdateEngine( aDeltaTimeSecs );
    iSnowfall->RenderEngine( iCamera );
	}

// ----------------------------------------------------------------------
// CSnow::CSnow
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------
//
CSnow::CSnow( TUint aWidth, TUint aHeight )
	: CFiniteStateMachine()
	{
    iScreenWidth  = aWidth;
    iScreenHeight = aHeight;

	}

// ----------------------------------------------------------------------
// CSnow::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------
//
void CSnow::ConstructL( void )
	{
	}

// ---------------------------------------------------------
// CSnow::OnEnterState( TInt aState )
// Called by TFiniteStateMachine when the f.s.m enters a new state
// ---------------------------------------------------------
void CSnow::OnEnterStateL( TInt /*aState*/ )
	{
	}

// -------------------------------------------------------------------------------------------------------
// CSnow::OnStartLoadingTextures()
// Called for a MTextureLoadingListener by the texture manager when texture loading operation starts
// -------------------------------------------------------------------------------------------------------

void CSnow::OnStartLoadingTexturesL()
	{
    SetStateL( ELoadingTextures );
	}

// ------------------------------------------------------------------------------------------------------------
// CSnow::OnEndLoadingTextures()
// Called for a MTextureLoadingListener by the texture manager when texture loading operation is completed
// ------------------------------------------------------------------------------------------------------------
void CSnow::OnEndLoadingTexturesL()
	{
	if ( GetState() == ELoadingTextures )
		{
        SetStateL( ERunning );
		}
	}

// -----------------------------------------------------------------------------
// CSnow::SetScreenSize
// Reacts to the dynamic screen size change during execution of this program.
// -----------------------------------------------------------------------------
//
void CSnow::SetScreenSize( TUint aWidth, TUint aHeight )
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

    // Recalculate cloud vertices based on the aspect ratio
    iCloudVertices[ 0 ] = FRUSTUM_RIGHT * aspectRatio;
    iCloudVertices[ 1 ] = 0.f;
    iCloudVertices[ 2 ] = -FRUSTUM_NEAR;

    iCloudVertices[ 3 ] = FRUSTUM_RIGHT * aspectRatio;
    iCloudVertices[ 4 ] = FRUSTUM_TOP;
    iCloudVertices[ 5 ] = -FRUSTUM_NEAR;

    iCloudVertices[ 6 ] = FRUSTUM_LEFT * aspectRatio;
    iCloudVertices[ 7 ] = FRUSTUM_TOP;
    iCloudVertices[ 8 ] = -FRUSTUM_NEAR;

    iCloudVertices[ 9 ] = FRUSTUM_LEFT * aspectRatio;
    iCloudVertices[ 10 ] = 0.f;
    iCloudVertices[ 11 ] = -FRUSTUM_NEAR;
    }

// ----------------------------------------------------------------------
// CSnowfall::CSnowfall
// C++ default constructor can NOT contain any code, that
// ----------------------------------------------------------------------
//
CSnowfall::CSnowfall()
	{
	};


// ----------------------------------------------------------------------
// CSnowfall::NewL
// 2nd Two-phased constructor.
// Creates and initializes a CSnowfall object:
//   snowflakes are released from within
//   the rectangle (aPosition.iX-aWidth/2, aPosition.iY, aPosition.iX-aWidth/2)
//                 (aPosition.iX+aWidth/2, aPosition.iY, aPosition.iX+aWidth/2).
//   Once their height is < iGroundLevel, they are reset.// Returns:	The current time.
// ----------------------------------------------------------------------
//

CSnowfall* CSnowfall::NewL(GLint aParticlesCount, TVector aPosition,
                           GLfloat aWidth, GLfloat aDepth, GLfloat aGroundLevel,
                           CTextureManager * aTextureManager)
	{
    /* Symbian 2-phase constructor. Calls both the default
       C++ constructor and Symbian ConstructL methods */
    CSnowfall* self = new (ELeave) CSnowfall;
    CleanupStack::PushL( self );
    self->ConstructL( aParticlesCount, aPosition, aWidth, aDepth, aGroundLevel, aTextureManager);
    CleanupStack::Pop();

    return self;
	}

// ----------------------------------------------------------------------
// CSnowfall::ConstructL
// 2nd phase constructor
// Initializes a CSnowfall object:
//   snowflakes are released from within
//   the rectangle (aPosition.iX-aWidth/2, aPosition.iY, aPosition.iX-aWidth/2)
//                 (aPosition.iX+aWidth/2, aPosition.iY, aPosition.iX+aWidth/2).
//   Once their height is < iGroundLevel, they are reset.
// ----------------------------------------------------------------------
//
void CSnowfall::ConstructL(GLint aParticlesCount, TVector aPosition,
                           GLfloat aWidth, GLfloat aDepth, GLfloat aGroundLevel,
                           CTextureManager * aTextureManager)
	{
    CParticleEngine::ConstructL(aParticlesCount, aPosition);

    iWidth = aWidth;
    iDepth = aDepth;
    iGroundLevel = aGroundLevel;

    TTime now;
    now.HomeTime();
    iSeed = now.Int64();

    ResetEngine();

	_LIT(KSnowName, "snow.jpg");
    aTextureManager->RequestToLoad(KSnowName, &iTexture );
	}

// Destructor.

CSnowfall::~CSnowfall()
	{
	}

// ----------------------------------------------------------------------
// CSnowfall::ResetParticle
// Implementation of CParticleEngine::ResetParticle(GLint)
// ----------------------------------------------------------------------
//
void CSnowfall::ResetParticle(GLint aIndex)
	{
    TParticle& Particle = iParticles[aIndex];

    Particle.iPosition = TVector(
        randrange( -iWidth / 0.5f, iWidth / 0.5f, iSeed ),
		0,
		randrange( -iDepth / 0.5f, iDepth / 0.5f, iSeed ) ) + iPosition;

    Particle.iVelocity = TVector(
        randrange( -0.2f,   0.2f, iSeed ),
        randrange( -20.f, -10.f, iSeed ),
        randrange( -0.2f,   0.2f, iSeed ) );
	}

// ----------------------------------------------------------------------
// CSnowfall::UpdateEngine
// Implementation of CParticleEngine::UpdateEngine(GLfloat aElapsedTime)
// ----------------------------------------------------------------------
//
void CSnowfall::UpdateEngine(GLfloat aDeltaTimeSecs)
	{
    for ( GLint i = 0; i < iParticlesCount; i++ )
		{
        iParticles[i].iPosition += (iParticles[i].iVelocity * aDeltaTimeSecs);
        if ( iParticles[i].iPosition.iY < iGroundLevel )
			{
            ResetParticle(i);
			}
		}
	}

// ----------------------------------------------------------------------
// CSnowfall::RenderEngine
// Implementation of CParticleEngine::RenderEngine(TCamera &)
// ----------------------------------------------------------------------
//
void CSnowfall::RenderEngine(TCamera& aCamera)
	{
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    TVector CameraPos = aCamera.GetPosition();

    glBindTexture(  GL_TEXTURE_2D, iTexture.iID );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glVertexPointer( 3, GL_BYTE, 0, snowVertices );
    glTexCoordPointer( 2, GL_BYTE, 0, snowTextureCoords );

    for ( GLint i = 0; i < iParticlesCount; i++ )
		{
        glLoadIdentity();
        glTranslatef(iParticles[i].iPosition.iX-CameraPos.iX,
                     iParticles[i].iPosition.iY-CameraPos.iY,
                     iParticles[i].iPosition.iZ-CameraPos.iZ);

        glDrawElements( GL_TRIANGLES, 2 * 3, GL_UNSIGNED_BYTE, snowTriangles );
		}

    glDisable(GL_BLEND);
	}


