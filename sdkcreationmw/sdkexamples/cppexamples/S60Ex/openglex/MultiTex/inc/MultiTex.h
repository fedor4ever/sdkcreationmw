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


#ifndef MULTITEX_H
#define MULTITEX_H

//  INCLUDES
#include <e32base.h>  // for CBase definition
#include <GLES/gl.h>  // OpenGL ES header file
#include <GLES/egl.h> // OpenGL ES header file
#include "Utils3d.h"    // Utilities (state machine, texture manager etc.)
#include "Glutils.h"    // Misc GLU and GLUT functions

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
class CMultiTex : public CFiniteStateMachine, public MTextureLoadingListener
    {
    public:  // Constructors and destructor

        /**
         * Factory method for creating a new CMultiTex object.
         */
        static CMultiTex* NewL( TUint aWidth, TUint aHeight);

        /**
         * Destructor. Does nothing.
         */
        virtual ~CMultiTex();

    public: // New functions

        /**
         * Initializes OpenGL ES, sets the vertex and color
         * arrays and pointers. Also selects the shading mode and
         * creates the CTextureManager instance and starts loading
         * of required textures.
         */
        void AppInitL( TAny* aInstance );

        /**
         * Called upon application exit. Destroys the CTextureManager
         * instance that was created in AppInitL() method.
         */
        void AppExit( void );

        /**
         * Renders one frame with Pbuffer context to Ppbuffer surface.
         * @param aFrame Number of the frame to be rendered.
         */
        void AppCyclePbuffer( TInt aFrame );

        /**
         * Renders one frame with Window context to Window surface.
         * @param aFrame Number of the frame to be rendered.
         */
        void AppCycleWindow( TInt aFrame );

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
         * Calculate light texture for environment map lights.
         */
        void CalculateLightsL( void );

        /**
        * Draws lights with given dimensions.
        * @param  iFrame The current frame number
        */
        void DrawLights( TInt iFrame );

        /**
         * Notifies that the screen size has dynamically changed during execution of
         * this program. Resets the viewport to this new size and sets this size
         * to the CTextureManager.
         * @param aWidth New width of the screen.
         * @param aHeight New height of the screen.
         */
        void SetScreenSize( TUint aWidth, TUint aHeight );

    protected: // New functions

        /**
         * Standard constructor that must never Leave.
         * Stores the given screen width and height.
         * @param aWidth Width of the screen.
         * @param aHeight Height of the screen.
         */
        CMultiTex( TUint aWidth, TUint aHeight);

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


    private: // Data

    	/** Texture manager that is used to load the used textures. */
    	CTextureManager * iTextureManager;
        /** Width of the screen */
        TUint iScreenWidth;
        /** Height of the screen */
        TUint iScreenHeight;
        /** Aspect ratio of the screen (iScreenHeight / iScreenWidth). */
        GLfloat iAspectRatio;

    	/** World texture. */
    	TTexture iWorldTexture;
        /** Pointer to the light texture data. */
        TUint8* iLightTex;
        /** List for light and env map texture objects. */
        GLuint iLightTexObjects[2];
    };
#endif // MULTITEX_H

// End of File
