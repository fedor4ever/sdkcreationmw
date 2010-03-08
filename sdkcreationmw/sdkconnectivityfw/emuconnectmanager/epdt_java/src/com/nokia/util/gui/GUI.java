/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.util.gui;


import javax.swing.JLabel;
import javax.swing.JTextArea;
import javax.swing.LookAndFeel;
import javax.swing.text.JTextComponent;


public abstract class GUI
{
    
    //======================================================================
    //          T E X T    C O M P O N E N T
    //======================================================================

    /**
     * Creates a readonly text component which looks like a {@link JLabel}.
     * But unlike a {@link JLabel}, it wraps the text and supports
     * selection and copy to clipboard.
     */
    public static JTextComponent createTextComponent(String text) {
        JTextArea tc = new TextComponent();
        tc.setEditable(false);
        tc.setWrapStyleWord(true);
        tc.setText(text);
        return tc;
    }
    
    /**
     * A text component which looks like a {@link JLabel}.
     * Unlike a {@link JLabel}, it wraps the text and supports
     * selection and copy to clipboard.
     */
    private static class TextComponent extends JTextArea {
        TextComponent() {
             setup();
        }

        /**
         * Sets up the text component to look like {@link JLabel}.
         * Invoked from the constructor or after the UI switch
         */
        private void setup() {
            setOpaque(false);
            setLineWrap(true);
            LookAndFeel.installColorsAndFont(this, "Label.background",
                                                   "Label.foreground",
                                                   "Label.font");
        }
        public boolean isFocusTraversable() {
            return false;
        }
        public void updateUI() {
            super.updateUI();
            setup();
        }

        /**
         * JTextArea allows paste into a disabled and/or readonly
         * text area. Ha!
         */
        public void paste() {
            if (isEditable() && isEnabled()) {
                super.paste();
            }
        }
    }
}
