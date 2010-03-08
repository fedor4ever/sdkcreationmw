/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "lensflare.h"
#include <aknnotewrappers.h>
#include <e32math.h>

#include "model.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS
#define scl 1.0

// LOCAL CONSTANTS AND MACROS

/* Lens flare configuration */
static const TFlareConfig LensFlareConfigs[] =
	{
    { 0, (GLfloat)(+1.2 / 01.0), (GLfloat)(0.15 / scl) },
    { 0, (GLfloat)(+1.0 / 01.0), (GLfloat)(1.00 / scl) },
    { 2, (GLfloat)(+1.0 / 01.5), (GLfloat)(0.50 / scl) },
    { 1, (GLfloat)(+1.0 / 01.6), (GLfloat)(0.25 / scl) },
    { 2, (GLfloat)(+1.0 / 02.0), (GLfloat)(0.50 / scl) },
    { 0, (GLfloat)(+1.0 / 02.3), (GLfloat)(0.25 / scl) },
    { 1, (GLfloat)(+1.0 / 17.0), (GLfloat)(0.35 / scl) },
    { 2, (GLfloat)(+1.0 / 09.0), (GLfloat)(0.20 / scl) },
    { 2, (GLfloat)(-1.0 / 02.0), (GLfloat)(0.50 / scl) },
    { 1, (GLfloat)(-1.0 / 04.0), (GLfloat)(0.25 / scl) },
    { 1, (GLfloat)(-1.0 / 05.0), (GLfloat)(0.45 / scl) }
	};


/* Length of LensFlareConfigs */
static const TUint8 LensFlareConfigCount = sizeof(LensFlareConfigs)/sizeof(LensFlareConfigs[0]);

/* Define vertice coordinates for the ground plane */
static const GLbyte planeVertices[4 * 3] =
	{
    +1,0,-1,
    +1,0,+1,
    -1,0,+1,
    -1,0,-1
	};

/* Define indices for drawing the triangles of the ground plane */
static const GLubyte planeTriangles[2 * 3] =
	{
    0,2,1,
    0,3,2
	};

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CLensFlare::NewL
// Two-phased constructor. Can leave.
// -----------------------------------------------------------------------------
//
CLensFlare* CLensFlare::NewL( TUint aWidth, TUint aHeight )
	{
    /* Symbian 2-phase constructor. Calls both the default
       C++ constructor and Symbian ConstructL methods */
    CLensFlare* self = new (ELeave) CLensFlare( aWidth, aHeight );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
	}

/**
 * Destructor.
 */
CLensFlare::~CLensFlare()
	{
	}

// -----------------------------------------------------------------------------
// CLensFlare::CLensFlare
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLensFlare::CLensFlare( TUint aWidth, TUint aHeight )
	: CFiniteStateMachine()
	{
    iScreenWidth  = aWidth;
    iScreenHeight = aHeight;

	}

// -----------------------------------------------------------------------------
// CLensFlare::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLensFlare::ConstructL( void )
	{
	}

// -----------------------------------------------------------------------------
// CLensFlare::AppInit
// Initializes the application.
// -----------------------------------------------------------------------------
//
void CLensFlare::AppInitL( void )
	{
    // Construct a texture manager that uses the application's private
    // directory as the location for all textures.
    iTextureManager = CTextureManager::NewL ( iScreenWidth, iScreenHeight,
	                                          FRUSTUM_TOP, FRUSTUM_BOTTOM, FRUSTUM_RIGHT, FRUSTUM_LEFT, FRUSTUM_NEAR,
	                                          this );

    // Initialize viewport and projection.
    SetScreenSize( iScreenWidth, iScreenHeight );

    /* Set the screen background color. */
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

    /* Enable back face culling. */
    glEnable( GL_CULL_FACE  );
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_DEPTH_TEST );

    /* Disable lighting */
    glDisable( GL_LIGHTING );

    /* In this example, speed is  more important than quality */
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );

    /* Enable vertex arrays. */
    glEnableClientState( GL_VERTEX_ARRAY );
    /* Enable texture arrays. */
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    glTexEnvx( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

    iDistance = 250;
    iElevation = 8;
    iAzimuth = 90;
    iDirection = +1;

    iLookUpTable = CLookUpTable::NewL( CLookUpTable::ESin | CLookUpTable::ECos );

    TBufC<MAX_TEXTURE_NAME> LensFlareTextureNames[4];

    //Set of textures that will be used by the lens flare effect.
    _LIT(KLens1, "lens1.jpg");
    _LIT(KLens2, "lens2.jpg");
    _LIT(KLens3, "lens3.jpg");
    LensFlareTextureNames[0] = KLens1;
    LensFlareTextureNames[1] = KLens2;
    LensFlareTextureNames[2] = KLens3;

    iLensFlareEffect = CLensFlareEffect::NewL(  LensFlareTextureNames, 3,
                                                LensFlareConfigs, LensFlareConfigCount,
                                                iTextureManager,
                                                iScreenWidth, iScreenHeight);

    //Start to load the textures.
    iTextureManager->DoLoadL();
	}

// -----------------------------------------------------------------------------
// CLensFlare::AppExit
// Release any allocations made in AppInit.
// -----------------------------------------------------------------------------
//
void CLensFlare::AppExit( void )
	{
    delete iTextureManager;
    delete iLensFlareEffect;
    delete iLookUpTable;
	}


// -----------------------------------------------------------------------------
// CLensFlare::DrawText
// -----------------------------------------------------------------------------
//
void CLensFlare::DrawText(TBool aReflection)
	{
    glDisable( GL_DEPTH_TEST );

    glLoadIdentity();

    T3DModel::MakeWorldViewMatrix( iCamera, TVector(0.0f, 0.0f, 0.0f));

    if ( aReflection )
        {
        glScalef( 1.0f , -1.0f, 1.0f);// reflect across Y=0 plane.
        }
    /* Scale the model to fit the screen. */
    glScalef( 0.01f, 0.01f, 0.01f );

    glDisable( GL_TEXTURE_2D );
    glColor4f( 0, 0, 1.f, 1.0f ); // Blue (nokia's logo color)
    glVertexPointer( 3, GL_SHORT, 0, TextVertices );
    glDrawElements( GL_TRIANGLES, TextIndicesCount, GL_UNSIGNED_SHORT, TextIndices );
    glEnable( GL_TEXTURE_2D );

    glEnable( GL_DEPTH_TEST );
	}

// -----------------------------------------------------------------------------
// CLensFlare::DrawPlane
// Draw the ground plane.
// -----------------------------------------------------------------------------
//
void CLensFlare::DrawPlane()
	{
    glLoadIdentity();
    T3DModel::MakeWorldViewMatrix(iCamera, TVector(0.0f, 0.0f, 0.0f));
    glScalef( 250, 0, 250 );

    //Enable alpha blending in order to blend the reflected text with the groud plane.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f( 0.7f, 0.0f, 0.0f, 0.70f ); // 70% of groud plane color - 30% of text color.

    glDisable( GL_TEXTURE_2D );
    glVertexPointer( 3, GL_BYTE, 0, planeVertices );
    glDrawElements( GL_TRIANGLES, 2 * 3, GL_UNSIGNED_BYTE, planeTriangles );
    glEnable( GL_TEXTURE_2D );

    glDisable(GL_BLEND);
	}

// -----------------------------------------------------------------------------
// CLensFlare::AppCycle
// Draws and animates the objects.
// -----------------------------------------------------------------------------
//
void CLensFlare::AppCycle( TInt /*aFrame*/, TReal /*aTimeSecs*/, TReal aDeltaTimeSecs )
	{

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    /* Move the camera. */
    TurnCameraAzimuth(iDirection, aDeltaTimeSecs);

    GLfloat sinA, cosA, sinE, cosE;

    sinE = iLookUpTable->Sin((GLushort)iElevation);
    cosE = iLookUpTable->Cos((GLushort)iElevation);
    sinA = iLookUpTable->Sin((GLushort)iAzimuth);
    cosA = iLookUpTable->Cos((GLushort)iAzimuth);

    GLfloat x = iDistance * cosE * cosA;
    GLfloat y = iDistance * sinE;
    GLfloat z = iDistance * cosE * sinA;

    iCamera.LookAt(TVector(x, y, z ), TVector(0.0f, 0.0f, 0.0f ), TVector(0.0f, 1.0f, 0.0f));

    /* Draw the gound plane and the text in the normal and the reflected form.*/
    DrawText(ETrue);
    DrawPlane();
    DrawText(EFalse);

    /* Animate and draw the lens flare. */
    iLensAngle += (20 * iDirection * aDeltaTimeSecs);
    if ( iLensAngle < 0 )
        {
        iLensAngle = 359;
        }
    else if ( iLensAngle > 359 )
        {
        iLensAngle = 0;
        }
    else
        {
        }

    GLfloat sin = iLookUpTable->Sin((GLushort)iLensAngle);
    GLfloat cos = iLookUpTable->Cos((GLushort)iLensAngle);

    iLensRadius = 50 * (iScreenWidth  / 176.0) * (sin > 0 ? sin : -sin);
    TReal xx = iLensRadius * cos + iScreenWidth / 2;
    TReal yy = iLensRadius * sin + iScreenHeight / 2;

    iLensFlareEffect->DrawAt((TInt)xx, (TInt)yy, iScreenWidth / 176.0);
	}

// ---------------------------------------------------------
// CLensFlare::TurnCameraAzimuth
// Turn the camera in azimuth.
// ---------------------------------------------------------
void CLensFlare::TurnCameraAzimuth(GLint aDirection, GLfloat aTime)
	{
	iAzimuth += (aDirection * 20 * aTime);
	if ( iAzimuth < 0 )
		{
		iAzimuth = 359;
		}
	else if ( iAzimuth > 359 )
		{
		iAzimuth = 0;
		}
	else
		{
		}
	}

// ---------------------------------------------------------
// CLensFlare::TurnCameraElevation
// Turn the camera in elevation.
// ---------------------------------------------------------
void CLensFlare::TurnCameraElevation(GLint aDirection, GLfloat aTime)
	{
	iElevation += ( aDirection * 20 * aTime);
	if ( iElevation < 5 )
		{
		iElevation = 5;
		}
	else if ( iElevation > 175 )
		{
		iElevation = 175;
		}
	else
		{
		}
	}


// -------------------------------------------------------------------------------------------------------
// CLensFlare::OnStartLoadingTextures()
// Called for a MTextureLoadingListener by the texture manager when texture loading operation starts
// -------------------------------------------------------------------------------------------------------
void CLensFlare::OnStartLoadingTexturesL()
	{
    SetStateL( ELoadingTextures );
	}

// ------------------------------------------------------------------------------------------------------------
// CLensFlare::OnEndLoadingTextures()
// Called for a MTextureLoadingListener by the texture manager when texture loading operation is completed
// ------------------------------------------------------------------------------------------------------------

void CLensFlare::OnEndLoadingTexturesL()
	{
	if ( GetState() == ELoadingTextures )
		{
		SetStateL( ERunning );
		}
	}

// -----------------------------------------------------------------------------
// CLensFlare::SetScreenSize
// Reacts to the dynamic screen size change during execution of this program.
// -----------------------------------------------------------------------------
//
void CLensFlare::SetScreenSize( TUint aWidth, TUint aHeight )
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

// =============================================================================
// CLensFlareEffect
// =============================================================================

// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLensFlareEffect::CLensFlareEffect
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLensFlareEffect::CLensFlareEffect()
	{
	};

// -----------------------------------------------------------------------------
// CLensFlareEffect::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
 void CLensFlareEffect::ConstructL(const TBufC<MAX_TEXTURE_NAME> * aTextureNames,
 	TUint8 aTextureCount,
	const TFlareConfig * aFlareConfigs,
	TUint8 aFlareConfigCount,
	CTextureManager * aTextureManager,
	GLuint aScreenWidth,
	GLuint aScreenHeight)
	{
	iTextures = new (ELeave) TTexture [aTextureCount];
	iTextureCount = aTextureCount;
	iFlareConfigs = aFlareConfigs;
	iFlareConfigCount = aFlareConfigCount;
	iTextureManager = aTextureManager;

	iCenterX = aScreenWidth>>1;
	iCenterY = aScreenHeight>>1;

	for ( TInt i = 0; i < iTextureCount; i++ )
		{
		iTextureManager->RequestToLoad(aTextureNames[i], &iTextures[i] );
		}
	}

// -----------------------------------------------------------------------------
// CLensFlareEffect::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLensFlareEffect* CLensFlareEffect::NewL(const TBufC<MAX_TEXTURE_NAME> * aTextureNames,
	TUint8 aTextureCount,
	const TFlareConfig * aFlareConfigs, TUint8 aFlareConfigCount,
	CTextureManager * aTextureManager,
	GLuint aScreenWidth, GLuint aScreenHeight )
	{
	CLensFlareEffect* self = new (ELeave) CLensFlareEffect;
	CleanupStack::PushL( self );
	self->ConstructL(aTextureNames, aTextureCount, aFlareConfigs,
		aFlareConfigCount,
		aTextureManager,
		aScreenWidth, aScreenHeight);
	CleanupStack::Pop();

	return self;
	}

// Destructor
CLensFlareEffect::~CLensFlareEffect()
	{
	delete[] iTextures;
	}


// -----------------------------------------------------------------------------
// CFiniteStateMachine::DrawAt
// Renders the lens flare effect at a given screen coordinates.
// Uses the CTextureManager::Blit, which in turn draws two triangles (forming
// a single quad)
// -----------------------------------------------------------------------------
//
void CLensFlareEffect::DrawAt(GLuint aLightX, GLuint aLightY, GLfloat aImageScale)
	{
	//Computing the lens flare vector.
	TInt DirX = aLightX - iCenterX;
	TInt DirY = aLightY - iCenterY;

	GLfloat Scale;
	GLfloat BlitCenterX, BlitCenterY;
	GLfloat BlitWidth_div_2, BlitHeight_div_2;

	glEnable(    GL_BLEND );
	glBlendFunc( GL_ONE, GL_ONE);
    glEnable(    GL_TEXTURE_2D );

	for ( TInt i = 0; i < iFlareConfigCount; i++ )
		{
		TInt TextureIndex = iFlareConfigs[i].iIndex;
		Scale = iFlareConfigs[i].iLengthScale;

		BlitCenterX = DirX*Scale+iCenterX;
		BlitCenterY = DirY*Scale+iCenterY;
		BlitWidth_div_2   = (iTextures[TextureIndex].iTextureWidth  *
			iFlareConfigs[i].iImageScale * aImageScale) / 4;
		BlitHeight_div_2  = (iTextures[TextureIndex].iTextureHeight *
			iFlareConfigs[i].iImageScale * aImageScale) / 4;

		iTextureManager->Blit(iTextures[TextureIndex],
			(GLuint)(BlitCenterX - BlitWidth_div_2),
			(GLuint)(BlitCenterY - BlitHeight_div_2),
			(GLuint)(BlitCenterX + BlitWidth_div_2),
			(GLuint)(BlitCenterY + BlitHeight_div_2));
		}

    glDisable( GL_TEXTURE_2D );
	glDisable( GL_BLEND );
	}
