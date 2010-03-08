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
import javax.swing.border.AbstractBorder;

/**
 * A class which implements a line border of a single color.
 * The lines may have different thickness on different sides
 * of the component
 *
 */
public class LinesBorder extends AbstractBorder {

    private Insets borderInsets;
    private Color lineColor;

    /**
     * Creates a <code>LinesBorder</code> with the specified color and a
     * thickness = 1.
     * @param color the color for the border
     */
    public LinesBorder(Color color) {
        this(color, 1);
    }

    /**
     * Creates a <code>LinesBorder</code> with the specified color and
     * thickness.
     * @param color the color of the border.
     * @param thickness the thickness of the border.
     */
    public LinesBorder(Color color, int thickness)  {
        this(color, thickness, thickness, thickness, thickness);
    }

    /**
     * Creates a black <code>LinesBorder</code> with specified thickness on
     * each side.
     * @param top    the inset from the top.
     * @param left   the inset from the left.
     * @param bottom the inset from the bottom.
     * @param right  the inset from the right.
     */
    public LinesBorder(int top, int left, int bottom, int right) {
        this(Color.black, top, left, bottom, right);
    }

    /**
     * Creates a black <code>LinesBorder</code> border with specified
     * thickness on each side.
     * @param insets the insets specifying line thickness on each side.
     */
    public LinesBorder(Insets insets) {
        this(Color.black, insets.top, insets.left, insets.bottom, insets.right);
    }

    /**
     * Creates a black <code>LinesBorder</code> with the specified color
     * and specified thickness on each side.
     * @param color  the color of the border.
     * @param insets the insets specifying line thickness on each side.
     */
    public LinesBorder(Color color, Insets insets) {
        this(color, insets.top, insets.left, insets.bottom, insets.right);
    }

    /**
     * Creates a <code>LinesBorder</code> with the specified color and
     * thickness on each side.
     * @param color  the color of the border.
     * @param top    the inset from the top.
     * @param left   the inset from the left.
     * @param bottom the inset from the bottom.
     * @param right  the inset from the right.
     */
    public LinesBorder(Color color, int top, int left, int bottom, int right) {
        lineColor = color;
        borderInsets = new Insets(top, left, bottom, right);
    }

    /**
     * Paints the border for the specified component with the
     * specified position and size.
     * @param c the component for which this border is being painted.
     * @param g the paint graphics.
     * @param x the x position of the painted border.
     * @param y the y position of the painted border.
     * @param w the width of the painted border.
     * @param h the height of the painted border.
     */
    public void paintBorder(Component c, Graphics g, int x, int y, int w, int h) {
        int i;
        Color oldColor = g.getColor();
        g.setColor(lineColor);
        g.translate(x,y);
        for (i=0; i<borderInsets.top; i++)    g.drawLine(0, i, w-1, i);
        for (i=0; i<borderInsets.left; i++)   g.drawLine(i, 0, i, h-1);
        for (i=0; i<borderInsets.bottom; i++) g.drawLine(0, h-1-i, w-1, h-1-i);
        for (i=0; i<borderInsets.right; i++)  g.drawLine(w-1-i, 0, w-1-i, h-1);
        g.setColor(oldColor);
        g.translate(-x,-y);
    }

    /**
     * Returns the insets of the border.
     * @param c the component for which this border insets value applies
     * @return the insets of the border.
     */
    public Insets getBorderInsets(Component c) {
        return new Insets(borderInsets.top, borderInsets.left,
                          borderInsets.bottom, borderInsets.right);
    }

    /**
     * Reinitialize the insets parameter with this Border's current Insets.
     * @param c the component for which this border insets value applies
     * @param insets the object to be reinitialized.
     * @return reinitialized <code>insets</code> if it's not <code>null</code>;
     *         otherwise allocates and returns a fresh {@link Insets} object.
     */
    public Insets getBorderInsets(Component c, Insets insets) {
        if (insets == null) {
            insets = (Insets)borderInsets.clone();
        } else {
            insets.top    = borderInsets.top;
            insets.left   = borderInsets.left;
            insets.bottom = borderInsets.bottom;
            insets.right  = borderInsets.right;
        }
        return insets;
    }

    /**
     * Sets border insets.
     * @param insets the new border insets
     */
    public void setBorderInsets(Insets insets) {
        setBorderInsets(insets.top, insets.left, insets.bottom, insets.right);
    }

    /**
     * Sets border insets.
     * @param thickness the thickness of the border
     */
    public void setBorderInsets(int thickness) {
        setBorderInsets(thickness, thickness, thickness, thickness);
    }

    /**
     * Sets border insets.
     * @param top    the inset from the top.
     * @param left   the inset from the left.
     * @param bottom the inset from the bottom.
     * @param right  the inset from the right.
     */
    public void setBorderInsets(int top, int left, int bottom, int right) {
        borderInsets.top = top;
        borderInsets.left = left;
        borderInsets.bottom = bottom;
        borderInsets.right = right;
    }

    /**
     * Returns the color of the border.
     * @return the color of the border.
     */
    public Color getLineColor() {
        return lineColor;
    }

    /**
     * Sets the line color.
     * @param color the line color.
     */
    public void setLineColor(Color color) {
        lineColor = color;
    }

    /**
     * Returns whether or not the border is opaque.
     * @return whether or not the border is opaque.
     */
    public boolean isBorderOpaque() {
        return true;
    }

}
