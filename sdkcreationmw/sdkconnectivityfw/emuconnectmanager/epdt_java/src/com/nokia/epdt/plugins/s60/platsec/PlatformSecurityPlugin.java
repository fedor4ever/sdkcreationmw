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



package com.nokia.epdt.plugins.s60.platsec;

import java.util.Properties;
import org.apache.log4j.Logger;
import com.nokia.epdt.core.CoreController;
import com.nokia.epdt.core.action.Action;
import com.nokia.epdt.core.action.ActionEventQueue;
import com.nokia.epdt.core.action.ActionInterface;
import com.nokia.epdt.core.action.ActionListener;
import com.nokia.epdt.core.config.epocini.EpocIniConfigurator;
import com.nokia.epdt.core.plugin.CustomPlugin;
import com.nokia.epdt.core.plugin.PluginType;

/**
 *
 * PlatformSecurityPlugin
 */
public class PlatformSecurityPlugin extends CustomPlugin implements SecEpocIniConstants {
    private static final Integer UID = new Integer(0x1020457A);
    private static Logger log = Logger.getLogger(PlatformSecurityPlugin.class);
    private static Properties pluginProperties = loadProperties();
    private PlatformSecurityJPanel jPanel;
    private ActionEventQueue actionEventQueue = null;
    
    //Properties
    //key: id
    //value: ON/OFF
    //Here we mix capabilities and other settings
    //in PlatSec dialog ("enable debug messages" etc) together
    //the settings in epoc.ini are ON or OFF
    //if a capability is granted (check box on), value in a property
    //collection is "ON" and vise versa.
    //whereas capabilities are added into epoc.ini 
    //as a list for PlatSecDisabledCaps key
    private Properties modelProperties = new Properties();
    private Properties prevModelProperties = new Properties();

    private EpocIniConfigurator configurator;
    

    private class PlatformSecurityActionListener implements ActionListener {
        public void actionPerformed(Action e) {
                // OK button action
            if ( e.getName() == Action.PREFERENCES_OK) {
                prevModelProperties = (Properties)modelProperties.clone();
                writeData(modelProperties);
                actionEventQueue.put(new Action(Action.PREFERENCES_REMOVE_PLUGIN_MODEL_CHANGED, UID, Action.CORE, new int[0]));
                actionEventQueue.put(new Action(Action.PREFERENCES_HIDE_PLUGIN, UID, Action.CORE, new int[0]));
                // Apply button action
            } else if ( e.getName() == Action.PREFERENCES_APPLY) {
                prevModelProperties = (Properties)modelProperties.clone();
                writeData(modelProperties);
                actionEventQueue.put(new Action(Action.PREFERENCES_REMOVE_PLUGIN_MODEL_CHANGED, UID, Action.CORE, new int[0]));
                // cancel button action
            } else if ( e.getName() == Action.PREFERENCES_CANCEL) {
                modelProperties = (Properties) prevModelProperties.clone();
                actionEventQueue.put(new Action(Action.PREFERENCES_REMOVE_PLUGIN_MODEL_CHANGED, UID, Action.CORE, new int[0]));
                actionEventQueue.put(new Action(Action.PREFERENCES_HIDE_PLUGIN, UID, Action.CORE, new int[0]));
            }
        }
    }

    private static Properties loadProperties() {
    	Properties p = new Properties();
        try {
            p.load(PlatformSecurityPlugin.class.getResourceAsStream("platsec.properties"));
        } catch(Throwable e) {
            log.warn("properties load error", e);
        }
        return p;
    }

    public PlatformSecurityPlugin() {
        super(UID, pluginProperties.getProperty("plugin.name"), PluginType.PREFERENCES);
        jPanel = new PlatformSecurityJPanel(this);
        this.setJPanelView(jPanel);
    }

    private boolean readData(Properties p) {
        EpocIniConfigurator c = configurator;
        p.setProperty(PLATSEC_DIAGNOSTICS, c.getProperty(PLATSEC_DIAGNOSTICS));
        p.setProperty(PLATSEC_PROCESS_ISOLATION, c.getProperty(PLATSEC_PROCESS_ISOLATION));
        p.setProperty(PLATSEC_ENFORCEMENT, c.getProperty(PLATSEC_ENFORCEMENT));
        p.setProperty(PLATSEC_ENFORCE_SYS_BIN, c.getProperty(PLATSEC_ENFORCE_SYS_BIN));
        p.setProperty(PLATSEC_PROCESS_ISOLATION, c.getProperty(PLATSEC_PROCESS_ISOLATION));
        String capabilitiesList = c.getProperty(PLAT_SEC_DISABLED_CAPS);
        for(int i=0; i< CAPABILITIES.length ;i++)
        {
        	if(capabilitiesList.lastIndexOf(CAPABILITIES[i]) == -1)
        	{
        		p.setProperty(CAPABILITIES[i], "OFF");
        	}
        	else
        	{
        		p.setProperty(CAPABILITIES[i], "ON");
        	}
        }
        return false;
    }
    private boolean writeData(Properties p) {
    	
    	((PlatformSecurityJPanel)jPanel).showRestartWarning();
        EpocIniConfigurator c = configurator;
        c.setProperty(PLATSEC_DIAGNOSTICS, p.getProperty(PLATSEC_DIAGNOSTICS));
        c.setProperty(PLATSEC_PROCESS_ISOLATION, p.getProperty(PLATSEC_PROCESS_ISOLATION));
        c.setProperty(PLATSEC_ENFORCEMENT, p.getProperty(PLATSEC_ENFORCEMENT));
        c.setProperty(PLATSEC_ENFORCE_SYS_BIN, p.getProperty(PLATSEC_ENFORCE_SYS_BIN));
        c.setProperty(PLATSEC_PROCESS_ISOLATION, p.getProperty(PLATSEC_PROCESS_ISOLATION));
        StringBuffer capsList = new StringBuffer();// = p.getProperty(PLATSEC_PROCESS_ISOLATION);
        for(int i=0; i< CAPABILITIES.length ;i++)
        {
        	if((p.getProperty(CAPABILITIES[i])).equals("ON"))
        	{
        		if(capsList.length()>0)
        		{//separator to be added except for the first cap 
        			capsList.append("+");
        		}
        		capsList.append(CAPABILITIES[i]);
        	}
        }
        if(capsList.length()==0)
        {
        	capsList.append("NONE");
        }
        c.setProperty(PLAT_SEC_DISABLED_CAPS, capsList.toString());
        configurator.saveSettings();
        return true;
    }

    /**
     * Bind the plugin's with the action interface.
     * @param a the action interface which is used to mediate and receive actions
     */
    public void bindActionInterface(ActionInterface a) {
        a.setActionListener(new PlatformSecurityActionListener(), UID, ActionListener.PLUGIN);
        actionEventQueue = a.getActionEventQueue();
    }

    Properties getPluginProperties() {
    	return pluginProperties;
    }
    boolean getModelProperty(String key) {
        return modelProperties.getProperty(key) != null &&
        		(modelProperties.getProperty(key) ).equals("ON");
    }

    void setModelProperty(String key, boolean value) {
        //setModelProperty(key, Boolean.toString(value));
    	modelProperties.setProperty(key, value?"ON":"OFF");
        actionEventQueue.put(new Action(
                Action.PREFERENCES_PLUGIN_MODEL_CHANGED, UID,
                Action.CORE, new int[0]));
    }

    public void bindCore(CoreController controller) {
    	configurator = controller.getEpocIniConfigurator();
        readData(modelProperties);
        readData(prevModelProperties);
        jPanel.update();
    }


}
