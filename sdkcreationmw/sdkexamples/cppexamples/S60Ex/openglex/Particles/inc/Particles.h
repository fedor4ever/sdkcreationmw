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


#ifndef __PARTICLESH__
#define __PARTICLESH__

//  INCLUDES

#include <e32base.h> // for CBase definition
#include <GLES\gl.h>      // OpenGL ES header file
#include "Textureutils.h"
#include "Mathutils.h"
#include "Utils3d.h"

// FORWARD DECLARATIONS

/**
 * Class that does the actual OpenGL ES rendering.
 * Subclass of CFiniteStateMachine because the application
 * needs to handle loading textures and running states.
 */
class CParticles
    : public CFiniteStateMachine, public MTextureLoadingListener
    {
    public: // Constructors and destructor

        /**
         * Factory method for creating a new CParticles object.
         * Calls the hidden constructors.
         */
        static CParticles* NewL( TUint aWidth, TUint aHeight, TUint aParticleCount = 64 );

        /**
         * Destructor. Does nothing.
         */
        virtual ~CParticles();

    protected: // Hidden constructors

        /**
         * Standard constructor that must never Leave.
         * Stores the given screen width and height.
         * @param aWidth Width of the screen.
         * @param aHeight Height of the screen.
         * @param aParticleCount Particle count.
         */
        CParticles( TUint aWidth, TUint aHeight, TUint aParticleCount );

        /**
         * Second phase constructor that can call methods that may leave. Does nothing.
         */
        void ConstructL( void );


    public: // New methods

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
        void AppCycle( TInt aFrame, TReal aTimeSecs, TReal aDeltaTimeSecs );

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
         * Notifies that the screen size has dynamically changed during execution of
         * this program. Resets the viewport to this new size and sets this size
         * to the CTextureManager.
         * @param aWidth New width of the screen.
         * @param aHeight New height of the screen.
         */
        void SetScreenSize( TUint aWidth, TUint aHeight );

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

    private: // Methods
        /**
         *  Animate the particles.
         *  @param aTime Animation time parameter (seconds).
         */
        void UpdateParticles( TReal aTime );

        /**
         *  Draw the particles.
         */
        void DrawParticles( TReal aTime );

        /**
         *  Draw a feedback effect.
         *  @param aTime Animation time parameter (seconds).
         */
        void DrawFeedbackEffect( TReal aTime );

        /**
         *  Update the feedback effect based on the contents of the framebuffer.
         */
        void UpdateFeedbackEffect();

    private: // Data

        /** Texture manager that is used to load the used textures. */
        CTextureManager *iTextureManager;
        /** Width of the screen */
        TUint iScreenWidth;
        /** Height of the screen */
        TUint iScreenHeight;
        /** Camera. */
        TCamera iCamera;
        /** Particle texture */
        TTexture iParticleTexture;
        /** Particle count */
        TUint iParticleCount;
        /** Particle coordinates */
        GLfixed *iParticleCoords;
        /** Particle colors */
        GLfixed *iParticleColors;
        /** Feedback texture handle */
        GLuint iFeedbackTextureID;
        /** Feedback texture width */
        TInt iFeedbackTextureWidth;
        /** Feedback texture height */
        TInt iFeedbackTextureHeight;
    };

#endif

// End of File

