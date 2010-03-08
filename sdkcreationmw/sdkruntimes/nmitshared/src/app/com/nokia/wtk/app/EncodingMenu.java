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



package com.nokia.wtk.app;

import java.awt.event.ActionListener;
import java.io.InputStreamReader;
import java.nio.charset.Charset;
import java.nio.charset.IllegalCharsetNameException;
import java.nio.charset.UnsupportedCharsetException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.Map;

import javax.swing.ButtonGroup;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JRadioButton;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

/**
 * A class that presents a list of supported encodings that both
 * we are interested in and that is supported by the platform.
 * The JMenu takes a Saveable; when a different encoding is
 * selected, the menu calls Saveable.setCharEncoding().
 *
 */
public class EncodingMenu extends JMenu {
    /**
     * List of character encodings we advertise that we support.
     * @see java.nio.charset.Charset 
     * @see Java internationalization document on encoding in documentation set.
     */
    final private static String[] SUPPORTED_ENCODINGS = { 
            "US-ASCII", 
            "UTF-8", 
            "UTF-16", // AKA ISO-10646-UCS-2.  Java implements this as UTF-16BE (w BOM)
            "ISO-8859-1",
            "ISO-8859-2",
            "ISO-8859-3",
            "ISO-8859-4",
            "ISO-8859-5",
            "ISO-8859-6",
            "ISO-8859-7",
            "ISO-8859-8",
            "ISO-8859-9",
            "GBK",
            "BIG5",
            "SHIFT_JIS"
    };

    // Setting menu is initialized with
    private static final String INITIAL_DEFAULT = "UTF-8";
    private static final String SYS_DEFAULT_LABEL = "System Default [";
    private static String gDefaultEncLabel = "";

    // The name of the platform-default encoding (we don't expose name to user)
    private static String gDefaultEncoding = "";

    private static String[] gAvailableEncodings = null;

    // OBJECT DATA

    // Document we're setting the encoding for, if any.
    private Saveable mEditor = null; 

    // Common listener for each menu item 
    private ActionListener mEncListener = null;

    // convenience array of checkboxes 
    private JRadioButton[] mCheckBoxes = null;

    /***************** Static Methods ***********************/

    /**
     * Return only encodings we are interested in AND which are
     * supported on this platform.
     **/
    public static String[] getFullList() {
        if (gAvailableEncodings == null) {
            ArrayList list = new ArrayList();
            Charset cs = null;

            // First add the supported ones on the list
            for (int i = 0; i < SUPPORTED_ENCODINGS.length; i++) {
                String name = SUPPORTED_ENCODINGS[i];
                try {
                    //
                    // If this charset is not supported,
                    // catch the exception and don't add to list.
                    //
                    cs = Charset.forName(name);
                    list.add(name);
                } catch (IllegalArgumentException ex1) {
                }
            }

            // Finally, add default encoding for this platform.
            try {
                InputStreamReader sr = new InputStreamReader(System.in);
                cs = Charset.forName(sr.getEncoding());
                gDefaultEncoding = cs.displayName();
                gDefaultEncLabel = SYS_DEFAULT_LABEL + gDefaultEncoding + "]";
                list.add(gDefaultEncLabel);
            } catch (IllegalArgumentException ex1) { 
            }
          
            gAvailableEncodings = (String[]) list.toArray(new String[] {});
        }
        return gAvailableEncodings;
    }

    /**
     * Translate between a menu item string and the actual encoding name.
     * Currently, this is only to fix the System Default issue.
     * @param name char encoding String
     * @return normalized char encoding String
     */
    public static String normalizeEncodingName(String name) {
        if (name == null) return null;
        getFullList();	// be sure list is initialized
        if (name.equals(getPlatformEncodingLabel())) {
            name = getPlatformEncoding();
        }
        return name;
    }

    /**
     * Return the name of the platform encoding
     */
    public static String getPlatformEncoding() {
        return gDefaultEncoding;
    }

    /**
     * Return the shown string for the platform encoding
     *   ... just because they wanted the words "Sys Default" next to it.
     */
    public static String getPlatformEncodingLabel() {
        return gDefaultEncLabel;
    }

    /***************** Object Methods ***********************/

    /**
     * Constructor
     * @see com.nokia.wtk.app.Saveable
     */
   public EncodingMenu() {
       this("Encoding", null);
   }

    /**
     * Constructor
     * @param Saveable editor we are setting encoding for (or null if none)
     * @see com.nokia.wtk.app.Saveable
     */
   public EncodingMenu(final Saveable editor) {
       this("Encoding", editor);
   }

    /**
     * Constructor
     * @param String title for menu.
     * @param Saveable editor we are setting encoding for (or null if none)
     * @see com.nokia.wtk.app.Saveable
     */
   public EncodingMenu(String title, final Saveable editor) {
        super(title);
        mEditor = editor;
        if (mEditor != null) {
            mEncListener = new ActionListener() {
              public void actionPerformed(ActionEvent ae) {
                  String enc = (String)ae.getActionCommand();
                  enc = normalizeEncodingName(enc);
                  mEditor.setCharEncoding(enc);
              }
            };
        }
        ButtonGroup group = new ButtonGroup();
        String[] encs = getFullList();
        mCheckBoxes = new JRadioButton[encs.length];
        for (int i = 0; i < encs.length; i++) {
            JRadioButton rb = new JRadioButton(encs[i]);
            mCheckBoxes[i] = rb;
            group.add(rb);
            if (encs[i].equals(INITIAL_DEFAULT)) {
                rb.setSelected(true);
            }    
            if (mEncListener != null) {
                rb.addActionListener(mEncListener);
            }
            super.add(rb);
        }
    }

    /**
     * Set the encoding selection that matches the argument.
     * @param String encoding string
     */
    public void setEncoding(String enc) {
        if (enc == null || enc.length() == 0) {
            return;
        }
        for (int i = 0; i < mCheckBoxes.length; i++) {
            String n = mCheckBoxes[i].getActionCommand();
            if (enc.equalsIgnoreCase(n)) {
                mCheckBoxes[i].setSelected(true);
                return;
            }
        }

        // Special case system default, since label is different from value

        if (enc.equalsIgnoreCase(gDefaultEncoding)) {
            for (int i = 0; i < mCheckBoxes.length; i++) {
            	String n = mCheckBoxes[i].getActionCommand();
                if (n.equalsIgnoreCase(gDefaultEncLabel)) {
                    mCheckBoxes[i].setSelected(true);
                    return;
                }
            }
        }
    }

    /**
     * Unit Test of list
     */
    public static void main(String[] args) {
        String[] list = EncodingMenu.getFullList();
        System.out.println("Supported Encodings:");
        for (int i = 0; i < list.length; i++) {
            System.out.println("  " + list[i]);
        }
    }
}

