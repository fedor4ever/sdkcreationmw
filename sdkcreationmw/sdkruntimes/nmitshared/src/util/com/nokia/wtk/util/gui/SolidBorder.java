/*
* Copyright (c) 2000 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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



package com.nokia.wtk.util.gui;

/* java.awt */
import java.awt.Color;
import java.awt.Insets;
import java.awt.Graphics;
import java.awt.Component;

/* javax.swing.border */
import javax.swing.border.EmptyBorder;

/**
 * Paints a solid border.
 *
 */
public class SolidBorder extends EmptyBorder {

    /* The color to fill the component */
    private Color fillColor;

    /**
     * Creates a solid border with the specified insets.
     * The border is filled with the background color of the component.
     *
     * @param insets the border insets (the same on each side)
     */
    public SolidBorder(int insets) {
        this(insets, insets, insets, insets);
    }

    /**
     * Creates a solid border with the specified insets.
     * The border is filled with the background color of the component.
     *
     * @param top the top inset of the border
     * @param left the left inset of the border
     * @param bottom the bottom inset of the border
     * @param right the right inset of the border
     */
    public SolidBorder(int top, int left, int bottom, int right) {
        this(null, top, left, bottom, right);
    }

    /**
     * Creates a solid border with the specified insets and color.
     *
     * @param c the color to fill the border. If this parameter is
     *          <code>null</code>, the border is filled with the
     *          background color of the component.
     * @param insets the border insets (the same on each side)
     */
    public SolidBorder(Color c, int insets) {
        this(c, insets, insets, insets, insets);
    }

    /**
     * Creates a solid border with the specified insets and color.
     *
     * @param c the color to fill the border. If this parameter is
     *          <code>null</code>, the border is filled with the
     *          background color of the component.
     * @param top the top inset of the border
     * @param left the left inset of the border
     * @param bottom the bottom inset of the border
     * @param right the right inset of the border
     */
    public SolidBorder(Color c, int top, int left, int bottom, int right) {
        super(top, left, bottom, right);
        fillColor = c;
    }

    /**
     * Creates a solid border with the specified insets.
     *
     * @param borderInsets the insets of the border
     */
    public SolidBorder(Insets borderInsets)   {
        this(null, borderInsets);
    }

    /**
     * Creates a solid border with the specified insets.
     *
     * @param c the color to fill the border. If this parameter is
     *          <code>null</code>, the border is filled with the
     *          background color of the component.
     * @param borderInsets the insets of the border
     */
    public SolidBorder(Color c, Insets borderInsets)   {
        super(borderInsets);
        fillColor = c;
    }

    /**
     * Gets the border's fill color.
     *
     * @return the border's fill color, <code>null</code> if the border is
     *         filled with the background color of the component
     */
    public Color getFillColor() {
        return fillColor;
    }

    /**
     * Sets the fill color for this border. If the fill color is
     * <code>null</code>, the border is filled with the background
     * color of the component.
     *
     * @param color the fill color for this border, <code>null</code>
     *     to fill the border with the background color of the component
     */
    public void setFillColor(Color color) {
        fillColor = color;
    }

    /**
     * Returns whether or not the border is opaque.  If the border
     * is opaque, it is responsible for filling in it's own
     * background when painting.
     *
     * @return returns true as this border always fills the background
     */
    public boolean isBorderOpaque() {
        return true;
    }

    /**
     * Paints the border for the specified component with the
     * specified position and size.
     *
     * @param c the component for which this border is being painted.
     * @param g the paint graphics.
     * @param x the x position of the painted border.
     * @param y the y position of the painted border.
     * @param w the width of the painted border.
     * @param h the height of the painted border.
     */
    public void paintBorder(Component c,Graphics g,int x,int y,int w,int h) {
        if (left > 0 || right > 0 || top > 0 || bottom > 0) {
            Color oldcolor = g.getColor();
            g.setColor((fillColor == null) ? c.getBackground() : fillColor);
            if (top > 0) g.fillRect(x, y, w, top);
            if (left > 0) g.fillRect(x, y, left, h);
            if (right > 0) g.fillRect(x+w-right, y, right, h);
            if (bottom > 0) g.fillRect(x, y+h-bottom, w, bottom);
            g.setColor(oldcolor);
        }
    }
}
