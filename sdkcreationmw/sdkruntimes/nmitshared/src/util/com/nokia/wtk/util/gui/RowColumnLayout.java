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
import java.awt.Insets;
import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.LayoutManager2;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.Enum;

/**
 * A layout similar to <code>FlowLayout</code> but it wraps the children
 * in a different way.
 *
 */
public class RowColumnLayout implements LayoutManager2 {

    /**
     * Enumeration of alignment modes for {@link RowColumnLayout}.
     */
    public static class Mode extends Enum {
        public static final Mode ROWS = new Mode("rows");
        public static final Mode COLUMNS = new Mode("columns");
        private Mode(String tag) {
            super(tag);
        }
    }

    // the layout modes
    public static final Mode ROWS = Mode.ROWS;
    public static final Mode COLUMNS = Mode.COLUMNS;
    public static final Mode DEFAULT = Mode.ROWS;

    /**
     * The layout mode.
     *
     * @see getMode
     * @see setMode
     */
    private Mode mode;

    /**
     * The horizontal space between components or columns.
     *
     * @see getHgap
     * @see setHgap
     */
    private int hgap;

    /**
     * The vertical gap between components or rows.
     *
     * @see getVgap
     * @see setVgap
     */
    private int vgap;

    /**
     * Constructs a new row layout with a default horizontal and vertical
     * gap.
     */
    public RowColumnLayout() {
        this(DEFAULT, GUI.GAP, GUI.GAP);
    }

    /**
     * Constructs a new layout with the specified mode and a
     * default horizontal and vertical gap.
     *
     * @param mode the layout mode
     */
    public RowColumnLayout(Mode mode) {
        this(mode, GUI.GAP, GUI.GAP);
    }

    /**
     * Creates a new layout manager with the indicated layout mode
     * and the indicated horizontal and vertical gaps.
     *
     * @param mode the layout mode
     * @param hgap the horizontal gap between components or columns.
     * @param vgap the vertical gap between components or rows.
     */
    public RowColumnLayout(Mode mode, int hgap, int vgap) {
        this.hgap = hgap;
        this.vgap = vgap;
        setMode(mode);
    }

    /**
     * Gets the alignment mode for this layout.
     *
     * @return the alignment mode for this layout.
     */
    public Mode getMode() {
        return mode;
    }

    /**
     * Sets the alignment mode for this layout.
     *
     * @param mode the alignment mode.
     * @see #getMode()
     */
    public void setMode(Mode mode) {
        this.mode = mode;
    }

    /**
     * Gets the horizontal gap between components.
     *
     * @return     the horizontal gap between components.
     * @see        #setHgap
     */
    public int getHgap() {
        return hgap;
    }

    /**
     * Sets the horizontal gap between components.
     *
     * @param hgap the horizontal gap between components
     * @see        #getHgap
     */
    public void setHgap(int hgap) {
        this.hgap = hgap;
    }

    /**
     * Gets the vertical gap between components.
     *
     * @return     the vertical gap between components.
     * @see        #setVgap
     */
    public int getVgap() {
        return vgap;
    }

    /**
     * Sets the vertical gap between components.
     *
     * @param vgap the vertical gap between components
     * @see        #getVgap
     */
    public void setVgap(int vgap) {
        this.vgap = vgap;
    }

    /**
     * Types of component size queries.
     */
    private static abstract class Size extends Enum {
        public static final Size MINIMUM = new MinimumSize();
        public static final Size MAXIMUM = new MaximumSize();
        public static final Size PREFERRED = new PreferredSize();

        protected Size(String tag) { super(tag); }
        abstract Dimension getSize(Component c);
        private static class MinimumSize extends Size {
            private MinimumSize() { super("minimum"); }
            Dimension getSize(Component c) { return c.getMinimumSize(); }
        }
        private static class MaximumSize extends Size {
            private MaximumSize() { super("maximum"); }
            Dimension getSize(Component c) { return c.getMaximumSize(); }
        }
        private static class PreferredSize extends Size {
            private PreferredSize() { super("preferred"); }
            Dimension getSize(Component c) { return c.getPreferredSize(); }
        }
    }

    /**
     * Returns the dimensions for this layout given the components
     * in the specified target container.
     *
     * @param target the component which needs to be laid out
     * @param size defines which size we determine
     * @return    the dimensions to lay out the subcomponents of the
     *             specified container.
     *
     * @see #minimumLayoutSize
     * @see #maximumLayoutSize
     * @see #preferredLayoutSize
     *
     * @todo handle overflow, watch for Integer.MAX_VALUE
     */
    private Dimension getLayoutSize(Container target, Size size) {
        synchronized (target.getTreeLock()) {
            Insets insets = target.getInsets();
            int w = target.getWidth();
            int h = target.getHeight();
            int dx = insets.left + insets.right;
            int dy = insets.top + insets.bottom;

            Dimension dim = new Dimension(0, 0);
            if (mode == Mode.ROWS) {
                dim.width = w;
                dim.height = dy;
            } else {
                dim.width = dx;
                dim.height = h;
            }

            boolean firstRowColumn = true;
            int n = target.getComponentCount();
            int currentCount = 0;
            int currentSize = 0;
            int step = 0; // row height or column width
            for (int i = 0; i < n; i++) {
                Component c = target.getComponent(i);
                if (c.isVisible()) {
                    Dimension d = size.getSize(c);
                    if (mode == Mode.ROWS) {
                        if (currentCount == 0) {
                            currentSize = d.width + dx;
                            currentCount++;
                            step = d.height;
                        } else if ((currentSize + hgap + d.width) <= w) {
                            currentSize += hgap + d.width;
                            currentCount++;
                            step = Math.max(step, d.height);
                        } else {
                            if (firstRowColumn) {
                                firstRowColumn = false;
                            } else {
                                step += vgap;
                            }

                            dim.width = Math.max(dim.width, currentSize);
                            dim.height += step;

                            currentSize = d.width + dx;
                            currentCount = 1;
                            step = d.height;
                        }
                    } else {    // Mode.COLUMNS
                        if (currentCount == 0) {
                            currentSize = d.height + dy;
                            currentCount++;
                            step = d.width;
                        } else if ((currentSize + vgap + d.height) <= h) {
                            currentSize += vgap + d.height;
                            currentCount++;
                            step = Math.max(step, d.width);
                        } else {
                            if (firstRowColumn) {
                                firstRowColumn = false;
                            } else {
                                step += hgap;
                            }

                            dim.height = Math.max(dim.height, currentSize);
                            dim.width += step;

                            currentSize = d.height + dy;
                            currentCount = 1;
                            step = d.width;
                        }
                    }
                }
            }

            if (mode == Mode.ROWS) {
                // the last row
                if (!firstRowColumn) step += vgap;
                dim.width = Math.max(dim.width, currentSize);
                dim.height += step;
            } else {
                // the last column
                if (!firstRowColumn) step += hgap;
                dim.height = Math.max(dim.height, currentSize);
                dim.width += step;
            }

            return dim;
        }
    }

    /**
     * Returns the preferred dimensions for this layout given the components
     * in the specified target container.
     *
     * @param target the component which needs to be laid out
     * @return    the preferred dimensions to lay out the
     *            subcomponents of the specified container.
     *
     * @see Container
     * @see #minimumLayoutSize
     * @see #maximumLayoutSize
     */
    public Dimension preferredLayoutSize(Container target) {
        return getLayoutSize(target, Size.PREFERRED);
    }

    /**
     * Returns the minimum dimensions needed to layout the components
     * contained in the specified target container.
     *
     * @param target the component which needs to be laid out
     * @return    the minimum dimensions to lay out the
     *            subcomponents of the specified container.
     *
     * @see #preferredLayoutSize
     * @see #maximumLayoutSize
     */
    public Dimension minimumLayoutSize(Container target) {
        return getLayoutSize(target, Size.MINIMUM);
    }

    /**
     * Returns the maximum size of this component.
     *
     * @param target the container being layed out
     * @return the maximum size of this component.
     *
     * @see #preferredLayoutSize
     * @see #minimumLayoutSize
     */
    public Dimension maximumLayoutSize(Container target) {
        return getLayoutSize(target, Size.MAXIMUM);
    }

    /**
     * Lays out the container. This method lets each component take
     * its preferred size by reshaping the components in the
     * target container in order to satisfy the constraints of
     * this <code>RowColumnLayout</code> object.
     *
     * @param target the specified component being laid out.
     */
    public void layoutContainer(Container target) {
        synchronized (target.getTreeLock()) {
            Insets insets = target.getInsets();
            int w = target.getWidth();
            int h = target.getHeight();
            int x = insets.left;
            int y = insets.top;
            int step = 0;

            Size size;
            Dimension pref = preferredLayoutSize(target);
            if (pref.width <= w || pref.height <= h) {
                size = Size.PREFERRED;
            } else {
                size = Size.MINIMUM;
            }

            int n = target.getComponentCount();
            int currentCount = 0;
            for (int i = 0; i < n; i++) {
                Component c = target.getComponent(i);
                if (c.isVisible()) {
                    Dimension d = size.getSize(c);
                    if (mode == Mode.ROWS) {
                        if (currentCount == 0) {
                            c.setBounds(x, y, d.width, d.height);
                            x = insets.left + d.width + hgap;
                            step = d.height;
                            currentCount++;
                        } else if ((x+hgap+d.width+insets.right) <= w) {
                            c.setBounds(x, y, d.width, d.height);
                            x += d.width + hgap;
                            step = Math.max(d.height, step);
                            currentCount++;
                        } else {
                            y += step + vgap;
                            c.setBounds(insets.left, y, d.width, d.height);
                            x = insets.left + d.width + hgap;
                            currentCount = 1;
                        }
                    } else {    // Mode.COLUMNS
                        if (currentCount == 0) {
                            c.setBounds(x, y, d.width, d.height);
                            y = insets.top + d.height + vgap;
                            step = d.width;
                            currentCount++;
                        } else if ((y+vgap+d.height +insets.bottom) <= h) {
                            c.setBounds(x, y, d.width, d.height);
                            y += vgap + d.height;
                            step = Math.max(step, d.width);
                            currentCount++;
                        } else {
                            x += step + vgap;
                            c.setBounds(x, insets.top, d.width, d.height);
                            y = insets.top + d.height + vgap;
                            currentCount = 1;
                        }
                    }
                }
            }
        }
    }

    /**
     * Returns a string representation of this object. This method
     * is intended to be used only for debugging purposes, and the
     * content and format of the returned string may vary between
     * implementations. The returned string may be empty but may not
     * be <code>null</code>.
     *
     * @return  a string representation of this object.
     */
    public String toString() {
        return getClass().getName() + "[hgap=" + hgap + ",vgap=" + vgap +
            ",mode=" + mode + "]";
    }

    /**
     * Adds the specified component to the layout. Not used by this class.
     * @param name the name of the component
     * @param comp the component to be added
     */
    public void addLayoutComponent(String name, Component comp) {
    }

    /**
     * Removes the specified component from the layout. Not used by
     * this class.
     * @param comp the component to remove
     */
    public void removeLayoutComponent(Component comp) {
    }

    /**
     * Adds the specified component to the layout, using the specified
     * constraint object.
     * @param comp the component to be added
     * @param constraints  where/how the component is added to the layout.
     */
    public void addLayoutComponent(Component comp, Object constraints) {
    }

    /**
     * Invalidates the layout, indicating that if the layout manager
     * has cached information it should be discarded.
     *
     * @param target the container being layed out
     */
    public void invalidateLayout(Container target) {
    }

    /**
     * Returns the alignment along the x axis.  This specifies how
     * the component would like to be aligned relative to other
     * components.  The value should be a number between 0 and 1
     * where 0 represents alignment along the origin, 1 is aligned
     * the furthest away from the origin, 0.5 is centered, etc.
     *
     * @param target the container being layed out
     * @return the alignment along the x axis.
     */
    public float getLayoutAlignmentX(Container target) {
        return 0;
    }

    /**
     * Returns the alignment along the y axis.  This specifies how
     * the component would like to be aligned relative to other
     * components.  The value should be a number between 0 and 1
     * where 0 represents alignment along the origin, 1 is aligned
     * the furthest away from the origin, 0.5 is centered, etc.
     *
     * @param target the container being layed out
     * @return the alignment along the y axis.
     */
    public float getLayoutAlignmentY(Container target) {
        return 0;
    }
}
