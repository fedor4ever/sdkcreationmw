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


#ifndef __LENSFLAREH__
#define __LENSFLAREH__

// INCLUDES
#include <e32base.h> // for CBase definition
#include <GLES\gl.h> // OpenGL ES header file
#include "Utils3d.h"

// CONSTANTS

// MACROS
#define FRUSTUM_LEFT   -1.f     //left vertical clipping plane
#define FRUSTUM_RIGHT  +1.f     //right vertical clipping plane
#define FRUSTUM_BOTTOM -1.f     //bottom horizontal clipping plane
#define FRUSTUM_TOP    +1.f     //top horizontal clipping plane
#define FRUSTUM_NEAR   +3.f     //near depth clipping plane
#define FRUSTUM_FAR    +1000.f  //far depth clipping plane

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CLensFlareEffect;

// CLASS DECLARATION

/**
 * Class that does the actual OpenGL ES rendering.
 * Subclass of CFiniteStateMachine because the application
 * needs to handle loading textures and running states.
 */
class CLensFlare : public CFiniteStateMachine, public MTextureLoadingListener
	{
	public:

        /**
         * Factory method for creating a new CLensflare object.
         */
		static CLensFlare* NewL( TUint aWidth, TUint aHeight);

        /**
         * Destructor. Does nothing.
         */
		virtual ~CLensFlare();

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
        void AppCycle( TInt aFrame, TReal aTimeSecs, TReal aDeltaTimeSecs );

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
		 * Draw the text.
		 * @param aReflection Flag that specifies if we are drawing the reflected text or not.
		 */
		void DrawText(TBool aReflection);

		/**
		 * Draw the ground plane.
		 */
		void DrawPlane();

		/**
		 * Turn the camera in azimuth.
		 * @param aDirection Direction to turn camera
		 * @param aTime Time
		 */
		void TurnCameraAzimuth(GLint aDirection, GLfloat aTime);

		/**
		 * Change the camera elevation.
		 * @param aDirection Direction to turn camera
		 * @param aTime Time.
		 */
		void TurnCameraElevation(GLint aDirection, GLfloat aTime);

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
		CLensFlare( TUint aWidth, TUint aHeight );

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


		/** Camera movement direction. */
		GLbyte iDirection;

	private: // Data

    	/** Texture manager that is used to load the used textures. */
		CTextureManager * iTextureManager;
        /** Width of the screen */
        TUint iScreenWidth;
        /** Height of the screen */
        TUint iScreenHeight;

		/** Lens-flare effect. */
		CLensFlareEffect * iLensFlareEffect;

		/** Trigonometric functions lookup-table. */
		CLookUpTable * iLookUpTable;

		/** Camera. */
		TCamera iCamera;

		/** Azimuth angle for camera. */
		GLfloat iAzimuth;
		/** Elevation angle for camera. */
		GLfloat iElevation;
		/** Camera distance. */
		GLfloat iDistance;
		/** Angle of the lensflare effect. */
		GLfloat iLensAngle;
		/** Lens radius. */
		GLfloat iLensRadius;

	};

// =============================================================================
// =============================================================================

/**
 *  This structure is used internally by the CLensFlareEffect class.
 *  It stores information that describes a component of a lens flare.
 */
struct TFlareConfig
	{
	/** Index of the texture used by this element. */
	TUint8 iIndex;
	/** Length scaling. */
	GLfloat iLengthScale;
	/** Texture scaling. */
	GLfloat iImageScale;
	};

// =============================================================================
// =============================================================================

/**
 *  Implementation of a simple lens flare effect.
 */
class CLensFlareEffect: public CBase
	{
	public: // Constructors and destructor

		/**
		 * Factory method that creates and initializes a CLensFlareEffect object with the given parameters.
		 * @param aTextureNames Array of OpenGL texture names for textures that will be used for the lens flare effect.
		 * @param aTextureCount Length of aTextureNames.
		 * @param aFlareConfigs Array of TFlareConfig objects that define the components of the lens flare effect.
		 * @param aFlareConfigCount Length of aFlareConfigs.
		 * @param aTextureManager Pointer to the CTextureManager object in use. This texture manager
		 *        will be used to load the textures and to blit them to the screen.
		 * @param aScreenWidth Width (in pixel) of the screen.
		 * @param aScreenHeight Height (in pixel) of the screen.
		 */
		static CLensFlareEffect* NewL(
			const TBufC<MAX_TEXTURE_NAME> * aTextureNames,
			TUint8 aTextureCount,
			const TFlareConfig * aFlareConfigs,
			TUint8 aFlareConfigCount,
			CTextureManager * aTextureManager,
			GLuint iScreenWidth,
			GLuint iScreenHeight);

		/**
		 * Destructor. Destroys all the textures loaded for the lens flare effect.
		 */
		virtual ~CLensFlareEffect();

	protected:
		/**
		 * Default C++ and Symbian ConstructL are placed in protected
		 * area to force the use of NewL when creating an instance.
		 */
		CLensFlareEffect();

		/**
		 * Constructs and initializes a CLensFlareEffect object with the given parameters.
		 * @param aTextureNames Array of OpenGL texture names for textures that will be used for the lens flare effect.
		 * @param aTextureCount Length of aTextureNames.
		 * @param aFlareConfigs Array of TFlareConfig objects that define the components of the lens flare effect.
		 * @param aFlareConfigCount Length of aFlareConfigs.
		 * @param aTextureManager Pointer to the CTextureManager object in use. This texture manager
		 *        will be used to load the textures and to blit them to the screen.
		 * @param aScreenWidth Width (in pixel) of the screen.
		 * @param aScreenHeight Height (in pixel) of the screen.
		 */
		void ConstructL( const TBufC<MAX_TEXTURE_NAME> * aTextureNames,
			TUint8 aTextureCount,
			const TFlareConfig * aFlareConfigs,
			TUint8 aFlareConfigCount,
			CTextureManager * aTextureManager,
			GLuint iScreenWidth,
			GLuint iScreenHeight);
	public:

		/**
 		 * Renders the lensflare effect at a given screen coordinates.
		 * @param aScreenX Screen X coordinate.
		 * @param aScreenY Screen Y coordinate.
		 * @param aImageScale Flare scale multiplier.
		 */
		void DrawAt(GLuint aSceenX, GLuint aScreenY,  GLfloat aImageScale);

	private: // Data
		/** Array of textures used by this lens flare effect. */
		TTexture *iTextures;
		/** Length of iTextures. */
		TUint8 iTextureCount;
		/**
		 * Array of TFlareConfig definning the components of the lens flare effect.
		 * Note that these are owned by the user of the CLensFlareEffect class and
		 * are NOT destroyed when CLensFlareEffect is destroyed.
		 */
		const TFlareConfig * iFlareConfigs;
		/** Number of TFlareConfigs stored in iFlareConfigs. */
		TUint8 iFlareConfigCount;
		/**
		 * Texture manager that is used to load the used textures and to blit them to the screen.
		 * Note that this is owned by the user of the CLensFlareEffect class and
		 * is NOT destroyed when CLensFlareEffect is destroyed.
		 */

		CTextureManager * iTextureManager;
		/** X-coordinate of the center of the screen. */
		GLuint iCenterX;
		/** Y-coordinate of the center of the screen. */
		GLuint iCenterY;
	};


#endif

// End of File
