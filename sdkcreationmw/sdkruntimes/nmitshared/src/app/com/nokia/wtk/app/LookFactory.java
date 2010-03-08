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

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Container;
import java.awt.Dialog;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Frame;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.Point;
import java.awt.Toolkit;
import java.awt.Window;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import javax.swing.AbstractAction;
import javax.swing.Action;
import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.Icon;
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JRootPane;
import javax.swing.KeyStroke;
import javax.swing.SwingUtilities;
import javax.swing.border.Border;
import javax.swing.border.SoftBevelBorder;

import com.nokia.wtk.util.gui.LightBevelBorder;
import com.nokia.wtk.util.gui.ThinBevelBorder;

/** A class to streamline look of borders and edges. Since border instances
 * should be shared, getting them all from here means the look of the whole
 * system will be easily synchronized.
 */
public class LookFactory {

    static {
        AppEnvironment.addResources(
            "resources/LookFactory.properties",
            LookFactory.class);
    }
    static public class CodeColorSet {
        final public Color selectionColor = Color.green.darker().darker();
        final public Color selectedTextColor = Color.white;
        final public Color backgroundColor = Color.white;
        final public Color foregroundColor = Color.black;
        final public Color backgroundNonEditableColor =
            new Color(255, 255, 224);
    }

    private final static CodeColorSet codeColorSet = new CodeColorSet();

    private final static int GAP =
        AppEnvironment.getInt(Resource.VISUAL_GAP, 2);

    private static Border thinLoweredBorder;
    private static Border lightLoweredBorder;
    private static Border loweredBorder;
    private static Border raisedBorder;
    private static Border etchedBorder;
    private static Border nullBorder;
    private static Border emptyBorder;
    private static Border mediumBorder;
    private static Border wideBorder;

    private static Font plainFont;
    private static Font boldFont;
    private static Font codeFont;
    private static Font codeBoldFont;

    /** Returns standard gap between elements of the layout */
    public static int getGap() {
        return GAP;
    }
    public static Border thinLoweredBorder() {
        if (thinLoweredBorder == null) {
            thinLoweredBorder = new ThinBevelBorder();
        }
        return thinLoweredBorder;
    }
    public static Border lightLoweredBorder() {
        if (lightLoweredBorder == null) {
            lightLoweredBorder = new LightBevelBorder();
        }
        return lightLoweredBorder;
    }
    public static Border loweredBorder() {
        if (loweredBorder == null) {
            loweredBorder = new SoftBevelBorder(SoftBevelBorder.LOWERED);
        }

        return loweredBorder;
    }
    public static Border raisedBorder() {
        if (raisedBorder == null) {
            raisedBorder = new SoftBevelBorder(SoftBevelBorder.RAISED);
        }
        return raisedBorder;
    }
    public static Border etchedBorder() {
        if (etchedBorder == null) {
            etchedBorder = BorderFactory.createEtchedBorder();
        }
        return etchedBorder;
    }
    public static Border nullBorder() {
        if (nullBorder == null) {

            nullBorder = BorderFactory.createEmptyBorder(0, 0, 0, 0);

        }
        return nullBorder;
    }
    public static Border emptyBorder() {
        if (emptyBorder == null) {
            emptyBorder = BorderFactory.createEmptyBorder(1, 1, 1, 1);
        }
        return emptyBorder;
    }
    public static Border mediumBorder() {
        if (mediumBorder == null) {
            mediumBorder = BorderFactory.createEmptyBorder(GAP, GAP, GAP, GAP);
        }
        return mediumBorder;
    }
    public static Border wideBorder() {
        if (wideBorder == null) {
            wideBorder = BorderFactory.createEmptyBorder(9, 9, 8, 8);
        }
        return wideBorder;
    }
    /**
     * Return the standard font that will be used system wide. The actual
     * font and size is taken from the properties file for LookFactory,
     * and can thus be overridden by the central application that
     * implements AppEnvironment. If all
     * parts of the application and libraries use this call consistently
     * for their look, the application developer will have a unified
     * look that can be set properly.
     * @return Font the standard font used. Defaults in this library to
     * 11 point sans serif.
     */
    public static Font getStandardPlainFont() {
        if (plainFont == null) {
            resetStandardPlainFont();
        }
        return plainFont;
    }

    /**
     * Returns the default standard plain font to what it was
     * defined as in the properties
     * resources file, no matter what the application has set with
     * setStandardPlainFont. This call is to give library and application
     * users a way to backtrack and return to a default situation. 
     * @return Font
     * @see getStandardPlainFont
     * @see setStandardPlainFont
     */
    public static void resetStandardPlainFont() {
        plainFont =
            new Font(
                AppEnvironment.getString(Resource.PLAIN_FONT_NAME, "sansserif"),
                Font.PLAIN,
                AppEnvironment.getInt(Resource.PLAIN_FONT_SIZE, 11));
        boldFont = null;

    }
    /**
     * Allows to set the system-wide standard plain font. Setting this
     * font will all elements created subsequently that call 
     * getStandardCodeFont have the font set here. This call will
     * also reset the bold font. Visual elements previously created
     * that called getStandardPlainFont() will not be updated. 
     * @param f the font to set. If the font is null, the call is a 
     * no-op.
     * @see getStandardPlainFont
     * @see getDefaultStandardPlainFont
     */
    public static void setStandardPlainFont(Font f) {
        if (f != null) {
            plainFont = f;
            boldFont = null;
        }
    }
    /**
     * A standard version of the plain font
     * @return Font a bold version of the standard plain font
     * @see getStandardPlainFont
     **/
    public static Font getStandardBoldFont() {
        if (boldFont == null) {
            boldFont = getStandardPlainFont().deriveFont(Font.BOLD);
        }
        return boldFont;
    }
    /**
     * Return the standard bold version of getStandardCodeFomt()
     * @return Font a bold version of standard Code font
     */
    public static Font getStandardCodeBoldFont() {
        if (codeBoldFont == null) {
            codeBoldFont = getStandardCodeFont().deriveFont(Font.BOLD);

        }
        return codeBoldFont;
    }
    /**
     * Return the standard font that will be used system wide for displaying
     * code. The actual
     * font and size is taken from the properties file for LookFactory,
     * and can thus be overridden by the central application that
     * implements AppEnvironment. If all
     * parts of the application and libraries use this call consistently
     * for their look, the application developer will have a unified
     * look that can be set properly.
     * @return Font the standard font used. Defaults in this library to
     * 11 point monospaced.
     */
    public static Font getStandardCodeFont() {
        if (codeFont == null) {
            resetStandardCodeFont();
        }
        return codeFont;

    }
    /**
         * Returns the default standard code font to the settings in the properties
         * resources file, no matter what the application has set with
         * setStandardPlainFont. This call is to give library and application
         * users a way to backtrack and return to a default situation.
     * @see getStandardCodeFont, setStandardCodeFont
         */
    public static void resetStandardCodeFont() {
        codeFont =
            new Font(
                AppEnvironment.getString(Resource.CODE_FONT_NAME, "monospaced"),
                Font.PLAIN,
                AppEnvironment.getInt(Resource.CODE_FONT_SIZE, 11));
        codeBoldFont = null;
    }
    /**
     * Allows to set the system-wide standard code font. Setting this
     * font will all elements created subsequently that call 
     * getStandardCodeFont have the font set here. This call will
     * also reset the bold font. Visual elements previously created
     * that called getStandardPlainFont() will not be updated. 
     * @param f the font to set. If the font is null, the call is a 
     * no-op.
     * @see getStandardCodeFont
     * @see getDefaultStandardCodeFont
     */
    public static void setStandardCodeFont(Font f) {
        if (f != null) {
            codeFont = f;
            codeBoldFont = null;
        }
    }

    public static CodeColorSet getCodeColorSet() {
        return codeColorSet;
    }

    /*
     * Given three components, return the mimumum height of the
     * components.  [Wouldn't varargs() be nice ...].
     *@param c1 component
     *@param c2 component
     *@param c3 component
     */
    public static int getSmallestHeight(
        JComponent c1,
        JComponent c2,
        JComponent c3) {

        Dimension d1 = null, d2 = null, d3 = null;
        int smallest = -1;

        if (c1 != null)
            d1 = c1.getMinimumSize();
        if (c2 != null)
            d2 = c2.getMinimumSize();
        if (c3 != null)
            d3 = c3.getMinimumSize();

        if (d1 != null)
            smallest = d1.height;
        if ((d2 != null) && ((smallest == -1) || (d2.height < smallest)))
            smallest = d2.height;
        if ((d3 != null) && ((smallest == -1) || (d3.height < smallest)))
            smallest = d3.height;

        return smallest;
    }

    /*
     * Given two components, change the size of the smaller
     * component so that it has the same width and height as
     * the larger component.
     * @param c1 the first component. Should not be null.
     * @param c2 the second component. Should not be null.
     * @param widthPad the number of pixels to expand the width.
     * Set to -1 for no equalizing.
     * @param defaultHeight the height to set for each component. Cannot be 0.
     */
    public static void equalizeSize(
        JComponent c1,
        JComponent c2,
        int widthPad,
        int defaultHeight) {
        Dimension d1, d2;

        d1 = c1.getMinimumSize();
        d2 = c2.getMinimumSize();

        if (widthPad != -1) {
            Dimension d3 = new Dimension();
            d3.width = Math.max(d1.width, d2.width) + widthPad;
            d3.height = defaultHeight;
            c1.setPreferredSize(d3);
            c2.setPreferredSize(d3);
        } else {
            c1.setPreferredSize(new Dimension(d1.width, defaultHeight));
            c2.setPreferredSize(new Dimension(d2.width, defaultHeight));
        }
    }

    /**
     * Sets preferred size for all the components so that they are all have
     * the same (maximum) width.
     *
     * @return the "maximized" width
     */
    public static int maximizeWidth(JComponent[] c) {
        return maximizeWidth(c, 0);
    }

    /**
     * Sets preferred size for all the components so that they are all have
     * the same (maximum) width.
     *
     * @return the "maximized" width
     */
    public static int maximizeWidth(JComponent[] c, int add) {
        int width = 0;
        for (int i = 0; i < c.length; i++) {
            width = Math.max(width, c[i].getPreferredSize().width);
        }
        width += add;
        for (int i = 0; i < c.length; i++) {
            Dimension size = c[i].getPreferredSize();
            size.width = width;
            c[i].setPreferredSize(size);
        }
        return width;
    }

    /**
     * Creates a panel that organizes its children in rows, two components
     * in each. Unlike <code>GridLayout(0,2)</code>, it does not equalize
     * the widths of the columns and does not stretch them vertically.
     */
    public static JPanel createRowPanel(Component[] c) {
        return createRowPanel(c, GAP);
    }

    /**
     * Creates a panel that organizes its children in rows, two components
     * in each. Unlike <code>GridLayout(0,2)</code>, it does not equalize
     * the widths of the columns and does not stretch them vertically.
     */
    public static JPanel createRowPanel(Component[] c, int gap) {
        return createRowPanel(c, gap, gap);
    }

    /**
     * Creates a panel that organizes its children in rows, two components
     * in each. Unlike <code>GridLayout(0,2)</code>, it does not equalize
     * the widths of the columns and does not stretch them vertically.
     */
    public static JPanel createRowPanel(Component[] c, int hgap, int vgap) {
        return createRowPanel(c, hgap, vgap, 1.0, 1.0);
    }

    /**
     * Creates a panel that organizes its children in rows, two components
     * in each. Unlike <code>GridLayout(0,2)</code>, it does not equalize
     * the widths of the columns and does not stretch them vertically.
     */
    public static JPanel createRowPanel(
        Component[] c,
        int hgap,
        int vgap,
        double weight1,
        double weight2) {
        JPanel p = new JPanel(new GridBagLayout());

        int n = c.length / 2;
        if (n > 0) {
            GridBagConstraints gbc1 = new GridBagConstraints();
            gbc1.anchor = GridBagConstraints.CENTER;
            gbc1.fill = GridBagConstraints.HORIZONTAL;
            gbc1.weightx = weight1;
            gbc1.insets.bottom = vgap;
            gbc1.insets.right = hgap;

            GridBagConstraints gbc2 = (GridBagConstraints) gbc1.clone();
            gbc2.gridwidth = GridBagConstraints.REMAINDER;
            gbc2.insets = new Insets(0, 0, vgap, 0);
            gbc2.weightx = weight2;
            gbc2.insets.right = 0;

            // middle rows
            for (int i = 0; i < (n - 1); i++) {
                Component c1 = c[2 * i];
                Component c2 = c[2 * i + 1];
                if (c2 != null) {
                    if (c1 != null)
                        p.add(c1, gbc1);
                    p.add(c2, gbc2);
                } else if (c1 != null) {
                    p.add(c1, gbc2); // use gbc2 for c1
                }
            }

            // last row
            gbc1.anchor = gbc2.anchor = GridBagConstraints.NORTHWEST;
            gbc1.gridheight = gbc2.gridheight = GridBagConstraints.REMAINDER;
            gbc1.weighty = gbc2.weighty = 1;
            gbc1.insets.bottom = gbc2.insets.bottom = 0;

            Component c1 = c[2 * n - 2];
            Component c2 = c[2 * n - 1];
            if (c2 != null) {
                if (c1 != null)
                    p.add(c1, gbc1);
                p.add(c2, gbc2);
            } else if (c1 != null) {
                gbc2.weightx = 1;
                p.add(c1, gbc2); // use gbc2 for c1
            }
        }

        return p;
    }

    /**
     * Creates a dialog given a content panel and set of actions.
     */
    public static void layoutDialog(
        JDialog dialog,
        Component content,
        Action[] actions,
        Action cancel) {
        layoutDialog(dialog, null, content, actions, cancel);

    }

    /**
     * Creates a dialog given an icon, a content panel and set of actions.
    
    
     */
    public static void layoutDialog(
        JDialog dialog,
        Icon icon,
        Component content,
        Action[] actions,
        Action cancel) {

        JPanel panel = new JPanel(new BorderLayout(getGap(), getGap()));
        panel.setBorder(mediumBorder());

        // add optional icon
        if (icon != null) {
            JPanel iconPanel = new JPanel(new BorderLayout());
            iconPanel.add(new JLabel(icon), BorderLayout.NORTH);
            iconPanel.add(Box.createGlue(), BorderLayout.CENTER);
            panel.add(iconPanel, BorderLayout.WEST);
        }

        // add the content
        panel.add(content, BorderLayout.CENTER);

        // and finally action buttons at the bottom
        JButton defaultButton = null;
        JButton cancelButton = null;
        if (actions != null && actions.length > 0) {
            JPanel buttons = new JPanel();
            buttons.setLayout(new BoxLayout(buttons, BoxLayout.X_AXIS));
            buttons.add(Box.createHorizontalGlue());
            for (int i = 0; i < actions.length; i++) {
                if (i > 0)
                    buttons.add(Box.createHorizontalStrut(getGap()));
                JButton button = new JButton(actions[i]);
                if (defaultButton == null)
                    defaultButton = button;
                if (actions[i] == cancel)
                    cancelButton = button;
                buttons.add(button);
            }
            panel.add(buttons, BorderLayout.SOUTH);
        }

        // support for Escape key:
        if (cancel != null) {

            // if we have a button associated with the cancel action,
            // simulate a button press when escape is pressed. Otherwise,
            // just use the provided action.
            Action cancelAction;
            if (cancelButton != null) {
                final JButton button = cancelButton;
                cancelAction = new AbstractAction(button.getText()) {
                    public void actionPerformed(ActionEvent e) {
                        if (button.isEnabled()) {
                            button.doClick();
                        }
                    }
                };
            } else {
                cancelAction = cancel;
            }

            // add appropriate action to the input map:
            String key = "cancel";
            panel.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW).put(
                KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0),
                key);
            panel.getActionMap().put(key, cancelAction);
        }

        // when dialog becomes visible, request focus to the first action
        if (defaultButton != null) {
            final JButton button = defaultButton;
            dialog.addWindowListener(new WindowAdapter() {
                public void windowOpened(WindowEvent e) {
                    JRootPane root = SwingUtilities.getRootPane(button);
                    if (root != null)
                        root.setDefaultButton(button);
                    button.requestFocus();
                }
            });
        }

        // add the panel to the content pane
        Container contentPane = dialog.getContentPane();
        contentPane.removeAll();
        contentPane.setLayout(new BorderLayout());
        contentPane.add(panel, BorderLayout.CENTER);

        // center dialog on screen or relative to its parent
        dialog.pack();
        dialog.setLocationRelativeTo(dialog.getParent());
    }

    /**
     * Creates a dialog given a content panel and set of actions.
     */
    public static void layoutDialog(
        JDialog dialog,
        Component content,
        Action[] actions,
        Action cancel,
        boolean focus) {
        layoutDialog(dialog, null, content, actions, cancel, focus);
    }

    /**
     * Creates a dialog given an icon, a content panel and set of actions.
     * the parameter focus is for the dialog needs to have original focus on the textfield
     * instead of one of the buttons.
     */
    public static void layoutDialog(
        JDialog dialog,
        Icon icon,
        Component content,
        Action[] actions,
        Action cancel,
        boolean focus) {

        JPanel panel = new JPanel(new BorderLayout(getGap(), getGap()));
        panel.setBorder(mediumBorder());

        // add optional icon
        if (icon != null) {
            JPanel iconPanel = new JPanel(new BorderLayout());
            iconPanel.add(new JLabel(icon), BorderLayout.NORTH);
            iconPanel.add(Box.createGlue(), BorderLayout.CENTER);
            panel.add(iconPanel, BorderLayout.WEST);
        }

        // add the content
        panel.add(content, BorderLayout.CENTER);

        // and finally action buttons at the bottom
        JButton defaultButton = null;
        JButton cancelButton = null;
        if (actions != null && actions.length > 0) {
            JPanel buttons = new JPanel();
            buttons.setLayout(new BoxLayout(buttons, BoxLayout.X_AXIS));
            buttons.add(Box.createHorizontalGlue());
            for (int i = 0; i < actions.length; i++) {
                if (i > 0)
                    buttons.add(Box.createHorizontalStrut(getGap()));
                JButton button = new JButton(actions[i]);
                if (defaultButton == null)
                    defaultButton = button;
                if (actions[i] == cancel)
                    cancelButton = button;
                buttons.add(button);
            }
            panel.add(buttons, BorderLayout.SOUTH);
        }

        // support for Escape key:
        if (cancel != null) {

            // if we have a button associated with the cancel action,
            // simulate a button press when escape is pressed. Otherwise,
            // just use the provided action.
            Action cancelAction;
            if (cancelButton != null) {
                final JButton button = cancelButton;
                cancelAction = new AbstractAction(button.getText()) {
                    public void actionPerformed(ActionEvent e) {
                        if (button.isEnabled()) {
                            button.doClick();
                        }
                    }
                };
            } else {
                cancelAction = cancel;
            }

            // add appropriate action to the input map:
            String key = "cancel";
            panel.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW).put(
                KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0),
                key);
            panel.getActionMap().put(key, cancelAction);
        }

        // when dialog becomes visible, request focus to the first action
        if (focus == true) {
            if (defaultButton != null) {
                final JButton button = defaultButton;
                dialog.addWindowListener(new WindowAdapter() {
                    public void windowOpened(WindowEvent e) {
                        JRootPane root = SwingUtilities.getRootPane(button);
                        if (root != null)
                            root.setDefaultButton(button);
                        button.requestFocus();
                    }
                });
            }
        }
        // add the panel to the content pane
        Container contentPane = dialog.getContentPane();
        contentPane.removeAll();
        contentPane.setLayout(new BorderLayout());
        contentPane.add(panel, BorderLayout.CENTER);

        // center dialog on screen or relative to its parent
        dialog.pack();
        dialog.setLocationRelativeTo(dialog.getParent());
    }
    /**
     * Returns the specified component's toplevel <code>Frame</code> or
     * <code>Dialog</code>.
     *
     * @param comp the <code>Component</code> to check for a
     *		<code>Frame</code> or <code>Dialog</code>
     * @return the <code>Frame</code> or <code>Dialog</code> that
     *		contains the component, or the default
     *         	frame if the component is <code>null</code>,
     *		or does not have a valid
     *         	<code>Frame</code> or <code>Dialog</code> parent
     */
    static Window getWindowForComponent(Component comp) {
        if (comp == null) {
            return JOptionPane.getRootFrame();
        } else if (comp instanceof Frame || comp instanceof Dialog) {
            return (Window) comp;
        } else {
            Window window = SwingUtilities.getWindowAncestor(comp);
            return ((window == null) ? JOptionPane.getRootFrame() : window);
        }
    }

    /**
     * Creates and returns a new modal <code>JDialog</code> that has the same
     * parent window as the specified component.
     *
     * @param parent determines the frame in which the dialog
     *		is displayed; if the <code>parent</code> has
     *		no <code>Frame</code>, a default <code>Frame</code> is used
     * @param title     the title string for the dialog
     * @return a new <code>JDialog</code>
     */
    public static JDialog createDialog(Component parent, String title) {
        Window window = getWindowForComponent(parent);
        if (window instanceof Frame) {
            return new JDialog((Frame) window, title, true);
        } else if (window instanceof Dialog) {
            return new JDialog((Dialog) window, title, true);
        } else {
            return new JDialog((Frame) null, title, true);
        }
    }

    /**
     * Position a menu so it completly displays on screen. This will take
     * care of menus not being displayed properly.
     * @param menu The menu in question
     * @param invoker the invoker of the menu
     * @param x the x coordinate in the coordinate space of the invoker where
     * the menu is requested to appear
     * @param y the y coordinate in the coordinate space of the invoker where
     * the menu is requested to appear
     * @return what the positon of the menu should be to be fully on screen,
     * in the coordinate space of invoker.
     */
    public static Point showMenuLocation(
        JPopupMenu menu,
        Component invoker,
        int x,
        int y) {
        Point screenPoint;
        int finalX = x, finalY = y;
        Dimension menuDims = menu.getPreferredSize();
        Dimension screen = Toolkit.getDefaultToolkit().getScreenSize();

        SwingUtilities.convertPointToScreen(
            screenPoint = new Point(x, y),
            invoker);

        if ((screenPoint.x + menuDims.width) > screen.width) {
            finalX -= menuDims.width; // flip
        }
        if ((screenPoint.y + menuDims.height + 30) > screen.height) {
            // last 30 is for Windows start bar
            finalY -= menuDims.height; // flip
        }
        return new Point(finalX, finalY);

    }
}
