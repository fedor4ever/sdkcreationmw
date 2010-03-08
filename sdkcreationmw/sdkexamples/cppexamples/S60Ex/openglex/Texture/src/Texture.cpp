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
#include "Texture.h"

// MACROS
#define LIGHT_MAX    (1 << 16)
#define LIGHTCOLOR(r, g, b, a)       \
       (GLfixed)(r * LIGHT_MAX),     \
       (GLfixed)(g * LIGHT_MAX),     \
       (GLfixed)(b * LIGHT_MAX),     \
       (GLfixed)(a * LIGHT_MAX)

/* Define global ambient light. */
static const GLfixed globalAmbient[4]      = { LIGHTCOLOR(0.0, 0.0, 0.0, 1.0) };

/* Define lamp parameters. */
static const GLfixed lightDiffuseLamp[4]   = { LIGHTCOLOR(1.0, 1.0, 1.0, 1.0) };
static const GLfixed lightAmbientLamp[4]   = { LIGHTCOLOR(0.3, 0.3, 0.3, 1.0) };
static const GLfixed lightSpecularLamp[4]  = { LIGHTCOLOR(0.5, 0.5, 0.5, 1.0) };
static const GLfixed lightPositionLamp[4]  = { 0, 0, 10, 0 };

/* Define vertice coordinates for the cube
   Duplication of vertices needed for texturing every surface of the cube */
static const GLbyte vertices[24 * 3] =
{
    /* top */
    -1,  1,  1,
    1,  1,  1,
    1, -1,  1,
    -1, -1,  1,

    /* front */
    1,  1,  1,
    1,  1, -1,
    1, -1, -1,
    1, -1,  1,

    /* right */
    -1,  1,  1,
    -1,  1, -1,
    1,  1, -1,
    1,  1,  1,

    /* left */
    1, -1,  1,
    1, -1, -1,
    -1, -1, -1,
    -1, -1,  1,

    /* back */
    -1, -1,  1,
    -1, -1, -1,
    -1,  1, -1,
    -1,  1,  1,

    /* bottom */
    -1,  1, -1,
    1,  1, -1,
    1, -1, -1,
    -1, -1, -1
};

/* Define normals for the cube */
static const GLbyte normals[24 * 3] =
{
    /* top */
    0,0,1,
    0,0,1,
    0,0,1,
    0,0,1,

    /* front */
    1,0,0,
    1,0,0,
    1,0,0,
    1,0,0,

    /* right */
    0,1,0,
    0,1,0,
    0,1,0,
    0,1,0,

    /* left */
    0,-1,0,
    0,-1,0,
    0,-1,0,
    0,-1,0,

    /* back */
    -1,0,0,
    -1,0,0,
    -1,0,0,
    -1,0,0,

    /* bottom */
    0,0,-1,
    0,0,-1,
    0,0,-1,
    0,0,-1
};

/* Define indices for drawing the triangles ( 5 sides of the cube )
   Nasa Hubble texture is drawn on these sides. */
static const GLubyte triangles[10 * 3] =
{
    /* top */
     1,0,3,
     1,3,2,

     /* front */
     5,4,7,
     5,7,6,

     /* right */
     9,8,11,
     9,11,10,

     /* left */
     13,12,15,
     13,15,14,

     /* back */
     17,16,19,
     17,19,18
};


/* The 6th side of the cube.
   ogles.jpg texture is drawn on this side. */
static const GLubyte triangles2[2 * 3] =
{
    /* bottom */
    21,22,23,
    21,23,20
};


/* Macro for changing the input texture coordinate values from
   GLubyte [0,255] to GLbyte [-128,127]. See more info below. */
#define tex(u,v) (GLbyte)( (u) - 128 ) , (GLbyte)( (v) - 128 )


/* Input texture coordinates for each of the object vertices.
   The coordinates are given in GLbyte [-128,127] format.
   Since the texture picture coordinates are between values
   [0,255] for both width and height, we have defined a macro
   to change the input coordinates into a appropriate form.
   It is easier to think the texture coordinates as corresponding
   image pixel coordinates. This alone is not enough because
   if texture coordinates are not given between values [0,1],
   texture wrapping will occur. Therefore we need to
   scale the texture coordinates with appropriate texture matrix,
   which is defined in AppInit(). */
static const GLbyte nokTexCoords[24 * 2] =
{
    /* top, whole texture nasa_hubble.h */
    tex(0,0),
    tex(255,0),
    tex(255,255),
    tex(0,255),

    /* front, spiral with tail */
    tex(0,255),
    tex(127,255),
    tex(127,127),
    tex(0,127),

    /* right, red nebulae */
    tex(127,255),
    tex(255,255),
    tex(255,127),
    tex(127,127),

    /* left, plasma cloud */
    tex(0,127),
    tex(127,127),
    tex(127,0),
    tex(0,0),

    /* back, 2 spirals */
    tex(127,127),
    tex(255,127),
    tex(255,0),
    tex(127,0),

    /* bottom, whole texture ogles.jpg */
    tex(255,255),
    tex(255,0),
    tex(0,0),
    tex(0,255)
};


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTexture::CTexture
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTexture::CTexture( TUint aWidth, TUint aHeight ): CFiniteStateMachine()
{
    iScreenWidth  = aWidth;
    iScreenHeight = aHeight;
}


// -----------------------------------------------------------------------------
// CTexture::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTexture::ConstructL( void )
{
}


// -----------------------------------------------------------------------------
// CTexture::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTexture* CTexture::NewL( TUint aWidth, TUint aHeight )
{
    /* Symbian 2-phase constructor. Calls both the default
       C++ constructor and Symbian ConstructL methods */
    CTexture* self = new (ELeave) CTexture( aWidth, aHeight );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}


// Destructor
CTexture::~CTexture()
{
}


// -----------------------------------------------------------------------------
// CTexture::AppInit
// OpenGL ES initialization code. Set the required OpenGL ES state
// -----------------------------------------------------------------------------
//
void CTexture::AppInitL()
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

    /* Enable back face culling, texturing, and normalization. */
    glEnable( GL_CULL_FACE  );
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_NORMALIZE  );

    /* Initialize appropriate texture matrix. First we have to translate the
       input texture coordinate values to be within a range of [0,255]. Hence
       we translate the x- and y-coordinate values by 128. Recall that the
       values in nokTexCoords are between [-128,127], now they are in a range
       of [0,255]. After that we scale the values by 1/255 to make the values
       to be in range [0,1]. */
    glMatrixMode( GL_TEXTURE );
    glLoadIdentity();
    glScalef(     1.0f/255.0f, 1.0f/255.0f, 1.0f );
    glTranslatef( 128.0f,      128.0f,      0.0f );

    /* Remember to change the matrix mode to modelview. */
    glMatrixMode( GL_MODELVIEW );

    /* Enable vertex and texture arrays. */
    glEnableClientState( GL_VERTEX_ARRAY        );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY        );

    /* Set array pointers. */
    glVertexPointer(   3, GL_BYTE, 0, vertices     );
    glTexCoordPointer( 2, GL_BYTE, 0, nokTexCoords );
    glNormalPointer(   GL_BYTE, 0, normals         );

    /* Set up global ambient light. */
    glLightModelxv( GL_LIGHT_MODEL_AMBIENT, globalAmbient );

    /* Set up lamp. */
    glEnable(   GL_LIGHT0 );
    glLightxv( GL_LIGHT0, GL_DIFFUSE,  lightDiffuseLamp  );
    glLightxv( GL_LIGHT0, GL_AMBIENT,  lightAmbientLamp  );
    glLightxv( GL_LIGHT0, GL_SPECULAR, lightSpecularLamp );
    glLightxv( GL_LIGHT0, GL_POSITION, lightPositionLamp );

    /* Set shading mode*/
    glShadeModel( GL_FLAT );

    /* Set the blending function for translucency. */
    glBlendFunc( GL_ONE, GL_ONE );

    /* Do not use perspective correction, can be enabled from the menu */
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );

    /* Symbian menu item initialization. See CTextureAppUi.cpp for more. */
    iPerspectiveCorrEnabled = EFalse; // no texture perspective correction
    iTextureWrapEnabled     = EFalse; // no texture wrapping
    iLightingEnabled        = EFalse; // no lighting
    iBlendingEnabled        = EFalse; // no blending

    // Pushing the textures into the loading queue.
    _LIT(KHubbleTexture, "nasa_hubble.jpg");
    _LIT(KOGLESTexture,  "ogles.jpg");
    iTextureManager->RequestToLoad(KHubbleTexture, &iNasaHubble );
    iTextureManager->RequestToLoad(KOGLESTexture,  &iOpenGLES );

    //Start to load the textures.
    iTextureManager->DoLoadL();
}


// -----------------------------------------------------------------------------
// CTexture::AppExit
// Relase any allocations made in AppInit
// -----------------------------------------------------------------------------
//
void CTexture::AppExit( void )
{
    delete iTextureManager;
}


// -----------------------------------------------------------------------------
// CTexture::AppCycle
// OpenGL ES main rendering loop.
// -----------------------------------------------------------------------------
//
void CTexture::AppCycle( TInt aFrame )
{
    glClear( GL_COLOR_BUFFER_BIT );

    /* Animate and draw the cube. */
    glLoadIdentity();
    gluLookAt( 0.f, 0.f, 120.f,     // Camera position
               0.f, 0.f,   0.f,     // Look at point
               0.f, 1.f,   0.f );   // Up direction


    glRotatex( aFrame << 16, 1 << 16, 0      , 0       );
    glRotatex( aFrame << 15, 0      , 1 << 16, 0       );
    glRotatex( aFrame << 14, 0      , 0      , 1 << 16 );

    /* Scale the cube coordinates to fit the screen. */
    glScalex( 20 << 16, 20 << 16, 20 << 16 );

    /* Draw the first 5 sides of the cube with the texture Nasa Hubble. */
    glBindTexture(  GL_TEXTURE_2D, iNasaHubble.iID );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glDrawElements( GL_TRIANGLES, 10 * 3, GL_UNSIGNED_BYTE, triangles );

    /* Draw the remaining side of the cube with the ogles.jpg texture. */
    glBindTexture(  GL_TEXTURE_2D, iOpenGLES.iID );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glDrawElements( GL_TRIANGLES, 2 * 3, GL_UNSIGNED_BYTE, triangles2 );
}

// -------------------------------------------------------------------------------------------------------
// CBillboard::OnStartLoadingTextures()
// Called for a MTextureLoadingListener by the texture manager when texture loading operation starts
// -------------------------------------------------------------------------------------------------------

void CTexture::OnStartLoadingTexturesL()
	{
    SetStateL( ELoadingTextures );
	}

// ------------------------------------------------------------------------------------------------------------
// CBillboard::OnEndLoadingTextures()
// Called for a MTextureLoadingListener by the texture manager when texture loading operation is completed
// ------------------------------------------------------------------------------------------------------------
void CTexture::OnEndLoadingTexturesL()
	{
	if ( GetState() == ELoadingTextures )
		{
		SetStateL( ERunning );
		}
	}

// ---------------------------------------------------------
// CBillboard::OnEnterState( TInt aState )
// Called by TFiniteStateMachine when the f.s.m enters a new state
// ---------------------------------------------------------

void CTexture::OnEnterStateL( TInt /*aState*/ )
	{
	    // Nothing to do here...
	}

//----------------------------------------------------------
// The following methods are called by the CTextureAppUi
// class when handling the incoming menu events.
//----------------------------------------------------------


// -----------------------------------------------------------------------------
// CTexture::TogglePerspective
// User defines whether or not to use perspective correction when rendering
// -----------------------------------------------------------------------------
//
void CTexture::TogglePerspective( void )
{
    if( iPerspectiveCorrEnabled)
    {
    	iPerspectiveCorrEnabled = EFalse;
    	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );
    }
    else
    {
    	iPerspectiveCorrEnabled = ETrue;
    	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
    }
}

/**
 * Enable/Disable texture wrapping.
 */
// -----------------------------------------------------------------------------
// CTexture::ToggleWrapping
// Implement texture wrapping by changing the texture matrix scale
// -----------------------------------------------------------------------------
//
void CTexture::ToggleWrapping( void )
{
    if( iTextureWrapEnabled )
    {
    	iTextureWrapEnabled = EFalse;
    	glMatrixMode( GL_TEXTURE );
    	glLoadIdentity();
    	glScalef( 1.0f/255.0f, 1.0f/255.0f, 1.0f );
    	glTranslatef( 128.0f, 128.0f, 1.0f );
    	glMatrixMode( GL_MODELVIEW );
    }
    /* Texture wrapping is implemented here simply by changing the scale of
       texture matrix. Now the coordinate values are between [0,2], which
       causes the texture to wrap one time. */
    else
    {
    	iTextureWrapEnabled = ETrue;
    	glMatrixMode( GL_TEXTURE );
    	glLoadIdentity();
    	glScalef( 1.0f/128.0f, 1.0f/128.0f, 1.0f );
    	glTranslatef( 128.0f, 128.0f, 1.0f );
    	glMatrixMode( GL_MODELVIEW );
    }
}

/**
 * Enable/Disable lighting.
 */
// -----------------------------------------------------------------------------
// CTexture::ToggleLighting
// When using textures with lights the texenv must be set to GL_MODULATE,
// and shade model must be GL_SMOOTH.
// -----------------------------------------------------------------------------
//
void CTexture::ToggleLighting( void )
{
    // If texture loading is still going on, return from this method without doing anything.
	if ( GetState() == ELoadingTextures )
    {
        return;
    }

    if( iLightingEnabled )
    {
    	iLightingEnabled = EFalse;
    	glShadeModel( GL_FLAT );  // Don't need smooth shading when no lights
    	glDisable( GL_LIGHTING ); // Disable lighting

    	/* Set both textures environments to GL_REPLACE. */
    	glBindTexture( GL_TEXTURE_2D, iNasaHubble.iID );
    	glTexEnvx(  GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    	glBindTexture( GL_TEXTURE_2D, iOpenGLES.iID );
    	glTexEnvx(  GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    }
    else
    {
    	iLightingEnabled = ETrue;
    	glShadeModel( GL_SMOOTH ); // For the lighting to work properly
    	glEnable( GL_LIGHTING );   // Enable lighting

    	/* Set both textures environments to GL_MODULATE. */
    	glBindTexture( GL_TEXTURE_2D, iNasaHubble.iID );
    	glTexEnvx(  GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    	glBindTexture( GL_TEXTURE_2D, iOpenGLES.iID );
    	glTexEnvx(  GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    }
}


// -----------------------------------------------------------------------------
// CTexture::ToggleBlending
// User defines whether or not blending is enabled from the menu.
// -----------------------------------------------------------------------------
//
void CTexture::ToggleBlending( void )
{
    if( iBlendingEnabled )
    {
        // Blending was enabled, disable blending
        iBlendingEnabled = EFalse;
        glDisable( GL_BLEND );
        glEnable( GL_CULL_FACE  );
    }
    else
    {
        // Blending was disabled, enable blending
        iBlendingEnabled = ETrue;
        glEnable( GL_BLEND );
        glDisable( GL_CULL_FACE  );
    }
}

// -----------------------------------------------------------------------------
// CTexture::SetScreenSize
// Reacts to the dynamic screen size change during execution of this program.
// -----------------------------------------------------------------------------
//
void CTexture::SetScreenSize( TUint aWidth, TUint aHeight )
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
