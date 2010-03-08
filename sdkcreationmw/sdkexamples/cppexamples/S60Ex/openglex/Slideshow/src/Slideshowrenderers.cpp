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


// INCLUDE FILES

#include "Slideshowrenderers.h"
#include <e32math.h>

// ----------------------------------------------------------------------------
// Static background renderer
// ----------------------------------------------------------------------------

void CStaticRenderer::Render(const TTexture& texture, TBool filter)
    {
    GLfixed tw = 1 << 16;
    GLfixed th = 1 << 16;
    GLfixed texCoords[] =
        {
        0,  th,
        tw, th,
        0,  0,
        tw, 0,
        };
    GLfixed vertices[] =
        {
        0,       0,
        1 << 16, 0,
        0,       1 << 16,
        1 << 16, 1 << 16,
        };

    // Set orthogonal projection
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrthof(0, 1, 0, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Set texture parameters
    glBindTexture(GL_TEXTURE_2D, texture.iID);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter ? GL_LINEAR : GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter ? GL_LINEAR : GL_NEAREST);

    // Draw the textured quad
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer( 2, GL_FIXED, 0, texCoords );
    glVertexPointer( 2, GL_FIXED, 0, vertices );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW);
    }

// ----------------------------------------------------------------------------
// Mask transition renderer
// ----------------------------------------------------------------------------

CMaskTransition::CMaskTransition(const TTexture& mask, TSweepStyle sweepStyle):
    iSweepStyle(sweepStyle), iMask(mask)
    {
    }

void CMaskTransition::Render(const TTexture& texture, float ratio)
    {
    GLfixed tw = 1 << 16;
    GLfixed th = 1 << 16;
    GLfixed texCoords[] =
        {
        0,  th,
        tw, th,
        0,  0,
        tw, 0,
        };
    GLfixed vertices[] =
        {
        0,       0,
        1 << 16, 0,
        0,       1 << 16,
        1 << 16, 1 << 16,
        };

    // Set orthogonal projection
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrthof(0, 1, 0, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Set up blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Assign the mask texture to the first texture unit
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, iMask.iID);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer( 2, GL_FIXED, 0, texCoords );

    // Transform the texture matrix according to the selected
    // sweep style. The transformation is animated based on the
    // ratio parameter.
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();

    switch (iSweepStyle)
        {
        case ESweepCurved:
            ratio = 1 - ratio;
            glScalef(.5, .5, .5);
            glTranslatef(0 * sin(2 * ratio * PI) + .5, 1 * cos(ratio * PI) + .5, 0);
            glRotatef(22.5 * sin(ratio * PI), 0, 0, 1);
            break;
        case ESweepCurved2:
            ratio = 1 - ratio;
            ratio += 0.5;
            glScalef(.5, .5, .5);
            glTranslatef(0 * cos(2 * ratio * PI) + .5, 1 * sin(ratio * PI) + .5, 0);
            glRotatef(22.5 * cos(ratio * PI), 0, 0, 1);
            break;
        case ESweepHorizontal:
            glRotatef(90, 0, 0, 1);
            glTranslatef(2 * ratio - 1, -1, 0);
            break;
        case ESweepVertical:
            glTranslatef(0, 2 * ratio - 1, 0);
            break;
        }

    glMatrixMode(GL_MODELVIEW);

    // Assign the picture to the second texture unit
    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture.iID);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Set full transparency as the texture environment color
    GLfloat c[] = {1, 1, 1, 0};
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, c);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);

    /*
     * Set up the texture environment combiner. Interpolation
     * combining is used here, so that the output is a blend
     * of the picture and full transparency based on the mask
     * texture. The blending equation is:
     *
     * output = image * mask + transparent * (1 - mask)
     */
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);
    glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE);         // picture
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_CONSTANT);        // transparent white
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_SRC2_RGB, GL_PREVIOUS);        // mask
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_COLOR);

    // Set the same combiner for the alpha channel
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_INTERPOLATE);
    glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_TEXTURE);         // picture
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
    glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_ALPHA, GL_CONSTANT);        // transparent white
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
    glTexEnvi(GL_TEXTURE_ENV, GL_SRC2_ALPHA, GL_PREVIOUS);        // mask
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_ALPHA, GL_SRC_ALPHA);

    /*
     * Set up vertex pointers for the second texture unit. Note
     * That glClientActiveTexture() is used instead of
     * glActiveTexture(), because vertex and texture coordinate
     * pointers are client side features.
     */
    glClientActiveTexture(GL_TEXTURE1);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer( 2, GL_FIXED, 0, texCoords );
    glVertexPointer( 2, GL_FIXED, 0, vertices );

    // Draw the textured quad
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    glColor4f(1, 1, 1, 1);

    // Reset the state
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glActiveTexture(GL_TEXTURE0);
    glClientActiveTexture(GL_TEXTURE0);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW);
    }

// ----------------------------------------------------------------------------
// Whisk transition renderer
// ----------------------------------------------------------------------------

CWhiskTransition::CWhiskTransition(TDirection dir):
    iDir(dir)
    {
    }

void CWhiskTransition::Render(const TTexture& texture, float ratio)
    {
    glBindTexture(GL_TEXTURE_2D, texture.iID);
    GLfixed tw = 1 << 16;
    GLfixed th = 1 << 16;
    GLfixed texCoords[] =
        {
        0,  th,
        tw, th,
        0,  0,
        tw, 0,
        };
    GLfixed vertices[] =
        {
        0,       0,
        1 << 16, 0,
        0,       1 << 16,
        1 << 16, 1 << 16,
        };

    // Set orthogonal projection
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrthof(0, 1, 0, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Move the picture
    glTranslatef(((iDir == ELeft) ? -1 : 1) * ratio * ratio, 0, 0);

    // Set texture parameters
    glBindTexture(GL_TEXTURE_2D, texture.iID);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Set up blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1, 1, 1, 1 - ratio);

    // Draw the textured quad
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer( 2, GL_FIXED, 0, texCoords );
    glVertexPointer( 2, GL_FIXED, 0, vertices );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // Reset the state
    glDisable(GL_BLEND);
    glColor4f(1, 1, 1, 1);
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW);
    }

// ----------------------------------------------------------------------------
// Curtain transition renderer
// ----------------------------------------------------------------------------

CCurtainTransition::CCurtainTransition(TDirection dir):
    iDir(dir)
    {
    }

void CCurtainTransition::Render(const TTexture& texture, float ratio)
    {
    glBindTexture(GL_TEXTURE_2D, texture.iID);
    const TInt resolution = 8;
    GLfixed vertices[resolution * resolution * 2];
    GLfixed texCoords[resolution * resolution * 2];
    GLshort indices[resolution * (resolution - 1) * 2 + (resolution - 2) * 4];

    // Build triangle strip indices
    TInt x, y;
    GLshort *i = indices;

    for (y = 0; y < resolution - 1; y++)
        {
        for (x = 0; x < resolution; x++)
            {
            *i++ = y * resolution + x;
            *i++ = (y + 1) * resolution + x;
            }

        // Insert degenerate triangles to join triangle strips
        if (y < resolution - 1)
            {
            *i = *(i - 1); i++;
            *i = *(i + 1) = (y + 1) * resolution; i += 2;
            *i++ = (y + 2) * resolution;
            }
        }

    // Calculate vertex positions
    for (y = 0; y < resolution; y++)
        {
        for (x = 0; x < resolution; x++)
            {
            // Calculate a grid position for this vertex and displace it using a sum of sines, that converges
            // to zero as the animation advances.
            GLfloat u = x / (GLfloat)(resolution - 1);
            GLfloat v = y / (GLfloat)(resolution - 1);
            GLint d = (iDir == ELeft) ? -1 : 1;
            vertices[(y * resolution + x) * 2 + 0] = (-1 << 16) + x * (2 << 16) / (resolution - 1) +
                (GLint)((1 << 16) * (sin(u * 5 + ratio * 4) + sin(v * 7 + ratio * 3 + texture.iID) + 8 * d * (1 - v * v)) * ratio * ratio / 4);
            vertices[(y * resolution + x) * 2 + 1] = (-1 << 16) + y * (2 << 16) / (resolution - 1) +
                (GLint)((1 << 16) * (sin(u * 7 + ratio * 3) + sin(v * 5 + ratio * 4 + texture.iID)) * ratio * ratio / 4);
            texCoords[(y * resolution + x) * 2 + 0] = x * (1 << 16) / (resolution - 1);
            texCoords[(y * resolution + x) * 2 + 1] = (resolution - y - 1) * (1 << 16) / (resolution - 1);
            }
        }

    // Set orthogonal projection
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glFrustumf(-1, 1, -1, 1, 1, 2);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Move the picture
    glTranslatef(0, 0, -1);

    // Set texture parameters
    glBindTexture(GL_TEXTURE_2D, texture.iID);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Set up blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1, 1, 1, 1 - ratio);

    // Draw the textured grid
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer( 2, GL_FIXED, 0, texCoords );
    glVertexPointer( 2, GL_FIXED, 0, vertices );
    glDrawElements( GL_TRIANGLE_STRIP, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_SHORT, indices );

    // Reset the state
    glDisable(GL_BLEND);
    glColor4f(1, 1, 1, 1);
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW);
    }
