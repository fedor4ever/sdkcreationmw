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

/* java.util */
import java.util.Properties;

/* java.net */
import java.net.URL;

/* java.awt */
import java.awt.Window;
import java.awt.Container;
import java.awt.BorderLayout;
import java.awt.GridBagLayout;
import java.awt.GridBagConstraints;

/* java.awt.event */
import java.awt.event.ActionEvent;

/* javax.swing */
import javax.swing.Icon;
import javax.swing.JPanel;
import javax.swing.Action;
import javax.swing.BoxLayout;
import javax.swing.ImageIcon;
import javax.swing.JComponent;
import javax.swing.AbstractAction;
import javax.swing.AbstractButton;
import javax.swing.SwingUtilities;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.Trace;

/**
 * A panel that implements progressive disclosure. This panel consist of a top
 * component - a label, a text entry box, another panel - that will always be
 * displayed, with a small clickable disclousre triangle to the left of it.
 * Clicking on this triangle will make a hidden panel visible. Clicking on this
 * triangle again will hide that panel again.<p>
 * To use a disclosure panel:
 * <ol>
 * <li> Create a disclosure panel object </li>
 * <li> use setTopComponent method to set the top component</li>
 * <li> add whatever necessary to the panel returned by getHiddenPanel</li>
 * <li> add disclosure panel wherever desired.</li>
 * </ol>
 */
public class DisclosurePanel extends JPanel {

    //======================================================================
    //      A C T I O N
    //======================================================================

    private class DisclosureAction extends AbstractAction {
        DisclosureAction(boolean visible) {
            super();
            init(visible);
        }

        public void actionPerformed(ActionEvent e) {
            setHiddenPanelVisible(!visible);
        }
        void init(boolean visible) {
            putValue(
                AbstractAction.SMALL_ICON,
                visible ? getOpenIcon() : getClosedIcon());
        }
    }
    private final static String CLOSED_KEY = "disclosurepanel.icon.closed";
    private final static Icon CLOSED_ICON = getIcon(CLOSED_KEY);
    private final static String OPEN_KEY = "disclosurepanel.icon.open";
    private final static Icon OPEN_ICON = getIcon(OPEN_KEY);

    /**
     * Propety fired when status of hidden panel changes. will fire with
     * new value True is property is now visible, false otherwise.
     */
    public final static String PANEL_VISIBLE_PROPERTY = "DisclosurePanelVisible";
    public final static String ABOUT_TO_HIDE_PROPERTY = "AboutToHidePanel";
    public final static String ABOUT_TO_SHOW_PROPERTY = "AboutToShowPanel";

    private static Properties props;

    /**
     * Gets a resource icon value from the 'properties' file.
     * @param key the name of the resource
     */
    private static ImageIcon getIcon(String key) {
        URL url = null;
        try {
            url = DisclosurePanel.class.getResource(getResource(key));
        } catch (Exception e) {
            Trace.Debug.println(
                "Disclousre Panel, failure to get icon, "
                    + "key: "
                    + key
                    + "message: "
                    + e.getMessage());
        }
        return (url != null) ? new ImageIcon(url) : null;
    }

    /**
     * Gets a resource URL value from the 'properties' file.
     * @param key the name of the resource
     */
    private static String getResource(String key) {
        try {
            if (props == null) {
                props = new Properties();
                props.load(
                    DisclosurePanel.class.getResourceAsStream(
                        "resources/DisclosurePanel.properties"));
            }
        } catch (Exception e) {
            Trace.Debug.println(
                "Disclousre Panel, failure load props "
                    + ", message: "
                    + e.getMessage());
        }
        return props.getProperty(key);
    }
    private DisclosureAction action;

    private Icon closedIcon = null;
    private JPanel hiddenPanel = new JPanel();
    private Icon openIcon = null;
    private AbstractButton switchButton;
    private JComponent topComponent = null;
    private final JPanel topLabel = new JPanel();
    private boolean visible = false;

    /**
     * Creates an empty disclosure panel. The hidden panel will not be visible
     * when this object is added to a visible UI.
     **/
    public DisclosurePanel() {
        super(new BorderLayout());
        action = new DisclosureAction(visible);
        topLabel.setLayout(new BorderLayout());
        topLabel.add(createButtonPanel(), BorderLayout.WEST);
        add(topLabel, BorderLayout.NORTH);
        hiddenPanel.setLayout(new BoxLayout(hiddenPanel, BoxLayout.Y_AXIS));
    }

    private JPanel createButtonPanel() {
        JPanel p = new JPanel(new GridBagLayout());
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.gridheight = gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.anchor = GridBagConstraints.WEST;
        gbc.weightx = gbc.weighty = 1;
        p.add(getButton(), gbc);
        return p;
    }

    private AbstractButton getButton() {
        if (switchButton == null) {
            switchButton = new RolloverJButton(action);
            switchButton.setOpaque(false);
            switchButton.setFocusPainted(false);
        }
        return switchButton;
    }

    /**
     * Get the Icon in use when the hidden panel is visible
     * @return Icon the icon
     */
    public Icon getOpenIcon() {
        if (openIcon == null) {
            return OPEN_ICON;
        }
        return openIcon;
    }

    /**
     * Sets the icon in use in the switch when the hidden panel is visible.
     * @param icon The Icon to set. Setting null will make the switch
     * show the default icon.
     */
    public void setOpenIcon(Icon icon) {
        openIcon = icon;
        action.init(visible);
    }

    /**
     * Get the icon shown in the switch when the hidden panel is not visible
     * @return Icon the icon
     */
    public Icon getClosedIcon() {
        if (closedIcon == null) {
            return CLOSED_ICON;
        }
        return closedIcon;
    }

    /**
     * Sets the icon in use in the switch when the hidden panel is not visible.
     * @param icon The Icon to set. Setting null will make the switch
     * show the default icon.
     */
    public void setClosedIcon(Icon icon) {
        closedIcon = icon;
        action.init(visible);
    }

    /**
     * Returns the "disclosure" action
     */
    public Action getDisclosureAction() {
        return action;
    }

    /**
     * Gets the panel of which the visiblity can be toggled.
     * @return The actual panel that become s visible when the triangle is
     * clicked. Add components here.
     **/
    public final JPanel getHiddenPanel() {
        return this.hiddenPanel;
    }

    /**
     * Get the button to show/hide the large panel.
     * @return a component that hides or shows the panels
     */
    protected JComponent getSwitch() {
        return getButton();
    }

    /**
     * Checks whether the hidden panel is visible.
     */
    public final boolean isHiddenPanelVisible() {
        return visible;
    }

    /**
     * Sets the state of the panel. Making the panel visible will actually
     * revalidate the whole layout.
     */
    public void setHiddenPanelVisible(boolean val) {
        // This call could be called directly, or by the ActionListener on the
        // switch above, so we cannot predict whether switch and panel status
        // are in sync.
        if (this.visible != val) {
            if (getButton().isSelected() != val) {
                getButton().setSelected(val);
            }
            if (val) {
                firePropertyChange(
                    ABOUT_TO_SHOW_PROPERTY,
                    Boolean.FALSE,
                    Boolean.TRUE);
                add(hiddenPanel, BorderLayout.CENTER);
                firePropertyChange(
                    PANEL_VISIBLE_PROPERTY,
                    Boolean.FALSE,
                    Boolean.TRUE);
            } else {
                firePropertyChange(
                    ABOUT_TO_HIDE_PROPERTY,
                    Boolean.FALSE,
                    Boolean.TRUE);
                remove(hiddenPanel);
                firePropertyChange(
                    PANEL_VISIBLE_PROPERTY,
                    Boolean.TRUE,
                    Boolean.FALSE);
            }
            this.visible = val;
            getDisclosureAction().putValue(
                Action.SMALL_ICON,
                visible ? getOpenIcon() : getClosedIcon());

            // notify the top-most parent that our preferred size has changed
            Container parent = getParent();
            if (getParent() != null) {
                Window window = SwingUtilities.getWindowAncestor(this);
                if (window != null) {
                    window.invalidate();
                    window.validate();
                }
                parent.repaint();
            }
        }
    }

    /**
     * Sets the component next to the triangle. This component will always be
     * visible.
     * @param comp the component that should always be visible. Adding a
     *     component will remove the previous component if already set.
     **/
    public void setTopComponent(JComponent comp) {
        comp.setAlignmentX(0);
        //comp.setOpaque(false);
        if (topComponent != null) {
            topLabel.remove(topComponent);
        }
        topLabel.add(comp, BorderLayout.CENTER);
        topComponent = comp;
    }
}
