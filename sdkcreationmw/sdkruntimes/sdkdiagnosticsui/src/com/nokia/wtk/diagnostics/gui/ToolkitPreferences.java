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





package com.nokia.wtk.diagnostics.gui;

/* java.util */
import java.awt.Component;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;

import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextArea;
import javax.swing.LookAndFeel;
import javax.swing.SwingConstants;
import javax.swing.border.CompoundBorder;
import javax.swing.border.EmptyBorder;
import javax.swing.border.TitledBorder;
import javax.swing.text.JTextComponent;

import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.app.LookFactory;
import com.nokia.wtk.util.MessageWriters;
import com.nokia.wtk.util.PropertySet;
import com.nokia.wtk.util.gui.PropPage;
import com.nokia.wtk.util.gui.PropPagePanel;

/**
 * Toolkit preferences
 *
 */
class       ToolkitPreferences
implements  SwingConstants, MessageWriters, Resource {

    private static final int GAP = LookFactory.getGap();
    private static PropertySet ps = null;

    public static final String HOME_PAGE_PROPERTY = "home_page";

    static {
        AppEnvironment.addResources(
                        "resources/ToolkitPreferences.properties",
                        ToolkitPreferences.class);
    }

    /**
      * toolkit search for this method for the preference participant and register the properties.
      */
     public static PropertySet getPreferences() {
        if (ps == null){
            ps = new PropertySet();
            ps.addProperty(HOME_PAGE_PROPERTY, String.class, null, true);
       }
        return ps;
     }


    /**
     * Creates all available pages in the right order
     */
    public static PropPage [] createPages(PropertySet ps) {
        // NOTE: the names of subsets must match those in Toolkit.properties
        return new PropPage [] {
        };
    }

    //======================================================================
    //      U T I L I T I E S
    //======================================================================


    private static void setPrefSize(PropPagePanel panel) {
        panel.setPreferredSize(new Dimension(
                                    AppEnvironment.getInt(PREF_PANEL_WIDTH),
                                    AppEnvironment.getInt(PREF_PANEL_HEIGHT)));
    }

    /**
     * Same as createRow() but the left component occupies the extra space
     */
    private static Component createRow2(Component left, Component right) {

        JPanel panel = new JPanel(new GridBagLayout());
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.gridheight = GridBagConstraints.REMAINDER;
        gbc.weighty = 1;
        gbc.weightx = 1;

        panel.add(left, gbc);

        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.insets.left = GAP;
        gbc.weightx = 0;

        panel.add(right, gbc);
        return panel;
    }

    private static Component createRawPanel(Component[] c, String title) {

        Component [] rowComponents;

        if (c == null) {
            rowComponents = new Component[] {
                new JLabel("Not Available", JLabel.CENTER), null
            };
        } else {
            rowComponents = new Component[2*c.length];
            for (int i=0; i<c.length; i++) {
                rowComponents[2*i] = c[i];
                rowComponents[2*i+1] = null;
            }
        }

        JPanel rawPanel = LookFactory.createRowPanel(rowComponents,0);
        rawPanel.setBorder(new CompoundBorder(
                           new TitledBorder(title),
                           new EmptyBorder(0,GAP,0,GAP)));

        return rawPanel;
    }

    /**
     * Creates a text component which looks like a {@link JLabel}.
     * But unlike a {@link JLabel}, it wraps the text and supports
     * selection and copy to clipboard.
     */
    private static JTextComponent createTextComponent(String text) {
        JTextArea tc = new JTextArea() {
            { setup(); }
            private void setup() {
                setOpaque(false);
                setLineWrap(true);
                LookAndFeel.installColorsAndFont(this, "Label.background",
                                                       "Label.foreground",
                                                       "Label.font");
            }

            // this method is deprecated- should not be used
            /*
            public boolean isFocusTraversable() {
                return false;
            }
            */

            public boolean isFocusable() {
                return false;
            }
            public void updateUI() {
                super.updateUI();
                setup();
            }
            // JTextArea allows paste into a disabled and/or readonly
            // text area. Ha!
            public void paste() {
                if (isEditable() && isEnabled()) {
                    super.paste();
                } else {
                    getToolkit().beep();
                }
            }
        };
        tc.setEditable(false);
        tc.setWrapStyleWord(true);
        tc.setText(text);
        return tc;
    }




}


