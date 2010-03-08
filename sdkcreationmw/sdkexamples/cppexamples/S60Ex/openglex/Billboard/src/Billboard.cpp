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
#include "billboard.h"
#include <aknnotewrappers.h>
#include <e32math.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const GLint KNumberOfTreeInRow  = 10;                    // Number of trees in a row.
const GLint KNumberOfTreeInCol  = 10;                    // Number of trees in a col.
const GLint KTreeStep           = 10;                    // Min. Distance between two trees.
const GLfixed KCamRotationSpeed = FLOAT_2_FIXED(  2.5f ); // Speed at which the camera rotates.
const GLfixed KCamMovementSpeed = FLOAT_2_FIXED(  2.5f ); // Speed at which the camera moves forwards/backwards
const GLfixed KMaxCamDistance   = INT_2_FIXED( 100 );    // Maximum allowed camera distance from the center of the scene
const GLfixed KMinCamDistance   = INT_2_FIXED( 10 );     // Minimum allowed camera distance from the center of the scene

/* Define vertice coordinates for the ground plane */
static const GLbyte planeVertices[4 * 3] =
	{
    +1,0,-1,
    +1,0,+1,
    -1,0,+1,
    -1,0,-1
	};

/* Define texture coordinates for ground plane */
static const GLshort planeTextureCoords[4 * 2] =
	{
        0,    0,
        0, 1000,
     1000, 1000,
     1000,    0
	};


/* Define indices for drawing the triangles of the ground plane */
static const GLubyte planeTriangles[2 * 3] =
	{
    0,2,1,
    0,3,2
	};


/* Define vertice coordinates for drawing the trees */
static const GLbyte treeVertices[4 * 3] =
	{
    +1,  0, 0,
    +1, +1, 0,
    -1, +1, 0,
    -1,  0, 0
	};

/* Define texture coordinates for drawing the trees */
static const GLbyte treeTextureCoords[4 * 2] =
	{
      0, 100,
      0,   0,
    100,   0,
    100, 100
	};

/* Define indices for drawing the trees */
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

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CBillboard::NewL
// Symbian 2-phase constructor. Can leave.
// -----------------------------------------------------------------------------
//
CBillboard* CBillboard::NewL( TUint aWidth, TUint aHeight, CBillboardInput* aInputHandler  )
	{
    /* Symbian 2-phase constructor. Calls both the default
       C++ constructor and Symbian ConstructL methods */
    CBillboard* self = new (ELeave) CBillboard( aWidth, aHeight, aInputHandler  );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
	}

//  Destructor.
CBillboard::~CBillboard()
	{
	}

// -----------------------------------------------------------------------------
// CBillboard::AppInit
// Initializes the application.
// -----------------------------------------------------------------------------
//
void CBillboard::AppInitL( void )
	{
    // Construct a texture manager that uses the application's private
    // directory as the source location for all textures.
    iTextureManager = CTextureManager::NewL ( iScreenWidth,  iScreenHeight,
                                              FRUSTUM_TOP,   FRUSTUM_BOTTOM,
                                              FRUSTUM_RIGHT, FRUSTUM_LEFT,
                                              FRUSTUM_NEAR,  this );

    // Initialize viewport and projection.
    SetScreenSize( iScreenWidth, iScreenHeight );

    /* Set the screen background color. */
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    /* Enable back face culling. */
    glEnable( GL_CULL_FACE  );
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_DEPTH_TEST );

    /*Disable lighting */
    glDisable( GL_LIGHTING );

    //Enable alpha test for color keying.
    glEnable( GL_ALPHA_TEST );
    glAlphaFuncx( GL_GREATER, FLOAT_2_FIXED(0.0f));

    glMatrixMode( GL_TEXTURE );
    glLoadIdentity();
    glScalex( fixedDiv( FIXED_ONE, INT_2_FIXED(100) ),
              fixedDiv( FIXED_ONE, INT_2_FIXED(100) ),
              FIXED_ONE );


    glMatrixMode( GL_MODELVIEW );

    /* Enable vertex arrays. */
    glEnableClientState( GL_VERTEX_ARRAY );
    /* Enable texture arrays. */
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

    iCloudTextureCoords[0] = INT_2_FIXED( 0 ); iCloudTextureCoords[1] = INT_2_FIXED( 50 );
    iCloudTextureCoords[2] = INT_2_FIXED( 0 ); iCloudTextureCoords[3] = INT_2_FIXED( 0 );
    iCloudTextureCoords[4] = INT_2_FIXED( 50 ); iCloudTextureCoords[5] = INT_2_FIXED( 0 );
    iCloudTextureCoords[6] = INT_2_FIXED( 50 ); iCloudTextureCoords[7] = INT_2_FIXED( 50 );

    // Initiallly distance is 40 and camera is only rotating counter-clockwise.
    iDistance    = FLOAT_2_FIXED( 40.f );

    iLookUpTable = CLookUpTable::NewL( CLookUpTable::ESinx | CLookUpTable::ECosx );

    // Defining a color key interval for the tree texture.
    TUint8 MinColor[] = {  50, 0,  50};
    TUint8 MaxColor[] = { 200, 0, 200};

    // Push the textures into the loading queue.
    _LIT( KTreeTexture,  "tree.gif"  );
    _LIT( KGrassTexture, "grass.jpg" );
    _LIT( KCloudTexture, "cloud.jpg" );
    iTextureManager->RequestToLoad( KTreeTexture , &iTreeTexture, MinColor, MaxColor, false );
    iTextureManager->RequestToLoad( KGrassTexture, &iGrassTexture, false );
    iTextureManager->RequestToLoad( KCloudTexture, &iCloudTexture, false );

    //Start to load the textures.
    iTextureManager->DoLoadL();
	}

// -----------------------------------------------------------------------------
// CBillboard::AppExit
// Release any allocations made in AppInit.
// -----------------------------------------------------------------------------
//
void CBillboard::AppExit( void )
	{
    delete iTextureManager;
    delete iLookUpTable;
	}

// -----------------------------------------------------------------------------
// CBillboard::DrawPlane
// Draw the ground plane.
// -----------------------------------------------------------------------------
//
void CBillboard::DrawPlane()
	{
    glBindTexture(  GL_TEXTURE_2D, iGrassTexture.iID );

    // Disable texture filtering
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glLoadIdentity();
    T3DModelx::MakeWorldViewMatrix(
        iCamera,
        TVectorx( INT_2_FIXED( 0 ),
                  INT_2_FIXED( 0 ),
                  INT_2_FIXED( 0 ) ) );
    glScalex( INT_2_FIXED( 100 ), INT_2_FIXED( 0 ), INT_2_FIXED( 100 ) );

    glVertexPointer( 3, GL_BYTE, 0, planeVertices );
    glTexCoordPointer( 2, GL_SHORT, 0, planeTextureCoords );

    glDrawElements( GL_TRIANGLES, 2 * 3, GL_UNSIGNED_BYTE, planeTriangles );

	}

// -----------------------------------------------------------------------------
// CBillboard::DrawCloud
// Draw clouds.
// -----------------------------------------------------------------------------
//
void CBillboard::DrawCloud( GLfloat aTimeSecs  )
	{
    //Moving the clouds.
    iTextureOffset = FLOAT_2_FIXED( aTimeSecs );
    GLfixed leftEdge = iTextureOffset;
    GLfixed rightEdge = iTextureOffset + INT_2_FIXED( 50 );
    iCloudTextureCoords[0] = iCloudTextureCoords[2] = leftEdge;
    iCloudTextureCoords[4] = iCloudTextureCoords[6] = rightEdge;

    glLoadIdentity();

    glBindTexture( GL_TEXTURE_2D, iCloudTexture.iID );
    // Enable texture filtering
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glVertexPointer( 3, GL_FLOAT, 0, iCloudVertices );
    glTexCoordPointer( 2, GL_FIXED, 0, iCloudTextureCoords );
    glDrawElements( GL_TRIANGLES, 2 * 3, GL_UNSIGNED_BYTE, cloudTriangles );
	}

// -----------------------------------------------------------------------------
// CBillboard::DrawTree
// Draw a tree.
// -----------------------------------------------------------------------------
//
void CBillboard::DrawTree( TInt aRow, TInt aCol )
	{
    GLint x = KTreeStep * ( aCol - ( KNumberOfTreeInCol >> 1 ) );
    GLint z = KTreeStep * ( aRow - ( KNumberOfTreeInRow >> 1 ) );

    glLoadIdentity();

    // Turns the tree so that it always faces the camera. (Billboarding technique)
    T3DModelx::MakeBillboardWorldViewMatrix(
        iCamera,
        TVectorx( INT_2_FIXED( x ),
                  INT_2_FIXED( 0 ),
                  INT_2_FIXED( z ) ) );
    glScalex( INT_2_FIXED( 1 ), INT_2_FIXED( 3 ), INT_2_FIXED( 1 ) );
    glBindTexture(  GL_TEXTURE_2D, iTreeTexture.iID );

    // Disable texture filtering
    glTexParameterf( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glVertexPointer(   3, GL_BYTE, 0, treeVertices );
    glTexCoordPointer( 2, GL_BYTE, 0, treeTextureCoords );
    glDrawElements(    GL_TRIANGLES, 2 * 3, GL_UNSIGNED_BYTE, treeTriangles );
	}


// ----------------------------------------------------------------------------
// CBillboard::AppCycle
// Draws and animates the objects.
// aFrame = Number of the frame to be rendered.
// aTimeSecs = Seconds elapsed since the application started running.
// aDeltaTimeSecs = Seconds elapsed since last call to AppCycle().
// ----------------------------------------------------------------------------
//
void CBillboard::AppCycle( GLuint /*aFrame*/, GLfloat aTimeSecs, GLfloat aDeltaTimeSecs )
	{
	glClear( GL_DEPTH_BUFFER_BIT );

    GLfixed fixedDeltaTimeSecs = FLOAT_2_FIXED( aDeltaTimeSecs );

    // Rotate the camera
    TurnCamera( fixedDeltaTimeSecs );

    // Move the camera forwards/backwards;
    MoveCamera( fixedDeltaTimeSecs );

    // Calculate the current camera transform
    GLfixed x = fixedMul( iDistance, iLookUpTable->Cosx( iAzimuth ) );
    GLfixed z = fixedMul( iDistance, iLookUpTable->Sinx( iAzimuth ) );
    iCamera.LookAt( TVectorx(              x,   INT_2_FIXED( 3 ),               z   ),
                    TVectorx( INT_2_FIXED( 0 ), INT_2_FIXED( 0 ),  INT_2_FIXED( 0 ) ),
                    TVectorx( INT_2_FIXED( 0 ), INT_2_FIXED( 1 ),  INT_2_FIXED( 0 ) ) );

    // Draws the clouds, the gound plane and the trees
    glDisable( GL_DEPTH_TEST );
    DrawCloud( aTimeSecs );
    DrawPlane();
    glEnable( GL_DEPTH_TEST );

    for ( TInt row = 0;row < KNumberOfTreeInRow; row++ )
        {
        for ( TInt column = 0;column < KNumberOfTreeInCol; column++ )
			{
			DrawTree( row, column );
			}
        }
	}


// -----------------------------------------------------------------------------
// CBillboard::CBillBoard
// Default C++ and Symbian ConstructL are placed in protected
// area to force the use of NewL when creating an instance.
// -----------------------------------------------------------------------------
//
CBillboard::CBillboard( TUint aWidth, TUint aHeight, CBillboardInput* aInputHandler  )
	: CFiniteStateMachine()
	{
    iScreenWidth  = aWidth;
    iScreenHeight = aHeight;
    iInputHandler = aInputHandler;
    iIsAutorotateOn = ETrue;
	}

// -----------------------------------------------------------------------------
// CBillboard::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CBillboard::ConstructL( void )
	{
	}

// ---------------------------------------------------------
// CBillboard::OnEnterState( TInt aState )
// Called by TFiniteStateMachine when the f.s.m enters a new state
// ---------------------------------------------------------

void CBillboard::OnEnterStateL( TInt /*aState*/ )
	{
	}

// -------------------------------------------------------------------------------------------------------
// CBillboard::OnStartLoadingTextures()
// Called for a MTextureLoadingListener by the texture manager when texture loading operation starts
// -------------------------------------------------------------------------------------------------------

void CBillboard::OnStartLoadingTexturesL()
	{
    SetStateL( ELoadingTextures );
	}

// ------------------------------------------------------------------------------------------------------------
// CBillboard::OnEndLoadingTextures()
// Called for a MTextureLoadingListener by the texture manager when texture loading operation is completed
// ------------------------------------------------------------------------------------------------------------
void CBillboard::OnEndLoadingTexturesL()
	{
	if ( GetState() == ELoadingTextures )
		{
		SetStateL( ERunning );
		}
	}

// -----------------------------------------------------------------------------
// CBillboard::SetScreenSize
// Reacts to the dynamic screen size change during execution of this program.
// -----------------------------------------------------------------------------
//
void CBillboard::SetScreenSize( TUint aWidth, TUint aHeight )
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

// ------------------------------------------------------------------------------------------------------------
// CBillboard::TurnCamera()
// Turn the camera in azimuth.
// ------------------------------------------------------------------------------------------------------------
void CBillboard::TurnCamera( GLfixed aDeltaTimeSecs )
	{
	if( iIsAutorotateOn )
	    {
        // Autorotation is on: rotate the camera at constant speed
        iAzimuth += fixedMul( KCamRotationSpeed, aDeltaTimeSecs );
        }
    // Autorotation is off: read joystick left/right movement and rotate the camera
    if( iInputHandler->IsInputPressed( EJoystickLeft ) )
        {
        iIsAutorotateOn = EFalse;
        iAzimuth += fixedMul( KCamRotationSpeed, aDeltaTimeSecs );
        }
    else if( iInputHandler->IsInputPressed( EJoystickRight ) )
        {
        iIsAutorotateOn = EFalse;
        iAzimuth -= fixedMul( KCamRotationSpeed, aDeltaTimeSecs );
        }
    else if( iInputHandler->IsInputPressed( EJoystickFire ) )
        {
        // User pressed joystick button, turn autorotation on
        iIsAutorotateOn = ETrue;
        }


     // Keep the azimuth within range of [0, 359]
    while( iAzimuth < INT_2_FIXED( 0 ) )
        {
        iAzimuth += INT_2_FIXED( 360 );
        }

    while( iAzimuth >= INT_2_FIXED( 360 ) )
        {
        iAzimuth -= INT_2_FIXED( 360 );
        }
	}

// ------------------------------------------------------------------------------------------------------------
// CBillboard::MoveCamera()
// Move the camera forwards or backwards.
// ------------------------------------------------------------------------------------------------------------
void CBillboard::MoveCamera( GLfixed aDeltaTimeSecs )
	{
    if( iInputHandler->IsInputPressed( EJoystickUp ) )
        {
        iDistance -= fixedMul( KCamMovementSpeed, aDeltaTimeSecs );
    	if ( iDistance < KMinCamDistance )
    		{
    		iDistance = KMinCamDistance;
    		}
        }
    if( iInputHandler->IsInputPressed( EJoystickDown ) )
        {
        iDistance += fixedMul( KCamMovementSpeed, aDeltaTimeSecs );
    	if ( iDistance > KMaxCamDistance )
    		{
    		iDistance = KMaxCamDistance;
    		}
        }
	}

