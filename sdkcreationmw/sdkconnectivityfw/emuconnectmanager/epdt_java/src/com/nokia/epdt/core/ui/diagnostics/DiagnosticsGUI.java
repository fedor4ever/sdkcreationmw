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

/* java.io */
import java.io.File;
import java.util.ResourceBundle;

/* java.awt.event */
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

/* javax.swing */
import javax.swing.JFrame;

/* org.apache.log4j */
import org.apache.log4j.Logger;

/* com.nokia.wtk */
import com.nokia.wtk.diagnostics.debug.DebugViews;
import com.nokia.wtk.util.PropertySet;

/* com.nokia.epdt.core */
import com.nokia.epdt.core.Constants;
import com.nokia.epdt.core.CoreController;
import com.nokia.epdt.core.CoreModel;
import com.nokia.epdt.core.connection.ConnectionStatus;
import com.nokia.epdt.core.plugin.PluginInstance;

/* com.nokia.epdt.core.action */
import com.nokia.epdt.core.action.Action;
import com.nokia.epdt.core.action.ActionListener;

/**
 * DiagnosticsGUI
 */
public class DiagnosticsGUI implements Constants {

    private static final Logger log = Logger.getLogger(DiagnosticsGUI.class);
    private static final String CONFIG_FILE = "diagnostics.settings";
    private static final String CONFIG_COMMENT = "Series60 Diagnostics";

    private PropertySet settings;
    private CoreModel coreModel;
    private CoreController coreController;
    private JFrame frame;

    private class DiagnosticsJFrameWindowListener extends WindowAdapter {
        /**
         * Invoked when a window is in the process of being closed.
         * The close operation can be overridden at this point.
         */
        public void windowClosing(WindowEvent e) {
            settings.save(new File(CONFIG_FILE), CONFIG_COMMENT);
            if (coreModel.getConnectionStatus() == ConnectionStatus.IDLE &&
               !coreModel.getPreferencesUI().isVisible() &&
               !coreModel.getUtilitiesUI().isVisible() ) {
                coreController.exitProgramCall();
            }
        }
    }

    private class DiagnosticsActionListener implements ActionListener {
        public void actionPerformed(Action e) {
            if (e.getName() == Action.PROGRAM_CLOSING) {
                settings.save(new File(CONFIG_FILE), CONFIG_COMMENT);
            }
        }
    }

    /**
     * Creates a new instance of DiagnosticsGUI
     */
    public DiagnosticsGUI(CoreController controller) {
        coreController = controller;
        coreModel = controller.getModel();
        coreController.getActionInterface().setActionListener(
            new DiagnosticsActionListener(),
            // and I'm not sure what the next two parameters mean
            new Integer(0), ActionListener.CORE);

        settings = new PropertySet();
        DebugViews diagnostics = new DebugViews(coreModel, settings);
        PluginInstance [] plugins = coreModel.getDiagnosticsPlugins();
        for (int i=0; i<plugins.length; i++) {
            try {
                diagnostics.addView(new ViewFactory(plugins[i]));
            } catch (Throwable x) {
                log.error(x,x);
            }
        }

        frame = diagnostics.getMainWindow().getFrame();
        frame.addWindowListener(new DiagnosticsJFrameWindowListener());
        if (coreModel.isExitOnClose()) {
            frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        } else {
            frame.setDefaultCloseOperation(JFrame.HIDE_ON_CLOSE);
        }
        ResourceBundle bundle = controller.getResourceBundle();
        frame.setTitle(bundle.getString(DIAGNOSTICS_JFRAME_TITLE));
        frame.pack();

        // load settings
        settings.load(new File(CONFIG_FILE), true);
    }

    public JFrame getFrame() {
        return frame;
    }
}
