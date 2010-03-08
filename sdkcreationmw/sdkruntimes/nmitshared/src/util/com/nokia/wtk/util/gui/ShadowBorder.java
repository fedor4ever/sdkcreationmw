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
import java.awt.Shape;
import java.awt.Color;
import java.awt.Image;
import java.awt.Insets;
import java.awt.Toolkit;
import java.awt.Graphics;
import java.awt.Rectangle;
import java.awt.Component;

/* java.awt.geom */
import java.awt.geom.Area;

/* java.awt.image */
import java.awt.image.ColorModel;
import java.awt.image.ImageObserver;
import java.awt.image.ImageProducer;
import java.awt.image.DirectColorModel;
import java.awt.image.MemoryImageSource;

/* javax.swing */
import javax.swing.JComponent;

/* javax.swing.border */
import javax.swing.border.AbstractBorder;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.Utils;
import com.nokia.wtk.util.Trace;

/**
 * Paints a semi-transparent shadow.
 * <p>
 * <strong>Note:</strong> only SOUTH_EAST direction is currently implemented.
 *
 */
public class ShadowBorder extends AbstractBorder {

    // Note: only SOUTH_EAST direction is currently implemented
    public static final int SOUTH_EAST = 0;
    public static final int SOUTH_WEST = 1;
    public static final int NORTH_EAST = 2;
    public static final int NORTH_WEST = 3;

    private static final int SHADOW_THICKNESS = 7;

    public static final String PROP_FILL_BG = "ShadowBorder.drawBackground";
    public static final String PROP_USE_PARENT_BG = "ShadowBorder.useParentBackground";

    private static final ImageObserver OBSERVER = new DummyImageObserver();
    private static final ColorModel ACM = // ACM == alpha color model
        new DirectColorModel(11,0x100,0x200,0x400,0xff);

    // transparency levels for various parts of the border, from 0 (fully
    // transparent) to 255 inclusive (fully opaque)
    private static final int V_CORNER_1_ROWS = 11;
    private static final Image V_CORNER_1 = createImage(new int [] {
          6,   5,   4,   2,   1,   0,   0,
         19,  17,  13,   7,   3,   1,   0,
         43,  38,  29,  16,   7,   2,   0,
         76,  67,  51,  29,  13,   4,   0,
         98,  87,  67,  38,  17,   5,   1,
        107,  97,  75,  43,  19,   6,   1,
        104,  98,  78,  45,  20,   6,   1,
         94,  94,  77,  45,  21,   6,   1,
         87,  91,  76,  45,  21,   6,   1,
         82,  89,  76,  45,  21,   6,   1,
         81,  88,  75,  45,  21,   6,   1,
    }, SHADOW_THICKNESS);

    private static final int V_TILE_ROWS = 50;
    private static final Image V_TILE = createImage(new int [] {
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
         80,  88,  75,  45,  21,   6,   1,
    }, SHADOW_THICKNESS);

    private static final int V_CORNER_2_ROWS = 12;
    private static final Image V_CORNER_2 = createImage(new int [] {
         81,  88,  75,  45,  21,   6,   1,
         83,  89,  76,  45,  21,   6,   1,
         87,  91,  76,  45,  21,   6,   1,
         95,  94,  77,  45,  20,   6,   1,
        105,  98,  78,  45,  20,   6,   1,
        107,  97,  75,  43,  19,   6,   1,
         97,  87,  66,  38,  17,   5,   1,
         75,  66,  50,  28,  13,   4,   0,
         42,  37,  28,  16,   7,   2,   0,
         19,  17,  13,   7,   3,   1,   0,
          6,   5,   4,   2,   1,   0,   0,
          0,   0,   0,   0,   0,   0,   0,
    }, SHADOW_THICKNESS);

    private static final int H_CORNER_1_COLUMNS = 3;
    private static final Image H_CORNER_1 = createImage(new int [] {
        101, 103, 107,
         99, 100, 100,
         79,  79,  78,
         45,  45,  44,
         20,  20,  20,
          6,   6,   6,
          1,   1,   0,
    }, H_CORNER_1_COLUMNS);

    private static final int H_TILE_COLUMNS = 50;
    private static final Image H_TILE = createImage(new int [] {
         99,  99,  99,  99,  99,  99,  99,  99,  99,  99,
         99,  99,  99,  99,  99,  99,  99,  99,  99,  99,
         99,  99,  99,  99,  99,  99,  99,  99,  99,  99,
         99,  99,  99,  99,  99,  99,  99,  99,  99,  99,
         99,  99,  99,  99,  99,  99,  99,  99,  99,  99,
         99,  99,  99,  99,  99,  99,  99,  99,  99,  99,
         99,  99,  99,  99,  99,  99,  99,  99,  99,  99,
         99,  99,  99,  99,  99,  99,  99,  99,  99,  99,
         99,  99,  99,  99,  99,  99,  99,  99,  99,  99,
         99,  99,  99,  99,  99,  99,  99,  99,  99,  99,
         79,  79,  79,  79,  79,  79,  79,  79,  79,  79,
         79,  79,  79,  79,  79,  79,  79,  79,  79,  79,
         79,  79,  79,  79,  79,  79,  79,  79,  79,  79,
         79,  79,  79,  79,  79,  79,  79,  79,  79,  79,
         79,  79,  79,  79,  79,  79,  79,  79,  79,  79,
         45,  45,  45,  45,  45,  45,  45,  45,  45,  45,
         45,  45,  45,  45,  45,  45,  45,  45,  45,  45,
         45,  45,  45,  45,  45,  45,  45,  45,  45,  45,
         45,  45,  45,  45,  45,  45,  45,  45,  45,  45,
         45,  45,  45,  45,  45,  45,  45,  45,  45,  45,
         20,  20,  20,  20,  20,  20,  20,  20,  20,  20,
         20,  20,  20,  20,  20,  20,  20,  20,  20,  20,
         20,  20,  20,  20,  20,  20,  20,  20,  20,  20,
         20,  20,  20,  20,  20,  20,  20,  20,  20,  20,
         20,  20,  20,  20,  20,  20,  20,  20,  20,  20,
          6,   6,   6,   6,   6,   6,   6,   6,   6,   6,
          6,   6,   6,   6,   6,   6,   6,   6,   6,   6,
          6,   6,   6,   6,   6,   6,   6,   6,   6,   6,
          6,   6,   6,   6,   6,   6,   6,   6,   6,   6,
          6,   6,   6,   6,   6,   6,   6,   6,   6,   6,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
    }, H_TILE_COLUMNS);

    private static final int H_CORNER_2_COLUMNS = 9;
    private static final Image H_CORNER_2 = createImage(new int [] {
          6,  19,  43,  75,  97, 107, 106, 103, 101,
          5,  17,  38,  66,  86,  97, 100, 100,  99,
          4,  13,  28,  50,  66,  75,  78,  79,  79,
          2,   7,  16,  28,  37,  42,  44,  45,  45,
          1,   3,   7,  13,  17,  19,  20,  20,  20,
          0,   1,   2,   4,   5,   6,   6,   6,   6,
          0,   0,   0,   0,   0,   0,   0,   1,   1,
    }, H_CORNER_2_COLUMNS);

    // instance variables
    private Insets borderInsets = new Insets(0,0,0,0);
    private int direction;

    /**
     * Creates an <code>ShadowBorder</code> with default shadow direction
     * (south east).
     */
    public ShadowBorder() {
        this(SOUTH_EAST);
    }

    /**
     * Creates an <code>ShadowBorder</code> with the shadow direction.
     * @param dir the shadow direction
     */
    public ShadowBorder(int dir)  {
        setDirection(dir);
    }

    /**
     * Gets the shadow direction.
     * @return the shadow direction.
     */
    public int getDirection() {
        return direction;
    }

    /**
     * Sets the shadow direction.
     * @param dir the shadow direction
     */
    public void setDirection(int dir) {
        switch (dir) {
        case SOUTH_WEST:
        case NORTH_EAST:
        case NORTH_WEST:
            throw new UnsupportedOperationException("sorry, not implemented");
        case SOUTH_EAST:
            direction = dir;
            break;
        default:
            throw new IllegalArgumentException(Integer.toString(dir));
        }
        computeInsets();
    }

    /**
     * Re-calculates the border insets
     */
    private void computeInsets() {
        switch (direction) {
        case SOUTH_EAST:
            borderInsets.top = 0;
            borderInsets.bottom = SHADOW_THICKNESS;
            borderInsets.left = 0;
            borderInsets.right = SHADOW_THICKNESS;
            break;
        case SOUTH_WEST:
            borderInsets.top = 0;
            borderInsets.bottom = SHADOW_THICKNESS;
            borderInsets.left = SHADOW_THICKNESS;
            borderInsets.right = 0;
            break;
        case NORTH_EAST:
            borderInsets.top = SHADOW_THICKNESS;
            borderInsets.bottom = 0;
            borderInsets.left = 0;
            borderInsets.right = SHADOW_THICKNESS;
            break;
        case NORTH_WEST:
            borderInsets.top = SHADOW_THICKNESS;
            borderInsets.bottom = 0;
            borderInsets.left = SHADOW_THICKNESS;
            borderInsets.right = 0;
            break;
        }
    }

    /**
     * Returns the insets of the border.
     * @param c the component for which this border insets value applies
     * @return the insets of the border.
     */
    public Insets getBorderInsets(Component c) {
        return (Insets)borderInsets.clone();
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
     * Determines whether this border needs to fill its background with
     * the component's background color.
     * @param c the component to check
     * @return  <code>true</code> if the border background has to be filled
     *          with component's background color prior to drawing the shadow
     */
    private static boolean needToFillBackgroundFor(Component c) {
        if (c instanceof JComponent) {
            Object val = ((JComponent)c).getClientProperty(PROP_FILL_BG);
            if (val != null) {
                return Boolean.TRUE.equals(val);
            }
        }
        return c.isOpaque();
    }

    /**
     * Determines whether this border needs to fill its background with
     * background color of the component's parent.
     * @param c the component to check
     * @return  <code>true</code> if the border background has to be filled
     *          with component's background color prior to drawing the shadow
     */
    private static boolean useParentBackground(Component c) {
        if (c instanceof JComponent) {
            Object val = ((JComponent)c).getClientProperty(PROP_USE_PARENT_BG);
            if (val != null) {
                return Boolean.TRUE.equals(val);
            }
        }
        return false;
    }

    /**
     * Finds the color to fill to border background.
     * @param c the component for which the border is being painted
     * @return the background color or <code>null</code> if the background
     *          does not need to be filled
     */
    private static Color getFillColorFor(Component c) {
        if (needToFillBackgroundFor(c)) {
            if (useParentBackground(c)) {
                Component parent = c.getParent();
                if (parent != null) {
                    return getFillColorFor(parent);
                }
            } else {
                return c.getBackground();
            }
        }
        return null;
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
        Color fillColor = getFillColorFor(c);
        Color saveColor = g.getColor();
        Shape clip = g.getClip();
        Rectangle rc = new Rectangle();

        int x1 = Math.max(x + w - SHADOW_THICKNESS, x);
        int x2 = x + w;
        int y1 = Math.max(y + h - SHADOW_THICKNESS, y);
        int y2 = y + h;

        // draw the south part
        boolean drawSouth = false;
        if (x2 > x && y2 > y1) {
            if (clip instanceof Rectangle) {
                Rectangle clipRect = (Rectangle)clip;
                rc.setBounds(clipRect);
                if (GUI.intersect(rc, x, y1, x2 - x, y2 - y1)) {
                    g.setClip(rc.x, rc.y, rc.width, rc.height);
                    drawSouth = true;
                }
            } else {
                rc.setBounds(x, y1, x2 - x, y2 - y1);
                if (clip.contains(rc)) {
                    g.setClip(rc.x, rc.y, rc.width, rc.height);
                    drawSouth = true;
                } else if (clip.intersects(rc)) {
                    Area clipArea = new Area(clip);
                    clipArea.intersect(new Area(rc));
                    if (!clipArea.isEmpty()) {
                        g.setClip(clipArea);
                        drawSouth = true;
                    }
                }
            }
        }

        if (drawSouth) {
            if (fillColor != null) {
                g.setColor(fillColor);
                g.fillRect(x, y1, (x2 - x), (y2 - y1));
            }

            // the lower left corner
            drawImage(g, H_CORNER_2, x, y1);

            // the lower right corner
            int x3 = x + H_CORNER_2_COLUMNS;
            int x4 = x2 - SHADOW_THICKNESS;
            if ((x4 - x3) >= H_CORNER_1_COLUMNS) {
                x4 -= H_CORNER_1_COLUMNS;
                drawImage(g, H_CORNER_1, x4, y1);
            }

            // draw lower tiles
            if (x3 < x4) {
                drawSouth = false;
                if (clip instanceof Rectangle) {
                    Rectangle clipRect = (Rectangle)clip;
                    rc.setBounds(clipRect);
                    if (GUI.intersect(rc, x3, y1, x4 - x3, y2 - y1)) {
                        g.setClip(rc.x, rc.y, rc.width, rc.height);
                        drawSouth = true;
                    }
                } else {
                    rc.setBounds(x3, y1, x4 - x3, y2 - y1);
                    if (clip.contains(rc)) {
                        g.setClip(rc.x, rc.y, rc.width, rc.height);
                        drawSouth = true;
                    } else if (clip.intersects(rc)) {
                        Area clipArea = new Area(clip);
                        clipArea.intersect(new Area(rc));
                        if (!clipArea.isEmpty()) {
                            g.setClip(clipArea);
                            drawSouth = true;
                        }
                    }
                }
                if (drawSouth) {
                    for (int i = x3; i < x4; i += H_TILE_COLUMNS) {
                        drawImage(g, H_TILE, i, y1);
                    }
                }
            }
        }

        // draw the east part
        boolean drawEast = false;
        if (clip instanceof Rectangle) {
            Rectangle clipRect = (Rectangle)clip;
            rc.setBounds(clipRect);
            if (GUI.intersect(rc, x1, y, x2 - x1, y2 - y)) {
                g.setClip(rc.x, rc.y, rc.width, rc.height);
                drawEast = true;
            }
        } else {
            rc.setBounds(x1, y, x2 - x1, y2 - y);
            if (clip.contains(rc)) {
                g.setClip(rc.x, rc.y, rc.width, rc.height);
                drawEast = true;
            } else if (clip.intersects(rc)) {
                Area clipArea = new Area(clip);
                clipArea.intersect(new Area(rc));
                if (!clipArea.isEmpty()) {
                    g.setClip(clipArea);
                    drawEast = true;
                }
            }
        }

        if (drawEast) {
            if (fillColor != null) {
                g.setColor(fillColor);
                g.fillRect(x1, y, (x2 - x1), (y2 - y));
            }

            // the upper right corner
            drawImage(g, V_CORNER_1, x1, y);

            // the lower right corner
            int y3 = y + V_CORNER_1_ROWS;
            int y4 = y2 - V_CORNER_2_ROWS;
            drawImage(g, V_CORNER_2, x1, y4);

            // draw east tiles
            if (y3 < y4) {
                drawEast = false;
                if (clip instanceof Rectangle) {
                    Rectangle clipRect = (Rectangle)clip;
                    rc.setBounds(clipRect);
                    if (GUI.intersect(rc, x1, y3, x2 - x1, y4 - y3)) {
                        g.setClip(rc.x, rc.y, rc.width, rc.height);
                        drawEast = true;
                    }
                } else {
                    rc.setBounds(x1, y3, x2 - x1, y4 - y3);
                    if (clip.contains(rc)) {
                        g.setClip(rc.x, rc.y, rc.width, rc.height);
                        drawEast = true;
                    } else if (clip.intersects(rc)) {
                        Area clipArea = new Area(clip);
                        clipArea.intersect(new Area(rc));
                        if (!clipArea.isEmpty()) {
                            g.setClip(clipArea);
                            drawEast = true;
                        }
                    }
                }

                if (drawEast) {
                    for (int i = y3; i < y4; i+=V_TILE_ROWS) {
                        drawImage(g, V_TILE, x1, i);
                    }
                }
            }
        }

        g.setColor(saveColor);
        g.setClip(clip);
    }

    /**
     * Draws an image. This method exists for the purpose of catching
     * IllegalArgumentException(Width and height must be &gt; 0) that
     * occur from time to time in Java 1.3 when the Graphics has the
     * clip area of unusual (non-rectangular) shape. I have's figured
     * out the reason for those exceptions, but since they are gone
     * in Java 1.4, it looks to me like a bug in Java 1.3. So we just
     * catch those exceptions.
     *
     * @param g     the graphics to paint to
     * @param image the image to draw
     * @param x     the x coordinate of the upper left corner of the image
     * @param y     the y coordinate of the upper left corner of the image
     */
    private static void drawImage(Graphics g, Image image, int x, int y) {
        try {
            g.drawImage(image, x, y, OBSERVER);
        } catch (IllegalArgumentException e) {
            // we only expect this to happen under Java 1.3
            // if that occurs under a newer version of Java,
            // we dump the stack
            if (Utils.is1dot4orBetter()) {
                Trace.printStackTrace(e);
            }
        }
    }

    /**
     * Creates a semi-transparent image from the data containing only
     * the alpha channel.
     *
     * @param data the data.
     * @param w the scan size (width) of the image
     * @return the created image.
     */
    private static Image createImage(int [] data, int w) {
        int h = data.length/w;
        ImageProducer src = new MemoryImageSource(w,h,ACM,data,0,w);
        return Toolkit.getDefaultToolkit().createImage(src);
    }

    /**
     * Simple implementation of ImageObserver interface for drawing images
     */
    private static class DummyImageObserver implements ImageObserver {
        private DummyImageObserver() {}
        public boolean imageUpdate(Image i,int f,int x,int y,int w,int h) {
            return true;
        }
    }
}
