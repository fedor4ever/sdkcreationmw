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


package com.nokia.epdt.plugins.s60.winsock;

/* java.beans */
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

/* java.awt */
import java.awt.Component;
import java.awt.Container;
import java.awt.BorderLayout;

/* javax.swing */
import javax.swing.Icon;
import javax.swing.ImageIcon;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.border.Border;
import javax.swing.border.EmptyBorder;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.Property;
import com.nokia.wtk.util.PropertySet;
import com.nokia.wtk.util.PropertyEditors;

/* com.nokia.wtk.util.gui */
import com.nokia.wtk.util.gui.BorderPanel;
import com.nokia.wtk.util.gui.GUI;

/**
 * WinsockPluginJPanel
 *
 * @version 1.0
 */
class WinsockPluginJPanel extends JPanel implements WinsockConstants {

    private static final String WINSOCK_LAYOUT_KEY = "Winsock";
    private static final String NPACKET_LAYOUT_KEY = "Npacket";
    private static String RESTART_NOTE =
    	"Emulator must be restarted in order to activate the changes.";
    private static String RESTART_NOTE_TITLE = "Preferences";

    private static final int GAP = 6;
    private static final Border BORDER_1 = new EmptyBorder(GAP,GAP,GAP,GAP);
    private static final Border BORDER_2 = new EmptyBorder(0,BorderPanel.COMPONENT_INSET_H+GAP,GAP,GAP);

    private static final String WARNING_ICON = "resources/info.gif";
    private static final String WARNING_TEXT = "Note: settings on this page are " +
        "enabled only when preferences dialog is launched from S60 emulator.";

    /**
     * Creates new WinsockPluginJPanel
     * @param ps properties to edit
     */
    public WinsockPluginJPanel(PropertySet ps) {
        super(new BorderLayout(GAP, GAP));

        Component [] components = new Component [] {
            new JLabel("Proxy server name:"),
            PropertyEditors.createTextField(ps,WINSOCK_PROXY_SERVER_PROP,20),
            new JLabel("Proxy server port:"),
            PropertyEditors.createTextField(ps,WINSOCK_PROXY_PORT_PROP,20)
        };

        // Properties
        Property useProxy = ps.getProperty(WINSOCK_USE_PROXY_PROP);
        Property enableUI = ps.getProperty(WINSOCK_UI_ENABLED_PROP);

        // Proxy settings
        Component checkBox = PropertyEditors.createCheckBox(useProxy,"Use HTTP proxy:");
        BorderPanel bp = new BorderPanel(checkBox);
        JPanel proxyPanel = GUI.createRowPanel(components, 0.0, 1.0);
        proxyPanel.setBorder(BORDER_2);
        bp.getContentPanel().add(proxyPanel);
        new PanelStatusUpdater(proxyPanel, checkBox, enableUI, useProxy);

        // Warning panel
        JPanel warning = new JPanel(new BorderLayout(GAP,GAP));
        //warning.add(GUI.createTextComponent(WARNING_TEXT),BorderLayout.CENTER);
        warning.add(GUI.createTextComponent(WARNING_TEXT),BorderLayout.CENTER);
        //Icon icon = GUI.loadIcon(getClass(),WARNING_ICON); Alternative
        ImageIcon icon = new ImageIcon(ClassLoader.getSystemResource(WARNING_ICON));
        if (icon != null) {
            JLabel warningIcon = new JLabel(icon);
            warningIcon.setVerticalAlignment(JLabel.TOP);
            warning.add(warningIcon, BorderLayout.WEST);
        }
        new WarningPanelUpdater(warning,enableUI);

        // Layout
        add(bp, BorderLayout.NORTH);
        add(warning, BorderLayout.SOUTH);
        setBorder(BORDER_1);
    }

    /**
     * Shows or hides the panel based on the value of the boolean property
     */
    private static class WarningPanelUpdater implements PropertyChangeListener {
        private Component panel;
        WarningPanelUpdater(Component panel, Property hidePanelProp) {
            this.panel = panel;
            hidePanelProp.addPropertyChangeListener(this);
            panel.setVisible(!Boolean.TRUE.equals(hidePanelProp.getValue()));
        }

        public void propertyChange(PropertyChangeEvent e) {
            panel.setVisible(!Boolean.TRUE.equals(e.getNewValue()));
        }
    }

    /**
     * Enables or disables the panel based on the value of the boolean property
     */
    private static class PanelStatusUpdater implements PropertyChangeListener {
        private Container panel;
        private Component checkBox;
        private Property enableAllProp, enablePanelProp;
        PanelStatusUpdater(Container panel, Component checkBox,
                           Property enableAllProp, Property enablePanelProp) {
            this.panel = panel;
            this.checkBox = checkBox;
            this.enableAllProp = enableAllProp;
            this.enablePanelProp = enablePanelProp;
            enableAllProp.addPropertyChangeListener(this);
            enablePanelProp.addPropertyChangeListener(this);
            update();
        }
        public void propertyChange(PropertyChangeEvent e) {
            update();
        }
        private void update() {
            boolean enableAll = Boolean.TRUE.equals(enableAllProp.getValue());
            checkBox.setEnabled(enableAll);
            GUI.enable(panel, enableAll &&
                       Boolean.TRUE.equals(enablePanelProp.getValue()));
        }
    }
    public void showRestartWarning()
    {
        JOptionPane.showMessageDialog(this,
        		RESTART_NOTE, RESTART_NOTE_TITLE,
				JOptionPane.INFORMATION_MESSAGE);

    }
}
