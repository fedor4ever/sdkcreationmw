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


#ifndef ENVMAPPING_H
#define ENVMAPPING_H

//  INCLUDES
#include <e32base.h>      // for CBase definition
#include <GLES/gl.h>      // OpenGL ES header file
#include "duck.h"         // Data for Duck model
#include "head.h"         // Data for Head model
#include "Utils3d.h"         // Utilities (state machine, texture handler etc.)

// MACROS
#define DUCK_MODEL 11
#define HEAD_MODEL 22

#define NUM_OF_MODELS 2    // i.e., duck and head models

#define COORD_SCALE 16384   // a value used to scale the texture coordinates
#define CAMERA_DISTANCE 270 // object distance from the camera

#define TEX_WIDTH     256 // Texture size must be power of two and max size
#define TEX_HEIGHT    256 // for textures is 256 x 256

#define FRUSTUM_LEFT   -1.f     //left vertical clipping plane
#define FRUSTUM_RIGHT   1.f     //right vertical clipping plane
#define FRUSTUM_BOTTOM -1.f     //bottom horizontal clipping plane
#define FRUSTUM_TOP     1.f     //top horizontal clipping plane
#define FRUSTUM_NEAR    3.f     //near depth clipping plane
#define FRUSTUM_FAR  1000.f     //far depth clipping plane

// CLASS DECLARATION

/**
 * Class that does the actual OpenGL ES rendering.
 * Subclass of CFiniteStateMachine because the application
 * needs to handle loading textures and running states.
 */
class CEnvMapping : public CFiniteStateMachine, public MTextureLoadingListener
{
  public:  // Constructors and destructor

        /**
         * Factory method for creating a new CSimpleCube object.
         */
        static CEnvMapping* NewL( TUint aWidth, TUint aHeight );

        /**
         * Destructor. Does nothing.
         */
        virtual ~CEnvMapping();

  public: // New functions

        /**
         * Initializes OpenGL ES, sets the vertex and color
         * arrays and pointers. Also selects the shading mode and
         * creates the CTextureManager instance and starts loading
         * of required textures.
         */
        void AppInitL( void );

        /**
         * Called upon application exit. Destroys the CTextureManager
         * instance that was created in AppInitL() method.
         */
        void AppExit( void );

        /**
         * Renders one frame.
         * @param aFrame Number of the frame to be rendered.
         */
        void AppCycle( TInt aFrame );

    	/**
    	 * Called when the finite state machine enters a new state.
    	 * Does nothing in this implementation.
    	 * @param aState State that is about to be entered.
    	 */
    	void OnEnterStateL( TInt aState );

    	/**
    	 * Called when texture manager starts loading the textures.
    	 * Sets the current state to "loading textures".
    	 */
    	void OnStartLoadingTexturesL();

    	/**
    	 * Called when texture manager has completed texture loading.
    	 * Changes the current state to "running".
    	 */
    	void OnEndLoadingTexturesL();

        /**
         * Initializes arrays used to calculate objects texture coordinates.
         * Reserves dynamically memories for the arrays depending on the size on the models.
         */
        void InitializeArraysL( void );

        /**
         * Calculates texture coordinates for the models.
         */
        void CalculateTextureCoordinatesL( void );

        /**
         * Rotates object left.
         */
        void RotateObjectLeft( void );

        /**
         * Rotates object right.
         */
        void RotateObjectRight( void );

        /**
         * Draw the environment mapped duck model.
         * @param aFrame Current frame in animation
         */
        void DrawDuck( TInt aFrame );

        /**
         * Draw the environment mapped head model.
         * @param aFrame Current frame in animation
         */
        void DrawHead( TInt aFrame );

        /**
         * Change to duck model
         */
        void DuckModel( void );

        /**
         * Change to head model
         */
        void HeadModel( void );

        /**
         * Notifies that the screen size has dynamically changed during execution of
         * this program. Resets the viewport to this new size and sets this size
         * to the CTextureManager.
         * @param aWidth New width of the screen.
         * @param aHeight New height of the screen.
         */
        void SetScreenSize( TUint aWidth, TUint aHeight );

  protected:

        /**
         * Standard constructor that must never Leave.
         * Stores the given screen width and height.
         * @param aWidth Width of the screen.
         * @param aHeight Height of the screen.
         */
        CEnvMapping( TUint aWidth, TUint aHeight );

        /**
         * Second phase contructor. Does nothing.
         */
        void ConstructL( void );

  public: // Data

    	/**
    	 * Application states:
    	 * ELoadingTextures - indicates that the app. is loading textures.
    	 * ERunning - indicates that the app. is running.
    	 */
		enum
			{
			ELoadingTextures,
			ERunning
			};

  private:  // Data

    	/** Texture manager that is used to load the used textures. */
        CTextureManager* iTextureManager;
        /** Environment map texture. */
        TTexture iEnvMapTexture;

        /** Width of the screen */
        TUint iScreenWidth;
        /** Height of the screen */
        TUint iScreenHeight;

        /** Num of object faces. Used when calculating texture coords. */
        GLshort iNumFaces;
        /** Num of object vertices. Used when calculating texture coords. */
        GLshort iNumVertices;

        /** Whether object is rotated left or not. */
        TBool iRotateObjectLeftEnabled;
        /** Whether object is rotated right or not. */
        TBool iRotateObjectRightEnabled;
        /** Current rotation angle of the object. */
        GLint iObjectRotAngle;
        /** Currently shown object model (DUCK_MODEL or HEAD_MODEL). */
        GLint iObjectModel;

        /** Vertex normals stored here are used as straight texture coordinates for the duck's environmental mapping. */
        GLshort iDuckTexCoords[duckVertices * 3];
        /** Vertex normals stored here are used as straight texture coordinates for the head's environmental mapping. */
        GLshort iHeadTexCoords[headVertices * 3]; // headVertices == MAX_VERTEX_NUM

        /** Temporary storage of face normals. */
        GLfloat* iFaceNormals;

        /**
         * Tells how many faces are connected to each vertex. This value is used
         * when calculating an average normal for each vertices. That normal is
         * then used are direct texture coordinate value for that particular vertex.
         */
        GLubyte* iNormalCounter;
};

#endif
