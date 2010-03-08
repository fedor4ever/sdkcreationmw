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
import java.awt.Component;

/* javax.swing.border */
import javax.swing.border.Border;

/**
 * A class which implements a simple thin single line bevel border.
 * The border colors are defined by the background color of the component
 * being painted, unlike {@link ThinBevelBorder} where they are either
 * specified or by default defined by UIManager (<tt>"controlLtHighlight"</tt>
 * and <tt>"controlDkShadow"</tt> colors).
 *
 */
public class LightBevelBorder extends ThinBevelBorder {

    /**
     * Shared instance of lowered <code>LightBevelBorder</code>
     */
    private static LightBevelBorder lowered = null;

    /**
     * Shared instance of raised <code>LightBevelBorder</code>
     */
    private static LightBevelBorder raised = null;

    /**
     * Returns shared instance of lowered <code>LightBevelBorder</code>
     */
    public static Border getLoweredBorder() {
        if (lowered == null) {
            lowered = new LightBevelBorder(LOWERED);
        }
        return lowered;
    }

    /**
     * Returns shared instance of raised <code>LightBevelBorder</code>
     */
    public static Border getRaisedBorder() {
        if (raised == null) {
            raised = new LightBevelBorder(RAISED);
        }
        return raised;
    }

    /**
     * Creates a lowered bevel border whose colors will be derived
     * from the background color of the component passed into the
     * {@link #paintBorder paintBorder()} method.
     */
    public LightBevelBorder() {
        super(LOWERED);
    }

    /**
     * Creates a bevel border with the specified type and whose
     * colors will be derived from the background color of the
     * component passed into the {@link #paintBorder paintBorder()}
     * method.
     *
     * @param bevelType the type of bevel for the border
     */
    public LightBevelBorder(int bevelType) {
        super(bevelType);
    }

    /**
     * Returns the highlight color of the bevel border.
     */
    public Color getHighlightColor(Component c) {
        Color back = getBackground(c);
        Color bright = back.brighter();
        if (back.equals(bright)) {
            // white background, me guess...
            Color dark = back.darker();
            int r1 = back.getRed();
            int r2 = dark.getRed();
            int g1 = back.getGreen();
            int g2 = dark.getGreen();
            int b1 = back.getBlue();
            int b2 = dark.getBlue();
            bright = new Color((r1+r2)/2,(g1+g2)/2,(b1+b2)/2,back.getAlpha());
        }
        return bright;
    }

    /**
     * Returns the shadow color of the bevel border.
     */
    public Color getShadowColor(Component c) {
        Color back = getBackground(c);
        Color dark = back.darker();
        if (back.equals(dark)) {
            // black background, me guess...
            Color bright = back.brighter();
            int r1 = back.getRed();
            int r2 = bright.getRed();
            int g1 = back.getGreen();
            int g2 = bright.getGreen();
            int b1 = back.getBlue();
            int b2 = bright.getBlue();
            dark = new Color((r1+r2)/2,(g1+g2)/2,(b1+b2)/2,back.getAlpha());
        }
        return dark;
    }

    /**
     * Determines the effective background color for the component.
     * If the component is transparent, if walks up the component stack
     * and if it finds an opaque parent, it returns the parent's background
     * color. Otherwise, it returns the background color of the component
     * itself.
     *
     * @param c the component for which to determine the background color
     * @return the effective background color for the specified component.
     */
    private static Color getBackground(Component c) {
        Component c1 = c;
        while (c1 != null && !c1.isOpaque()) c1 = c1.getParent();
        return (c1 != null) ? c1.getBackground() : c.getBackground();
    }
}
