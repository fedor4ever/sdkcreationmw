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


#ifndef __RAINH__
#define __RAINH__

//  INCLUDES

#include <e32base.h> // for CBase definition
#include <GLES\gl.h>      // OpenGL ES header file
#include "Glutils.h"    // Misc GLU and GLUT functions
#include "Utils3d.h"

// MACROS

#define FRUSTUM_LEFT   -1.f     //left vertical clipping plane
#define FRUSTUM_RIGHT  +1.f     //right vertical clipping plane
#define FRUSTUM_BOTTOM -1.f     //bottom horizontal clipping plane
#define FRUSTUM_TOP    +1.f     //top horizontal clipping plane
#define FRUSTUM_NEAR   +3.f     //near depth clipping plane
#define FRUSTUM_FAR    +1000.f  //far depth clipping plane

// FORWARD DECLARATIONS

class CRainfall;

/**
 * Class that does the actual OpenGL ES rendering.
 * Subclass of CFiniteStateMachine because the application
 * needs to handle loading textures and running states.
 */
class CRain
	: public CFiniteStateMachine, public MTextureLoadingListener
	{
	public: // Constructors and destructor

        /**
         * Factory method for creating a new CRain object.
         */
		static CRain* NewL( TUint aWidth, TUint aHeight );

        /**
         * Destructor. Does nothing.
         */
		virtual ~CRain();

	public:
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
         * @param aTimeSecs Seconds elapsed since the application started running.
         * @param aDeltaTimeSecs Seconds elapsed since last call to AppCycle().
         */
        void AppCycle( GLuint aFrame, GLfloat aTimeSecs, GLfloat aDeltaTimeSecs );

    	/**
    	 * Called when the finite state machine enters a new state.
    	 * Does nothing in this implementation.
    	 * @param aState State that is about to be entered.
    	 */
		void OnEnterStateL( TInt aState);

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
		 * Draws the ground plane.
		 */
		void DrawPlane();

		/**
		 * Draws a tree at the given location.
		 * @param aX X-coordinate for the tree.
		 * @param aZ Z-coordinate for the tree.
		 */
		void DrawTree(GLfloat aX, GLfloat aZ);

		/**
		 * Draws the clouds.
		 * @param aDeltaTimeSecs Time that has elapsed since last frame.
		 */
		void DrawCloud(GLfloat aTimeSecs);

		/**
		 * Returns pointer to the particle-engine effect
		 * @return Effect
		 */
		CRainfall* GetRainfall();

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
		CRain( TUint aWidth, TUint aHeight );

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



	private: // Data

    	/** Texture manager that is used to load the used textures. */
		CTextureManager * iTextureManager;
        /** Width of the screen */
        TUint iScreenWidth;
        /** Height of the screen */
        TUint iScreenHeight;

		/** Tree texture. */
		TTexture iTreeTexture;
		/** Ground texture. */
		TTexture iGrassTexture;
		/** Cloud texture. */
		TTexture iCloudTexture;
		/** Cloud texture coordinates. */
		GLfloat iCloudTextureCoords[8];
		/** Cloud texture offset. */
		GLfloat iTextureOffset;

		/** Rainfall particle effect. */
		CRainfall * iRainfall;

		/** Camera. */
		TCamera iCamera;

        /** Vertice coordinates drawing the clouds. */
        GLfloat iCloudVertices[12];
	};

/**
 * Rainfall particle effect class. Raindrops are released from within the rectangle
 * (iPosition.iX-iWidth / 2, iPosition.iY, iPosition.iX-iWidth / 2)
 * (iPosition.iX+iWidth / 2, iPosition.iY, iPosition.iX+iWidth / 2).
 * Once particle's y coordinate is below (smaller than) iGroundLevel, it is reset.
 */
class CRainfall: public CParticleEngine
{
	public:
        /**
         * Factory method for creating a new CRain object.
         * @param aParticlesCount Number of particles used to create the rain effect.
         * @param aPosition Position of the rain particle generator.
         * @param aWidth Width of the rain area.
         * @param aDepth Height of the rain area.
         * @param aGroundLevel Ground level after which particle is reset back to beginning.
         */
		static CRainfall* NewL(GLint aParticlesCount, TVector aPosition,
							   GLfloat aWidth, GLfloat aDepth, GLfloat aGroundLevel);
		/** Destructor. Does nothing in this implementation. */
		virtual ~CRainfall();

	protected:

        /**
         * Standard constructor that must never Leave.
         * Does nothing in this implementation.
         */
		CRainfall();

        /**
         * Second phase contructor. Constructs the rain effect using the given parameters.
         * @param aParticlesCount Number of particles used to create the rain effect.
         * @param aPosition Position of the rain particle generator.
         * @param aWidth Width of the rain area.
         * @param aDepth Height of the rain area.
         * @param aGroundLevel Ground level after which particle is reset back to beginning.
         */
		void ConstructL(GLint aParticlesCount, TVector aPosition,
						GLfloat aWidth, GLfloat aDepth, GLfloat aGroundLevel);


	public: // New functions

	    /**
	     * Returns a random float number.
	     * @return Random float number.
	     */
		GLfloat Random();

	public: // Functions from base classes

		/**
		 * Implementation of CParticleEngine::ResetParticle() method
		 * Resets the given particle to the rain "start plane" and gives it random location inside
		 * the rain area (defined when creating this effect) and random downwards speed.
		 * @param aIndex Particle to be reset.
		 */
		void ResetParticle(GLint aIndex);

		/**
		 * Implementation of CParticleEngine::UpdateEngine() method.
		 * Moves all the particles according to their current velocity vector and resets all particles
		 * that have fallen beneath the ground level back to the beginning plain.
		 * @param aElapsedTime Elapsed time since last frame. Used when calculating how much
		 *        each particle has moved.
		 */
		void UpdateEngine(GLfloat aElapsedTime);

		/**
		 * Implementation of CParticleEngine::RenderEngine().
		 * Renders each particle to the screen as simple lines.
		 */
		void RenderEngine(TCamera &aCamera);

	public: // Data

		/** Width of the particle source area. This defines the width of the area where the rain falls. */
		GLfloat iWidth;
		/** Depth of the particle source area. This defines the depth of the area where the rain falls. */
		GLfloat iDepth;
		/** Ground level that defines the plane after which the particles are reset back to the source plain. */
		GLfloat iGroundLevel;

	protected: // Data

		/** Seed for the random generator (the time when ConstructL was called). */
		TInt64 iSeed;
};

#endif

// End of File

