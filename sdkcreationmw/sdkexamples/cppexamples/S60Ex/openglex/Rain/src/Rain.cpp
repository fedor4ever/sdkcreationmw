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

#include "rain.h"
#include <aknnotewrappers.h>
#include <e32math.h>

// CONSTANTS

/* Define vertice coordinates for rain */
static const GLbyte rainVertices[2 * 3] =
	{
    0,3,0,
    0,0,0,
	};

/* Define vertice coordinates for the ground plane */
static const GLbyte planeVertices[4 * 3] =
	{
    +1,0,-1,
    +1,0,+1,
    -1,0,+1,
    -1,0,-1
	};

/* Define texture coordinates for ground plane */
static const GLbyte planeTextureCoords[4 * 2] =
	{
      0, 10,
      0,  0,
     10,  0,
     10, 10
	};


/* Define indices for drawing the triangles of the ground plane */
static const GLubyte planeTriangles[2 * 3] =
	{
    0,2,1,
    0,3,2
	};


/* Define vertice coordinates for tree */
static const GLbyte treeVertices[4 * 3] =
	{
    +1, 0,0,
    +1,+1,0,
    -1,+1,0,
    -1, 0,0
	};

/* Define texture coordinates for tree */
static const GLbyte treeTextureCoords[4 * 2] =
	{
    0, 1,
    0, 0,
    1, 0,
    1, 1
	};

/* Define indices for drawing the tree */
static const GLubyte treeTriangles[2 * 3] =
	{
    0,1,2,
    0,2,3
	};


/* Define indices for drawing the clouds */
static const GLubyte cloudTriangles [] =
	{
    0,1,2,
    0,2,3
	};

// LOCAL CONSTANTS AND MACROS

#define TREE_STEP_X 30.0f
#define TREE_STEP_Z 100.0f


// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRain::NewL
// Symbian 2-phase constructor. Can leave.
// -----------------------------------------------------------------------------
//
CRain* CRain::NewL( TUint aWidth, TUint aHeight )
	{
	/* Symbian 2-phase constructor. Calls both the default
	   C++ constructor and Symbian ConstructL methods */
	CRain* self = new (ELeave) CRain( aWidth, aHeight );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();

	return self;
	}


// Destructor.

CRain::~CRain()
	{
	}

// -----------------------------------------------------------------------------
// CRain::AppInit
// Application initialization code.
// -----------------------------------------------------------------------------
//
void CRain::AppInitL( void )
    {
    // Construct a texture manager that uses the application's private
    // directory as the location for all textures.
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

	iRainfall = CRainfall::NewL( 1000, TVector(0, 80, -(FRUSTUM_FAR+FRUSTUM_NEAR)/2), 120, (FRUSTUM_FAR-FRUSTUM_NEAR), 0);

	iCamera.LookAt(TVector(0, 30.0f, 0), TVector(0, 0, -(FRUSTUM_FAR+FRUSTUM_NEAR)/2), TVector(0, 1, 0));

    // Defining a color key interval for the tree texture.
	TUint8 MinColor[] = { 50, 0, 50};
	TUint8 MaxColor[] = { 200, 0, 200};

	// Pushing the textures into the loading queue.
	_LIT(KTreeName, "tree.gif");
	_LIT(KGrassName, "grass.jpg");
	_LIT(KCloudName, "cloud.jpg");

	iTextureManager->RequestToLoad(KTreeName , &iTreeTexture, MinColor, MaxColor );
	iTextureManager->RequestToLoad(KGrassName, &iGrassTexture );
	iTextureManager->RequestToLoad(KCloudName, &iCloudTexture );

	//Start to load the textures.
	iTextureManager->DoLoadL();
	}

// -----------------------------------------------------------------------------
// CRain::AppExit
// Application cleanup code.
// -----------------------------------------------------------------------------
//
void CRain::AppExit( void )
	{
	delete iRainfall;
	delete iTextureManager;
	}

// -----------------------------------------------------------------------------
// CRain::DrawPlane
// Draws the ground plane.
// -----------------------------------------------------------------------------
//
void CRain::DrawPlane()
	{
    glBindTexture(  GL_TEXTURE_2D, iGrassTexture.iID );
    glTexParameterf( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameterf( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    glLoadIdentity();
	T3DModel::MakeWorldViewMatrix(iCamera, TVector(0.0f, 0.0f, 0.0f));
	glScalef( 1000, 0, 1000 );

    glVertexPointer( 3, GL_BYTE, 0, planeVertices );
	glTexCoordPointer( 2, GL_BYTE, 0, planeTextureCoords );
    glDrawElements( GL_TRIANGLES, 2 * 3, GL_UNSIGNED_BYTE, planeTriangles );
	}

// -----------------------------------------------------------------------------
// CRain::DrawCloud
// Draws the clouds.
// -----------------------------------------------------------------------------
//
void CRain::DrawCloud( GLfloat aDeltaTimeSecs )
	{
    iTextureOffset += (GLfloat)(0.01f * aDeltaTimeSecs);

	iCloudTextureCoords[0] = iCloudTextureCoords[2]= iTextureOffset;
    iCloudTextureCoords[4] = iCloudTextureCoords[6]= iTextureOffset+0.4f;

    glLoadIdentity();
	glBindTexture(  GL_TEXTURE_2D, iCloudTexture.iID );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameterf( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	glVertexPointer( 3, GL_FLOAT, 0, iCloudVertices );
	glTexCoordPointer( 2, GL_FLOAT, 0, iCloudTextureCoords );
    glDrawElements( GL_TRIANGLES, 2 * 3, GL_UNSIGNED_BYTE, cloudTriangles );
	}

// -----------------------------------------------------------------------------
// CRain::DrawTree
// Draws a tree.
// -----------------------------------------------------------------------------
//
void CRain::DrawTree(GLfloat aX, GLfloat aZ)
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
void CRain::AppCycle( GLuint /*aFrame*/, GLfloat /*aTimeSecs*/, GLfloat aDeltaTimeSecs )
	{
    glClear( GL_DEPTH_BUFFER_BIT );

	/* Draws the clouds, and the ground plane */
    glDisable( GL_DEPTH_TEST );
	DrawCloud( aDeltaTimeSecs );
	DrawPlane();

	//Draw the trees.
	DrawTree( -TREE_STEP_X, -1*TREE_STEP_Z);
    DrawTree( +TREE_STEP_X, -1*TREE_STEP_Z);
	DrawTree( -TREE_STEP_X, -2*TREE_STEP_Z);
    DrawTree( +TREE_STEP_X, -2*TREE_STEP_Z);

	glEnable( GL_DEPTH_TEST );

	/*Update and render the particle engine */
	iRainfall->UpdateEngine(aDeltaTimeSecs);
    iRainfall->RenderEngine(iCamera);
	}

// ----------------------------------------------------------------------
// CRain::CRain
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------
//
CRain::CRain( TUint aWidth, TUint aHeight )
    : CFiniteStateMachine()
	{
	iScreenWidth  = aWidth;
	iScreenHeight = aHeight;

	}

// ----------------------------------------------------------------------
// CRain::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------
//
void CRain::ConstructL( void )
	{
	}

// ---------------------------------------------------------
// CRain::OnEnterState( TInt aState )
// Called by TFiniteStateMachine when the f.s.m enters a new state
// ---------------------------------------------------------
void CRain::OnEnterStateL( TInt /*aState*/ )
	{
	}

// -------------------------------------------------------------------------------------------------------
// CRain::OnStartLoadingTextures()
// Called for a MTextureLoadingListener by the texture manager when texture loading operation starts
// -------------------------------------------------------------------------------------------------------

void CRain::OnStartLoadingTexturesL()
	{
	SetStateL( ELoadingTextures );
	}

// ------------------------------------------------------------------------------------------------------------
// CRain::OnEndLoadingTextures()
// Called for a MTextureLoadingListener by the texture manager when texture loading operation is completed
// ------------------------------------------------------------------------------------------------------------

void CRain::OnEndLoadingTexturesL()
	{
	if ( GetState() == ELoadingTextures )
		{
		SetStateL( ERunning );
		}
	}

// ----------------------------------------------------------------------
// CRain::GetRainfall
// Get the particle effect
// ----------------------------------------------------------------------
//
CRainfall* CRain::GetRainfall()
	{
	return iRainfall;
	}

// -----------------------------------------------------------------------------
// CRain::SetScreenSize
// Reacts to the dynamic screen size change during execution of this program.
// -----------------------------------------------------------------------------
//
void CRain::SetScreenSize( TUint aWidth, TUint aHeight )
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
// CRainfall::CRainfall
// C++ default constructor can NOT contain any code, that can leave
// ----------------------------------------------------------------------
//
CRainfall::CRainfall()
	{
	}

// ----------------------------------------------------------------------
// CRainfall::NewL
// 2nd Two-phased constructor.
// Creates and initializes a CRainfall object:
//   raindrops are released from within
//   the rectangle (aPosition.iX-aWidth/2, aPosition.iY, aPosition.iX-aWidth/2)
//                 (aPosition.iX+aWidth/2, aPosition.iY, aPosition.iX+aWidth/2).
//   Once their height is < iGroundLevel, they are reset.// Returns:	The current time.
// ----------------------------------------------------------------------
//
CRainfall* CRainfall::NewL(GLint aParticlesCount, TVector aPosition,
						   GLfloat aWidth, GLfloat aDepth, GLfloat aGroundLevel)
	{
	/* Symbian 2-phase constructor. Calls both the default
	   C++ constructor and Symbian ConstructL methods */
	CRainfall* self = new (ELeave) CRainfall;
	CleanupStack::PushL( self );
	self->ConstructL( aParticlesCount, aPosition, aWidth, aDepth, aGroundLevel);
	CleanupStack::Pop();

	return self;
	}

// ----------------------------------------------------------------------
// CRainfall::ConstructL
// 2nd phase constructor
// Initializes a CRainfall object:
//   raindrops are released from within
//   the rectangle (aPosition.iX-aWidth/2, aPosition.iY, aPosition.iX-aWidth/2)
//                 (aPosition.iX+aWidth/2, aPosition.iY, aPosition.iX+aWidth/2).
//   Once their height is < iGroundLevel, they are reset.
// ----------------------------------------------------------------------
//
void CRainfall::ConstructL(GLint aParticlesCount, TVector aPosition,
						   GLfloat aWidth, GLfloat aDepth, GLfloat aGroundLevel)
	{
	CParticleEngine::ConstructL(aParticlesCount, aPosition);

	iWidth = aWidth;
	iDepth = aDepth;
	iGroundLevel = aGroundLevel;

	TTime now;
	now.HomeTime();
	iSeed = now.Int64();

	ResetEngine();
	}

// Destructor.
CRainfall::~CRainfall()
	{
	}

// ----------------------------------------------------------------------
// CRainfall::ResetParticle
// Implementation of CParticleEngine::ResetParticle(GLint)
// ----------------------------------------------------------------------
//
void CRainfall::ResetParticle(GLint aIndex)
	{

	TParticle & Particle = iParticles[aIndex];

    Particle.iPosition = TVector( iWidth * (Random() - 0.5f), 0, iDepth * (Random() - 0.5f)) + iPosition;
    Particle.iVelocity = TVector( 0, randrange( -40.f, -20.f, iSeed ), 0);
	}

// ----------------------------------------------------------------------
// CRainfall::UpdateEngine
// Implementation of CParticleEngine::UpdateEngine(GLfloat aElapsedTime)
// ----------------------------------------------------------------------
//
void CRainfall::UpdateEngine(GLfloat aDeltaTimeSecs)
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
// CRainfall::UpdateEngine
// Implementation of CParticleEngine::RenderEngine(TCamera &)
// ----------------------------------------------------------------------
//
void CRainfall::RenderEngine( TCamera & aCamera )
	{
    glDisable( GL_TEXTURE_2D );
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f( 1.0f, 1.0f, 1.0f, 0.5f );

	TVector CameraPos = aCamera.GetPosition();

	glVertexPointer( 3, GL_BYTE, 0, rainVertices );
    for ( GLint i = 0; i < iParticlesCount; i++ )
		{
		glLoadIdentity();
		glTranslatef( iParticles[i].iPosition.iX - CameraPos.iX,
			          iParticles[i].iPosition.iY - CameraPos.iY,
					  iParticles[i].iPosition.iZ - CameraPos.iZ );

		glDrawArrays( GL_LINES, 0, 2 );
		}

    glEnable( GL_TEXTURE_2D );
    glDisable(GL_BLEND);
	}

// ----------------------------------------------------------------------
// CRainfall::Random
// Implementation of CParticleEngine::Random
// Create a random float number.
// ----------------------------------------------------------------------
//
GLfloat CRainfall::Random()
	{
	return ((GLfloat)(Math::FRand(iSeed)));
	}

