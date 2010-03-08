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
#include <e32math.h>

#include "EnvMapping.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEnvMapping::CEnvMapping
// Default C++ constructor. Cannot leave.
// -----------------------------------------------------------------------------
//
CEnvMapping::CEnvMapping( TUint aWidth, TUint aHeight ): CFiniteStateMachine()
{
    iFaceNormals   = 0; // Initialize member variables
    iNormalCounter = 0;

    iScreenWidth  = aWidth;
    iScreenHeight = aHeight;
}


// -----------------------------------------------------------------------------
// CEnvMapping::ConstructL
// Symbian contructor method, which is called when an instance of CEnvMapping
// is created.
// -----------------------------------------------------------------------------
//

void CEnvMapping::ConstructL( void )
{
    /* Calculate texture coordinates for the models */
    CalculateTextureCoordinatesL();
}


// -----------------------------------------------------------------------------
// CEnvMapping::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------

CEnvMapping* CEnvMapping::NewL( TUint aWidth, TUint aHeight )
{
    /* Symbian 2-phase constructor. Calls both the default
       C++ constructor and Symbian ConstructL methods */
    CEnvMapping* self = new (ELeave) CEnvMapping( aWidth, aHeight );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}

// -----------------------------------------------------------------------------
// CEnvMapping::AppExit
// Release any allocations made in AppInit.
// -----------------------------------------------------------------------------
//

void CEnvMapping::AppExit( void )
{
    delete iTextureManager;
}

// Destructor.
CEnvMapping::~CEnvMapping()
{
    delete iFaceNormals;
    delete iNormalCounter;
}


// -----------------------------------------------------------------------------
// CEnvMapping::AppInit
// Initialize OpenGL ES, set the vertex and color arrays and pointers,
// and select the shading mode.
// -----------------------------------------------------------------------------
//

void CEnvMapping::AppInitL( void )
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
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE  );
    glEnable( GL_TEXTURE_2D );

    /* Enable vertex arrays. */
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    /* Set array pointers. */
    glVertexPointer( 3, GL_SHORT, 0, objVertexdataDuck );
    glTexCoordPointer( 3, GL_SHORT, 0, iDuckTexCoords );

    /* Set the initial shading mode */
    glShadeModel( GL_FLAT );

    /* Do not use perspective correction */
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );

    iRotateObjectLeftEnabled  = EFalse;
    iRotateObjectRightEnabled = EFalse;
    iObjectRotAngle    = 0;

    // Push the textures into the loading queue.
    _LIT(KEnvMapTexture, "envmap.jpg");
    iTextureManager->RequestToLoad( KEnvMapTexture, &iEnvMapTexture );

    //Start to load the textures.
    iTextureManager->DoLoadL();
}


// -----------------------------------------------------------------------------
// CEnvMapping::InitializeArraysL
// Initialize arrays used to calculate objects texture coordinates.
// Reserves dynamically memories for the arrays depending on the size on the models.
// -----------------------------------------------------------------------------
//

void CEnvMapping::InitializeArraysL( void )
{
    _LIT(KModelError, "Incorrect Model");

    if( iFaceNormals )
    {
        delete iFaceNormals;
    }

    if( iNormalCounter )
    {
        delete iNormalCounter;
    }

    if( iObjectModel == DUCK_MODEL )
    {
        iFaceNormals    = new (ELeave) GLfloat[ duckVertices*3 ];
        iNormalCounter  = new (ELeave) TUint8[ duckVertices ];

        for( TInt i = 0; i < duckVertices;   i++ ) { iNormalCounter[i] = 0;  }
        for( TInt j = 0; j < 3*duckVertices; j++ ) { iFaceNormals[j]   = 0.; }
        for( TInt k = 0; k < 3*duckVertices; k++ ) { iDuckTexCoords[k] = 0;  }
    }
    else if( iObjectModel == HEAD_MODEL )
    {
        iFaceNormals    = new (ELeave) GLfloat[ 3*headVertices ];
        iNormalCounter  = new (ELeave) TUint8[ headVertices ];

        for( TInt i = 0; i < headVertices;   i++ ) { iNormalCounter[i] = 0;  }
        for( TInt j = 0; j < 3*headVertices; j++ ) { iFaceNormals[j]   = 0.; }
        for( TInt k = 0; k < 3*headVertices; k++ ) { iHeadTexCoords[k] = 0;  }
    }
    else User::Panic( KModelError, 1 );
}


// -----------------------------------------------------------------------------
// CEnvMapping::CalculateTextureCoordinatesL
// Calculates texture coordinates for the models used.
// Texture coordinates for one vertex is defined by calculating an average
// normal of all the surface normals adjacent to that vertex.
// -----------------------------------------------------------------------------
//

void CEnvMapping::CalculateTextureCoordinatesL( void )
{
    _LIT(KNormalError,    "Normal length 0");
    _LIT(KAvgNormalError, "Avg. Normal length 0");

    iObjectModel = HEAD_MODEL; // calculate tex coords first to head model

    GLushort vertex1 = 0; // indexes to the object vertex data
    GLushort vertex2 = 0;
    GLushort vertex3 = 0;

	GLfloat pnt1[3] = { .0f, .0f, .0f }; // vertex triplet that forms a triangle
    GLfloat pnt2[3] = { .0f, .0f, .0f };
    GLfloat pnt3[3] = { .0f, .0f, .0f };

    GLfloat vec1[3]; // two vectors calculated from the vertex triplet
    GLfloat vec2[3];

    GLfloat norm[3];          // face normal
    GLfloat normLen;          // normal lenght
    GLint   iNumFaces = 0;    // number of faces in a model
    GLint   iNumVertices = 0; // number of vertices in a model

    for( TInt obj = 0; obj < NUM_OF_MODELS; obj++ )
    {

        if( iObjectModel == DUCK_MODEL )
        {
            iNumFaces    = duckFaces;
            iNumVertices = duckVertices;
            InitializeArraysL(); // initialize all arrays
        }
        else if( iObjectModel == HEAD_MODEL )
        {
            iNumFaces    = headFaces;
            iNumVertices = headVertices;
            InitializeArraysL(); // initialize all arrays
        }

        for( TInt i = 0; i < iNumFaces; i++ )
        {
            // Form two vectors vec1 and vec2 from vertex triplet,
            // pnt1/2/3 are the three vertices that form a triangle.
            if( iObjectModel == DUCK_MODEL )
            {
                vertex1 = objFacedataDuck[i*3];
                vertex2 = objFacedataDuck[i*3+1];
                vertex3 = objFacedataDuck[i*3+2];

                pnt1[0] = objVertexdataDuck[ 3*vertex1     ];
                pnt1[1] = objVertexdataDuck[ 3*vertex1 + 1 ];
                pnt1[2] = objVertexdataDuck[ 3*vertex1 + 2 ];

                pnt2[0] = objVertexdataDuck[ 3*vertex2     ];
                pnt2[1] = objVertexdataDuck[ 3*vertex2 + 1 ];
                pnt2[2] = objVertexdataDuck[ 3*vertex2 + 2 ];

                pnt3[0] = objVertexdataDuck[ 3*vertex3     ];
                pnt3[1] = objVertexdataDuck[ 3*vertex3 + 1 ];
                pnt3[2] = objVertexdataDuck[ 3*vertex3 + 2 ];
            }
            else if( iObjectModel == HEAD_MODEL )
            {
                vertex1 = objFacedataHead[i*3];
                vertex2 = objFacedataHead[i*3+1];
                vertex3 = objFacedataHead[i*3+2];

                pnt1[0] = objVertexdataHead[ 3*vertex1     ];
                pnt1[1] = objVertexdataHead[ 3*vertex1 + 1 ];
                pnt1[2] = objVertexdataHead[ 3*vertex1 + 2 ];

                pnt2[0] = objVertexdataHead[ 3*vertex2     ];
                pnt2[1] = objVertexdataHead[ 3*vertex2 + 1 ];
                pnt2[2] = objVertexdataHead[ 3*vertex2 + 2 ];

                pnt3[0] = objVertexdataHead[ 3*vertex3     ];
                pnt3[1] = objVertexdataHead[ 3*vertex3 + 1 ];
                pnt3[2] = objVertexdataHead[ 3*vertex3 + 2 ];
            }

            vec1[0] = pnt2[0] - pnt1[0];
            vec1[1] = pnt2[1] - pnt1[1];
            vec1[2] = pnt2[2] - pnt1[2];

            vec2[0] = pnt3[0] - pnt1[0];
            vec2[1] = pnt3[1] - pnt1[1];
            vec2[2] = pnt3[2] - pnt1[2];

            // calculate crossproduct between the two vectors. That way
            // we have the face normal.
            norm[0] = vec1[1]*vec2[2] - vec1[2]*vec2[1];
            norm[1] = vec1[2]*vec2[0] - vec1[0]*vec2[2];
            norm[2] = vec1[0]*vec2[1] - vec1[1]*vec2[2];

            normLen = sqrt( norm[0]*norm[0] + norm[1]*norm[1] + norm[2]*norm[2] );

            /* If normal lenght is zero, generate panic */
            if( normLen == 0.0 )
            {
                User::Panic( KNormalError, 1 );
            }

            // normalize the calculated normal
            norm[0] /= normLen;
            norm[1] /= normLen;
            norm[2] /= normLen;

            // store normal to each vertex index that were used to calculate
            // the normal. Add also corresponding vertex counter
            iFaceNormals[ 3*vertex1   ] += norm[0];
            iFaceNormals[ 3*vertex1+1 ] += norm[1];
            iFaceNormals[ 3*vertex1+2 ] += norm[2];
            iNormalCounter[ vertex1 ]   += 1;

            iFaceNormals[ 3*vertex2   ] += norm[0];
            iFaceNormals[ 3*vertex2+1 ] += norm[1];
            iFaceNormals[ 3*vertex2+2 ] += norm[2];
            iNormalCounter[ vertex2 ]   += 1;

            iFaceNormals[ 3*vertex3   ] += norm[0];
            iFaceNormals[ 3*vertex3+1 ] += norm[1];
            iFaceNormals[ 3*vertex3+2 ] += norm[2];
            iNormalCounter[ vertex3 ]   += 1;
        }

        // calculate and normalize average normal for each vertex
        // scale the resulting normal values by COORD_SCALE for GLshort range
        for( TInt j = 0; j < iNumVertices; j++ )
        {
            /* If iNormalCounter value is zero, skip that vertex.
               If the value is zero, it means that the vertex number j is not
               used in the object's geometry. */
            if( iNormalCounter[j] != 0 )
            {

                // iFaceNormals contains the sum of all the normals common to each vertex
                // divide by the iNormalCounter value to get the average normal
                iFaceNormals[3*j]   = (iFaceNormals[3*j]/iNormalCounter[j]  );
                iFaceNormals[3*j+1] = (iFaceNormals[3*j+1]/iNormalCounter[j]);
                iFaceNormals[3*j+2] = (iFaceNormals[3*j+2]/iNormalCounter[j]);

                // normalize the result, then scale the normal values by COORD_SCALE to
                // make GLshort range, and store the resulting normal.
                normLen = sqrt( iFaceNormals[3*j]*iFaceNormals[3*j] +
                                iFaceNormals[3*j+1]*iFaceNormals[3*j+1] +
                                iFaceNormals[3*j+2]*iFaceNormals[3*j+2] );

                /* If normal lenght is zero, generate panic */
                if( normLen == 0.0 )
                {
                    User::Panic( KAvgNormalError, 1 );
                }

                if( iObjectModel == DUCK_MODEL )
                {
                    iDuckTexCoords[3*j]   = (GLshort) (COORD_SCALE*iFaceNormals[3*j]/normLen  );
                    iDuckTexCoords[3*j+1] = (GLshort) (COORD_SCALE*iFaceNormals[3*j+1]/normLen);
                    iDuckTexCoords[3*j+2] = (GLshort) (COORD_SCALE*iFaceNormals[3*j+2]/normLen);
                }
                else if( iObjectModel == HEAD_MODEL )
                {
                    iHeadTexCoords[3*j]   = (GLshort) (COORD_SCALE*iFaceNormals[3*j]/normLen  );
                    iHeadTexCoords[3*j+1] = (GLshort) (COORD_SCALE*iFaceNormals[3*j+1]/normLen);
                    iHeadTexCoords[3*j+2] = (GLshort) (COORD_SCALE*iFaceNormals[3*j+2]/normLen);
                }
            }
        }
        // Change the model to head model. Default start model is duck.
        iObjectModel = DUCK_MODEL;
    }
}

// ---------------------------------------------------------
// CBillboard::OnEnterState( TInt aState )
// Called by TFiniteStateMachine when the f.s.m enters a new state
// ---------------------------------------------------------

void CEnvMapping::OnEnterStateL( TInt /*aState*/ )
	{
	}

// -------------------------------------------------------------------------------------------------------
// CBillboard::OnStartLoadingTextures()
// Called for a MTextureLoadingListener by the texture manager when texture loading operation starts
// -------------------------------------------------------------------------------------------------------

void CEnvMapping::OnStartLoadingTexturesL()
	{
    SetStateL( ELoadingTextures );
	}

// ------------------------------------------------------------------------------------------------------------
// CBillboard::OnEndLoadingTextures()
// Called for a MTextureLoadingListener by the texture manager when texture loading operation is completed
// ------------------------------------------------------------------------------------------------------------
void CEnvMapping::OnEndLoadingTexturesL()
	{
	if ( GetState() == ELoadingTextures )
		{
        /* Bind the envmap texture to iEnvMapTexture and set the texture environment. */
        glBindTexture( GL_TEXTURE_2D, iEnvMapTexture.iID );
	    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexEnvx( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

		SetStateL( ERunning );
		}
	}


// -----------------------------------------------------------------------------
// CEnvMapping::AppCycle
// Draws and animates the objects
// -----------------------------------------------------------------------------
//

void CEnvMapping::AppCycle( TInt aFrame )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    /* Update user controlled object rotation */
    if( iRotateObjectLeftEnabled )
    {
        iObjectRotAngle -= 2;
    }
    else if( iRotateObjectRightEnabled )
    {
        iObjectRotAngle += 2;
    }

    if( iObjectModel == DUCK_MODEL )
    {
        DrawDuck( aFrame );
    }
    else if ( iObjectModel == HEAD_MODEL )
    {
        DrawHead( aFrame );
    }
}


// -----------------------------------------------------------------------------
// CEnvMapping::DrawDuck
// Draw the environment mapped duck model.
// -----------------------------------------------------------------------------
//

void CEnvMapping::DrawDuck( TInt aFrame )
{
    /* Set the correct texture matrix */
    glMatrixMode( GL_TEXTURE );
    glLoadIdentity();
    glTranslatef( 0.5f, 0.5f, 0.5f );    // texture coord. values [0,1]
    glScalef( 1.0f/(2.0f*COORD_SCALE),   // scale down the input texture
              1.0f/(2.0f*COORD_SCALE),   // coord. values to be in range
              1.0f/(2.0f*COORD_SCALE) ); // [-0.5, 0.5]

    /* First in texture matrix, insert the current camera and object
       rotations. */
    glRotatex((-90 << 16) + (GLint) (sin(aFrame * 0.17f) * 15 * 65536), 1 << 16, 0, 0);
    glRotatex((GLint) (sin(aFrame * 0.13f) * 10 * 65536), 0, 1 << 16, 0);
    glRotatex( iObjectRotAngle << 16, 0, 0, 1 << 16);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslatex( 0 , -20 << 16 , -CAMERA_DISTANCE << 16 );
    glRotatex((-90 << 16) + (GLint) (sin(aFrame * 0.17f) * 15 * 65536), 1 << 16, 0, 0);
    glRotatex((GLint) (sin(aFrame * 0.13f) * 10 * 65536), 0, 1 << 16, 0);
    glRotatex( iObjectRotAngle << 16, 0, 0, 1 << 16);
    glScalex( 3 << 6, 3 << 6, 3 << 6 ); // scale the duck to fit the screen

    glBindTexture( GL_TEXTURE_2D, iEnvMapTexture.iID );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glDrawElements( GL_TRIANGLES, duckFaces * 3, GL_UNSIGNED_SHORT, objFacedataDuck );
}


// -----------------------------------------------------------------------------
// CEnvMapping::DrawHead
// Draw the environment mapped head model.
// -----------------------------------------------------------------------------
//

void CEnvMapping::DrawHead( TInt aFrame )
{
    /* Set the correct texture matrix */
    glMatrixMode( GL_TEXTURE );
    glLoadIdentity();
    glTranslatef( 0.5f, 0.5f, 0.5f );    // texture coord. values [0,1]
    glScalef( 1.0f/(2.0f*COORD_SCALE),   // scale down the input texture
              1.0f/(2.0f*COORD_SCALE),   // coord. values to be in range
              1.0f/(2.0f*COORD_SCALE) ); // [-0.5, 0.5]
    /* First in texture matrix, insert the current camera and object
       rotations. */
    glRotatex( -90 << 16, 1 << 16, 0, 0);
    glRotatex( aFrame << 16, 5 << 16, 1 << 12, 0);
    glRotatex( iObjectRotAngle << 16, 0, 0, 1 << 16);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslatex( 0 , 0, -CAMERA_DISTANCE << 16 );
    glRotatex( -90 << 16, 1 << 16, 0, 0);
    glRotatex( aFrame << 16, 5 << 16, 1 << 12, 0);
    glRotatex( iObjectRotAngle << 16, 0, 0, 1 << 16);

    glScalex( 1 << 10, 1 << 10, 1 << 10 ); // scale the duck to fit the screen
    glBindTexture( GL_TEXTURE_2D, iEnvMapTexture.iID );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glDrawElements( GL_TRIANGLES, headFaces * 3, GL_UNSIGNED_SHORT, objFacedataHead );
}


//----------------------------------------------------------
// The following methods are called by the CEnvMappingAppUi
// class when handling the incoming menu events.
//----------------------------------------------------------


// -----------------------------------------------------------------------------
// CEnvMapping::RotateObjectLeft
// Rotares object to left
// -----------------------------------------------------------------------------
//

void CEnvMapping::RotateObjectLeft( void )
{
    iRotateObjectLeftEnabled = !iRotateObjectLeftEnabled;
}

// -----------------------------------------------------------------------------
// CEnvMapping::RotateObjectRight
// Rotares object to right
// -----------------------------------------------------------------------------
//
void CEnvMapping::RotateObjectRight( void )
{
    iRotateObjectRightEnabled = !iRotateObjectRightEnabled;
}

// -----------------------------------------------------------------------------
// CEnvMapping::DuckModel
// Change to duck model.
// Set the correct vertex and texture coordinate pointers.
// -----------------------------------------------------------------------------
//

void CEnvMapping::DuckModel( void )
{
    iObjectModel = DUCK_MODEL;
    glVertexPointer( 3, GL_SHORT, 0, objVertexdataDuck );
    glTexCoordPointer( 3, GL_SHORT, 0, iDuckTexCoords );
}

// -----------------------------------------------------------------------------
// CEnvMapping::HeadModel
// Change to head model.
// Set the correct vertex and texture coordinate pointers.
// -----------------------------------------------------------------------------
//

void CEnvMapping::HeadModel( void )
{
    iObjectModel = HEAD_MODEL;
    glVertexPointer( 3, GL_SHORT, 0, objVertexdataHead );
    glTexCoordPointer( 3, GL_SHORT, 0, iHeadTexCoords );
}

// -----------------------------------------------------------------------------
// CEnvMapping::SetScreenSize
// Reacts to the dynamic screen size change during execution of this program.
// -----------------------------------------------------------------------------
//
void CEnvMapping::SetScreenSize( TUint aWidth, TUint aHeight )
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



//  End of File
