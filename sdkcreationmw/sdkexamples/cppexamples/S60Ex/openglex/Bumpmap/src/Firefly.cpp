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

#include <e32cmn.h>  // Abs, Min and Max
#include "firefly.h"

// CONSTANTS

/** Vertices for drawing the glow of the firefly. */
static const GLfloat KGlowVerticeData[]=
    {
    -10.f,  10.f, 0.f,
     10.f,  10.f, 0.f,
     10.f, -10.f, 0.f,
    -10.f, -10.f, 0.f
    };

/** Texture coordinates for drawing the glow of the firefly. */
static const GLfloat KGlowTexCoordsData[] =
	{
	0.f,  1.f,
	1.f,  1.f,
	1.f,  0.f,
	0.f,  0.f
	};

/** Indices for drawing the glow of the firefly. */
static const GLubyte KGlowIndices[] =
	{
	0, 3, 1, 1, 3, 2
	};

/** Vertices for drawing the firefly itself. */
static const GLfloat KFlyVerticeData[] =
	{
	-.3f,  0.f, 0.f,
	 .3f,  0.f, 0.f,
	 0.f, -.3f, 0.f,
	 0.f,  .3f, 0.f
	};

// ============================= MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CFireFly::NewL
// Symbian 2-phase constructor. Can leave.
// -----------------------------------------------------------------------------
//
CFireFly* CFireFly::NewL( GLfloat aLeft, GLfloat aRight, GLfloat aBottom, GLfloat aTop,
                          GLfloat aMinZCoord, GLfloat aMaxZCoord, CTextureManager* aTexManager )
	{
    // Symbian 2-phase constructor. Calls both the default
    // C++ constructor and Symbian ConstructL methods
    CFireFly* self = new (ELeave) CFireFly( aLeft, aRight, aBottom, aTop, aMinZCoord, aMaxZCoord );

    CleanupStack::PushL( self );
    self->ConstructL( aTexManager );
    CleanupStack::Pop();

	return self;
	}

// -----------------------------------------------------------------------------
// CFireFly::CFireFly
// C++ constructor
// -----------------------------------------------------------------------------
//
CFireFly::CFireFly( GLfloat aLeft, GLfloat aRight, GLfloat aBottom, GLfloat aTop, GLfloat aMinZCoord, GLfloat aMaxZCoord )
	{
	iLeft        = aLeft;
	iRight       = aRight;
	iBottom      = aBottom;
	iTop         = aTop;
	iMinZ        = aMinZCoord;
	iMaxZ        = aMaxZCoord;
	iPosition.iZ = aMinZCoord;
	iTarget.iZ   = aMinZCoord;
	}

// ----------------------------------------------------------------------
// CFireFly::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------
//
void CFireFly::ConstructL( CTextureManager* aTexManager )
	{
	TTime now;
	iSeed = now.Int64();

    // Add required textures to the texture manager's loading queue
	_LIT( KFireFlyGlowName, "firefly.jpg" );
    aTexManager->RequestToLoad( KFireFlyGlowName, &iGlowTexture, false );
	}

// ---------------------------------------------------------
// CFireFly::~()
// Destructor.
// ---------------------------------------------------------
CFireFly::~CFireFly()
	{
	}

// ---------------------------------------------------------
// CFireFly::DoMovement()
// Calculates the movement of the firefly.
// ---------------------------------------------------------
void CFireFly::DoMovement( GLuint /*aFrame*/, GLfloat /*aTimeSecs*/, GLfloat aDeltaTimeSecs )
	{
    // Speed in x and y directions is based on the distance to the target
    iSpeed.iX = ( iTarget.iX - iPosition.iX ) / 40.f;
    iSpeed.iY = ( iTarget.iY - iPosition.iY ) / 40.f;
    iSpeed.iZ = ( iTarget.iZ - iPosition.iZ ) / 40.f;

    // Make sure the minimum speed is 0.4 in every direction
    if( iSpeed.iX != 0.f )
        {
        iSpeed.iX = iSpeed.iX > 0.f ? Max( iSpeed.iX, 0.4f ) : Min( iSpeed.iX, -0.4f );

        }
    if( iSpeed.iY != 0.f )
        {
        iSpeed.iY = iSpeed.iY > 0.f ? Max( iSpeed.iY, 0.4f ) : Min( iSpeed.iY, -0.4f );
        }
    if( iSpeed.iZ != 0.f )
        {
        iSpeed.iZ = iSpeed.iZ > 0.f ? Max( iSpeed.iZ, 0.4f ) : Min( iSpeed.iZ, -0.4f );
        }


    iPosition += ( iSpeed * ( aDeltaTimeSecs * 40.f ) );

    // Check that we don't overshoot the target
    if ( ( iSpeed.iX < 0 &&  iPosition.iX < iTarget.iX ) || ( iSpeed.iX > 0 &&  iPosition.iX > iTarget.iX ) )
        {
        iPosition.iX = iTarget.iX;
        iSpeed.iX    = 0.0f;
        }

    if ( ( iSpeed.iY < 0 &&  iPosition.iY < iTarget.iY ) || ( iSpeed.iY > 0 &&  iPosition.iY > iTarget.iY ) )
        {
        iPosition.iY = iTarget.iY;
        iSpeed.iY    = 0.0f;
        }

    if ( ( iSpeed.iZ < 0 &&  iPosition.iZ < iTarget.iZ ) || ( iSpeed.iZ > 0 &&  iPosition.iZ > iTarget.iZ ) )
        {
        iPosition.iZ = iTarget.iZ;
        iSpeed.iZ    = 0.0f;
        }

    // Check if the fly has reached it's current target,
    // if it has generate a new target
    if( iPosition.iX == iTarget.iX && iPosition.iY == iTarget.iY && iPosition.iZ == iTarget.iZ )
		{
		iTarget.iX = randrange( iLeft, iRight, iSeed );
		iTarget.iY = randrange( iBottom, iTop, iSeed );
		iTarget.iZ = randrange( iMinZ, iMaxZ, iSeed );
		}
	}


// ---------------------------------------------------------
// CFireFly::Draw()
// Draws the firefly at it's current position.
// ---------------------------------------------------------
void CFireFly::Draw()
	{
    // Colour and normal arrays are not needed to draw the firefly
    glDisableClientState( GL_COLOR_ARRAY  );
    glDisableClientState( GL_NORMAL_ARRAY );

    // Vertex and texture coordinate arrays are needed
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	glPushMatrix();

    // Translate all drawing to the current location of the firefly
    glTranslatef( iPosition.iX, iPosition.iY, iPosition.iZ );

    // To enhance the illusion of the fly flying towards and away from the viewer
    // we scale the size based on the distance of the fly from the viewer.
    GLfloat scale = ( iPosition.iZ - iMinZ ) / ( iMaxZ - iMinZ );
    scale += 0.2f;
    glScalef( scale, scale, scale );

    // Setup colour and normal to be used when drawing the firefly
    glColor4f(  0.f, 0.93f, 0.87f, 1.f );
    glNormal3f( 0.f, 0.f, 1.f );

    // Use texture unit 0 to render the glow
    glClientActiveTexture(GL_TEXTURE0);
    glActiveTexture( GL_TEXTURE0 );
    glEnable( GL_TEXTURE_2D );

    // Set blending function translucency and enable blending
    glBindTexture( GL_TEXTURE_2D,  iGlowTexture.iID );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR );

    // First draw the "halo" for the firefly
    glVertexPointer(   3, GL_FLOAT, 0, KGlowVerticeData );
    glTexCoordPointer( 2, GL_FLOAT, 0, KGlowTexCoordsData );
    glDrawElements(   GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, KGlowIndices );

    // Disable texturing and blending before drawing the firefly itself
    glBlendFunc( GL_SRC_ALPHA,GL_ONE );
    glDisable( GL_TEXTURE_2D );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );

    // Draw the firefly as a dot
    glColor4f( 1.f, 1.f, 1.f, 1.f );
    glVertexPointer( 3, GL_FLOAT, 0, KFlyVerticeData );
    glDrawArrays( GL_LINES, 0, 4 );

	glPopMatrix();

    // Restore states and translation
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
	}
