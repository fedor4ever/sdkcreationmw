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


/*
 * SystemOutputPlugin.java
 * Created on 2004-11-29
 */
package com.nokia.epdt.plugins.s60.output;

/* java.awt */
import java.awt.Image;
import java.awt.BorderLayout;

/* javax.swing */
import javax.swing.ImageIcon;
import javax.swing.JComponent;
import javax.swing.JPanel;

/* org.apache.log4j */
import org.apache.log4j.Logger;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.PropertySet;

/* com.nokia.wtk.diagnostics */
import com.nokia.wtk.diagnostics.gui.log.MessageLogView;
import com.nokia.wtk.diagnostics.views.InterViewCommunicator;
import com.nokia.wtk.diagnostics.views.ViewAndControlsInterface;
import com.nokia.wtk.diagnostics.views.AbstractViewFactoryInterface;

/* com.nokia.epdt.core */
import com.nokia.epdt.core.messaging.Message;
import com.nokia.epdt.core.messaging.MessageInterface;
import com.nokia.epdt.core.messaging.MessageListener;
import com.nokia.epdt.core.plugin.CustomPlugin;
import com.nokia.epdt.core.plugin.PluginType;

/**
 * SystemOutputPlugin
 *
 */
public class SystemOutPlugin extends CustomPlugin
    implements SystemOutConstants, AbstractViewFactoryInterface {

    private static final Integer UID = new Integer(0x1020456C);
    private static final Logger log = Logger.getLogger(SystemOutPlugin.class);
    private JPanel jPanel;

    private MessageLogView logView;

    /**
     * Receives ECMT messages
     */
    private class SystemOutPluginMessageListener implements MessageListener {
        public void messageReceived(Message message) {
            try {
                String str = new String(message.getBody(), "utf-8");
                getLogView().getDefaultLog().getLogWriter().print(str);
            } catch (Exception x) {
                log.error(null, x);
            }
        }
    }

    /**
     * Creates instance of SystemOutPlugin
     */
    public SystemOutPlugin() {
    }

    /**
     * Getter for property pluginUID.
     * @return Value of property pluginUID.
     */
    public Integer getPluginUID() {
        return UID;
    }

    /**
     * Getter for property pluginType.
     * @return Value of property pluginType.
     */
    public PluginType getPluginType() {
        return PluginType.DIAGNOSTICS;
    }

    /**
     * Getter for property name.
     * @return Value of property name.
     */
    public String getName() {
        return OutputResourceBundle.getInstance().getValue(PLUGIN_NAME);
    }

    /**
     * Getter for property icon.
     * @param type ignored
     * @return Value of property icon.
     */
    public Image getIcon(int type) {
        return (new ImageIcon()).getImage();
    }

    /**
     * Getter for panel.
     * @return plug-in module user interface
     */
    public JPanel getJPanelView() {
        // This method should never be invoked...
        if (jPanel == null) {
            jPanel = new JPanel(new BorderLayout());
            jPanel.add(getLogView().getPodContent(), BorderLayout.CENTER);
        }
        return jPanel;
    }

    /**
     * Bind the plugin's with the message interface.
     * @param m the message interface which is used to send and receive
     * messages
     */
    public void bindMessageInterface(MessageInterface m) {
        m.setMessageListener(new SystemOutPluginMessageListener(), UID);
    }

    /**
     * Creates and returns MessageLogView.
     * @return the MessageLogView implementation
     */
    private MessageLogView getLogView() {
        if (logView == null) {
            String name = getName();
            logView = new MessageLogView(name, name);
        }
        return logView;
    }

    // ======================================================================
    // AbstractViewFactoryInterface
    // ======================================================================

    /**
     * Returns the string that uniquely identifies the view. It's used as a
     * key for the view's properties
     *
     * @return the view key
     */
    public String getKey() {
        return "systemout";
    }

    /**
     * Returns properties of the view
     * @return properties of the view
     */
    public PropertySet getViewProperties() {
        return getLogView().getViewProperties();
    }

    /**
     * Returns the elements to show when the pod is opened large to show.
     * @return a ViewAndControlsInterface object that will be shown,
     * both the content panel and the controls, when the user requests
     * full disclosure.
     */
    public ViewAndControlsInterface getPodComponent() {
        return getLogView().getPodComponent();
    }

    /**
     * Returns the small component to show.
     * @return a JComponent that will be shown when the user just wishes
     * to see the overview.
     */
    public JComponent getSmallPodComponent() {
        return getLogView().getSmallPodComponent();
    }

    /**
     * @return the short display name of the of the component.
     */
    public String getViewTitle() {
        return getLogView().getViewTitle();
    }

    /**
     * @return a longer description of the component
     */
    public String getViewDescription() {
        return getLogView().getViewDescription();
    }

    /**
     * @return true if the view is expanded by default
     */
    public boolean getDefaultViewOpen() {
        return getLogView().getDefaultViewOpen();
    }

    /**
     * @return the default width of the view
     */
    public int getDefaultViewWidth() {
        return getLogView().getDefaultViewWidth();
    }


    /**
     * @return the default height of the view
     */
    public int getDefaultViewHeight() {
        return getLogView().getDefaultViewHeight();
    }

    /**
     * Registers with {@link InterViewCommunicator} to receive inter-views
     * notifications. This method is invoked when view is being added to
     * diagnostics.
     *
     * @param comm the communicator to register with
     */
    public void bindToCommunicator(InterViewCommunicator comm) {
        getLogView().bindToCommunicator(comm);
    }

    /**
     * Unregisters from {@link InterViewCommunicator}. This method is invoked
     * when view is being removed from diagnostics
     *
     * @param comm the communicator to unregister from
     */
    public void unbindFromCommunicator(InterViewCommunicator comm) {
        getLogView().unbindFromCommunicator(comm);
    }
}
