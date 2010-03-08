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

/* java.io */
import java.io.Serializable;

/* java.awt */
import java.awt.Graphics;
import java.awt.Insets;
import java.awt.Rectangle;
import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.BorderLayout;
import java.awt.LayoutManager;
import java.awt.LayoutManager2;

/* java.swing */
import javax.swing.JPanel;
import javax.swing.UIManager;
import javax.swing.JComponent;

/* java.swing.border */
import javax.swing.border.Border;
import javax.swing.border.AbstractBorder;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.Enum;

/**
 * A Panel with an arbitrary border with the addition of an arbitrary
 * {@link Component} in a specified position and justification. Looks
 * much like a <code>TitledBorder<code> but allows you to place an
 * arbitrary component in place of the text label. It could not be
 * implemented as a border because a component that needs to receive
 * focus and keyboard events must be a part of the component tree.
 *
 */
public class BorderPanel extends JPanel {

    private static class Constraint extends Enum {
        Constraint(String s) { super(s); }
    };

    // layout constraints
    private static final Constraint BORDER_COMPONENT = new Constraint("TITLE");
    private static final Constraint CONTENT_PANEL = new Constraint("CONTENT");

    /** Use the default vertical orientation for the component. */
    static public final int DEFAULT_POSITION = 0;
    /** Position the component above the border's top line. */
    static public final int ABOVE_TOP        = 1;
    /** Position the component in the middle of the border's top line. */
    static public final int TOP              = 2;
    /** Position the component below the border's top line. */
    static public final int BELOW_TOP        = 3;
    /** Position the component above the border's bottom line. */
    static public final int ABOVE_BOTTOM     = 4;
    /** Position the component in the middle of the border's bottom line. */
    static public final int BOTTOM           = 5;
    /** Position the component below the border's bottom line. */
    static public final int BELOW_BOTTOM     = 6;

    /** Use the default justification for the component. */
    static public final int DEFAULT_JUSTIFICATION   = 7;
    /** Position component at the left side of the border line. */
    static public final int LEFT    = 8;
    /** Position component in the center of the border line. */
    static public final int CENTER  = 9;
    /** Position component at the right side of the border line. */
    static public final int RIGHT   = 10;

    // instance data
    private ComponentBorder actualBorder;
    private JPanel contentPanel;
    private Component borderComponent;
    private int componentPosition;
    private int componentJustification;
    private boolean borderPaintedUnderComponent = false;
    private boolean horizontalFill = false;
    private boolean tiny = false;
    private boolean layoutValid = false;
    private boolean inited = false;

    /**
     * creates <code>BorderPanel</code> with default border and
     * no title component
     */
    public BorderPanel() {
        this(null, null);
    }

    /**
     * creates <code>BorderPanel</code> with specified border and
     * no title component
     */
    public BorderPanel(Border border) {
        this(border, null);
    }

    /**
     * creates <code>BorderPanel</code> with specified title component
     * and default border
     */
    public BorderPanel(Component c) {
        this(null, c);
    }

    /**
     * creates <code>BorderPanel</code> with specified border and
     * title component
     *
     * @param border  the border
     * @param c       the component the border should contain
     */
    public BorderPanel(Border border, Component c) {
        this(border, c, DEFAULT_JUSTIFICATION, DEFAULT_POSITION);
    }

    /**
     * creates <code>BorderPanel</code> with specified border and
     * title component
     *
     * @param border  the border
     * @param c       the component the border should contain
     * @param justify the justification for the border component
     * @param pos the position for the border component
     */
    public BorderPanel(Border border, Component c, int justify, int pos) {
        super(false);
        inited = true;
        actualBorder = new ComponentBorder(border);
        contentPanel = new JPanel(new BorderLayout());
        borderComponent = c;
        setComponentJustification(justify);
        setComponentPosition(pos);
        super.setBorder(actualBorder);
        super.setLayout(new Layout());
        add(contentPanel, CONTENT_PANEL);
        if (c != null) add(c, BORDER_COMPONENT);
    }

    /**
     * layout of <code>BorderPanel</code> cannot be changed.
     * This method always throws {@link UnsupportedOperationException}
     */
    public final void setLayout(LayoutManager mgr) {
        if (inited) {
            throw new UnsupportedOperationException("cannot change layout for BorderPanel");
        } else {
            // JPanel sets layout in its constructor, ignore it
        }
    }

    /**
     * border of the <code>BorderPanel</code> cannot be changed.
     * This method always throws {@link UnsupportedOperationException}
     */
    public final void setBorder(Border b) {
        if (inited) {
            throw new UnsupportedOperationException("cannot change border for BorderPanel");
        } else {
            // JPanel sets border in its constructor, ignore it
        }
    }

    /**
     * returns the content panel. Other components must be added to the
     * content panel rather than to the <code>BorderPanel</code> itself
     */
    public final JPanel getContentPanel() {
        return contentPanel;
    }

    /**
     * sets the border component
     */
    public final void setBorderComponent(Component c) {
        if (borderComponent != c) {
            int n;
            while ((n = getComponentCount()) > 1) remove(n-1);
            borderComponent = c;
            if (c != null) add(c, BORDER_COMPONENT);
            invalidate();
        }
    }

    /**
     * returns the position of the border component.
     */
    public final int getComponentPosition() {
        return componentPosition;
    }

    /**
     * returns the justification of the border component.
     */
    public final int getComponentJustification() {
        return componentJustification;
    }

    /**
     * returns the "real" position of the border component.
     * That is, if the position is {@link #DEFAULT_POSITION} this method
     * returns that actual position it translates into.
     */
    public final int getRealComponentPosition() {
        return ((componentPosition == DEFAULT_POSITION) ? TOP :
                 componentPosition);
    }

    /**
     * returns the "real" justification of the border component.
     * That is, if the position is {@link #DEFAULT_JUSTIFICATION} this method
     * returns that actual justification it translates into.
     */
    public final int getRealComponentJustification() {
        return ((componentJustification == DEFAULT_JUSTIFICATION) ? LEFT :
                 componentJustification);
    }

    /**
     * Returns the horizontal fill parameter. If this parameter is
     * <code>true</code>, the border component gets stretched to fill
     * all available horizontal space
     */
    public final boolean getHorizontalFill() {
        return horizontalFill;
    }

    /**
     * sets the position of the border component.
     * @param pos the position for the border component
     */
    public final void setComponentPosition(int pos) {
        if (componentPosition != pos) {
            switch (pos) {
            case ABOVE_TOP:
            case TOP:
            case BELOW_TOP:
            case ABOVE_BOTTOM:
            case BOTTOM:
            case BELOW_BOTTOM:
            case DEFAULT_POSITION:
                componentPosition = pos;
                invalidate();
                break;
            default:
                throw new IllegalArgumentException(pos + " is not a valid position.");
            }
        }
    }

    /**
     * sets the justification of the border component.
     * @param justify the justification for the border component
     */
    public final void setComponentJustification(int justify) {
        if (componentJustification != justify) {
            switch (justify) {
            case LEFT:
            case CENTER:
            case RIGHT:
            case DEFAULT_JUSTIFICATION:
                componentJustification = justify;
                invalidate();
                break;
            default:
                throw new IllegalArgumentException(justify + " is not a valid justification.");
            }
        }
    }

    /**
     * Sets the horizontal fill parameter
     *
     * @param fill <code>true</code> to resize the border component to
     * completely fill the upper part of the border
     */
    public final void setHorizontalFill(boolean fill) {
        if (horizontalFill != fill) {
            horizontalFill = fill;
            invalidate();
            validate();
            repaint();
        }
    }

    /**
     * Determines whether or not to paint the border under the border
     * component. The default is <code>false</code>.
     *
     * @param paint <code>true</code> paint the border under the border
     * component
     */
    public final void setBorderPaintedUnderComponent(boolean paint) {
        if (borderPaintedUnderComponent != paint) {
            borderPaintedUnderComponent = paint;
            repaint();
        }
    }

    //=======================================================================
    //          L A Y O U T    M A N A G E R
    //=======================================================================

    /** space between the border and the component's edge */
    static public final int EDGE_SPACING = 2;

    /** space between the border and border component */
    static public final int COMPONENT_SPACING = 2;

    /** horizontal inset of border component that is left or right justified */
    static public final int COMPONENT_INSET_H = 5;

    /**
     * layout manager for {@link BorderPanel}
     */
    private class Layout implements LayoutManager2, Serializable {

        /**
         * adds the specified component with the specified name to
         * the layout.
         *
         * @param name the component name
         * @param comp the component to be added
         */
        public void addLayoutComponent(String name, Component comp) {
            addLayoutComponent(comp, name);
        }

        /**
         * adds the specified component to the layout, using the specified
         * constraint object.
         */
        public void addLayoutComponent(Component comp, Object constraint) {
            // cannot add arbitrary components
            if (constraint!=BORDER_COMPONENT && constraint!=CONTENT_PANEL) {
                throw new IllegalArgumentException("invalid constraint");
            }
        }

        /**
         * removes the specified component from this border layout. This
         * method is called when a container calls its <code>remove</code> or
         * <code>removeAll</code> methods. Most applications do not call this
         * method directly.
         *
         * @param   comp   the component to be removed.
         */
        public void removeLayoutComponent(Component comp) {
        }

        /**
         * determines the minimum or preferred size of the
         * container using this layout manager.
         * <p>
         *
         * @param   target    the container in which to do the layout.
         * @param   dim       the dimensions of the content panel
         *
         * @return  the updated dimensions.
         *
         */
        public Dimension updateLayoutSize(Container target, Dimension dim) {

            if (target != BorderPanel.this) {
                throw new IllegalArgumentException("wrong container");
            }

            Border border = actualBorder.getBorder();

            // border only
            if (border != null && borderComponent == null) {
                Insets insets = border.getBorderInsets(target);
                dim.width += insets.left + insets.right;
                dim.height += insets.top + insets.bottom;

            // component only
            } else if (border == null && borderComponent != null) {
                Dimension d = borderComponent.getPreferredSize();
                dim.height += 2*COMPONENT_SPACING + d.height;
                dim.width = Math.max(d.width+2*COMPONENT_INSET_H+EDGE_SPACING,
                                     dim.width) + 2*EDGE_SPACING;

            // both border and component
            } else if (border != null && borderComponent != null) {
                Dimension d = borderComponent.getPreferredSize();

                // horizontal size
                Insets insets = border.getBorderInsets(target);
                dim.width = Math.max(d.width + 2*COMPONENT_INSET_H, dim.width)
                            + insets.left + insets.right + 2*EDGE_SPACING;

                // vertical size
                dim.height += insets.top + insets.bottom + EDGE_SPACING;
                switch (getRealComponentPosition()) {
                case ABOVE_TOP:
                case BELOW_TOP:
                case ABOVE_BOTTOM:
                case BELOW_BOTTOM:
                    dim.height += d.height + 2*COMPONENT_SPACING;
                    break;
                case TOP:
                    dim.height += Math.max(d.height + COMPONENT_SPACING*2,
                                           insets.top + EDGE_SPACING) -
                                           insets.top;
                    break;
                case BOTTOM:
                    dim.height += Math.max(d.height + COMPONENT_SPACING*2,
                                           insets.bottom + EDGE_SPACING) -
                                           insets.bottom;
                    break;
                }
            }

            return dim;
        }

        /**
         * determines the minimum size of the container using this
         * layout manager.
         *
         * @param   target   the container in which to do the layout.
         * @return  the minimum dimensions needed to lay out the subcomponents
         *          of the specified container.
         */
        public Dimension minimumLayoutSize(Container target) {
            return updateLayoutSize(target, contentPanel.getMinimumSize());
        }

        /**
         * determines the preferred size of the container using this
         * layout manager.
         *
         * @param   target   the container in which to do the layout.
         * @return  the minimum dimensions needed to lay out the subcomponents
         *          of the specified container.
         */
        public Dimension preferredLayoutSize(Container target) {
            return updateLayoutSize(target, contentPanel.getPreferredSize());
        }

        /**
         * returns the maximum dimensions for this layout given the components
         * in the specified target container.
         *
         * @param target the component which needs to be laid out
         *
         * @see #minimumLayoutSize
         * @see #preferredLayoutSize
         */
        public Dimension maximumLayoutSize(Container target) {
            return new Dimension(Integer.MAX_VALUE, Integer.MAX_VALUE);
        }

        /**
         * returns the alignment along the x axis.  This specifies how
         * the component would like to be aligned relative to other
         * components.  The value should be a number between 0 and 1
         * where 0 represents alignment along the origin, 1 is aligned
         * the furthest away from the origin, 0.5 is centered, etc.
         */
        public float getLayoutAlignmentX(Container parent) {
            return 0.5f;
        }

        /**
         * returns the alignment along the y axis.  This specifies how
         * the component would like to be aligned relative to other
         * components.  The value should be a number between 0 and 1
         * where 0 represents alignment along the origin, 1 is aligned
         * the furthest away from the origin, 0.5 is centered, etc.
         */
        public float getLayoutAlignmentY(Container parent) {
            return 0.5f;
        }

        /**
         * invalidates the layout, indicating that if the layout manager
         * has cached information it should be discarded.
         */
        public void invalidateLayout(Container target) {
            layoutValid = false;
        }

        /**
         * lays out the container argument using this border layout.
         */
        public void layoutContainer(Container target) {
            if (target != BorderPanel.this) {
                throw new IllegalArgumentException("wrong container");
            }

            int width = target.getWidth();
            int height = target.getHeight();

            Border border = actualBorder.getBorder();

            if (borderComponent == null) {
                if (border != null) {
                    Insets insets = border.getBorderInsets(target);
                    contentPanel.setBounds(insets.left, insets.top,
                        Math.max(width - insets.right - insets.left, 0),
                        Math.max(height - insets.bottom - insets.top, 0));
                } else {
                    contentPanel.setBounds(0, 0, width, height);
                }

            } else {
                int top = 0;
                int bottom = 0;
                int left = 0;
                int right = 0;

                if (border != null) {
                    Insets insets = border.getBorderInsets(target);
                    top = insets.top;
                    bottom = insets.bottom;
                    left = insets.left + EDGE_SPACING;
                    right = insets.right + EDGE_SPACING;
                }

                int pos = getRealComponentPosition();
                Dimension dim = borderComponent.getPreferredSize();

                if (horizontalFill) {
                    switch (pos) {
                    case TOP:
                    case BOTTOM:
                    case BELOW_TOP:
                    case ABOVE_BOTTOM:
                        int extra = (2*COMPONENT_INSET_H + left + right);
                        dim.width = Math.max(width - extra,0);
                        break;
                    case ABOVE_TOP:
                    case BELOW_BOTTOM:
                        dim.width = width;
                        break;
                    }
                }

                int x = 0;
                int y = 0;
                int w = dim.width;
                int h = dim.height;
                tiny = false;

                if (height >= (2*COMPONENT_SPACING + EDGE_SPACING)+
                              dim.height + top + bottom) {

                    if (border != null) {
                        switch (pos) {
                        case TOP:
                        case ABOVE_TOP:
                        case BELOW_TOP:
                            bottom += EDGE_SPACING;
                            break;

                        case BOTTOM:
                        case ABOVE_BOTTOM:
                        case BELOW_BOTTOM:
                            top += EDGE_SPACING;
                            break;
                        }
                    }

                    switch (pos) {
                    case ABOVE_TOP:
                        y = COMPONENT_SPACING;
                        top += dim.height + COMPONENT_SPACING + EDGE_SPACING;
                        break;

                    case BELOW_BOTTOM:
                        y = height - dim.height - COMPONENT_SPACING;
                        bottom += dim.height + 2*COMPONENT_SPACING;
                        break;

                    case TOP:
                        if (dim.height+2*COMPONENT_SPACING > top+EDGE_SPACING) {
                            top = dim.height + 2*COMPONENT_SPACING;
                            y = COMPONENT_SPACING;
                        } else {
                            top += EDGE_SPACING;
                            y = (top - dim.height)/2;
                        }
                        break;

                    case BOTTOM:
                        if (dim.height+2*COMPONENT_SPACING > bottom+EDGE_SPACING) {
                            bottom = dim.height + 2*COMPONENT_SPACING;
                            y = height - dim.height - COMPONENT_SPACING;
                        } else {
                            bottom += EDGE_SPACING;
                            y = height - dim.height - (bottom - dim.height)/2;
                        }
                        break;

                    case BELOW_TOP:
                        y = EDGE_SPACING + top + COMPONENT_SPACING;
                        top += EDGE_SPACING + dim.height + 2*COMPONENT_SPACING;
                        break;

                    case ABOVE_BOTTOM:
                        y = height - EDGE_SPACING - bottom - COMPONENT_SPACING -
                            dim.height;
                        bottom += EDGE_SPACING + dim.height + 2*COMPONENT_SPACING;
                        break;
                    }
                } else {
                    tiny = true;
                    h = Math.min(height, dim.height);
                    y = Math.min(COMPONENT_SPACING, (height-h)/2);
                    switch (pos) {
                    case BOTTOM:
                    case ABOVE_BOTTOM:
                    case BELOW_BOTTOM:
                        y = height - h - y;
                        break;
                    }
                }

                if (width >= ((COMPONENT_INSET_H+EDGE_SPACING)*2+dim.width+left+right)) {
                    switch (getRealComponentJustification()) {
                    case LEFT:
                        x = left + COMPONENT_INSET_H;
                        break;
                    case RIGHT:
                        x = width - right - COMPONENT_INSET_H - dim.width;
                        break;
                    case CENTER:
                        x = (width - dim.width)/2;
                        break;
                    }
                } else {
                    w = Math.min(width, dim.width);
                    x = Math.min(left+COMPONENT_INSET_H, (width-w)/2);
                }

                borderComponent.setBounds(x, y, w, h);

                if (tiny) {
                    contentPanel.setBounds(left, top, 0, 0);
                } else {
                    contentPanel.setBounds(left, top,
                                           width - right - left,
                                           height - bottom - top);
                }
            }

            layoutValid = true;
        }
    }

    //=======================================================================
    //          B O R D E R
    //=======================================================================

    /**
     * this class implements a border for {@link BorderPanel}
     */
    private class ComponentBorder extends AbstractBorder {

        private Border border;

        /**
         * Creates a <code>ComponentBorder</code> instance with the specified
         * border, component, component-justification, and component-position.
         *
         * @param border  the border
         */
        public ComponentBorder(Border border) {
            this.border = border;
        }


        /**
         * returns the border of the component border.
         */
        public Border getBorder() {
            Border b = border;
            if (b == null) b = UIManager.getBorder("TitledBorder.border");
            return b;
        }

        /**
         * Paints the border for the specified component with the specified
         * position and size.
         *
         * @param c the component for which this border is being painted
         * @param g the paint graphics
         * @param x the x position of the painted border
         * @param y the y position of the painted border
         * @param w the width of the painted border
         * @param h the height of the painted border
         */
        public void paintBorder(Component c,Graphics g,int x,int y,int w,int h) {

            if (c != BorderPanel.this) {
                throw new IllegalArgumentException("wrong component");
            }

            Border b = getBorder();

            // handle the trivial case when there's no component
            if (borderComponent == null) {
                if (b != null) {
                    b.paintBorder(c, g, x, y, w, h);
                }
                return;
            }

            // validate layout if we haven't done so yet
            if (!layoutValid) getLayout().layoutContainer(BorderPanel.this);
            Rectangle compRect = borderComponent.getBounds();

            Rectangle grooveRect = new Rectangle(x, y, w, h);
            Insets insets;
            if (b != null) {
                insets = b.getBorderInsets(c);
                insets.left += EDGE_SPACING;
                insets.right += EDGE_SPACING;
                grooveRect.x += EDGE_SPACING;
                grooveRect.width -= 2*EDGE_SPACING;
            } else {
                insets = new Insets(0, 0, 0, 0);
            }

            int diff = 0;
            int totalCompHeight = compRect.height + 2*COMPONENT_SPACING;
            int componentSpacing = COMPONENT_SPACING;
            if (totalCompHeight > h) {
                if (compRect.height >= h) {
                    componentSpacing = 0;
                } else {
                    componentSpacing = (h - compRect.height)/2;
                }
            }

            int pos = getRealComponentPosition();
            if (b != null) {
                switch (pos) {
                case TOP:
                case ABOVE_TOP:
                case BELOW_TOP:
                    insets.bottom += EDGE_SPACING;
                    break;

                case BOTTOM:
                case ABOVE_BOTTOM:
                case BELOW_BOTTOM:
                    grooveRect.y += EDGE_SPACING;
                    insets.top += EDGE_SPACING;
                    break;
                }
                grooveRect.height -= EDGE_SPACING;
            }

            switch (pos) {
            case ABOVE_TOP:
                grooveRect.y += totalCompHeight;
                grooveRect.height -= totalCompHeight;
                break;
            case TOP:
                if (totalCompHeight > (EDGE_SPACING + insets.top)) {
                    int dy = componentSpacing+(compRect.height-insets.top)/2;
                    if (dy > grooveRect.y) {
                        grooveRect.height -= dy - grooveRect.y;
                        grooveRect.y = dy;
                     }
                }
                break;
            case BELOW_TOP:
            case ABOVE_BOTTOM:
                break;
            case BOTTOM:
                if (totalCompHeight > (EDGE_SPACING + insets.bottom)) {
                    int dy = componentSpacing+(compRect.height-insets.bottom)/2;
                    if (dy < grooveRect.height) {
                        grooveRect.height -= dy;
                     }
                }
                break;
            case BELOW_BOTTOM:
                grooveRect.height -= totalCompHeight;
                break;
            }

            // If title is positioned in middle of border we'll
            // need to paint the border in sections to leave
            // space for the component's background to show
            // through the title.
            if (b != null) {
                if (pos == TOP || pos == BOTTOM) {
                    if (borderPaintedUnderComponent) {

                        // clear the double buffereing flag, otherwise
                        // things get screwed up big time
                        boolean wasDoubleBuffered = false;
                        if (borderComponent instanceof JComponent) {
                            JComponent jc = (JComponent)borderComponent;
                            wasDoubleBuffered = jc.isDoubleBuffered();
                            jc.setDoubleBuffered(false);
                            setDoubleBuffered(jc, false);
                        } else if (borderComponent instanceof Container) {
                            setDoubleBuffered((Container)borderComponent, false);
                        }

                        // paint the border
                        b.paintBorder(c, g, grooveRect.x, grooveRect.y,
                                      grooveRect.width, grooveRect.height);

                        // ask the component to paint itself
                        int x1 = borderComponent.getX();
                        int y1 = borderComponent.getY();
                        g.translate(x1, y1);
                        borderComponent.paint(g);
                        g.translate( -x1, -y1);

                        // restore the double buffering flag
                        if (wasDoubleBuffered) {
                            JComponent jc = (JComponent)borderComponent;
                            jc.setDoubleBuffered(true);
                        }
                    } else {

                        // save original clip
                        Rectangle saveClip = g.getClipBounds();

                        // paint strip left of component
                        Rectangle clipRect = new Rectangle(saveClip);
                        if (GUI.intersect(clipRect, x, y, compRect.x, h)) {
                            g.setClip(clipRect);
                            b.paintBorder(c, g, grooveRect.x, grooveRect.y,
                                          grooveRect.width, grooveRect.height);
                        }

                        // paint strip right of component
                        clipRect.setBounds(saveClip);
                        if (GUI.intersect(clipRect,
                                          compRect.x + compRect.width, y,
                                          x+w-compRect.width-compRect.x, h)) {
                            g.setClip(clipRect);
                            b.paintBorder(c, g, grooveRect.x, grooveRect.y,
                                          grooveRect.width, grooveRect.height);
                        }

                        // paint strip below or above component
                        clipRect.setBounds(saveClip);
                        if (pos == TOP) {
                            if (GUI.intersect(clipRect, x,
                                              grooveRect.y + insets.top, w,
                                              h - grooveRect.y - insets.top)) {

                                g.setClip(clipRect);
                                b.paintBorder(c, g, grooveRect.x, grooveRect.y,
                                              grooveRect.width,
                                              grooveRect.height);
                            }
                        } else { // pos == BOTTOM
                            if (GUI.intersect(clipRect, x, y, w,
                                              grooveRect.height + grooveRect.y
                                              - insets.bottom)) {
                                g.setClip(clipRect);
                                b.paintBorder(c, g, grooveRect.x, grooveRect.y,
                                              grooveRect.width,
                                              grooveRect.height);
                            }
                        }

                        // restore clip
                        g.setClip(saveClip);
                    }

                } else {
                    b.paintBorder(c, g, grooveRect.x, grooveRect.y,
                                        grooveRect.width, grooveRect.height);
                }
            }
        }

        /**
         * returns the insets of the border.
         * @param c the component for which this border insets value applies
         */
        public Insets getBorderInsets(Component c) {
            return getBorderInsets(c, null);
        }

        /**
         * reinitialize the insets parameter with this Border's current Insets.
         * @param c the component for which this border insets value applies
         * @param insets the object to be reinitialized
         */
        public Insets getBorderInsets(Component c, Insets insets) {

            Border b = getBorder();
            if (b != null) {
                if (b instanceof AbstractBorder) {
                    if (insets == null) insets = new Insets(0,0,0,0);
                    ((AbstractBorder)b).getBorderInsets(c, insets);
                } else {
                    Insets i = b.getBorderInsets(c);
                    if (insets == null) {
                        insets = i;
                    } else {
                        insets.top = i.top;
                        insets.right = i.right;
                        insets.bottom = i.bottom;
                        insets.left = i.left;
                    }
                }
            } else if (insets == null) {
                insets = new Insets(0,0,0,0);
            }

            int top = insets.top;
            int bottom = insets.bottom;

            insets.left += EDGE_SPACING;
            insets.right += EDGE_SPACING;

            int totalCompHeight = borderComponent.getHeight() +
                                  2*COMPONENT_SPACING;

            if (borderComponent != null)    {
                switch (getRealComponentPosition()) {
                case ABOVE_TOP:
                case BELOW_TOP:
                    insets.top += totalCompHeight;
                    insets.bottom += EDGE_SPACING;
                    break;
                case ABOVE_BOTTOM:
                case BELOW_BOTTOM:
                    insets.bottom += totalCompHeight;
                    insets.top += EDGE_SPACING;
                    break;
                case TOP:
                    insets.bottom += EDGE_SPACING;
                    insets.top = Math.max(borderComponent.getHeight() +
                        COMPONENT_SPACING*2, insets.top + EDGE_SPACING);
                    break;
                case BOTTOM:
                    insets.top += EDGE_SPACING;
                    insets.bottom += Math.max(borderComponent.getHeight() +
                        COMPONENT_SPACING*2, insets.bottom + EDGE_SPACING);
                    break;
                }
            }
            return insets;
        }

        /**
         * returns whether or not the border is opaque.
         */
        public boolean isBorderOpaque() {
            return false;
        }
    }

    /**
     * Sets the double buffered flag for the whole hierarhy of components
     * including the container passed in as a parameter
     *
     * @param cont the root of the hierarhy
     * @param buffered the flag to set
     */
    private static void setDoubleBuffered(Container cont, boolean buffered) {
        int n = cont.getComponentCount();
        for (int i=0; i<n; i++) {
            Component c = cont.getComponent(i);
            if (c instanceof JComponent) {
                ((JComponent)c).setDoubleBuffered(buffered);
            }
            if (c instanceof Container) {
                setDoubleBuffered((Container)c, buffered);
            }
        }
    }
}
