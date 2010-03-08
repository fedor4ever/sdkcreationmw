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
import java.io.InputStream;

/* java.awt */
import java.awt.Font;
import java.awt.Insets;
import java.awt.Window;
import java.awt.Graphics;
import java.awt.Dimension;
import java.awt.Component;
import java.awt.Container;
import java.awt.Rectangle;
import java.awt.Graphics2D;
import java.awt.GraphicsEnvironment;
import java.awt.GridBagLayout;
import java.awt.RenderingHints;
import java.awt.GridBagConstraints;

/* java.awt */
import java.awt.BorderLayout;

/* java.awt.event */
import java.awt.event.KeyEvent;
import java.awt.event.ActionEvent;
import java.awt.event.WindowEvent;
import java.awt.event.WindowAdapter;

/* javax.swing */
import javax.swing.Box;
import javax.swing.JMenu;
import javax.swing.JPanel;
import javax.swing.JLabel;
import javax.swing.JTable;
import javax.swing.Action;
import javax.swing.JDialog;
import javax.swing.JButton;
import javax.swing.ActionMap;
import javax.swing.JRootPane;
import javax.swing.JTextArea;
import javax.swing.KeyStroke;
import javax.swing.BoxLayout;
import javax.swing.ImageIcon;
import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;
import javax.swing.JComponent;
import javax.swing.LookAndFeel;
import javax.swing.JOptionPane;
import javax.swing.AbstractAction;
import javax.swing.AbstractButton;
import javax.swing.SwingUtilities;

/* javax.swing.border */
import javax.swing.border.EmptyBorder;

/* javax.swing.table */
import javax.swing.table.TableModel;
import javax.swing.table.TableColumn;
import javax.swing.table.TableColumnModel;
import javax.swing.table.TableCellRenderer;

/* javax.swing.text */
import javax.swing.text.JTextComponent;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.Trace;
import com.nokia.wtk.util.IOUtils;

/**
 * A set of static GUI utilities
 *
 */
public abstract class GUI {

    /** default gap between components */
    public static int GAP = 5;

    /** Default escape character used by <code>setMnemonicFromText()</code> */
    public static final char ESCAPE_CHARACTER = '^';

    /**
     * Prevents instantiation
     */
    private GUI() {
        throw new InternalError();
    }

    /**
     * Configures the menu item.
     * @param item the JMenuItem to configure
     * @return the JMenuItem that represents the action
     */
    public static JMenuItem configureMenuItem(JMenuItem item) {
        Action action = item.getAction();
        Object acceleratorKey = action.getValue(Action.ACCELERATOR_KEY);
        if (acceleratorKey instanceof KeyStroke) {
            item.setAccelerator((KeyStroke)acceleratorKey);
        }
        return item;
    }

    /**
     * Adds action to the popup menu and configures the menu item.
     * @param menu the menu to add the action to
     * @param action the action to add
     * @return the JMenuItem that represents the action
     */
    public static JMenuItem add(JPopupMenu menu, Action action) {
        return configureMenuItem(menu.add(action));
    }

    /**
     * Adds action to the menu and configures the menu item.
     * @param menu the menu to add the action to
     * @param action the action to add
     * @return the JMenuItem that represents the action
     */
    public static JMenuItem add(JMenu menu, Action action) {
        return configureMenuItem(menu.add(action));
    }

    /**
     * Loads icon from a file.
     */
    public static ImageIcon loadIcon(Class c, String iconFile) {
        try {
            InputStream in = c.getResourceAsStream(iconFile);
            if ( in != null ) {
                return new ImageIcon(IOUtils.readStream(in));
            }
        } catch ( Exception x ) {
            Trace.Warning.printStackTrace(x);
        }
        Trace.Warning.println("WARNING: failed to load icon " + iconFile);
        return null;
    }

    /**
     * Creates a row containing two components, one of which is JLabel.
     * The component on the right takes the extra space when the row is
     * resized.
     *
     * @param left the text for JLabel on the left
     * @param right the right component
     * @return the row panel
     */
    public static JPanel createRow(String left, Component right) {
        return createRightRow(new JLabel(left), right);
    }

    /**
     * Creates a row containing two components. The component on the right
     * takes the extra space when the row is resized.
     *
     * @param left  the left component
     * @param right the right component
     * @return the row panel
     */
    public static JPanel createRightRow(Component left, Component right) {

        JPanel panel = new JPanel(new GridBagLayout());
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.anchor = GridBagConstraints.CENTER;
        gbc.gridheight = GridBagConstraints.REMAINDER;
        gbc.weighty = 1;

        panel.add(left, gbc);

        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.insets.left = GAP;
        gbc.weightx = 1;

        panel.add(right, gbc);
        return panel;
    }

    /**
     * Creates a row containing two components, one of which is JLabel.
     * The component on the left takes the extra space when the row is
     * resized.
     *
     * @param left the text for JLabel on the left
     * @param right the right component
     * @return the row panel
     */
    public static JPanel createLeftRow(String left, Component right) {
        return createLeftRow(new JLabel(left), right);
    }

    /**
     * Creates a row containing two components. The component on the left
     * takes the extra space when the row is resized.
     *
     * @param left  the left component
     * @param right the right component
     * @return the row panel
     */
    public static JPanel createLeftRow(Component left, Component right) {

        JPanel panel = new JPanel(new GridBagLayout());
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.anchor = GridBagConstraints.CENTER;
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
    public static JPanel createRowPanel(Component[] c, double weight1, double weight2) {
        return createRowPanel(c, GAP, GAP, weight1, weight2);
    }

    /**
     * Creates a panel that organizes its children in rows, two components
     * in each. Unlike <code>GridLayout(0,2)</code>, it does not equalize
     * the widths of the columns and does not stretch them vertically.
     */
    public static JPanel createRowPanel(Component[] c, int hgap, int vgap, double weight1, double weight2) {
        JPanel p = new JPanel(new GridBagLayout());

        int n = c.length/2;
        if (n > 0) {
            GridBagConstraints gbc1 = new GridBagConstraints();
            gbc1.anchor = GridBagConstraints.CENTER;
            gbc1.fill = GridBagConstraints.HORIZONTAL;
            gbc1.weightx = weight1;
            gbc1.insets.bottom = vgap;
            gbc1.insets.right = hgap;

            GridBagConstraints gbc2 = (GridBagConstraints)gbc1.clone();
            gbc2.gridwidth = GridBagConstraints.REMAINDER;
            gbc2.insets = new Insets(0, 0, vgap, 0);
            gbc2.weightx = weight2;
            gbc2.insets.right = 0;

            // middle rows
            for (int i=0; i<(n-1); i++) {
                Component c1 = c[2*i];
                Component c2 = c[2*i+1];
                if (c2 != null) {
                    if (c1 != null) p.add(c1, gbc1);
                    p.add(c2, gbc2);
                } else if (c1 != null) {
                    p.add(c1, gbc2);    // use gbc2 for c1
                }
            }

            // last row
            gbc1.anchor        =
            gbc2.anchor        = GridBagConstraints.NORTHWEST;
            gbc1.gridheight    =
            gbc2.gridheight    = GridBagConstraints.REMAINDER;
            gbc1.weighty       =
            gbc2.weighty       = 1;
            gbc1.insets.bottom =
            gbc2.insets.bottom = 0;

            Component c1 = c[2*n-2];
            Component c2 = c[2*n-1];
            if (c2 != null) {
                if (c1 != null) p.add(c1, gbc1);
                p.add(c2, gbc2);
            } else if (c1 != null) {
                gbc2.weightx = 1;
                p.add(c1, gbc2);    // use gbc2 for c1
            }
        }

        return p;
    }

    /**
     * Removes prefix characters from the button text. The first character
     * followed the prefix character (not equal the prefix character) becomes
     * mnemonic character for the button. The double occurences of the prefix
     * character are replaces with a single prefix character
     */
    public static void setMnemonicFromText(AbstractButton button) {
        setMnemonicFromText(button,ESCAPE_CHARACTER);
    }

    /**
     * Removes prefix characters from the button text. The first character
     * followed the prefix character (not equal the prefix character) becomes
     * mnemonic character for the button. The double occurences of the prefix
     * character are replaces with a single prefix character
     */
    public static void setMnemonicFromText(AbstractButton button, char prefix) {
        String text = button.getText();
        int i = text.indexOf(prefix);
        if (i >= 0) {
            StringBuffer sb = new StringBuffer(text);
            sb.deleteCharAt(i);
            boolean foundMnemonic = false;
            boolean seenPrefixChar = true;
            while (i < sb.length()) {
                char c = sb.charAt(i);
                if (seenPrefixChar) {
                    seenPrefixChar = false;
                    if (!foundMnemonic && c != prefix) {
                        foundMnemonic = true;
                        button.setMnemonic(c);
                    }
                } else if (c == prefix) {
                    seenPrefixChar = true;
                    sb.deleteCharAt(i);
                    continue;
                }
                i++;
            }
            button.setText(sb.toString());
        }
    }

    /**
     * Unescapes the default escape characters (^).
     */
    public static String unescape(String text) {
        return unescape(text,ESCAPE_CHARACTER);
    }

    /**
     * Unescapes the specified escape character.
     */
    public static String unescape(String text, char escape) {
        if (text != null) {
            int i = text.indexOf(escape);
            if (i >= 0) {
                StringBuffer sb = new StringBuffer(text);
                sb.deleteCharAt(i);
                boolean foundMnemonic = false;
                boolean seenPrefixChar = true;
                while (i < sb.length()) {
                    char c = sb.charAt(i);
                    if (seenPrefixChar) {
                        seenPrefixChar = false;
                        if (!foundMnemonic && c != escape) {
                            foundMnemonic = true;
                        }
                    } else if (c == escape) {
                        seenPrefixChar = true;
                        sb.deleteCharAt(i);
                        continue;
                    }
                    i++;
                }
                text = sb.toString();
            }
        }
        return text;
    }

    /**
     * Sets preferred size for all the components so that they are all have
     * the same (maximum) width.
     *
     * @return the "maximized" width
     */
    public static int maximizeWidth(JComponent [] c) {
        return maximizeWidth(c, 0);
    }

    /**
     * Sets preferred size for all the components so that they are all have
     * the same (maximum) width.
     *
     * @return the "maximized" width
     */
    public static int maximizeWidth(JComponent [] c, int add) {
        int n = c.length;
        int width = 0;
        Dimension [] sizes = new Dimension[n];
        for (int i=0; i<n; i++) {
            Dimension prefSize = c[i].getPreferredSize();
            width = Math.max(width, prefSize.width);
            sizes[i] = prefSize;
        }
        width += add;
        for (int i=0; i<n; i++) {
            Dimension prefSize = sizes[i];
            prefSize.width = width;
            c[i].setPreferredSize(prefSize);
        }
        return width;
    }

    /**
     * Sets preferred width for the specified component, leaving preferred
     * height intact.
     *
     * @param c the component
     * @param w the preferred width
     */
    public static void setPreferredWidth(JComponent c, int w) {
        Dimension prefSize = c.getPreferredSize();
        prefSize.width = w;
        c.setPreferredSize(prefSize);
    }

    /**
     * Finds a {@link Window} for the specified {@link Component}.
     * If <code>c</code> itself is a {@link Window}, this function
     * returns <code>c</code>
     *
     * @return  a {@link Window} for <code>c</code>, <code>null</code> if none.
     */
    public static Window getWindow(Component c) {
        if (c instanceof Window) {
            return (Window)c;
        } else if (c != null) {
            return SwingUtilities.getWindowAncestor(c);
        } else {
            return null;
        }
    }

    /**
     * Enables or disables the whole hierarchy of widgets
     *
     * @param parent the top of the hierarchy
     * @param b <code>true</code> to enable all the components in the
     *           hierarchy, <code>false</code> to disable them
     */
    public static void enable(Container parent, boolean b) {
        int n = parent.getComponentCount();
        for (int i=0; i<n; i++) {
            Component c = parent.getComponent(i);
            c.setEnabled(b);
            if (c instanceof Container) {
                if ((c instanceof JComponent) && !(c instanceof JPanel)) {
                    // All JComponents are Containers. Don't recurse into
                    // any of them except for those derived from JPanel
                    continue;
                }
                enable((Container)c, b);
            }
        }
    }

    /**
     * Fills rectangle with dots.
     * The left and right edges of the rectangle are at
     * <code>x</code> and <code>x&nbsp;+&nbsp;width&nbsp;-&nbsp;1</code>.
     * The top and bottom edges are at
     * <code>y</code> and <code>y&nbsp;+&nbsp;height&nbsp;-&nbsp;1</code>.
     * The resulting rectangle covers an area
     * <code>width</code> pixels wide by
     * <code>height</code> pixels tall.
     * The rectangle is filled using the graphics context's current color.
     *
     * @param   g   the graphics context
     * @param   x   the <i>x</i> coordinate of the rectangle to be filled.
     * @param   y   the <i>y</i> coordinate of the rectangle to be filled.
     * @param   w   the width of the rectangle to be filled.
     * @param   h   the height of the rectangle to be filled.
     */
    public static void fillDotRect(Graphics g, int x, int y, int w, int h) {
        if (w > 0 && h > 0) {
            Object antialiasingValue = null;
            if (g instanceof Graphics2D) {
                Graphics2D g2 = (Graphics2D)g;
                antialiasingValue = g2.getRenderingHint(
                    RenderingHints.KEY_ANTIALIASING);
                g2.setRenderingHint(
                    RenderingHints.KEY_ANTIALIASING,
                    RenderingHints.VALUE_ANTIALIAS_OFF);
            }
            for (int x0 = x-2*((h-1)/2); x0<(x+w); x0 += 2) {
                int x1 = x0;
                int y1 = y;
                if (x0 < x) {
                    x1 = x;
                    y1 = y + (x-x0);
                }
                int x2 = Math.min(x+w-1, x0+h-1);
                int y2 = y+(x2-x0);
                g.drawLine(x1,y1,x2,y2);
            }
            if (g instanceof Graphics2D) {
                Graphics2D g2 = (Graphics2D)g;
                g2.setRenderingHint(
                    RenderingHints.KEY_ANTIALIASING,
                    antialiasingValue);
            }
        }
    }

    /**
     * Creates maximum insets
     * @param i1 the first insets
     * @param i2 the seconf insets
     * @return the maximum insets
     */
    public static Insets max(Insets i1, Insets i2) {
        return new Insets(Math.max(i1.top, i2.top),
                          Math.max(i1.left, i2.left),
                          Math.max(i1.bottom, i2.bottom),
                          Math.max(i1.right, i2.right));
    }

    /**
     * updates the destination {@link Rectangle} with the product
     * of its intersection with another rectangle. Kind of like
     * {@link Rectangle#intersection} but it does not create a new
     * {@link Rectangle}.
     *
     * @return <code>true</code> if rectangles intersect, <code>false</code>
     *         if there's no intersection
     */
    public static boolean intersect(Rectangle dest, Rectangle src) {
        return intersect(dest, src.x, src.y, src.width, src.height);
    }

    /**
     * updates the destination {@link Rectangle} with the product
     * of its intersection with another rectangle. Kind of like
     * {@link Rectangle#intersection} but it does not create a new
     * {@link Rectangle}.
     *
     * @return <code>true</code> if rectangles intersect, <code>false</code>
     *         if there's no intersection
     */
    public static boolean intersect(Rectangle dest,int x,int y,int w,int h) {
        int x1 = Math.max(x, dest.x);
        int x2 = Math.min(x + w, dest.x + dest.width);
        int y1 = Math.max(y, dest.y);
        int y2 = Math.min(y + h, dest.y + dest.height);
        if (x2 > x1 && y2 > y1) {
            dest.x = x1;
            dest.y = y1;
            dest.width = x2 - x1;
            dest.height = y2 - y1;
            return true;
        } else {
            dest.x =
            dest.y =
            dest.width =
            dest.height = 0;
            return false;
        }
    }

    /**
     * computes the union of the <code>dest</code> rectangle with the
     * specified <code>src</code> rectangle. Result is stored in the
     * <code>dest</code> rectangle, that represents the union of the
     * two rectangles
     *
     * @param dest the rectangle to be modified
     * @param src the second rectangle, not modified
     */
    public static void union(Rectangle dest, Rectangle src) {
        union(dest, src.x, src.y, src.width, src.height);
    }

    /**
     * computes the union of the <code>dest</code> rectangle with the
     * specified <code>src</code> rectangle. Result is stored in the
     * <code>dest</code> rectangle, that represents the union of the
     * two rectangles
     *
     * @param r the rectangle to be modified
     * @param x the x coordinate of the second rectangle
     * @param y the x coordinate of the second rectangle
     * @param width the width of the second rectangle
     * @param height the height of the second rectangle
     */
    public static void union(Rectangle r,int x,int y,int width,int height) {
        int x1 = Math.min(x, r.x);
        int x2 = Math.max(x + width, r.x + r.width);
        int y1 = Math.min(y, r.y);
        int y2 = Math.max(y + height, r.y + r.height);
        r.setBounds(x1, y1, x2 - x1, y2 - y1);
    }

    //======================================================================
    //          D I A L O G
    //======================================================================

    /**
     * A utility function that sets up a dialog given a content panel and
     * a set of actions.
     *
     * @param dialog    the dialog to set up
     * @param content   the dialog content
     * @param actions   dialog actions
     * @param cancel    the action to associated with the escape key
     *
     * @return the default button
     */
    public static JButton layoutDialog(JDialog dialog,
                                       Component content,
                                       Action[] actions,
                                       Action cancel) {

        JPanel panel = new JPanel(new BorderLayout(GUI.GAP, GUI.GAP));
        panel.setBorder(new EmptyBorder(GUI.GAP,GUI.GAP,GUI.GAP,GUI.GAP));

        // add the content
        panel.add(content, BorderLayout.CENTER);

        // and finally action buttons at the bottom
        JButton defaultButton = null;
        JButton cancelButton = null;
        if (actions != null && actions.length > 0) {
            JPanel buttons = new JPanel();
            buttons.setLayout(new BoxLayout(buttons, BoxLayout.X_AXIS));
            buttons.add(Box.createHorizontalGlue());
            for (int i=0; i<actions.length; i++) {
                if (i > 0) buttons.add(Box.createHorizontalStrut(GUI.GAP));
                JButton button = new JButton(actions[i]);
                if (defaultButton == null) defaultButton = button;
                if (actions[i] == cancel) cancelButton = button;
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
                cancelAction = new ButtonClickAction(cancelButton);
            } else {
                cancelAction = cancel;
            }

            // add appropriate action to the input map:
            String key = "cancel";
            panel.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW).
            put(KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE,0),key);
            panel.getActionMap().put(key, cancelAction);
        }

        // when dialog becomes visible, request focus to the first action
        if (defaultButton != null) {
            dialog.addWindowListener(new DefaultButtonInit(defaultButton));
        }

        // add the panel to the content pane
        Container contentPane = dialog.getContentPane();
        contentPane.removeAll();
        contentPane.setLayout(new BorderLayout());
        contentPane.add(panel, BorderLayout.CENTER);

        // center dialog on screen or relative to its parent
        dialog.pack();
        dialog.setLocationRelativeTo(dialog.getParent());
        return defaultButton;
    }

    /**
     * Action that programmatically clicks a button
     */
    private static class ButtonClickAction extends AbstractAction {
        private JButton button;
        private ButtonClickAction(JButton b) {
            super(b.getText());
            button = b;
        }
        public void actionPerformed(ActionEvent e) {
            if (button.isEnabled()) {
                button.doClick();
            }
        }
    }

    /**
     * Initializes default button when window is opened
     */
    private static class DefaultButtonInit extends WindowAdapter {
        private JButton defaultButton;
        private DefaultButtonInit(JButton button) {
            defaultButton = button;
        }
        public void windowOpened(WindowEvent e) {
            JRootPane root = SwingUtilities.getRootPane(defaultButton);
            if (root != null) root.setDefaultButton(defaultButton);
            defaultButton.requestFocus();
        }
    }

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
            } else {
                getToolkit().beep();
            }
        }
    }

    //======================================================================
    //          K E Y B O A R D    A C T I O N S
    //======================================================================

    public static final String ACTION_MAP_KEY = "actionMapKey";

    /**
     * Registers a keyboard action that has the "AcceleratorKey" attribute
     *
     * @param c the component to register the action with
     * @param a the action to register
     * @return <code>true</code> if the action had the "AcceleratorKey"
     *    attribute and has been registerd, <code>false</code> otherwise.
     */
    public static boolean registerKeyboardAction(JComponent c, Action a) {
        Object acceleratorKey = a.getValue(Action.ACCELERATOR_KEY);
        if (acceleratorKey instanceof KeyStroke) {
            registerKeyboardAction(c, (KeyStroke)acceleratorKey, a);
            return true;
        } else {
            return false;
        }
    }

    /**
     * Registers a keyboard action
     *
     * @param c the component to register the action with
     * @param k an int specifying the numeric code for a keyboard key
     * @param a the action to register
     */
    public static void registerKeyboardAction(JComponent c, int k, Action a) {
        registerKeyboardAction(c, KeyStroke.getKeyStroke(k,0),a);
    }

    /**
     * Registers a keyboard action
     *
     * @param c the component to register the action with
     * @param k the keystroke
     * @param a the action to register
     */
    public static void registerKeyboardAction(JComponent c, KeyStroke k, Action a) {
        Object actionMapKey = a.getValue(ACTION_MAP_KEY);
        ActionMap actionMap = c.getActionMap();
        boolean foundExistingActionMapKey = false;
        if (actionMapKey == null) {

            // try to find existing action map key for this action
            Object [] actionKeys = actionMap.keys();
            if (actionKeys != null) {
                for (int i=0; i<actionKeys.length; i++) {
                    if (a == actionMap.get(actionKeys[i])) {
                        foundExistingActionMapKey = true;
                        actionMapKey = actionKeys[i];
                        break;
                    }
                }
            }

            // generate arbitrary key
            if (!foundExistingActionMapKey) {
                int i = 1;
                while (actionMap.get(actionMapKey = "action"+(i++)) != null);
           }
        }
        c.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW).put(k, actionMapKey);
        if (!foundExistingActionMapKey) actionMap.put(actionMapKey, a);
    }

    //======================================================================
    //          M E S S A G E S   B O X E S
    //======================================================================

    private static final String DEFAULT_MESSAGE_TITLE  = "Message";
    private static final String DEFAULT_ERROR_TITLE    = "Error";
    private static final String DEFAULT_QUESTION_TITLE = "Question";
    private static final String DEFAULT_PROMPT_TITLE   = "Prompt";

    /**
     * displays an information message.
     *
     * @param msg the message text
     */
    public static void message( String msg ) {
        message(null, msg, null);
    }

    /**
     * displays an information message.
     *
     * @param parent the parent window for the message
     * @param msg the message text
     */
    public static void message( Component parent, String msg ) {
        message(parent, msg, null);
    }

    /**
     * displays an information message.
     *
     * @param msg the message text
     * @param title the title of the message box.
     */
    public static void message(String msg, String title) {
        message(null, msg, title);
    }

    /**
     * displays an information message.
     *
     * @param parent the parent window for the message
     * @param msg the message text
     * @param title the title of the message box.
     */
    public static void message(Component parent, String msg, String title) {
        if (title == null) title = DEFAULT_MESSAGE_TITLE;
        JOptionPane.showMessageDialog( parent, msg, title,
        JOptionPane.INFORMATION_MESSAGE);
    }

    /**
     * displays an error message.
     *
     * @param msg the message text
     */
    public static void error(String msg) {
        error(null, msg, null);
    }

    /**
     * displays an error message.
     *
     * @param parent the parent window for the message
     * @param msg the message text
     */
    public static void error(Component parent, String msg) {
        error(parent, msg, null);
    }

    /**
     * displays an error message.
     *
     * @param msg the message text
     * @param title the title of the message box.
     */
    public static void error(String msg, String title) {
        error(null, msg, title);
    }

    /**
     * displays an error message.
     *
     * @param parent the parent window for the message
     * @param msg the message text
     * @param title the title of the message box.
     */
    public static void error(Component parent, String msg, String title) {
        if (title == null) title = DEFAULT_ERROR_TITLE;
        JOptionPane.showMessageDialog( parent, msg, title,
        JOptionPane.ERROR_MESSAGE);
    }

    /**
     * ask a yes/no question.
     *
     * @param text the question to ask
     *
     * @return <tt>true</tt> if user clicked yes, otherwise <tt>false</tt>.
     */
    public static boolean question(String text) {
        return question(null, text, null);
    }

    /**
     * ask a yes/no question.
     *
     * @param parent the parent window for the message
     * @param text the question to ask
     *
     * @return <tt>true</tt> if user clicked yes, otherwise <tt>false</tt>.
     */
    public static boolean question(Component parent, String text) {
        return question(parent, text, null);
    }

    /**
     * ask a yes/no question.
     *
     * @param text the question to ask
     * @param title the title of the message box.
     *
     * @return <tt>true</tt> if user clicked yes, otherwise <tt>false</tt>.
     */
    public static boolean question(String text, String title) {
        if (title == null) title = DEFAULT_QUESTION_TITLE;
        return question(null, text, title);
    }

    /**
     * ask a yes/no question.
     *
     * @param parent the parent window for the message
     * @param text the question to ask
     * @param title the title of the message box.
     *
     * @return <tt>true</tt> if user clicked yes, otherwise <tt>false</tt>.
     */
    public static boolean question(Component parent,String text,String title) {
        return (JOptionPane.showConfirmDialog(parent, text, title,
                JOptionPane.YES_NO_OPTION) ==
                JOptionPane.YES_OPTION);
    }

    /**
     * prompt for input
     *
     * @param prompt the prompt string
     * @param def the default value
     *
     * @return the string entered by the user, of <code>null</code> if
     *         user cancelled the prompt dialog.
     */
    public static String prompt(String prompt, String def) {
        return prompt(null, prompt, null, def);
    }

    /**
     * prompt for input
     *
     * @param parent the parent window for the message
     * @param prompt the prompt string
     * @param def the default value
     *
     * @return the string entered by the user, of <code>null</code> if
     *         user cancelled the prompt dialog.
     */
    public static String prompt(Component parent, String prompt, String def) {
        return prompt(parent, prompt, null, def);
    }

    /**
     * prompt for input
     *
     * @param prompt the prompt string
     * @param title the title of the message box.
     * @param def the default value
     *
     * @return the string entered by the user, of <code>null</code> if
     *         user cancelled the prompt dialog.
     */
    public static String prompt(String prompt, String title, String def) {
        return prompt(null, prompt, title, def);
    }

    /**
     * prompt for input
     *
     * @param parent the parent window for the message
     * @param prompt the prompt string
     * @param title the title of the message box.
     * @param def the default value
     *
     * @return the string entered by the user, of <code>null</code> if
     *         user cancelled the prompt dialog.
     */
    public static String prompt(Component parent, String prompt,
                                String title, String def) {
        if (title == null) title = DEFAULT_PROMPT_TITLE;
        return (String)JOptionPane.showInputDialog(parent, prompt, title,
                       JOptionPane.QUESTION_MESSAGE, null, null, def);
    }

    //======================================================================
    //          J T A B L E    S U P P O R T
    //======================================================================

    /**
     * returns rows selected in the table. If no rows are selected, returns
     * an empty array.
     *
     * @param table the table to get selected rows from
     */
    public static int [] getSelectedRows(JTable table) {
        if (table.getRowCount() > 0) return table.getSelectedRows();
        return new int[0];
    }

    /**
     * fixes the width of the table column.
     */
    public static void fixColumnWidth(TableColumn column, int width) {
        column.setMinWidth(width);
        column.setMaxWidth(width);
        column.setPreferredWidth(width);
        column.setWidth(width);
    }

    /**
     * fixes the width of the specified column in the table.
     * @return the selected width of the column
     */
    public static int fixColumnWidth(JTable table, int column) {
        return fixColumnWidth(table, column, 10);
    }

    /**
     * sets preferred width of the specified column to optimal value. Optimal
     * width is defined as the maximum preferred width of all cells in the
     * specified column, including the header row.
     *
     * @return the optimal width of the column
     */
    public static int setOptimalColumnWidth(JTable table, int column) {

        TableColumnModel columnModel = table.getColumnModel();
        String columnName = table.getModel().getColumnName(column);
        int w = getOptimalColumnWidth(table, column);

        try {
            int pos = columnModel.getColumnIndex(columnName);
            TableColumn col = columnModel.getColumn(pos);
            col.setPreferredWidth(w);
            return w;
        } catch (Exception x) {
            Trace.Debug.printStackTrace(x);
            return -1;
        }
    }

    /**
     * sets preferred width for all the columns to optimal value. Optimal
     * width is defined as the maximum preferred width of all cells in the
     * specified column, including the header row.
     */
    public static void setOptimalColumnWidths( JTable table ) {
        TableModel tableModel = table.getModel();
        int n = tableModel.getColumnCount();
        for (int i=0; i<n; i++) {
            setOptimalColumnWidth(table, i);
        }
        table.sizeColumnsToFit(-1);
    }

    /**
     * returns the width of the specified column in the table. Optimal width
     * is defined as the maximum preferred width of all cells in the specified
     * column, including the header row.
     *
     * @return the optimal width of the column
     */
    public static int getOptimalColumnWidth(JTable table, int column) {

        int w = 0;
        Component c;
        TableModel tableModel = table.getModel();
        for (int row=0; row<tableModel.getRowCount(); row++) {
            TableCellRenderer cell = table.getCellRenderer(row, column);
            Object value = tableModel.getValueAt(row, column);
            w = Math.max(w, getPreferredCellWidth(cell,table,value,row,column));
        }

        // take care of the header row
        TableColumnModel columnModel = table.getColumnModel();
        TableColumn headerColumn = columnModel.getColumn(column);
        TableCellRenderer r = headerColumn.getHeaderRenderer();
        String columnName = tableModel.getColumnName(column);
        return Math.max(w, getPreferredCellWidth(r,table,columnName,-1,column));
    }

    /**
     * fixes the width of the specified column in the table.
     * @return the selected width of the column
     */
    public static int fixColumnWidth(JTable table, int column, int minWidth) {

        int w = getOptimalColumnWidth(table, column);

        TableColumnModel columnModel = table.getColumnModel();
        String columnName = table.getModel().getColumnName(column);

        try {
            int pos = columnModel.getColumnIndex(columnName);
            w = Math.max(w, minWidth);
            TableColumn col = columnModel.getColumn(pos);
            boolean resize = (col.getWidth() != w);

            fixColumnWidth(col, w);
            if (resize) {
                table.sizeColumnsToFit(-1);
                table.getTableHeader().repaint();
            }
            return w;

        } catch (Exception x) {
            Trace.Debug.printStackTrace(x);
            return -1;
        }
    }

    /**
     * fixes the width of all columns in the table.
     */
    public static void fixAllColumns( JTable table ) {
        TableModel tableModel = table.getModel();
        int n = tableModel.getColumnCount();
        for (int i=0; i<n; i++) {
            fixColumnWidth(table, i);
        }
    }

    /**
     * fixes the width of all columns in the table.
     */
    public static void fixAllColumns( JTable table, int minWidth ) {
        TableModel tableModel = table.getModel();
        int n = tableModel.getColumnCount();
        for (int i=0; i<n; i++) {
            fixColumnWidth(table, i, minWidth);
        }
    }

    /**
     * returns preferred width of the cell. If <tt>cell</tt> is <tt>null</tt>,
     * takes default cell renderer from the table.
     */
    public static int getPreferredCellWidth(TableCellRenderer cell,
                        JTable table, Object val, int row, int column)
    {
        Component c;
        int w = 0;

        if (cell == null)
            cell = table.getDefaultRenderer(val.getClass());

        // for normal cells we will try all possible combinations of "selected"
        // and "focused" states
        c = cell.getTableCellRendererComponent(table,val,false,false,row,column);
        w = Math.max(w, c.getPreferredSize().width);

        // however, if we are looking at header row, don't try "focused" and
        // "selected" states, otherwise DefaultTableCellRenderer crashes with
        // ArrayIndexOutOfBoundsException trying to call JTable.isCellEditable()
        if (row >= 0) {
            c = cell.getTableCellRendererComponent(table,val,true,false,row,column);
            w = Math.max(w, c.getPreferredSize().width);
            c = cell.getTableCellRendererComponent(table,val,false,true,row,column);
            w = Math.max(w, c.getPreferredSize().width);
            c = cell.getTableCellRendererComponent(table,val,true,true,row,column);
            w = Math.max(w, c.getPreferredSize().width);
        }

        // this is something I cannot explain. Sometimes strings still get
        // truncated even though (I think) there's enough space to show the
        // whole thing. Adding two pixels seems to solve the problem
        return w + 2;
    }

    //======================================================================
    //          F O N T    S U P P O R T
    //======================================================================

    /** Name of standard Microsoft font with extensive international support **/
    public static final String GLOBAL_FONT = "Arial Unicode MS";

    /**
     * Locate a physical font by name on this system.
     * @param fontName name of physical font
     * @return Font (in 11 point) if found, or null.
     */
    public static Font getFontByName(String fontName) {
        Font[] fonts;
        fonts = GraphicsEnvironment.getLocalGraphicsEnvironment().getAllFonts();
        for (int i = 0; i < fonts.length; i++) {
            String fn = fonts[i].getFontName();
            if (fn.equalsIgnoreCase(fontName)) {
                return fonts[i].deriveFont(11.0f);
            }
        }
        return null;
    }


}
