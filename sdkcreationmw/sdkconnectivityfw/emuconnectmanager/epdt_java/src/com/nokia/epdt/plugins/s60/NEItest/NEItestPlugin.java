/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * NEItest.java
 * Created on 2004-10-12
 */

package com.nokia.epdt.plugins.s60.NEItest;



import com.nokia.epdt.core.messaging.MessageInterface;


import com.nokia.epdt.core.plugin.CustomPlugin;
import com.nokia.epdt.core.plugin.PluginType;

import com.nokia.wtk.diagnostics.views.ViewAndControlsInterface;
import com.nokia.wtk.util.PropertySet;
import com.nokia.wtk.util.gui.MenuButton;

import javax.swing.ImageIcon;
import javax.swing.JComponent;
import javax.swing.JPanel;
import java.awt.Image;


/**
 * NEItestPlugin
 *
 * @author Nokia
 * @version 1.0
 */
public class NEItestPlugin extends CustomPlugin implements ViewAndControlsInterface
{
    private static final Integer UID = new Integer(10203455);

    NEIMessageHandler neiMessageHandler = new NEIMessageHandler();
    NEItestPanel pluginPanel = new NEItestPanel();
    {
        pluginPanel.setNeiMessageHandler(neiMessageHandler);
    }
    
    PropertySet properties;

 
    /**
     * Creates a new instance of NEItestPlugin
     */
    public NEItestPlugin() {
        super();
    }

    /**
     * Getter for property pluginUID.
     * @return Value of property pluginUID.
     */
    public Integer getPluginUID() {
        return NEItestPlugin.UID;
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
        return "NeiTest";
    }

    /**
     * Getter for property icon.
     * @return Value of property icon.
     */
    public Image getIcon(int type) {
        return (new ImageIcon()).getImage();
    }

    /**
     * Getter for property jPanelView.
     * @return plug-in module user interface
     */
    public JPanel getJPanelView() {
        return this.pluginPanel;
    }

    /**
     * Bind the plugin's with the action interface.
     * @param a the action interface which is used to mediate and receive actions
     */
   /* public void bindActionInterface(ActionInterface a) {

    }*/
    /**
     * Bind the plugin's with the message interface.
     * @param m the message interface which is used to send and receive messages
     */
    public void bindMessageInterface(MessageInterface m) 
    {
        m.setMessageListener(neiMessageHandler, this.UID);
        neiMessageHandler.setMessageSender( m.getMessageOutputQueue());
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
        return "taskman";
    }

    /**
     * Returns properties of the view
     * @return properties of the view
     */
    public PropertySet getViewProperties() {
        if (properties == null) {
            properties = new PropertySet();
        }
        return properties;
    }

    /**
     * Returns the elements to show when the pod is opened large to show.
     * @return a ViewAndControlsInterface object that will be shown,
     * both the content panel and the controls, when the user requests
     * full disclosure.
     */
    public ViewAndControlsInterface getPodComponent() {
        return this;
    }

    /**
     * Returns the small component to show.
     * @return a JComponent that will be shown when the user just wishes
     * to see the overview.
     */
    public JComponent getSmallPodComponent() {

        return pluginPanel;
    }

    /**
     * @return the short display name of the of the component.
     */
    public String getViewTitle() {
        return getName();
    }

    /**
     * @return a longer description of the component
     */
    public String getViewDescription() {
        return getName();
    }

    /**
     * @return true if the view is expanded by default
     */
    public boolean getDefaultViewOpen() {
        return true;
    }

    /**
     * @return the default width of the view
     */
    public int getDefaultViewWidth() {
        return 300;
    }


    /**
     * @return the default height of the view
     */
    public int getDefaultViewHeight() {
        return 200;
    }
    //=======================================================================
    // ViewAndControlsInterface
    //=======================================================================

    /**
     * Returns additional controls used to manipulate
     * the display of the view contents, and to get help info.
     * @return JComponent otherwise return null if the view does not
     * use special controls
     */
    public JComponent getControlsPanel() {
        return null;
    }

    /**
     * Returns a MenuButton that triggers one or multiple actions
     * related to the view display.
     * @return MenuButton otherwise return null
     */
    public MenuButton getMenuButton() {
        return null;
    }

    /**
     * This method returns the object that encapsulates the contents of
     * the view. Typically the object that implements the detailed view
     * returns a reference to itself.
     */
    public JComponent getPodContent() {
        return pluginPanel;
    }
  
}
