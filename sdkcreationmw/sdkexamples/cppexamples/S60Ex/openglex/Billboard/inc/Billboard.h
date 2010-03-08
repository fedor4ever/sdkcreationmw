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


#ifndef __BILLBOARH__
#define __BILLBOARH__

// INCLUDES
#include <e32base.h> // for CBase definition
#include <GLES\gl.h> // OpenGL ES header file
#include "Utils3d.h"
#include "Billboardinput.h"

// MACROS
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
class CBillboard : public CFiniteStateMachine, public MTextureLoadingListener
	{
	public:   // Constructors and destructor

        /**
         * Factory method for creating a new CBillboard object.
         * @param aWidth Width of the screen.
         * @param aHeight Height of the screen.
         * @param iInputHandler Input handler that maps keycodes to inputs and stores the current state for each key.
         */
		static CBillboard* NewL( TUint aWidth, TUint aHeight, CBillboardInput* aInputHandler );

        /**
         * Destructor. Does nothing.
         */
		virtual ~CBillboard();

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
         * @param aTimeSecs Seconds elapsed since the application started running.
         * @param aDeltaTimeSecs Seconds elapsed since last call to AppCycle().
         */
        void AppCycle( GLuint aFrame, GLfloat aTimeSecs, GLfloat aDeltaTimeSecs );

		/**
		 * Draws the ground plane. The plane is always drawn at fixed location.
		 */
		void DrawPlane();

		/**
		 * Draws a tree. The tree is always drawn at a fixed location based on the
		 * row and column, but the orientation of the tree depends on the current
		 * camera angle. Trees are always rotated so that they face the camera.
		 * @param aRow Tree row counter.
		 * @param aCol Tree column counter.
		 */
		void DrawTree( TInt aRow, TInt aCol );

		/**
		 * Draws and animates the clouds based on the time that has elapsed since
		 * last frame.
		 * @param aTimeSecs Total number of seconds elapsed.
		 */
		void DrawCloud( GLfloat aTimeSecs );

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
		 * Turn the camera in azimuth based on the joystick movement or if the automatic
		 * rotation is on based on the current iCamRotation value.
		 * @param aDeltaTimeSecs Time interval that is used as the basis for calculating the movement.
		 */
		void TurnCamera( GLfixed aDeltaTimeSecs );

		/**
		 * Move the camera forwards or backwards based on joystick movement and the delta time.
		 * @param aDeltaTimeSecs Time interval that is used as the basis for calculating the movement.
		 */
		void MoveCamera( GLfixed aDeltaTimeSecs );

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
         * @param iInputHandler Input handler that maps keycodes to inputs and stores the current state for each key.
         */
		CBillboard( TUint aWidth, TUint aHeight, CBillboardInput* aInputHandler );

        /**
         * Second phase contructor. Does nothing.
         */
		void ConstructL( void );

	public:  // Data

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

        /** Width of the screen */
        TUint iScreenWidth;
        /** Height of the screen */
        TUint iScreenHeight;

    	/** Texture manager that is used to load the used textures. */
		CTextureManager * iTextureManager;

		/** Tree texture. */
		TTexture iTreeTexture;
		/** Grass texture. */
		TTexture iGrassTexture;
		/** Cloud texture. */
		TTexture iCloudTexture;
		/** Cloud texture coordinates. */
		GLfixed iCloudTextureCoords[8];
		/** Cloud texture offset. Used to animate the clouds in the sky. */
		GLfixed iTextureOffset;

        /**
         * Input handler that maps keycodes to inputs and stores the current state
         * for each key. Owned by the C#Name#Container.
         */
        CBillboardInput* iInputHandler;
        /**
         * Indicates whether the camera should be automatically rotated around the
         * scene or if the camera is controlled by the joystick's left/right movements.
         */
        TBool iIsAutorotateOn;
		/** Camera. */
		TCamerax iCamera;
		/** Camera azimuth. */
		GLfixed iAzimuth;
		/** Camera distance. */
		GLfixed iDistance;

        /** Vertice coordinates drawing the clouds. */
        GLfloat iCloudVertices[12];

		/** Lookup-table for sin, cos, tan. */
		CLookUpTable * iLookUpTable;
};


#endif      // __BILLBOARH_

// End of File

