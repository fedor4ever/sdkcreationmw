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


// INCLUDES
#include <e32std.h>
#include <e32math.h>

#include "SimpleLight.h"

#include "duck.h"        // Geometry, normals, and indices for the duck model
#include "head.h"        // Geometry, normals, and indices for the head model

// MACROS
#define MATERIAL_MAX 1
#define LIGHT_MAX    1

#define cameraDistance 270


#define MATERIALCOLOR(r, g, b, a)     \
       (GLfloat)(r * MATERIAL_MAX),   \
       (GLfloat)(g * MATERIAL_MAX),   \
       (GLfloat)(b * MATERIAL_MAX),   \
       (GLfloat)(a * MATERIAL_MAX)

#define LIGHTCOLOR(r, g, b, a)        \
       (GLfloat)(r * LIGHT_MAX),      \
       (GLfloat)(g * LIGHT_MAX),      \
       (GLfloat)(b * LIGHT_MAX),      \
       (GLfloat)(a * LIGHT_MAX)

// CONSTANTS
/** Materials for the duck object. */
static const GLfloat objDiffuseDuck[4]  = { MATERIALCOLOR(0.8, 0.8, 0.2, 1.0) };
static const GLfloat objAmbientDuck[4]  = { MATERIALCOLOR(0.8, 0.8, 0.2, 1.0) };
static const GLfloat objSpecularDuck[4] = { MATERIALCOLOR(1.0, 1.0, 1.0, 1.0) };
static const GLfloat objEmissionDuck[4] = { MATERIALCOLOR(0.0, 0.0, 0.0, 1.0) };

/* Materials for the head object. */
static const GLfloat objDiffuseHead[4]  = { MATERIALCOLOR(0.8, 0.4, 0.0, 1.0) };
static const GLfloat objAmbientHead[4]  = { MATERIALCOLOR(0.8, 0.4, 0.0, 1.0) };
static const GLfloat objSpecularHead[4] = { MATERIALCOLOR(1.0, 1.0, 1.0, 1.0) };

/* Global ambient light. */
static const GLfloat globalAmbient[4]   = { LIGHTCOLOR(0.0, 0.0, 0.0, 1.0) };

/* Lamp parameters. */
static const GLfloat lightDiffuseLamp[4]   = { LIGHTCOLOR(0.7, 0.7, 0.7, 1.0) };
static const GLfloat lightAmbientLamp[4]   = { LIGHTCOLOR(0.3, 0.3, 0.3, 1.0) };
static const GLfloat lightPositionLamp[4]  = { 0, 10, 10, 0 };

/* Spotlight parameters. */
static const GLfloat lightDiffuseSpot[4]   = { LIGHTCOLOR(0.0, 0.0, 0.0, 1.0) };
static const GLfloat lightAmbientSpot[4]   = { LIGHTCOLOR(0.0, 0.0, 0.0, 1.0) };
static const GLfloat lightSpecularSpot[4]  = { LIGHTCOLOR(0.0, 0.0, 3.0, 1.0) };
static const GLfloat lightPositionSpot[4]  = {  0, -10, -1, 0 };
static const GLfloat lightDirectionSpot[4] = {  0,  10,  1, 1 };


// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// sin
// Help function to make the duck 'quak'.
// -----------------------------------------------------------------------------
//
GLfloat CSimpleLight::sin(GLfloat aRad)
    {
    TReal trg, src = (TReal)aRad;
    if ( Math::Sin(trg, src) == KErrNone )
        {
        return (GLfloat)trg;
        }
    return 0;

    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSimpleLight::CSimpleLight
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSimpleLight::CSimpleLight( TUint aWidth, TUint aHeight )
:iScreenWidth(aWidth), iScreenHeight(aHeight)
    {
    }

// -----------------------------------------------------------------------------
// CSimpleLight::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSimpleLight::ConstructL( void )
    {
    }


// -----------------------------------------------------------------------------
// CSimpleLight::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSimpleLight* CSimpleLight::NewL( TUint aWidth, TUint aHeight )
    {
    /* Symbian 2-phase constructor. Calls both the default
       C++ constructor and Symbian ConstructL methods */
    CSimpleLight* self = new (ELeave) CSimpleLight( aWidth, aHeight );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor.
CSimpleLight::~CSimpleLight()
    {
    }



// -----------------------------------------------------------------------------
// CSimpleLight::AppInit
// Initialize OpenGL ES, set the vertex and color arrays and pointers,
// and select the shading mode.
// -----------------------------------------------------------------------------
//
void CSimpleLight::AppInit( void )
    {
    // Initialize viewport and projection.
		SetScreenSize( iScreenWidth, iScreenHeight );

      // Set the screen background color.
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    // Enable depth testing, back face culling, and lighting.
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE  );
    glEnable( GL_LIGHTING   );
    /* Make OpenGL ES automatically normalize all normals after tranformations.
       This is important when making irregular xforms like scaling, or if we
       have specified nonunit-length normals. */
    glEnable( GL_NORMALIZE  );

    // Enable vertex and normal arrays.
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );

    // Set array pointers. Initially for the duck object.
    glVertexPointer( 3, GL_SHORT, 0, objVertexdataDuck );
    glNormalPointer( GL_BYTE, 0, objNormaldataDuck     );

    // Set up global ambient light.
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, globalAmbient );

    // Set up lamp.
    glEnable( GL_LIGHT0 );
    glLightfv(  GL_LIGHT0, GL_DIFFUSE,  lightDiffuseLamp  );
    glLightfv(  GL_LIGHT0, GL_AMBIENT,  lightAmbientLamp  );
    glLightfv(  GL_LIGHT0, GL_SPECULAR, lightDiffuseLamp  );
    glLightfv(  GL_LIGHT0, GL_POSITION, lightPositionLamp );

    // Set up spot.  Initially spot is disabled.
    glLightfv(  GL_LIGHT1, GL_DIFFUSE,  lightDiffuseSpot  );
    glLightfv(  GL_LIGHT1, GL_AMBIENT,  lightAmbientSpot  );
    glLightfv(  GL_LIGHT1, GL_SPECULAR, lightSpecularSpot );
    glLightfv(  GL_LIGHT1, GL_POSITION, lightPositionSpot );

    glLightf(   GL_LIGHT1, GL_CONSTANT_ATTENUATION,  1.5  );
    glLightf(   GL_LIGHT1, GL_LINEAR_ATTENUATION,    0.5  );
    glLightf(   GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.5  );

    glLightf(   GL_LIGHT1, GL_SPOT_CUTOFF,   17.0                );
    glLightf(   GL_LIGHT1, GL_SPOT_EXPONENT,  2.0                );
    glLightfv(  GL_LIGHT1, GL_SPOT_DIRECTION, lightDirectionSpot );

    // Set duck material
    glMaterialfv(   GL_FRONT_AND_BACK, GL_DIFFUSE,  objDiffuseDuck      );
    glMaterialfv(   GL_FRONT_AND_BACK, GL_AMBIENT,  objAmbientDuck      );
    glMaterialfv(   GL_FRONT_AND_BACK, GL_SPECULAR, objSpecularDuck     );
    glMaterialfv(   GL_FRONT_AND_BACK, GL_EMISSION, objEmissionDuck     );
    glMaterialx( GL_FRONT_AND_BACK, GL_SHININESS,   10 << 16         );

    // Set shading mode
    glShadeModel( GL_SMOOTH );

    // Do not use perspective correction
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );

    // Initialize menu state variables, Symbian menu stuff.
    iLightingEnabled = ETrue;          // Lighting is enabled
    iLampEnabled     = ETrue;          // Lamp is enabled
    iSpotEnabled     = EFalse;         // Spot is disabled
    iObjectModel     = EDuckModel;     // Default model is duck
    }


// -----------------------------------------------------------------------------
// CSimpleLight::AppExit
// Release any allocations made in AppInit.
// -----------------------------------------------------------------------------
//
void CSimpleLight::AppExit( void )
    {
    }


// -----------------------------------------------------------------------------
// CSimpleLight::AppCycle
// Draws and animates the objects
// -----------------------------------------------------------------------------
//
void CSimpleLight::AppCycle( TInt iFrame )
    {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glLoadIdentity();

    switch( iObjectModel )
        {
        case EDuckModel:
            {
            glTranslatex( 0 , -20 << 16 , -cameraDistance << 16 );

            glRotatex((-90 << 16) + (GLint) (sin(iFrame * 0.17f) * 15 * 65536),
                      1 << 16, 0, 0);
            glRotatex((GLint) (sin(iFrame * 0.13f) * 10 * 65536), 0, 1 << 16, 0);
            glRotatex(iFrame << 16, 0, 0, 1 << 16);

            // Use some magic numbers to scale the duck model to fit the screen.
            glScalex( 3 << 6, 3 << 6, 3 << 6 );
            glDrawElements( GL_TRIANGLES, objFaces * 3,
                            GL_UNSIGNED_BYTE, objFacedataDuck );
            }
            break;
        case EHeadModel:
            {
            glTranslatex( 0, 0, -cameraDistance << 16 );

            glRotatex(-90 << 16, 1 << 16, 0, 0);
            glRotatex( iFrame << 16, 5 << 16, 1 << 12, 0);
            glRotatex( iFrame << 16, 0, 0, 1 << 16);

            // Use some magic numbers to scale the head model to fit the screen.
            glScalex( 1 << 10, 1 << 10, 1 << 10 );
            glDrawElements( GL_TRIANGLES, objFacesHead * 3,
                            GL_UNSIGNED_SHORT, objFacedataHead );
            }
            break;
        default:
            // Do nothing.
            break;
        }
    }


//----------------------------------------------------------
// The following methods are called by the CSimpleLightAppUi
// class when handling the incoming menu events.
//----------------------------------------------------------


// -----------------------------------------------------------------------------
// CSimpleLight::ToggleLighting
// Enable/Disable lighting from the application menu.
// -----------------------------------------------------------------------------
//
void CSimpleLight::ToggleLighting( void )
    {
    if ( iLightingEnabled)
        {
        iLightingEnabled = EFalse;
        glDisable( GL_LIGHTING );
        }
    else
        {
        iLightingEnabled = ETrue;
        glEnable( GL_LIGHTING );
        }
    }


// -----------------------------------------------------------------------------
// CSimpleLight::ToggleLamp
// Enable/Disable lamp from the application menu.
// -----------------------------------------------------------------------------
//
void CSimpleLight::ToggleLamp( void )
    {
    if ( iLampEnabled )
        {
        iLampEnabled = EFalse;
        glDisable( GL_LIGHT0 );
        }
    else
        {
        iLampEnabled = ETrue;
        glEnable( GL_LIGHT0 );
        }
    }


// -----------------------------------------------------------------------------
// CSimpleLight::ToggleSpot
// Enable/Disable spot from the application menu.
// -----------------------------------------------------------------------------
//
void CSimpleLight::ToggleSpot( void )
    {
    if ( iSpotEnabled )
        {
        iSpotEnabled = EFalse;
        glDisable( GL_LIGHT1 );
        }
    else
        {
        iSpotEnabled = ETrue;
        glEnable( GL_LIGHT1 );
        }
    }


// -----------------------------------------------------------------------------
// CSimpleLight::DuckModel
// Change the current model to duck object.
// -----------------------------------------------------------------------------
//
void CSimpleLight::DuckModel( void )
    {
    iObjectModel = EDuckModel;

    // Set array pointers for duck model.
    glVertexPointer( 3, GL_SHORT, 0, objVertexdataDuck );
    glNormalPointer( GL_BYTE, 0, objNormaldataDuck );

    // Set duck material
    glMaterialfv(   GL_FRONT_AND_BACK, GL_AMBIENT,  objAmbientDuck  );
    glMaterialfv(   GL_FRONT_AND_BACK, GL_DIFFUSE,  objDiffuseDuck  );
    glMaterialfv(   GL_FRONT_AND_BACK, GL_SPECULAR, objSpecularDuck );
    glMaterialx( GL_FRONT_AND_BACK, GL_SHININESS,   10 << 16     );
    }


// -----------------------------------------------------------------------------
// CSimpleLight::HeadModel
// Change the current model to head object.
// -----------------------------------------------------------------------------
//
void CSimpleLight::HeadModel( void )
    {
    iObjectModel = EHeadModel;

    // Set array pointers for head model.
    glVertexPointer( 3, GL_SHORT, 0, objVertexdataHead );
    glNormalPointer( GL_BYTE, 0, objNormaldataHead );

    // Set head front material
    glMaterialfv(   GL_FRONT_AND_BACK, GL_AMBIENT,  objAmbientHead  );
    glMaterialfv(   GL_FRONT_AND_BACK, GL_DIFFUSE,  objDiffuseHead  );
    glMaterialfv(   GL_FRONT_AND_BACK, GL_SPECULAR, objSpecularHead );
    glMaterialx( GL_FRONT_AND_BACK, GL_SHININESS,   12 << 16     );

    }


// -----------------------------------------------------------------------------
// CSimpleLight::SetScreenSize
// Reacts to the dynamic screen size change during execution of this program.
// -----------------------------------------------------------------------------
//
void CSimpleLight::SetScreenSize( TUint aWidth, TUint aHeight )
    {
    iScreenWidth  = aWidth;
    iScreenHeight = aHeight;

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

//  End of File
