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


#ifndef MIPMAPPING_H
#define MIPMAPPING_H

//  INCLUDES

#include <e32base.h> // for CBase definition
#include <GLES\gl.h>      // OpenGL ES header file
#include "Textureutils.h"
#include "Mathutils.h"
#include "Utils3d.h"

// MACROS

/** Split the screen horizontally to highlight the benefit of mipmapping */
//#define MIPMAPPING_SPLIT_SCREEN

// FORWARD DECLARATIONS

/**
 * Class that does the actual OpenGL ES rendering.
 * Subclass of CFiniteStateMachine because the application
 * needs to handle loading textures and running states.
 */
class CMipmapping
	: public CFiniteStateMachine, public MTextureLoadingListener
	{
	public: // Constructors and destructor

        /**
         * Factory method for creating a new CMipmapping object.
		 * Calls the hidden constructors.
         */
		static CMipmapping* NewL( TUint aWidth, TUint aHeight, TUint aSegments = 16, TUint aSlices = 7 );

        /**
         * Destructor. Does nothing.
         */
		virtual ~CMipmapping();

	protected: // Hidden constructors

        /**
         * Standard constructor that must never Leave.
         * Stores the given screen width and height.
         * @param aWidth Width of the screen.
         * @param aHeight Height of the screen.
         * @param aSegments Number of segments in the tunnel.
         * @param aSlices Number of slices in one segment of the tunnel.
         */
		CMipmapping( TUint aWidth, TUint aHeight, TUint aSegments, TUint aSlices );

        /**
         * Second phase constructor that can call methods that may leave. Does nothing.
         */
		void ConstructL( void );


	public: // New methods

		enum EFilteringMode
		{
        EMipmappedNearestNeighbourFiltering,
		EMipmappedBilinearFiltering,
		EMipmappedTrilinearFiltering,
        ENearestNeighbourFiltering,
		EBilinearFiltering,
		};

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

        /**
         *  Set texture filtering mode.
         */
        void SetFilteringMode( EFilteringMode aMode );

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
         *  Update the tunnel vertices based on the current time.
         *  @param aTime Time parameter.
         */
        void UpdateTunnel( TReal aTime );

        /**
         *  The tunnel function calculates the coordinates and radius of the tunnel at a given point
         *  aTime with the animation phase aPhase.
         *  @param aTime Time parameter.
         *  @param aPhase Phase parameter. This controls the animation of the tunnel.
         *  @param aPos Coordinates of the tunnel (output).
         *  @param aRadius Radius of the tunnel (output).
         */
        void TunnelFunction( TReal aTime, TReal aPhase, TReal aPos[3], TReal& aRadius) const;

        /**
         *  Converts a real number to fixed point format.
         *  @param aReal Number to convert.
         *  @returns the given number in fixed point format.
         */
        inline static GLfixed RealToFixed( TReal aReal );

	private: // Data

    	/** Texture manager that is used to load the used textures. */
		CTextureManager * iTextureManager;
        /** Width of the screen */
        TUint iScreenWidth;
        /** Height of the screen */
        TUint iScreenHeight;
        /** Number of segments in the tunnel */
        TUint iSegments;
        /** Number of slices in one segment of the tunnel  */
        TUint iSlices;
        /** Tunnel texture */
        TTexture iTexture;
        /** Vertices of the tunnel  */
        GLfixed * iVertices;
        /** Normals of the tunnel */
        GLfixed * iNormals;
        /** Texture coordinates of the tunnel  */
        GLfixed * iTexCoords;
        /** Indices for the tunnel triangle strips  */
        GLshort * iIndices;
        /** Texture filtering mode */
        EFilteringMode iFilteringMode;
	};

#endif

// End of File

