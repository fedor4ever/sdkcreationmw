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


#ifndef TEXTURE_H
#define TEXTURE_H

// INCLUDES
#include <e32base.h> // for CBase definition
#include <GLES/gl.h> // OpenGL ES header file
#include "Utils3d.h" // Utilities (texmanager, textures etc.)
#include "Glutils.h" // Misc GLU and GLUT functions

// MACROS
#define TEX_WIDTH     256 // Texture size must be power of two and max size
#define TEX_HEIGHT    256 // for textures is 256 x 256

#define FRUSTUM_LEFT    -1.f  //left vertical clipping plane
#define FRUSTUM_RIGHT    1.f  //right vertical clipping plane
#define FRUSTUM_BOTTOM  -1.f  //bottom horizontal clipping plane
#define FRUSTUM_TOP      1.f  //top horizontal clipping plane
#define FRUSTUM_NEAR     3.f  //near depth clipping plane
#define FRUSTUM_FAR   1000.f  //far depth clipping plane

// CLASS DECLARATION

/**
 * Class that does the actual OpenGL ES rendering.
 * Subclass of CFiniteStateMachine because the application
 * needs to handle loading textures and running states.
 */
class CTexture : public CFiniteStateMachine, public MTextureLoadingListener
    {
    public:  // Constructors and destructor

        /**
         * Factory method for creating a new CSimpleCube object.
         */
        static CTexture* NewL( TUint aWidth, TUint aHeight );

        /**
         * Destructor. Does nothing.
         */
        virtual ~CTexture();

    public:  // New functions

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
         * Toggles perspective correction on and off.
         */
        void TogglePerspective( void );


        /**
         * Toggles texture wrapping on and off.
         */
        void ToggleWrapping( void );


        /**
         * Toggles lighting on and off.
         */
        void ToggleLighting( void );


        /**
         * Toggles blending enabled and disabled.
         */
        void ToggleBlending( void );

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
        CTexture( TUint aWidth, TUint aHeight );

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

        /** Whether perspective correction is used or not. */
        TBool iPerspectiveCorrEnabled;

        /** Whether texture wrapping is on or off. */
        TBool iTextureWrapEnabled;

        /** Whether lighting is on or off. */
        TBool iLightingEnabled;

        /** Whether blending is enabled or disabled. */
        TBool iBlendingEnabled;

    private:  // Data

    	/** Texture manager that is used to load the used textures. */
    	CTextureManager * iTextureManager;
        /** Width of the screen */
        TUint iScreenWidth;
        /** Height of the screen */
        TUint iScreenHeight;
    	/** Nasa Hubble space telescope texture. */
    	TTexture iNasaHubble;
    	/** "OpenGL ES" texture. */
    	TTexture iOpenGLES;
    };

#endif
