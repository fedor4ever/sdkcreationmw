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
* Description: 
 *
*/


package com.nokia.epdt.plugins.s60.securityconf;

/* java.io */
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

/* javax.swing */
import javax.swing.JPanel;

/* com.nokia.epdt.core */
import com.nokia.epdt.core.CoreUtils;

/* com.nokia.epdt.core.action */
import com.nokia.epdt.core.action.ActionEventQueue;
import com.nokia.epdt.core.action.ActionInterface;
import com.nokia.epdt.core.action.ActionListener;
import com.nokia.epdt.core.action.Action;

/* com.nokia.epdt.core.plugin */
import com.nokia.epdt.core.plugin.CustomPlugin;
import com.nokia.epdt.core.plugin.PluginType;

/**
 * MIDP security plugin.
 * Framework defined class
 */
public class SecurityConfPlugin extends CustomPlugin implements SecurityConstants {

    private static final Integer UID = new Integer(0x10203455);
    private ActionEventQueue actionEventQueue;
    private SecConfMainPanel secConfMainPanel;
    private SecConfModel secConfModel;

    public SecurityConfPlugin() throws IOException {
        super(UID, "MIDP Security", PluginType.PREFERENCES);
        secConfModel = new SecConfModel();
        File modelFile = new File(CoreUtils.EpocRoot() + MODEL_BACKUP_FILE);
        if (modelFile.exists()) {
            try {
                FileInputStream in = new FileInputStream(modelFile);
                try {
                    secConfModel.load(in);
                } finally {
                    in.close();
                }
            } catch (Exception x) {
                x.printStackTrace();
            }
        }
    }

    SecConfModel getSecConfModel() {
        return secConfModel;
    }

    /**
     * Getter for property jPanelView.
     * @return plug-in module user interface
     */
    public JPanel getJPanelView() {
        if (secConfMainPanel == null) {
            secConfMainPanel = new SecConfMainPanel(this);
        }
        return secConfMainPanel;
    }

    /**
     * Bind the plugin's with the action interface.
     * @param a the action interface which is used to mediate and receive
     * actions
     */
    public void bindActionInterface(ActionInterface a) {
        a.setActionListener(new CoreActionListener(),UID,ActionListener.PLUGIN);
        this.actionEventQueue = a.getActionEventQueue();
    }

    /**
     * Notifies the core that a plugin model is changed
     */
    void pluginModelChangedAction() {
        fireCoreActionEvent(Action.PREFERENCES_PLUGIN_MODEL_CHANGED);
    }

    private void fireCoreActionEvent(String a) {
        actionEventQueue.put(new Action(a, UID, Action.CORE));
    }

    /**
     * Class SecurityPluginActionListener
     * responsible for handling preferences dialog's OK and APPLY buttons
     */
    private class CoreActionListener implements ActionListener {
        public void actionPerformed(Action e) {

            if (e.getName().equals(Action.PREFERENCES_OK) ||
                e.getName().equals(Action.PREFERENCES_APPLY)) {

                File modelFile = new File(CoreUtils.EpocRoot() +
                                          MODEL_BACKUP_FILE);
                try {
                    FileOutputStream out = new FileOutputStream(modelFile);
                    try {
                        secConfModel.save(out);
                    } finally {
                        out.close();
                    }
                } catch (IOException x) {
                    x.printStackTrace();
                }

                File policyFile = new File(CoreUtils.EpocRoot() +
                                           POLICY_FILE_PATH_RELATIVE +
                                           POLICY_FILE_NAME);

                File backupPolicyFile = new File(CoreUtils.EpocRoot() +
                                                 POLICY_FILE_PATH_RELATIVE +
                                                 BACKUP_POLICY_FILE_NAME);

                // If a policy file hasn't been backed up, do that:
                if (policyFile.exists() && !backupPolicyFile.exists()) {
                    policyFile.renameTo(backupPolicyFile);
                }

                // Write new user created policy into the policy file:
                try {
                    FileOutputStream out = new FileOutputStream(policyFile);
                    try {
                        secConfModel.writePolicyFile(out);
                    } finally {
                        out.close();
                    }
                } catch (IOException x) {
                    x.printStackTrace();
                }

                fireCoreActionEvent(Action.PREFERENCES_REMOVE_PLUGIN_MODEL_CHANGED);
            }

            if (e.getName().equals(Action.PREFERENCES_OK)) {
                fireCoreActionEvent(Action.PREFERENCES_HIDE_PLUGIN);
            }
        }
    }
}
