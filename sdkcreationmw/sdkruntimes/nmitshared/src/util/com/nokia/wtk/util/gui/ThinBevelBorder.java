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

/* javax.swing */
import javax.swing.UIManager;

/* javax.swing.border */
import javax.swing.border.Border;
import javax.swing.border.BevelBorder;
import javax.swing.border.AbstractBorder;

/**
 * A class which implements a simple thin single line bevel border.
 *
 */
public class ThinBevelBorder extends AbstractBorder {

    /** Raised bevel type. */
    public static final int RAISED  = BevelBorder.RAISED;
    /** Lowered bevel type. */
    public static final int LOWERED = BevelBorder.LOWERED;

    private int bevelType;
    private Color highlightColor;
    private Color shadowColor;

    /**
     * Shared instance of lowered <code>ThinBevelBorder</code>
     */
    private static ThinBevelBorder lowered = null;

    /**
     * Shared instance of raised <code>ThinBevelBorder</code>
     */
    private static ThinBevelBorder raised = null;

    /**
     * Returns shared instance of lowered <code>ThinBevelBorder</code>
     */
    public static Border getLoweredBorder() {
        if (lowered == null) {
            lowered = new ThinBevelBorder(LOWERED);
        }
        return lowered;
    }

    /**
     * Returns shared instance of raised <code>ThinBevelBorder</code>
     */
    public static Border getRaisedBorder() {
        if (raised == null) {
            raised = new ThinBevelBorder(RAISED);
        }
        return raised;
    }

    /**
     * Creates a lowered bevel border whose colors will be derived
     * from the background color of the component passed into the
     * {@link #paintBorder paintBorder()} method.
     */
    public ThinBevelBorder() {
        this(LOWERED);
    }

    /**
     * Creates a bevel border with the specified type and whose
     * colors will be derived from the background color of the
     * component passed into the {@link #paintBorder paintBorder()}
     * method.
     *
     * @param bevelType the type of bevel for the border
     */
    public ThinBevelBorder(int bevelType) {
        this(bevelType, null, null);
    }

    /**
     * Creates a bevel border with the specified type, highlight and
     * shadow colors.
     *
     * @param bevelType the type of bevel for the border
     * @param highlight the color to use for the bevel highlight
     * @param shadow the color to use for the bevel shadow
     */
    public ThinBevelBorder(int bevelType, Color highlight, Color shadow) {
        this.bevelType = bevelType;
        this.highlightColor = highlight;
        this.shadowColor    = shadow;
    }

    /**
     * Returns the insets of the border.
     * @param c the component for which this border insets value applies
     */
    public Insets getBorderInsets(Component c) {
        return new Insets(1, 1, 1, 1);
    }

    /**
     * Reinitialize the insets parameter with this Border's current Insets.
     * @param c the component for which this border insets value applies
     * @param insets the object to be reinitialized
     */
    public Insets getBorderInsets(Component c, Insets insets) {
        insets.left = insets.top = insets.right = insets.bottom = 1;
        return insets;
    }

    /**
     * Returns the highlight color of the bevel border.
     */
    public Color getHighlightColor(Component c)   {
        return ((highlightColor != null ) ? highlightColor :
                UIManager.getColor("controlLtHighlight"));
    }

    /**
     * Returns the shadow color of the bevel border.
     */
    public Color getShadowColor(Component c)      {
        return ((shadowColor != null ) ? shadowColor :
                UIManager.getColor("controlDkShadow"));
    }

    /**
     * Returns the configured highlight color, <code>null</code> if none
     */
    public Color getHighlightColor() {
        return highlightColor;
    }

    /**
     * Returns the configured shadow color, <code>null</code> if none.
     */
    public Color getShadowColor() {
        return shadowColor;
    }

    /**
     * Paints the border for the specified component with the specified
     * position and size.
     * @param c the component for which this border is being painted
     * @param g the paint graphics
     * @param x the x position of the painted border
     * @param y the y position of the painted border
     * @param width the width of the painted border
     * @param height the height of the painted border
     */
    public void paintBorder(Component c, Graphics g, int x, int y, int width, int height) {
        if (bevelType == RAISED) {
             paintRaisedBevel(c, g, x, y, width, height);

        } else if (bevelType == LOWERED) {
             paintLoweredBevel(c, g, x, y, width, height);
        }
    }

    /**
     * Paints raised bevel border
     */
    protected void paintRaisedBevel(Component c, Graphics g,
                                    int x, int y, int w, int h)  {
        paintBevel(g, x, y, w, h, getHighlightColor(c), getShadowColor(c));
    }

    /**
     * Paints lowered bevel border
     */
    protected void paintLoweredBevel(Component c, Graphics g,
                                     int x, int y, int w, int h)  {
        paintBevel(g, x, y, w, h, getShadowColor(c), getHighlightColor(c));
    }

    /**
     * Paints bevel border
     */
    public static void paintBevel(Graphics g, int x, int y, int w, int h,
                                  Color c1, Color c2) {
        Color oldColor = g.getColor();

        g.translate(x, y);

        g.setColor(c1);
        g.drawLine(0, 0, 0, h-1);
        g.drawLine(1, 0, w-1, 0);

        g.setColor(c2);
        g.drawLine(1, h-1, w-1, h-1);
        g.drawLine(w-1, 1, w-1, h-2);

        g.translate(-x, -y);
        g.setColor(oldColor);
    }
}

