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




package com.nokia.wtk.diagnostics.views.support;

import java.awt.Insets;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

import javax.swing.Icon;
import javax.swing.JLabel;
import javax.swing.JPopupMenu;
import javax.swing.SwingConstants;

/**
 * <p>Title: </p>
 * <p>Description: A label that, when the mouse enters, will pop up a full
 * length menu/tooltip-like window to show its contentrs.</p>
 * <p>Company: Nokia Corporation</p>
 * @version 1.0
 */
public class PopLabel extends JLabel {
    /**
     * Creates a PopLabel with an empty string and no image.
     */
    public PopLabel() { this(" ", null, SwingConstants.LEFT); }

    /**
     * Creates a PopLabel with the specified text
     * @param text the text to set.
     */
    public PopLabel(String text) { this(text, null, SwingConstants.LEFT); }

    /**
     * Creates a PopLabel with an image and horzontal alignment
     * @param icon the image to set
     * @param horizontalAlignment the alignment of the text
     */
    public PopLabel(Icon icon, int horizontalAlignment) {
        this(" ", icon, horizontalAlignment);
    }

    /**
     * Creates a PopLabel with text and horzontal alignment
     * @param text the text to display
     * @param horizontalAlignment the alignment of the text
     */
    public PopLabel(String text, int horizontalAlignment) {
        this(text, null, horizontalAlignment);
    }
    /**
     * Creates a PopLabel with the specified image
     * @param icon the image to set
     */
    public PopLabel(Icon icon) { this("", icon, SwingConstants.LEFT); }

    /**
     * Creates a PopLabel with text, and icon, and a specific horizontal alignment
     * of the text and image
     * @param text the text to display
     * @param image the image to display
     * @param horizontalAlignment the alignment of the text or image, one
     * of SwingConstants.LEFT, RIGHT, or CENTER
     */
    public PopLabel(String text, Icon image, int horizontalAlignment) {
        super(text, image, horizontalAlignment);
        addMouseListener(new PopMouseListener(this));
    }

    class PopMouseListener extends MouseAdapter {
        JPopupMenu menu_ = new JPopupMenu();
        JLabel popLabel_ = new JLabel("");

        final JLabel label_;

        PopMouseListener(JLabel label) {
            label_ = label;
            menu_.setInvoker(label_);
            menu_.add(popLabel_);
            menu_.addMouseListener(new MouseAdapter() {
                public void mouseExited(MouseEvent me) {
                    menu_.setVisible(false);
                }
            } );
        }

        public void mouseExited( MouseEvent e ) {
            if ( !menu_.contains( e.getPoint() ) ) {
                menu_.setVisible( false );
            }
        }

        public void mouseEntered(MouseEvent e) {
            popLabel_.setText(label_.getText());
            popLabel_.setIcon(label_.getIcon());
            if ( label_.getSize().getWidth() <
                label_.getPreferredSize().getWidth() ) {
                Insets insets = null;
                if ( label_.getBorder() != null ) {
                    insets = label_.getBorder().getBorderInsets( label_ );
                }
                menu_.show( label_, ( insets != null ? insets.left : 0 ), 0 );
            }
        }
    }
}
