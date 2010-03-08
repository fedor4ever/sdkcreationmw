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
#include "MultiTex.h"
#include <e32std.h>
#include <e32math.h>
#include "MultiTexContainer.h"  // For accessing the contexts etc.
#include "model.h"

// CONSTANTS

/* Define vertice coordinates for the triangles */
static const GLbyte LightVertices[8 * 3] =
{
    -10,  10,  0,
    -10, -10,  0,
     10,  10,  0,
     10, -10,  0,

    -10,  10,  0,
    -10, -10,  0,
     10,  10,  0,
     10, -10,  0
};


/* Macro for changing the input texture coordinate values from
   GLubyte [0,255] to GLbyte [-128,127]. See more info below. */
#define tex(u,v) (GLbyte)( (u) - 128 ) , (GLbyte)( (v) - 128 )

static const GLbyte LightTexCoords[8 * 2] =
{
    // Red light
    tex(0,0),
    tex(0,255),
    tex(127,0),
    tex(127,255),

    // Green light
    tex(127,0),
    tex(127,255),
    tex(255,0),
    tex(255,255)
};



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMultiTex::NewL
//
// The usual Symbian NewL implementation.
// Creates the object, pushes it to cleanup stack and calls ContructL.
// Returns the contructed object or leaves.
// -----------------------------------------------------------------------------
//

CMultiTex* CMultiTex::NewL( TUint aWidth, TUint aHeight )
{
    /* Symbian 2-phase constructor. Calls both the default
       C++ constructor and Symbian ConstructL methods */
    CMultiTex* self = new (ELeave) CMultiTex( aWidth, aHeight );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}

// -----------------------------------------------------------------------------
// CMultiTex::CMultiTex
//
// C++ constructor. Initializes the size instance variables from arguments.
// -----------------------------------------------------------------------------
//

CMultiTex::CMultiTex( TUint aWidth, TUint aHeight ) :
    CFiniteStateMachine(),
    iScreenWidth( aWidth ),
    iScreenHeight( aHeight )
{
    if ( iScreenHeight != 0 )
    {
        iAspectRatio = (GLfloat)iScreenWidth / (GLfloat)iScreenHeight;
    }
    else
    {
        iAspectRatio = 1.0;
    }
}

// -----------------------------------------------------------------------------
// CMultiTex::~CMultiTex
//
// C++ destructor.
// -----------------------------------------------------------------------------
//

CMultiTex::~CMultiTex()
{
}

// -----------------------------------------------------------------------------
// CMultiTex::ConstructL
//
// 2nd phase constructor.
// -----------------------------------------------------------------------------
//

void CMultiTex::ConstructL( void )
{
}

// -----------------------------------------------------------------------------
// CMultiTex::CalculateEnvLightsL
//
// Texture map consist of two lights: red and green.
// Alpha is calculated so that is gradually goes from 255 to zero starting
// from the center of the spotlight.
// -----------------------------------------------------------------------------
//
void CMultiTex::CalculateLightsL( void )
{
    // Reserve memory for light texture
    iLightTex = new (ELeave) TUint8[64*32*4];
    if ( !iLightTex )
    {
        User::LeaveNoMemory();
    }

    TInt d         = 0;   // distance from the center of the spot
    GLfloat alpha = 0.f;  // alpha value

    for ( TInt i = 0; i < 32; i++ )
    {
        for ( TInt j = 0; j < 32; j++ )
        {
            // distance from the center of spotlight
            d = (i-16)*(i-16) + (j-16)*(j-16);

            // simple linear attenuation used for alpha
            alpha = 255.f*(1.f-(1.f/250.f)*(GLfloat)(d));

            // alpha < 0 corresponds to spotlight cut-off value
            if ( alpha < 0.f )
            {
                iLightTex[64*4*i+4*j]   = 255;      //r
                iLightTex[64*4*i+4*j+1] = 0;        //g
                iLightTex[64*4*i+4*j+2] = 0;        //b
                iLightTex[64*4*i+4*j+3] = 0;        //a

                iLightTex[64*4*i+4*j+32*4]   = 0;   //r
                iLightTex[64*4*i+4*j+1+32*4] = 255; //g
                iLightTex[64*4*i+4*j+2+32*4] = 0;   //b
                iLightTex[64*4*i+4*j+3+32*4] = 0;   //a
            }
            // inside the cut-off, set alpha as calculated above/
            else
            {
                iLightTex[64*4*i+4*j]   = 255;                   //r
                iLightTex[64*4*i+4*j+1] = 0;                     //g
                iLightTex[64*4*i+4*j+2] = 0;                     //b
                iLightTex[64*4*i+4*j+3] = (GLubyte)(alpha);      //a

                iLightTex[64*4*i+4*j+32*4]   = 0;                //r
                iLightTex[64*4*i+4*j+1+32*4] = 255;              //g
                iLightTex[64*4*i+4*j+2+32*4] = 0;                //b
                iLightTex[64*4*i+4*j+3+32*4] = (GLubyte)(alpha); //a
            }
        }
    }
}


// -----------------------------------------------------------------------------
// CMultiTex::AppInit
//
// Initializes OpenGL ES, sets the vertex and color arrays and pointers,
// and selects the shading mode.
// -----------------------------------------------------------------------------
//

void CMultiTex::AppInitL( TAny* aInstance )
{

    CMultiTexContainer* instance = (CMultiTexContainer*) aInstance;

    /*=========================================================================*/
    /*===================== Initialize Pbuffer context ========================*/
    /*=========================================================================*/

    /* Bind the pbuffer context to the current rendering thread and surface.*/
    eglMakeCurrent( instance->iEglDisplay, instance->iEglPbufferSurface,
                    instance->iEglPbufferSurface, instance->iEglPbufferContext );

    /* Create light texture map */
    CalculateLightsL();
    const GLvoid *texPtr = iLightTex;

    // Set the screen background color (black).
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    // Initialize viewport and projection.
    glViewport( 0, 0, 128, 128 );
    glMatrixMode( GL_PROJECTION );
    glFrustumf( -1.f, 1.f, -1.f, 1.f, 3.f, 1000.f );

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

    // Remember to change the matrix mode to modelview.
    glMatrixMode( GL_MODELVIEW );

    // Enable back face culling, texturing and blending.
    glEnable( GL_CULL_FACE  );
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE );

    // Enable vertex arrays.
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    // Set array pointers.
    glVertexPointer( 3, GL_BYTE, 0, LightVertices );
    glTexCoordPointer( 2, GL_BYTE, 0, LightTexCoords );

    // Set the initial shading mode
    glShadeModel( GL_FLAT );

    /* Create texture names */
    glGenTextures( 2, iLightTexObjects );

    // Bind and set the light texture.
    glBindTexture( GL_TEXTURE_2D, iLightTexObjects[0] );
    glTexImage2D(  GL_TEXTURE_2D, 0, GL_RGBA, 64, 32,
                   0, GL_RGBA, GL_UNSIGNED_BYTE, texPtr );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexEnvx( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD );

    /* We can now free the memory reserved for light texture,
       since it's loaded inside OpenGL ES */
    delete[] iLightTex;
    iLightTex = NULL;

    /* Bind and set the environment map texture. We pass NULL as pixels,
       since we use glCopyTexImage2D later to initialize the texture memory. */
    glBindTexture( GL_TEXTURE_2D, iLightTexObjects[1] );
    glTexImage2D(  GL_TEXTURE_2D, 0, GL_RGB, 128, 128,
                   0, GL_RGB, GL_UNSIGNED_BYTE, NULL );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexEnvx( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

    // Do not use perspective correction
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );


    /*=========================================================================*/
    /*====================== Initialize Window context ========================*/
    /*=========================================================================*/


    // Bind the Window context to the current rendering thread and surface.
    eglMakeCurrent( instance->iEglDisplay, instance->iEglSurface,
                    instance->iEglSurface, instance->iEglContext );

    // Set the screen background color.
    glClearColor( 0.f, 0.f, 0.4f, 1.f );

    /* Enable depth testing and back face culling. */
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE  );
    glEnable( GL_TEXTURE_2D );

    // Initialize viewport and projection.
    glViewport( 0, 0, iScreenWidth, iScreenHeight );
    glMatrixMode( GL_PROJECTION );

    // Calculate the view frustrum
    glFrustumf( FRUSTUM_LEFT * iAspectRatio, FRUSTUM_RIGHT * iAspectRatio,
                FRUSTUM_BOTTOM, FRUSTUM_TOP,
                FRUSTUM_NEAR, FRUSTUM_FAR );


    // Initialize appropriate texture matrix
    glMatrixMode( GL_TEXTURE );
    glLoadIdentity();
    glScalef(     1.0f/32767.0f, 1.0f/32767.0f, 1.0f );

    // Remember to change the matrix mode to modelview.
    glMatrixMode( GL_MODELVIEW );

    // Set shading mode
    glShadeModel( GL_FLAT );

    // Do not use perspective correction
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );


	// Construct a texture manager that uses the application's private
	// directory as the location for all textures.
    iTextureManager = CTextureManager::NewL ( iScreenWidth, iScreenWidth,
                                              iAspectRatio*1.f, iAspectRatio*-1.f, 1.f, -1.f, 3.f,
                                              this );

    // Pushing the textures into the loading queue.
    _LIT(KWorldTexture, "world.jpg");
    iTextureManager->RequestToLoad(KWorldTexture, &iWorldTexture );

    //Start to load the textures.
    iTextureManager->DoLoadL();

}

// -------------------------------------------------------------------------------------------------------
// CBillboard::OnStartLoadingTextures()
// Called for a MTextureLoadingListener by the texture manager when texture loading operation starts
// -------------------------------------------------------------------------------------------------------

void CMultiTex::OnStartLoadingTexturesL()
	{
    SetStateL( ELoadingTextures );
	}

// ------------------------------------------------------------------------------------------------------------
// CBillboard::OnEndLoadingTextures()
// Called for a MTextureLoadingListener by the texture manager when texture loading operation is completed
// ------------------------------------------------------------------------------------------------------------
void CMultiTex::OnEndLoadingTexturesL()
	{
    /* TEXTURE0 is used for Marble texture */
    glActiveTexture( GL_TEXTURE0 );
    glClientActiveTexture( GL_TEXTURE0 );

    // Enable texturing
    glEnable( GL_TEXTURE_2D );

    // Enable vertex and texture coordinates arrays.
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    // Set array pointers.
    glVertexPointer( 3, GL_SHORT, 0, objVertex );
    glTexCoordPointer( 2, GL_SHORT, 0, objTexCoords );

    /* Bind the marble.jpg texture to iWorldTexture, set the texture
       to the texPtr defined above, and set the texture environment. */
    glBindTexture( GL_TEXTURE_2D, iWorldTexture.iID );
    glTexEnvx( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    /* Texture to TEXTURE1 is taken from Pbuffer
       context, i.e, iLightTexObjects[1] */
    glActiveTexture( GL_TEXTURE1 );
    glClientActiveTexture( GL_TEXTURE1 );

    // Enable texturing
    glEnable( GL_TEXTURE_2D );

    // Enable vertex and texture coordinates arrays.
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    // Set array pointers.
    glVertexPointer( 3, GL_SHORT, 0, objVertex );
    glTexCoordPointer( 3, GL_SHORT, 0, envNormals );

    /* Bind the environment map texture created with Pbuffer context.
       Actual creation and updating is done in AppCycle. */
    glBindTexture( GL_TEXTURE_2D, iLightTexObjects[1] );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    // Set the texture env mode for environment map texture
    glTexEnvx( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD );

	if ( GetState() == ELoadingTextures )
		{
		SetStateL( ERunning );
		}
	}

// ---------------------------------------------------------
// CBillboard::OnEnterState( TInt aState )
// Called by TFiniteStateMachine when the f.s.m enters a new state
// ---------------------------------------------------------

void CMultiTex::OnEnterStateL( TInt /*aState*/ )
	{
	    // Nothing to do here...
	}

// -----------------------------------------------------------------------------
// CMultiTex::AppExit
//
// Release any allocations made in AppInit.
// -----------------------------------------------------------------------------
//

void CMultiTex::AppExit( void )
{
    delete iTextureManager;
}

// -----------------------------------------------------------------------------
// CMultiTex::DrawLight
//
// Draws lights.
// -----------------------------------------------------------------------------
//

void CMultiTex::DrawLights( TInt iFrame )
{
    // Draw red light
    glPushMatrix();
    glTranslatex(( 2 << 16) + (GLint) (sin(iFrame * 0.10f) * 5 * 65536) , (2 << 16) + (GLint) (cos(iFrame * 0.13f) * 5 * 65536) , 0 );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    glPopMatrix();

    // Draw green light
    glPushMatrix();
    glTranslatex( (-2 << 16) + (GLint) (sin(iFrame * 0.10f) * 5 * 65536) , (-2 << 16) + (GLint) (cos(iFrame * 0.10f) * 8 * 65536) , 0 );
    glDrawArrays( GL_TRIANGLE_STRIP, 4, 4 );
    glPopMatrix();

}

// -----------------------------------------------------------------------------
// CMultiTex::AppCyclePbuffer
//
// Create a dynamic environment map, where two spot lights (red, green) moves
// -----------------------------------------------------------------------------
//

void CMultiTex::AppCyclePbuffer( TInt iFrame )
{
    glClear( GL_COLOR_BUFFER_BIT );

    // Bind the light texture as current
    glBindTexture( GL_TEXTURE_2D, iLightTexObjects[0] );

    /* Animate and draw lights */
    glLoadIdentity();
    gluLookAt( 0.f, 0.f, 200.f,
               0.f, 0.f, 0.f,
               0.f, 1.f, 0.f );

    glScalex( 2 << 16, 2 << 16, 2 << 16 );
    DrawLights( iFrame );

    // Bind the environment map texture as current
    glBindTexture( GL_TEXTURE_2D, iLightTexObjects[1] );

    // Copy the content of the pbuffer to the currently bound texture
    glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 0, 0, 128, 128, 0 );
}

// -----------------------------------------------------------------------------
// CMultiTex::AppCycleWindow
//
// Draw the sphere with Marble and dynamic environment textures
// -----------------------------------------------------------------------------
//

void CMultiTex::AppCycleWindow( TInt iFrame )
{
    // Set the TEXTURE1 unit as active (env map texture)
    glActiveTexture( GL_TEXTURE1 );
    glClientActiveTexture( GL_TEXTURE1 );

    // Set the correct texture matrix
    glMatrixMode( GL_TEXTURE );
    glLoadIdentity();
    glTranslatef( 0.5f, 0.5f, 0.5f );
    glRotatex( 180 << 16, 0, 0, 1 << 16 );
    glRotatex( iFrame << 16, 0, 1 << 16, 0 );
    glScalef( 1.0f/65534.0f, 1.0f/65534.0f, 1.0f/65534.0f );

    glMatrixMode( GL_MODELVIEW );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


    glLoadIdentity();
    glTranslatex( 0 , 0 , -200 << 16 );
    glRotatex( 180 << 16, 0, 0, 1 << 16 );


    glRotatex( iFrame << 16, 0, 1 << 16, 0 );


    // Scale the world model to fit the screen.
    glScalex( 1 << 9, 1 << 9, 1 << 9 );
    glDrawElements( GL_TRIANGLES, objFaces * 3,
                    GL_UNSIGNED_SHORT, objIndices );
}

// -----------------------------------------------------------------------------
// CMultiTex::SetScreenSize
// Reacts to the dynamic screen size change during execution of this program.
// -----------------------------------------------------------------------------
//
void CMultiTex::SetScreenSize( TUint aWidth, TUint aHeight )
    {
    iScreenWidth  = aWidth;
    iScreenHeight = aHeight;

    // Notify the texture manager of screen size change
    iTextureManager->SetScreenSize( aWidth, aHeight );

    // Reinitialize viewport and projection.
    glViewport( 0, 0, iScreenWidth, iScreenHeight );

    // Recalculate the view frustrum
    if ( iScreenHeight != 0 )
        {
        iAspectRatio = (GLfloat)iScreenWidth / (GLfloat)iScreenHeight;
        }
    else
        {
        iAspectRatio = 1.0;
        }

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glFrustumf( FRUSTUM_LEFT * iAspectRatio, FRUSTUM_RIGHT * iAspectRatio,
                FRUSTUM_BOTTOM, FRUSTUM_TOP,
                FRUSTUM_NEAR, FRUSTUM_FAR );
    glMatrixMode( GL_PROJECTION );

    }


// End of File
