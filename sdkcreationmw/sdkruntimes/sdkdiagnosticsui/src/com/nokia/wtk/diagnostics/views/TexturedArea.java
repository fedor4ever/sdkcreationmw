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





package com.nokia.wtk.diagnostics.views;

import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.TexturePaint;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;

import javax.swing.ImageIcon;
import javax.swing.JComponent;
/**
 * <p>Title: </p>
 * <p>Description: A simple JComponent that repeats (by tiling) the image
 * given to it in its constructor as a texture</p>
 * <p>Company: Nokia Corporation</p>
 * @version 1.0
 */
public class TexturedArea extends JComponent {

    private TexturePaint textureP_;

    /**
     * Make a new stretchable textured area that will tile an icon provided
     */
    public TexturedArea() {}
    /**
     * Make a new stretchable textured area that will tile the icon provided
     * @param icon The ImageIcon to tile
     */
    public TexturedArea(ImageIcon icon) {
        setImage(icon);
    }
    public void paintComponent(Graphics g) {
        Graphics2D g2 = (Graphics2D) g;
        g2.setPaint(textureP_);
        g2.fill(g2.getClip());
        super.paintComponent(g);
    }
    /**
     * Set the background image for the textured icon
     * @param icon the iMageIcon that will be repeated over and over
     */
    public void setImage(ImageIcon icon) {
        BufferedImage bImage =
            new BufferedImage(
                icon.getIconWidth(),
                icon.getIconHeight(),
                BufferedImage.TYPE_INT_ARGB);
        Graphics bGraphics = bImage.getGraphics();
        bGraphics.drawImage(icon.getImage(), 0, 0, icon.getImageObserver());
        textureP_ =
            new TexturePaint(
                bImage,
                new Rectangle2D.Double(
                    0,
                    0,
                    bImage.getWidth(),
                    bImage.getHeight()));
        bImage.flush();
        revalidate();
    }
}
