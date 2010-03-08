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

/* java.util */
import java.util.Arrays;

/* java.awt */
import java.awt.Shape;
import java.awt.Insets;
import java.awt.Graphics;
import java.awt.Rectangle;
import java.awt.Component;

/* java.awt.geom */
import java.awt.geom.Area;

/* javax.swing */
import javax.swing.Icon;

/* javax.swing.border */
import javax.swing.border.AbstractBorder;

/**
 * An extension to MatteBorder that paints different sides and corners
 * with different icons.
 *
 */
public class IconBorder extends AbstractBorder {

    public static final int TOP_TILE = 0;
    public static final int BOTTOM_TILE = 1;
    public static final int LEFT_TILE = 2;
    public static final int RIGHT_TILE = 3;
    public static final int UPPER_LEFT_CORNER = 4;
    public static final int UPPER_RIGHT_CORNER = 5;
    public static final int LOWER_LEFT_CORNER = 6;
    public static final int LOWER_RIGHT_CORNER = 7;
    public static final int NUMBER_OF_ICONS = 8;

    // instance variables
    private Insets borderInsets = new Insets(0,0,0,0);
    private Icon[] borderIcons = new Icon[NUMBER_OF_ICONS];

    /**
     * Creates an <code>IconBorder</code> with no icons.
     */
    public IconBorder() {
    }

    /**
     * Creates an <code>IconBorder</code> with the specified icons.
     * @param icons the icons to use
     */
    public IconBorder(Icon [] icons)  {
        setIcons(icons);
    }

    /**
     * Creates an <code>IconBorder</code> with the specified icons.
     * @param cls the class to be used as the base for loading the icons
     * @param iconFiles the icon files to load the icons from
     */
    public IconBorder(Class cls, String [] iconFiles)  {
        setIcons(cls, iconFiles);
    }

    /**
     * Re-calculates the border insets
     */
    private void computeInsets() {
        borderInsets.top = 0;
        borderInsets.bottom = 0;
        borderInsets.left = 0;
        borderInsets.right = 0;

        // tiles
        if (borderIcons[TOP_TILE] != null) {
            borderInsets.top = borderIcons[TOP_TILE].getIconHeight();
        }
        if (borderIcons[BOTTOM_TILE] != null) {
            borderInsets.bottom = borderIcons[BOTTOM_TILE].getIconHeight();
        }
        if (borderIcons[LEFT_TILE] != null) {
            borderInsets.left = borderIcons[LEFT_TILE].getIconWidth();
        }
        if (borderIcons[RIGHT_TILE] != null) {
            borderInsets.right = borderIcons[RIGHT_TILE].getIconWidth();
        }

        // corners
        if (borderIcons[UPPER_LEFT_CORNER] != null) {
            Icon corner = borderIcons[UPPER_LEFT_CORNER];
            int width = corner.getIconWidth();
            int height = corner.getIconHeight();
            if (borderInsets.top != height && borderInsets.left != width) {
                borderInsets.top = Math.max(borderInsets.top, height);
                borderInsets.left = Math.max(borderInsets.left, width);
            }
        }
        if (borderIcons[UPPER_RIGHT_CORNER] != null) {
            Icon corner = borderIcons[UPPER_RIGHT_CORNER];
            int width = corner.getIconWidth();
            int height = corner.getIconHeight();
            if (borderInsets.top != height && borderInsets.right != width) {
                borderInsets.top = Math.max(borderInsets.top, height);
                borderInsets.right = Math.max(borderInsets.right, width);
            }
        }
        if (borderIcons[LOWER_LEFT_CORNER] != null) {
            Icon corner = borderIcons[LOWER_LEFT_CORNER];
            int width = corner.getIconWidth();
            int height = corner.getIconHeight();
            if (borderInsets.bottom != height && borderInsets.left != width) {
                borderInsets.bottom = Math.max(borderInsets.bottom, height);
                borderInsets.left = Math.max(borderInsets.left, width);
            }
        }
        if (borderIcons[LOWER_RIGHT_CORNER] != null) {
            Icon corner = borderIcons[LOWER_RIGHT_CORNER];
            int width = corner.getIconWidth();
            int height = corner.getIconHeight();
            if (borderInsets.bottom != height && borderInsets.right != width) {
                borderInsets.bottom = Math.max(borderInsets.bottom, height);
                borderInsets.right = Math.max(borderInsets.right, width);
            }
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
     * Gets the icons being used by the border.
     * @return the icons being used by the border.
     */
    public Icon [] getIcons() {
        return (Icon[])borderIcons.clone();
    }

    /**
     * Gets the icon being used by the border.
     * @param type the icon type
     * @return the icons being used by the border.
     */
    public Icon getIcon(int type) {
        return borderIcons[type];
    }

    /**
     * Sets the icon to be used by the border.
     * @param icon the icon to use
     * @param type the icon type
     */
    public void setIcon(Icon icon, int type) {
        borderIcons[type] = icon;
        computeInsets();
    }

    /**
     * Sets the icons to be used by the border.
     * @param cls the class to be used as the base for loading the icons
     * @param iconFiles the icon files to load the icons from
     */
    public void setIcons(Class cls, String [] iconFiles) {
        Icon [] icons = null;
        if (iconFiles != null) {
            icons = new Icon[iconFiles.length];
            for (int i=0; i<icons.length; i++) {
                icons[i] = GUI.loadIcon(cls, iconFiles[i]);
            }
        }
        setIcons(icons);
    }

    /**
     * Sets the icons to be used by the border.
     * @param icons the icons to use
     */
    public void setIcons(Icon [] icons) {
        if (icons == null) {
            Arrays.fill(borderIcons, null);
        } else {
            int n = Math.min(NUMBER_OF_ICONS, icons.length);
            System.arraycopy(icons, 0, borderIcons, 0, n);
        }
        computeInsets();
    }

    /**
     * Returns whether or not the border is opaque.
     * @return whether or not the border is opaque.
     */
    public boolean isBorderOpaque() {
        return false;
    }

    /**
     * Tests whether the original clip shape intersects with the specified
     * rectangle and sets the intersection to be the new clip shape.
     *
     * @param g  the graphics to the set the clip shape
     * @param clip the original clip shape
     * @param rc scratch rectangle
     * @param x  the x coordinate of the rectangle to intersect the clip with
     * @param y  the y coordinate of the rectangle to intersect the clip with
     * @param w  the width of the rectangle to intersect the clip with
     * @param h  the height of the rectangle to intersect the clip with
     *
     * @return <code>true</code> if the clip intersect with the rectangle and
     *         the Graphics has been set up for drawing. Otherwise, returns
     *         <code>false</code> and the Graphics clip shape is unchanged
     */
    private static boolean intersectClip(Graphics g, Shape clip, Rectangle rc,
                                         int x, int y, int w, int h) {
        if (clip instanceof Rectangle) {
            // optimized path for rectangular clips
            rc.setBounds((Rectangle)clip);
            if (GUI.intersect(rc, x, y, w, h)) {
                g.setClip(rc.x, rc.y, rc.width, rc.height);
                return true;
            }
        } else if (clip.intersects(x, y, w, h)) {
            // something unusual, handle with care
            rc.setBounds(x, y, w, h);
            Area clipArea = new Area(clip);
            clipArea.intersect(new Area(rc));
            if (clipArea.getBounds().height > 0) {
                g.setClip(clipArea);
                return true;
            }
        }
        return false;
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
        Shape clip = g.getClip();
        Rectangle rect = new Rectangle();

        // side tiles
        if (borderIcons[TOP_TILE] != null) {
            Icon tile = borderIcons[TOP_TILE];
            if (tile.getIconWidth() > 0) {
                int x1 = x;
                int x2 = x + w;
                int y1 = y;
                int y2 = y + tile.getIconHeight();

                Icon leftCorner = borderIcons[UPPER_LEFT_CORNER];
                if (leftCorner != null &&
                    leftCorner.getIconWidth() > borderInsets.left) {
                    x1 += leftCorner.getIconWidth();
                } else {
                    x1 += borderInsets.left;
                }

                Icon rightCorner = borderIcons[UPPER_RIGHT_CORNER];
                if (rightCorner != null &&
                    rightCorner.getIconWidth() > borderInsets.left) {
                    x2 -= rightCorner.getIconWidth();
                } else {
                    x2 -= borderInsets.right;
                }

                if (intersectClip(g, clip, rect, x1, y1, x2 - x1, y2 - y1)) {
                    for (int xt=x1; xt<x2; xt+= tile.getIconWidth()) {
                        tile.paintIcon(c, g, xt, y1);
                    }
                }
            }
        }
        if (borderIcons[BOTTOM_TILE] != null) {
            Icon tile = borderIcons[BOTTOM_TILE];
            if (tile.getIconWidth() > 0) {
                int x1 = x;
                int x2 = x + w;
                int y1 = y + h - tile.getIconHeight();
                int y2 = y + h;

                Icon leftCorner = borderIcons[LOWER_LEFT_CORNER];
                if (leftCorner != null &&
                    leftCorner.getIconWidth() > borderInsets.left) {
                    x1 += leftCorner.getIconWidth();
                } else {
                    x1 += borderInsets.left;
                }

                Icon rightCorner = borderIcons[LOWER_RIGHT_CORNER];
                if (rightCorner != null &&
                    rightCorner.getIconWidth() > borderInsets.left) {
                    x2 -= rightCorner.getIconWidth();
                } else {
                    x2 -= borderInsets.right;
                }

                if (intersectClip(g, clip, rect, x1, y1, x2 - x1, y2 - y1)) {
                    for (int xt=x1; xt<x2; xt+= tile.getIconWidth()) {
                        tile.paintIcon(c, g, xt, y1);
                    }
                }
            }
        }
        if (borderIcons[LEFT_TILE] != null) {
            Icon tile = borderIcons[LEFT_TILE];
            if (tile.getIconHeight() > 0) {
                int x1 = x;
                int x2 = x + tile.getIconWidth();
                int y1 = y;
                int y2 = y + h;

                Icon topCorner = borderIcons[UPPER_LEFT_CORNER];
                if (topCorner != null &&
                    topCorner.getIconHeight() > borderInsets.top) {
                    y1 += topCorner.getIconHeight();
                } else {
                    y1 += borderInsets.top;
                }

                Icon bottomCorner = borderIcons[LOWER_LEFT_CORNER];
                if (bottomCorner != null &&
                    bottomCorner.getIconHeight() > borderInsets.bottom) {
                    y2 -= bottomCorner.getIconHeight();
                } else {
                    y2 -= borderInsets.bottom;
                }

                if (intersectClip(g, clip, rect, x1, y1, x2 - x1, y2 - y1)) {
                    for (int yt=0; yt<y2; yt+= tile.getIconHeight()) {
                        tile.paintIcon(c, g, x1, yt);
                    }
                }
            }
        }
        if (borderIcons[RIGHT_TILE] != null) {
            Icon tile = borderIcons[RIGHT_TILE];
            if (tile.getIconHeight() > 0) {
                int x1 = x + w - tile.getIconWidth();
                int x2 = x + w;
                int y1 = y;
                int y2 = y + h;

                Icon topCorner = borderIcons[UPPER_RIGHT_CORNER];
                if (topCorner != null &&
                    topCorner.getIconHeight() > borderInsets.top) {
                    y1 += topCorner.getIconHeight();
                } else {
                    y1 += borderInsets.top;
                }

                Icon bottomCorner = borderIcons[LOWER_RIGHT_CORNER];
                if (bottomCorner != null &&
                    bottomCorner.getIconHeight() > borderInsets.bottom) {
                    y2 -= bottomCorner.getIconHeight();
                } else {
                    y2 -= borderInsets.bottom;
                }

                if (intersectClip(g, clip, rect, x1, y1, x2 - x1, y2 - y1)) {
                    for (int yt=0; yt<y2; yt+= tile.getIconHeight()) {
                        tile.paintIcon(c, g, x1, yt);
                    }
                }
            }
        }

        // corners
        if (borderIcons[UPPER_LEFT_CORNER] != null) {
            Icon corner = borderIcons[UPPER_LEFT_CORNER];
            int x1 = x;
            int x2 = x + corner.getIconWidth();
            int y1 = y;
            int y2 = y + borderInsets.top;
            if (intersectClip(g, clip, rect, x1, y1, x2 - x1, y2 - y1)) {
                corner.paintIcon(c, g, x1, y1);
            }
        }
        if (borderIcons[UPPER_RIGHT_CORNER] != null) {
            Icon corner = borderIcons[UPPER_RIGHT_CORNER];
            int x1 = x + w - corner.getIconWidth();
            int x2 = x + w;
            int y1 = y;
            int y2 = y + borderInsets.top;
            if (intersectClip(g, clip, rect, x1, y1, x2 - x1, y2 - y1)) {
                corner.paintIcon(c, g, x1, y1);
            }
        }
        if (borderIcons[LOWER_LEFT_CORNER] != null) {
            Icon corner = borderIcons[LOWER_LEFT_CORNER];
            int x1 = x;
            int x2 = x + corner.getIconWidth();
            int y1 = y + h - corner.getIconHeight();
            int y2 = y + h;
            if (intersectClip(g, clip, rect, x1, y1, x2 - x1, y2 - y1)) {
                corner.paintIcon(c, g, x1, y1);
            }
        }
        if (borderIcons[LOWER_RIGHT_CORNER] != null) {
            Icon corner = borderIcons[LOWER_RIGHT_CORNER];
            int x1 = x + w - corner.getIconWidth();
            int x2 = x + w;
            int y1 = y + h - corner.getIconHeight();
            int y2 = y + h;
            if (intersectClip(g, clip, rect, x1, y1, x2 - x1, y2 - y1)) {
                corner.paintIcon(c, g, x1, y1);
            }
        }
        g.setClip(clip);
    }

}
