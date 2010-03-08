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


package com.nokia.epdt.plugins.s60.seisuite;

/* java.io */
import java.io.IOException;

/* javax.swing */
import javax.swing.JPanel;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

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

/* com.symbian.tools.j2me.sei */
import com.symbian.tools.j2me.sei.utils.SuitePreferencesView;
import com.symbian.tools.j2me.sei.preferences.UserPreferences;

/**
 * Class SeiSuitePlugin
 *
 * Framework defined plugin class
 */
public class SeiSuitePlugin extends CustomPlugin {
    private ActionEventQueue actionEventQueue;
    private SuitePreferencesView suiteMainPanel;
    private UserPreferences userPreferences;
    private CoreModel coreModel;

//  TODO change the experimental UID to the real one:
    private static final Integer UID = new Integer(0x10203459);
    private static final Logger log = Logger.getLogger(SeiSuitePlugin.class);

    /**
     * Creates <tt>SeiSuitePlugin</tt>
     */
    public SeiSuitePlugin() {
        super(UID, "Suite", PluginType.PREFERENCES);
    }

    /**
     * Getter for property jPanelView.
     * @return plug-in module user interface
     */
    public JPanel getJPanelView() {
        if (suiteMainPanel == null) {
            try {
                suiteMainPanel = new SuitePreferencesView(
                    getUserPreferences().
                    getSuitePreferences());
                suiteMainPanel.setChangeListener(new ViewChangeListener());
            } catch (IOException x) {
                log.error(x,x);
            }
        }
        return suiteMainPanel;
    }

    /**
     * Bind the plugin's with the action interface.
     * @param a the action interface which is used to mediate and receive actions
     */
    public void bindActionInterface(ActionInterface a) {
        a.setActionListener(new CoreActionListener(), UID, ActionListener.PLUGIN);
        actionEventQueue = a.getActionEventQueue();
    }

    /**
     * Bind the plugin's with the epdt core.
     * @param c instance of CoreController that is assosiated with this plugin
     */
    public void bindCore(CoreController c) {
        coreModel = c.getModel();
    }

    private void fireCoreActionEvent(String a) {
        actionEventQueue.put(new Action(a, UID, Action.CORE));
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
                    getUserPreferences().save();
                } catch(IOException ex) {
                    log.debug("could not save suite preferences");
                }
                fireCoreActionEvent(Action.PREFERENCES_REMOVE_PLUGIN_MODEL_CHANGED);
            }
            if (e.getName() == Action.PREFERENCES_OK) {
                fireCoreActionEvent(Action.PREFERENCES_HIDE_PLUGIN);
            }
        }
    }

    /**
     * Handles change events from the view.
     */
    private class ViewChangeListener implements ChangeListener {
        public void stateChanged(ChangeEvent e) {
            fireCoreActionEvent(Action.PREFERENCES_PLUGIN_MODEL_CHANGED);
            suiteMainPanel.updateModel();
        }
    }
}
