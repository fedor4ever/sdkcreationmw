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


#ifndef __FIREFLYH__
#define __FIREFLYH__

//  INCLUDES

#include <e32base.h>  // for CBase definition
#include <GLES\gl.h>  // OpenGL ES header file
#include "Utils3d.h"

/**
 * Class that represents the firefly that is light source for the bumpmapping.
 */
class CFireFly
	{
	public: // Constructors and destructor

        /**
         * Factory method for creating a new FireFly object.
		 * Calls the hidden constructors.
         * @param aLeft Left boundary of the rectangular area where this firefly should move in.
         * @param aRight Right boundary of the rectangular area where this firefly should move in.
         * @param aBottom Bottom boundary of the rectangular area where this firefly should move in.
         * @param aTop Top boundary of the rectangular area where this firefly should move in.
		 * @param aMinZCoord Minimum Z-coordinate for the firefly.
		 * @param aMaxZCoord Maximum Z-coordinate for the firefly.
		 * @param aTexManager Texture manager that is used to load the required textures for the firefly.
         */
		static CFireFly* NewL( GLfloat aLeft, GLfloat aRight, GLfloat aBottom, GLfloat aTop,
		                       GLfloat aMinZCoord, GLfloat aMaxZCoord, CTextureManager* aTexManager );

        /**
         * Destructor. Destroys the position, target and speed vectors.
         */
		virtual ~CFireFly();

	protected: // Hidden constructors

        /**
         * Standard constructor that must never Leave.
         * Stores the given parameters to class attributes.
         * @param aLeft Left boundary of the rectangular area where this firefly should move in.
         * @param aRight Right boundary of the rectangular area where this firefly should move in.
         * @param aBottom Bottom boundary of the rectangular area where this firefly should move in.
         * @param aTop Top boundary of the rectangular area where this firefly should move in.
		 * @param aMinZCoord Minimum Z-coordinate for the firefly.
		 * @param aMaxZCoord Maximum Z-coordinate for the firefly.
         */
		CFireFly( GLfloat aLeft, GLfloat aRight, GLfloat aBottom, GLfloat aTop, GLfloat aMinZCoord, GLfloat aMaxZCoord );

        /**
         * Second phase constructor that can call methods that may leave.
		 * @param aTexManager Texture manager that is used to load the required textures for the firefly.
         */
		void ConstructL( CTextureManager* aTexManager );

	public: // New methods

		/**
		 * Calculates the movement of the firefly.
         * @param aFrame Number of the frame to be rendered.
		 * @param aTimeSecs Seconds elapsed since the application started running.
		 * @param aDeltaTimeSecs Seconds elapsed since last call to AppCycle().
		 */
		void DoMovement( GLuint aFrame, GLfloat aTimeSecs, GLfloat aDeltaTimeSecs );

		/**
		 * Draws the firefly at it's current position.
		 */
		void Draw();

	public: // Data

		/** Current position of the firefly. Modified in doMovement() method and also read by the CBumpMap class. */
		TVector iPosition;

	private: // Data

        /** Current target position of the firefly. */
        TVector iTarget;
		/** Current speed vector of the firefly. Used in doMovement() method to move the firefly. */
		TVector iSpeed;
		/** Texture that is used to draw the "glow" around the firefly. */
		TTexture iGlowTexture;

		/** Left boundary of the rectangular area where this firefly moves in. */
        GLfloat iLeft;
		/** Right boundary of the rectangular area where this firefly moves in. */
        GLfloat iRight;
		/** Bottom boundary of the rectangular area where this firefly moves in. */
        GLfloat iBottom;
		/** Top boundary of the rectangular area where this firefly moves in. */
        GLfloat iTop;
		/** Minimum Z-coordinate of the firefly. */
        GLfloat iMinZ;
		/** Maximum Z-coordinate of the firefly. */
        GLfloat iMaxZ;
		/** Seed for the random generator (initially the time when ConstructL was called). */
		TInt64 iSeed;
	};

#endif

// End of File