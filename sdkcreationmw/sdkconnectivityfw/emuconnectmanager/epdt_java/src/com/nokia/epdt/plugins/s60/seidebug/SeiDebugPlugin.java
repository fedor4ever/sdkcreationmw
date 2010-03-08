/*
* Copyright (c) 2000 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MIDP debugging
 *
*/


package com.nokia.epdt.plugins.s60.seidebug;

/* java.io */
import java.io.IOException;

/* javax.swing */
import javax.swing.JPanel;

/* org.apache.log4j */
import org.apache.log4j.Logger;

/* com.nokia.epdt.core */
import com.nokia.epdt.core.CoreModel;
import com.nokia.epdt.core.CoreController;
import com.nokia.epdt.core.action.Action;
import com.nokia.epdt.core.action.ActionEventQueue;
import com.nokia.epdt.core.action.ActionInterface;
import com.nokia.epdt.core.action.ActionListener;
import com.nokia.epdt.core.plugin.CustomPlugin;
import com.nokia.epdt.core.plugin.PluginType;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.PropertySet;
import com.nokia.wtk.util.PropertySetAdapter;

/* com.symbian.tools.j2me.sei.preferences */
import com.symbian.tools.j2me.sei.preferences.KDPPreferences;
import com.symbian.tools.j2me.sei.preferences.UserPreferences;
import com.symbian.tools.j2me.sei.preferences.SessionPreferences;

/**
 * Class SeiDebugPlugin Is a Preferences ECMT plugin.
 */
public class SeiDebugPlugin extends CustomPlugin implements UIConstants
{
    private static final Integer UID = new Integer(0x10209999);
    private UserPreferences userPreferences;
    private PropertySet properties;
    private CoreModel coreModel;
    private ActionEventQueue actionEventQueue;
    private PropertyChangeHandler propChangeHandler;
    private JPanel panel;

    private static final int DEFAULT_EMULATOR_PORT = 11011;
    private static final int DEFAULT_WLAN_LISTEN_PORT = 11022;
    private static final boolean DEFAULT_ATTACH_TO_EMULATOR = true;

    private static Logger log = Logger.getLogger(SeiDebugPlugin.class);

    /**
     * Creates <tt>SeiDebugPlugin</tt>
     */
    public SeiDebugPlugin() {
        super(UID, NAME, PluginType.PREFERENCES);
        propChangeHandler = new PropertyChangeHandler();
    }

    public void bindActionInterface(ActionInterface a) {
        actionEventQueue = a.getActionEventQueue();
        a.setActionListener(new CoreActionListener(),UID,ActionListener.PLUGIN);
    }

    public void bindCore(CoreController c) {
        coreModel = c.getModel();
    }

    public JPanel getJPanelView() {
        if (panel == null) {
            panel = new SeiDebugPanel(getProperties());
        }
        return panel;
    }

    private PropertySet getProperties() {
        if (properties == null) {
            properties = new PropertySet();
            properties.addBooleanProperty(PROP_ATTACH_TO_EMULATOR,
                                          DEFAULT_ATTACH_TO_EMULATOR);
            properties.addIntProperty(PROP_EMULATOR_PORT,
                                      DEFAULT_EMULATOR_PORT);
            updatePropertySetFromModel();
        }
        return properties;
    }

    private void fireActionEvent(String event) {
        actionEventQueue.put(new Action(event, UID, Action.CORE));
    }

    private void updatePropertySetFromModel() {
        if (properties != null) {
            // avoid unnecessary property change notifications
            properties.removePropertySetListener(propChangeHandler);
            try {
                UserPreferences prefs = getUserPreferences();
                KDPPreferences kdpPrefs = prefs.getKDPPreferences();
                SessionPreferences emulatorPrefs =
                    kdpPrefs.getSessionPreferences(
                    KDPPreferences.EMULATOR_SESSION_NAME);

                // Update properties from SEI preferences model
                properties.setBooleanValue(PROP_ATTACH_TO_EMULATOR,
                    kdpPrefs.isEKA2EmulatorDiscovery());
                properties.setIntValue(PROP_EMULATOR_PORT,
                    emulatorPrefs.getPort());
            } catch (IOException x) {
                log.error("preferences read error", x);
            } finally {
                // restore property change listener
                properties.addPropertySetListener(propChangeHandler);
            }
        }
    }

    private void updateModelFromPropertySet() throws IOException {
        PropertySet ps = getProperties();
        UserPreferences prefs = getUserPreferences();
        KDPPreferences kdpPrefs = prefs.getKDPPreferences();
        SessionPreferences emulatorPrefs =
            kdpPrefs.getSessionPreferences(
            KDPPreferences.EMULATOR_SESSION_NAME);

        // Update SEI preferences model
        kdpPrefs.setEKA2EmulatorDiscovery(ps.getBooleanValue(
            PROP_ATTACH_TO_EMULATOR, DEFAULT_ATTACH_TO_EMULATOR));
        emulatorPrefs.setPort(ps.getIntValue(
            PROP_EMULATOR_PORT, DEFAULT_EMULATOR_PORT));
    }

    /**
     * Returns shared instance of {@link UserPreferences}.
     * @return shared instance of {@link UserPreferences}.
     * @throws IOException if it fails to create {@link UserPreferences}.
     */
    private UserPreferences getUserPreferences() throws IOException {
        if (userPreferences == null) {
            Object prefs = coreModel.getInstanceOf(UserPreferences.class);
            if (prefs instanceof UserPreferences) {
                userPreferences = (UserPreferences)prefs;
            } else {
                try {
                    userPreferences = UserPreferences.load();
                } catch (Throwable e) {
                    userPreferences = new UserPreferences();
                }
                coreModel.setInstanceOf(UserPreferences.class, userPreferences);
            }
        }
        return userPreferences;
    }

    /**
     * Handles preferences dialog's OK and APPLY buttons
     */
    private class CoreActionListener implements ActionListener {
        public void actionPerformed(Action e) {
            if (e.getName() == Action.PREFERENCES_OK ||
                e.getName() == Action.PREFERENCES_APPLY) {
                try {
                    updateModelFromPropertySet();
                    getUserPreferences().save();
                } catch(IOException ex) {
                    log.debug("could not save preferences");
                }
                fireActionEvent(Action.PREFERENCES_REMOVE_PLUGIN_MODEL_CHANGED);
                if (e.getName() == Action.PREFERENCES_OK) {
                    fireActionEvent(Action.PREFERENCES_HIDE_PLUGIN);
                }
            }
        }
    }

    /**
     * Notifies  the core that the model has changed
     */
    private class PropertyChangeHandler extends PropertySetAdapter {
        public void propertySetChanged(PropertySet ps) {
            fireActionEvent(Action.PREFERENCES_PLUGIN_MODEL_CHANGED);
        }
    }
}
