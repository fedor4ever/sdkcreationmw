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

import java.awt.*;
import javax.swing.*;

/**
 * ComponentBox provides several class methods
 * that are useful for containers using BoxLayout --
 * even non-Box containers.  These methods extend
 * those of Box by providing support for visible components
 * with similar constrained layout to the invisible variants
 * provided by the Box methods.
 *
 * <p>
 *
 * The ComponentBox class can create several kinds
 * of visible component containers that affect layout:
 * glue, strut, and rigid components.
 * A "glue" component is allowed to vary in size to fit its container.
 * A "strut" component has an explicitly defined fixed size which cannot change.
 * A "rigid" component has a fixed size which is the preferred size of the component.
 * Variations on these component types affect horizontal, vertical and minimum-size behavior.
 *
 * @see Box
 * @see BoxLayout
 */
public class ComponentBox {
    /**
     * Creates a component that's always the specified size.
     *
     * @param c the component
     * @return the component
     * @see #createGlueComponent
     * @see #createHorizontalStrutComponent
     * @see #createVerticalStrutComponent
     */
    public static Component createRigidComponent(Component c) {
        if (c == null) {
            throw new IllegalArgumentException("null component");
        }
        Dimension d = new Dimension(PREFERRED_SIZE, PREFERRED_SIZE);
        return new ConstrainedComponent(c, d, d, d);
    }

    /**
     * Creates an invisible, fixed-width component.
     * In a horizontal box,
     * you typically use this method
     * to force a certain amount of space between two components.
     * In a vertical box,
     * you might use this method
     * to force the box to be at least the specified width.
     * The invisible component has no height
     * unless excess space is available,
     * in which case it takes its share of available space,
     * just like any other component that has no maximum height.
     *
     * @param c the component
     * @param width the width of the invisible component, in pixels >= 0
     * @return the component
     * @see #createVerticalStrut
     * @see #createGlue
     * @see #createRigidArea
     */
    public static Component createHorizontalStrutComponent(Component c, int width) {
        Dimension d = new Dimension(width, PREFERRED_SIZE);
        return (c != null) ? new ConstrainedComponent(c, d, d, d)
                           : Box.createHorizontalStrut(width);
    }

    /**
     * Creates an invisible, fixed-height component.
     * In a vertical box,
     * you typically use this method
     * to force a certain amount of space between two components.
     * In a horizontal box,
     * you might use this method
     * to force the box to be at least the specified height.
     * The invisible component has no width
     * unless excess space is available,
     * in which case it takes its share of available space,
     * just like any other component that has no maximum width.
     *
     * @param c the component
     * @param height the height of the invisible component, in pixels >= 0
     * @return the component
     * @see #createHorizontalStrut
     * @see #createGlue
     * @see #createRigidArea
     */
    public static Component createVerticalStrutComponent(Component c, int height) {
        Dimension d = new Dimension(PREFERRED_SIZE, height);
        return (c != null) ? new ConstrainedComponent(c, d, d, d)
                           : Box.createVerticalStrut(height);
    }

    /**
     * Creates an invisible "glue" component
     * that can be useful in a Box
     * whose visible components have a maximum width
     * (for a horizontal box)
     * or height (for a vertical box).
     * You can think of the glue component
     * as being a gooey substance
     * that expands as much as necessary
     * to fill the space between its neighboring components.
     *
     * <p>
     *
     * For example, suppose you have
     * a horizontal box that contains two fixed-size components.
     * If the box gets extra space,
     * the fixed-size components won't become larger,
     * so where does the extra space go?
     * Without glue,
     * the extra space goes to the right of the second component.
     * If you put glue between the fixed-size components,
     * then the extra space goes there.
     * If you put glue before the first fixed-size component,
     * the extra space goes there,
     * and the fixed-size components are shoved against the right
     * edge of the box.
     * If you put glue before the first fixed-size component
     * and after the second fixed-size component,
     * the fixed-size components are centered in the box.
     *
     * <p>
     *
     * To use glue,
     * call <code>Box.createGlue</code>
     * and add the returned component to a container.
     * The glue component has no minimum or preferred size,
     * so it takes no space unless excess space is available.
     * If excess space is available,
     * then the glue component takes its share of available
     * horizontal or vertical space,
     * just like any other component that has no maximum width or height.
     *
     * @param c the component
     * @return the component
     */
    public static Component createGlueComponent(Component c) {
        if (c != null) {
            return new ConstrainedComponent(c, new Dimension(SMALLEST_SIZE, SMALLEST_SIZE),
                                               new Dimension(SMALLEST_SIZE, SMALLEST_SIZE),
                                               new Dimension(LARGEST_SIZE, LARGEST_SIZE));
        } else {
            return Box.createGlue();
        }
    }

    /**
     * Creates a horizontal glue component.
     *
     * @param c the component
     * @return the component
     */
    public static Component createHorizontalGlueComponent(Component c) {
        if (c != null) {
            return new ConstrainedComponent(c, new Dimension(SMALLEST_SIZE, PREFERRED_SIZE),
                                               new Dimension(SMALLEST_SIZE, PREFERRED_SIZE),
                                               new Dimension(LARGEST_SIZE, PREFERRED_SIZE));
        } else {
            return Box.createHorizontalGlue();
        }
    }

    /**
     * Creates a vertical glue component.
     *
     * @param c the component
     * @return the component
     */
    public static Component createVerticalGlueComponent(Component c) {
        if (c != null) {
            return new ConstrainedComponent(c, new Dimension(PREFERRED_SIZE, SMALLEST_SIZE),
                                               new Dimension(PREFERRED_SIZE, SMALLEST_SIZE),
                                               new Dimension(PREFERRED_SIZE, LARGEST_SIZE));
        } else {
            return Box.createVerticalGlue();
        }
    }


    /**
     * An implementation of a lightweight component container
     * with constrained layout dimensions.
     */
    public static class ConstrainedComponent extends Container {
        /**
         * Constructor to create container with the given size ranges.
         *
         * @param c     The component
         * @param min   Minimum size
         * @param pref  Preferred size
         * @param max   Maximum size
         */
        public ConstrainedComponent(Component c, Dimension min, Dimension pref, Dimension max) {
            setLayout(new BorderLayout());
            component = c;
            reqMin = min;
            reqPref = pref;
            reqMax = max;
            add(c, BorderLayout.CENTER);
        }

        /**
         * Change the size requests for this shape.  An invalidate() is
         * propagated upward as a result so that layout will eventually
         * happen with using the new sizes.
         *
         * @param min   Value to return for getMinimumSize
         * @param pref  Value to return for getPreferredSize
         * @param max   Value to return for getMaximumSize
         */
        public void changeShape(Dimension min, Dimension pref, Dimension max) {
            reqMin = min;
            reqPref = pref;
            reqMax = max;
            invalidate();
        }

        // ---- Component methods ------------------------------------------

        /**
         * Returns the minimum size of the component.
         *
         * @return the size
         */
        public Dimension getMinimumSize() {
            return getConstrainedDimension(reqMin, MINIMUM_SIZE);
        }

        /**
         * Returns the preferred size of the component.
         *
         * @return the size
         */
        public Dimension getPreferredSize() {
            return getConstrainedDimension(reqPref, PREFERRED_SIZE);
        }

        /**
         * Returns the maximum size of the component.
         *
         * @return the size
         */
        public Dimension getMaximumSize() {
            return getConstrainedDimension(reqMax, MAXIMUM_SIZE);
        }

        /**
         * Returns an actual size of the component with constraints applied.
         *
         * @param constraint a constraint dimension of this constrained component
         * @param defaultConstraint a default constraint parameter to use for
         *        unknown constraint setting in <tt>constraint</tt>.
         * @return the constrained size
         */
        private Dimension getConstrainedDimension(Dimension constraint, int defaultConstraint) {
            int width = 0, height = 0;
            if (constraint.width == MINIMUM_SIZE) {
                width = component.getMinimumSize().width;
            } else if (constraint.width == PREFERRED_SIZE) {
                width = component.getPreferredSize().width;
            } else if (constraint.width == MAXIMUM_SIZE) {
                width = component.getMaximumSize().width;
            } else if (constraint.width >= SMALLEST_SIZE) {

                // respect component's preferred size
                if (defaultConstraint == PREFERRED_SIZE) {
                    width = Math.max(component.getPreferredSize().width,
                                     constraint.width);
                } else {
                    width = constraint.width;
                }

            } else if (defaultConstraint == MINIMUM_SIZE) {
                // Default to minimum component size
                width = component.getMinimumSize().width;
            } else if (defaultConstraint == PREFERRED_SIZE) {
                // Default to preferred component size
                width = component.getPreferredSize().width;
            } else if (defaultConstraint == MAXIMUM_SIZE) {
                // Default to maximum component size
                width = component.getMaximumSize().width;
            }

            if (constraint.height == MINIMUM_SIZE) {
                height = component.getMinimumSize().height;
            } else if (constraint.height == PREFERRED_SIZE) {
                height = component.getPreferredSize().height;
            } else if (constraint.height == MAXIMUM_SIZE) {
                height = component.getMaximumSize().height;
            } else if (constraint.height >= SMALLEST_SIZE) {

                // respect component's preferred size
                if (defaultConstraint == PREFERRED_SIZE ) {
                    height = Math.max(component.getPreferredSize().height,
                                      constraint.height);
                } else {
                    height = constraint.height;
                }

            } else if (defaultConstraint == MINIMUM_SIZE) {
                // Default to minimum component size
                height = component.getMinimumSize().height;
            } else if (defaultConstraint == PREFERRED_SIZE) {
                // Default to preferred component size
                height = component.getPreferredSize().height;
            } else if (defaultConstraint == MAXIMUM_SIZE) {
                // Default to maximum component size
                height = component.getMaximumSize().height;
            }

            return new Dimension(width, height);
        }

        // ---- member variables ---------------------------------------

        private Component component;
        private Dimension reqMin;
        private Dimension reqPref;
        private Dimension reqMax;
    }


    private static final int PREFERRED_SIZE = -1;
    private static final int MINIMUM_SIZE = -2;
    private static final int MAXIMUM_SIZE = -3;
    private static final int SMALLEST_SIZE = 0;
    private static final int LARGEST_SIZE = Short.MAX_VALUE;
}
