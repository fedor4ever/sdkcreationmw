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


#ifndef __SLIDESHOWRENDERERSH__
#define __SLIDESHOWRENDERERSH__

//  INCLUDES
#include <GLES/gl.h>
#include "Textureutils.h"

/**
 *  Static background renderer.
 *
 *  This class draws a textured polygon that covers the entire screen.
 */
class CStaticRenderer
    {
    public:
    /**
     *  Draw a textured polygon that fills the entire screen.
     *  @param texture The texture for the polygon.
     *  @param filter  Enable bilinear filtering to improve image quality
     */
    void Render(const TTexture& texture, TBool filter);
    };

/**
 *  Transition renderer interface.
 *
 *  A transition renderer draws the effect of one picture gradually
 *  changing into another. The actual effect depends on the particular
 *  implementation of the interface.
 */
class MTransitionRenderer
    {
    public:
    /**
        *  Draw a gradually appearing picture. The previous picture
        *  is assumed to already be on the screen. @see CStaticRenderer.
        *  @param texture The picture that should appear.
        *  @param ratio Picture visibility in the range 0..1. A ratio of 0
        *               means that the picture is not visible at all, while
        *               1 means the new picture fully covers the screen just
        *               as if it was drawn with CStaticRenderer.
        */
    virtual void Render(const TTexture& texture, GLfloat ratio) = 0;
    };

/**
 *  A mask transition effect.
 *
 *  This transition effect uses a mask texture to fade in a picture. It uses
 *  OpenGL ES texture environment combiners to modulate the picture with the
 *  mask.
 */
class CMaskTransition: public MTransitionRenderer
    {
    public:
        /**
         *  Different mask sweeping styles supported by the effect.
         */
        enum TSweepStyle
            {
            ESweepHorizontal,
            ESweepVertical,
            ESweepCurved,
            ESweepCurved2,
            };

        /**
         *  Constructor.
         *  @param mask Mask texture. The texture should be in RGBA format,
         *              opaque white at the top and fully transparent at
         *              the bottom.
         *  @param sweepStyle Sweep style to use for moving the mask.
         */
        CMaskTransition(const TTexture& mask, TSweepStyle sweepStyle);
        virtual void Render(const TTexture& texture, GLfloat ratio);
    private:
        /** Selected sweep style for this instance */
        TSweepStyle iSweepStyle;
        /** Mask texture */
        const TTexture& iMask;
    };

/**
 *  A whisk transition.
 *
 *  This transition introduces a new image by rapidly moving it on-screen
 *  from the left or right.
 */
class CWhiskTransition: public MTransitionRenderer
    {
    public:
        /**
         *  Direction to move the new picture to.
         */
        enum TDirection
            {
            ELeft,
            ERight
            };

        /**
         *  Constructor.
         *  @param dir Chooses whether the picture should come in from
         *             the left or right.
         */
        CWhiskTransition(TDirection dir);
        virtual void Render(const TTexture& texture, GLfloat ratio);
    private:
        /** Picture entry direction */
        TDirection iDir;
    };

/**
 *  A curtain transition.
 *
 *  This transition brings in a new picture as if it was drawn on a
 *  hanging curtain.
 */
class CCurtainTransition: public MTransitionRenderer
    {
    public:
        /**
         *  The side of the screen the curtain enters from.
         */
        enum TDirection
            {
            ELeft,
            ERight
            };

        /**
         *  Constructor.
         *  @param dir Chooses whether the curtain enters from
         *             the left or right side of the screen.
         */
        CCurtainTransition(TDirection dir);
        virtual void Render(const TTexture& texture, GLfloat ratio);
    private:
        /** Curtain entry direction */
        TDirection iDir;
    };

#endif

// End of File

