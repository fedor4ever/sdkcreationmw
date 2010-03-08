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


package com.nokia.epdt.core.ui.diagnostics;

import javax.swing.JPanel;
import javax.swing.JComponent;
import org.apache.log4j.Logger;
import com.nokia.epdt.core.plugin.Plugin;
import com.nokia.epdt.core.plugin.PluginInstance;
import com.nokia.epdt.core.Constants;
import com.nokia.wtk.diagnostics.views.AbstractViewFactoryInterface;
import com.nokia.wtk.diagnostics.views.ViewAndControlsInterface;
import com.nokia.wtk.diagnostics.views.InterViewCommunicator;
import com.nokia.wtk.diagnostics.views.DummyView;
import com.nokia.wtk.util.PropertySet;

/**
 * ViewFactory
 *
 */
public class ViewFactory implements AbstractViewFactoryInterface {
    private static Logger log = Logger.getLogger(ViewFactory.class);
    private AbstractViewFactoryInterface pluginView;
    private PluginInstance pluginInstance;
    private JComponent summaryView;
    private ViewAndControlsInterface mainView;
    private PropertySet properties;
    private String key;

    /**
     * Creates a new instance of ViewFactory
     * @param pi the plugin inswtance for which to create the view
     */
    public ViewFactory(PluginInstance pi) {
        super();
        pluginInstance = pi;

        // Check if plugin implements AbstractViewFactoryInterface
        Plugin plugin = pluginInstance.getPlugin();
        if (plugin instanceof AbstractViewFactoryInterface) {
            pluginView = (AbstractViewFactoryInterface)plugin;
            key = pluginView.getKey();
            properties = pluginView.getViewProperties();
        } else if (plugin instanceof ViewAndControlsInterface) {
            mainView = (ViewAndControlsInterface)plugin;
        }
    }

    /**
     * Returns the string that uniquely identifies the view. It's used as a
     * key for the view's properties
     *
     * @return the view key
     */
    public String getKey() {
        if (key == null) {
            key = pluginInstance.getPluginName();
        }
        return key;
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
     * Create the elements to show when the pod is opened large to show.
     * Plug-in manifest attribute: Plug-in-Class
     * @return a ViewAndControlsInterface object that will be shown,
     * both the content panel and the controls, when the user requests
     * full disclosure.
     */
    public ViewAndControlsInterface getPodComponent() {
        if (mainView == null) {
            if (pluginView != null) {
                mainView = pluginView.getPodComponent();
            }
            if (mainView == null) {
                mainView =  new DummyView("View creation failed.");
            }
        }
        return mainView;
    }

    /**
     * Create the small component to show.
     * Plug-in manifest attribute: Plug-in-Small-Class
     * @return a JComponent that will be shown when the user just wishes
     * to see the overview.
     */
    public JComponent getSmallPodComponent() {
        if (summaryView == null) {
            if (pluginView != null) {
                summaryView = pluginView.getSmallPodComponent();
            } else {
                try {
                     summaryView = (JComponent)pluginInstance.
                         instantiateClassByKey(Constants.PLUGIN_SMALL_CLASS);
                 } catch(Throwable t) {
                     log.error(t);
                 }
            }
            if (summaryView == null) {
                summaryView = new JPanel();
            }
        }
        return summaryView;
    }

    /**
     * Creates a long display name of the of the plugin.
     * Plug-in manifest attribute: Plug-in-Title-Long
     * @return a display name of the of the component
     */
    public String getViewDescription() {
        String description = null;
        if (pluginView != null) {
            description = pluginView.getViewDescription();
        }
        if (description == null) {
            description = getAttributeByKey(Constants.PLUGIN_TITLE_LONG);
        }
        if (description == null) {
            description = "";
        }
        return description;
    }

    /**
     * Creates a short display name of the of the plugin.
     * Plug-in manifest attribute: Plug-in-Title-Short
     * @return a short display name of the of the component.
     */
    public String getViewTitle() {
        String title = null;
        if (pluginView != null) {
            title = pluginView.getViewTitle();
        }
        if (title == null) {
            title = getAttributeByKey(Constants.PLUGIN_TITLE_SHORT);
        }
        if (title == null) {
            title = "";
        }
        return title;
    }

    /**
     * Getter for property viewOpen.
     * Plug-in manifest attribute: Plug-in-Open-Default
     * @return true if the view is expanded by default
     */
    public boolean getDefaultViewOpen() {
        boolean open = true;
        if (pluginView != null) {
            open = pluginView.getDefaultViewOpen();
        } else {
            String val = getAttributeByKey(Constants.PLUGIN_OPEN_DEFAULT);
            if (val != null) {
                open = !val.trim().equals("false");
            }
        }
        return open;
    }

    /**
     * Getter for property width.
     * Value of width is the default width of the view.
     * Plug-in manifest attribute: Plug-in-Width-Default
     * @return Value of property width.
     */
    public int getDefaultViewWidth() {
        if (pluginView != null) {
            return pluginView.getDefaultViewWidth();
        } else {
            return getAttributeByKey(Constants.PLUGIN_WIDTH_DEFAULT,400);
        }
    }

    /**
     * Getter for property height.
     * Value of height is the default height of the view.
     * Plug-in manifest attribute: Plug-in-Height-Default
     * @return Value of property height.
     */
    public int getDefaultViewHeight() {
        if (pluginView != null) {
            return pluginView.getDefaultViewHeight();
        } else {
            return getAttributeByKey(Constants.PLUGIN_HEIGHT_DEFAULT,200);
        }
    }

    /**
     * Registers with {@link InterViewCommunicator} to receive inter-views
     * notifications. This method is invoked when view is being added to
     * diagnostics.
     *
     * @param comm the communicator to register with
     */
    public void bindToCommunicator(InterViewCommunicator comm) {
        if (pluginView != null) {
            pluginView.bindToCommunicator(comm);
        }
    }

    /**
     * Unregisters from {@link InterViewCommunicator}. This method is invoked
     * when view is being removed from diagnostics
     *
     * @param comm the communicator to unregister from
     */
    public void unbindFromCommunicator(InterViewCommunicator comm) {
        if (pluginView != null) {
            pluginView.unbindFromCommunicator(comm);
        }
    }

    /**
     * Returns the value of the plugin jar attribute given the resource key
     * for the attribute name. Don't ask me why we need so many redirections.
     *
     * @param key the resource key for the attribute name
     * @return the value of the requested attribute, <code>null</code> if
     *   it's undefined.
     */
    private String getAttributeByKey(String key) {
        return pluginInstance.getAttributeByKey(key);
    }

    /**
     * Returns the value of the plugin jar attribute given the resource key
     * for the attribute name. Don't ask me why we need so many redirections.
     *
     * @param key the resource key for the attribute name
     * @param defval the default value in case if attribute is undefined
     * @return the value of the requested attribute, the default value if
     *   it's undefined.
     */
    private int getAttributeByKey(String key, int defval) {
        String str = pluginInstance.getAttributeByKey(key);
        if (str != null) {
            try {
                return Integer.parseInt(str.trim());
            } catch (NumberFormatException x) {
                log.warn("Not a number: " + str);
            }
        }
        return defval;
    }
}
