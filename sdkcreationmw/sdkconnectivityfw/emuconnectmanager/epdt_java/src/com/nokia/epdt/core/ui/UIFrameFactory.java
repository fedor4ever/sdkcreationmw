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

package com.nokia.epdt.core.ui;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Toolkit;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.Observable;
import java.util.Observer;
import java.util.ResourceBundle;

import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.border.EmptyBorder;

import org.apache.log4j.Logger;

import com.nokia.epdt.core.Constants;
import com.nokia.epdt.core.CoreController;
import com.nokia.epdt.core.CoreModel;
import com.nokia.epdt.core.connection.ConnectionStatus;
import com.nokia.epdt.core.plugin.Plugin;
import com.nokia.epdt.core.plugin.PluginInstance;
import com.nokia.epdt.core.ui.diagnostics.DiagnosticsGUI;

/**
 * UIFrameFactory
 * Created on 2005-10-05
 */
public class UIFrameFactory {
        /**
         * PreferencesJFrame
         */
        static class PreferencesJFrame extends JFrame implements Constants, Observer {
            private static final Logger log = Logger.getLogger(PreferencesJFrame.class);

            private CoreModel coreModel;
            private CoreController controller;

            private JButton okJButton;
            private JButton cancelJButton;
            private JButton applyJButton;

            private class PreferencesJFrameWindowListener extends WindowAdapter {

                /**
                 * Invoked when a window is in the process of being closed.
                 * The close operation can be overridden at this point.
                 */
                public void windowClosing(WindowEvent e) {
                    if (coreModel.getConnectionStatus() == ConnectionStatus.IDLE &&
                       !coreModel.isDiagnosticsUIVisible() &&
                       !coreModel.isUtilitiesUIVisible() ) {
                        controller.exitProgramCall();
                    }
                }
            }

            /**
             * Creates a new instance of PreferencesJFrame
             */
            public PreferencesJFrame(CoreController controller) {
                this.controller = controller;
                ResourceBundle bundle = controller.getResourceBundle();
                coreModel = controller.getModel();
                setDefaultCloseOperation(coreModel.isExitOnClose() ? EXIT_ON_CLOSE : HIDE_ON_CLOSE);
                addWindowListener(new PreferencesJFrameWindowListener());
                ImageIcon image = new ImageIcon(ClassLoader.getSystemResource(bundle.getString(PREFERENCES_JFRAME_ICON_IMAGE)));
                setIconImage(image.getImage());
                setTitle(bundle.getString(PREFERENCES_JFRAME_TITLE));

                // Add main content panel
                JPanel content = new JPanel(new BorderLayout(0, 0));
                content.add(buildCenterPanel(), BorderLayout.CENTER);
                content.add(buildSouthPanel(bundle), BorderLayout.SOUTH);
                getContentPane().add(content);
                setResizable(false);

                // Pack several times. For some panels (for example, those
                // containing multi-line wrapping text controls) it takes
                // more than one interation to come up with a reasonable
                // preferred layout size.
                //
                // It's not as ridiculous as it may seem. Think about
                // choosing preferred size for a multi-line wrapping text
                // control that has no preferred number of rows or columns.
                // The solution isn't obvious and the best ones involve more
                // than one layout attempt. Usually, two iterations is enough
                // but we allow up to 5, just in case.
                Dimension d1 = getPreferredSize();
                for (int i=0; i<5; i++) {
                    pack();
                    Dimension d2 = getPreferredSize();
                    if (d1.equals(d2)) {
                        // layout has settled
                        break;
                    }
                    d1 = d2;
                }
            }

            private JPanel buildCenterPanel() {
                JPanel panel = new JPanel(new BorderLayout(0, 0));
                // super(new BorderLayout(0, 0));
                panel.setBorder(new EmptyBorder(5,5,0,5));

                // Get plug-ins from the model
                JTabbedPane jTabbedPane = new JTabbedPane();
                // Add plug-ins to tabbed pane
                PluginInstance [] prefs = coreModel.getPreferencesPlugins();
                for (int i = 0; i < prefs.length; i++) {
                    try {
                        Plugin plugin = prefs[i].getPlugin();
                        jTabbedPane.addTab(plugin.getName(), plugin.getJPanelView());
                    } catch (Throwable x) {
                        log.error(x,x);
                    }
                }
                panel.add(jTabbedPane, BorderLayout.CENTER);
                return panel;
            }

            private JPanel buildSouthPanel(ResourceBundle bundle) {
                JPanel panel = new JPanel(new FlowLayout(FlowLayout.RIGHT));
                //this.setLayout(new FlowLayout(FlowLayout.RIGHT));
                okJButton = new JButton(bundle.getString(PREFERENCES_OK_JBUTTON_TEXT));
                cancelJButton = new JButton(bundle.getString(PREFERENCES_CANCEL_JBUTTON_TEXT));
                applyJButton = new JButton(bundle.getString(PREFERENCES_APPLY_JBUTTON_TEXT));
                applyJButton.setEnabled(coreModel.isPreferencesApplyJButtonEnabled());
                panel.add(this.okJButton);
                panel.add(this.cancelJButton);
                panel.add(this.applyJButton);

                return panel;
            }

            /**
             * Getter for okButton
             */
            public JButton getOkButton() {
                return okJButton;
            }

            /**
             * Getter for applyButton
             */
            public JButton getApplyButton() {
                return applyJButton;
            }

            /**
             * Getter for cancelButton
             */
            public JButton getCancelButton() {
                return cancelJButton;
            }

            /**
             * This method is called whenever the pan model is changed (observed object
             * is changed). An application calls an Observable object's notifyObservers
             * method to have all the object's observers notified of the change.
             * @param o the observable object.
             * @param arg an argument passed to the notifyObservers method.
             */
            public void update(Observable o, Object arg) {
                applyJButton.setEnabled(coreModel.isPreferencesApplyJButtonEnabled());
            }
        }

        /**
         * UtilitiesJFrame
         */
        static class UtilitiesJFrame extends JFrame implements Constants, Observer {

            private CoreModel coreModel;
            private CoreController controller;

            private JButton okJButton;
            private JButton cancelJButton;
            private JButton applyJButton;

            private class UtilitiesJFrameWindowListener extends WindowAdapter {
                /**
                 * Invoked when a window is in the process of being closed.
                 * The close operation can be overridden at this point.
                 */
                public void windowClosing(WindowEvent e) {
                    if (coreModel.getConnectionStatus() == ConnectionStatus.IDLE &&
                       !coreModel.isDiagnosticsUIVisible() &&
                       !coreModel.isPreferencesUIVisible() ) {
                        controller.exitProgramCall();
                    }
                }
            }

            /**
             * Creates a new instance of UtilitiesJFrame
             */
            public UtilitiesJFrame(CoreController controller) {
                ResourceBundle bundle = controller.getResourceBundle();
                this.controller = controller;
                coreModel = controller.getModel();
                if ( coreModel.isExitOnClose() ) {
                    this.setDefaultCloseOperation(EXIT_ON_CLOSE);
                } else {
                    this.setDefaultCloseOperation(HIDE_ON_CLOSE);
                }
                this.addWindowListener(new UtilitiesJFrame.UtilitiesJFrameWindowListener());
                Dimension d = Toolkit.getDefaultToolkit().getScreenSize();
                this.setSize(d.width/2, d.height/2);
                ImageIcon image = new ImageIcon(ClassLoader.getSystemResource(bundle.getString(UTILITIES_JFRAME_ICON_IMAGE)));
                this.setIconImage(image.getImage());
                this.setTitle(bundle.getString(UTILITIES_JFRAME_TITLE));
                // Set menu bar
                //this.setJMenuBar(MainJMenuBar.createMainJMenuBar(this));
                // Add main content panel
                JPanel content = new JPanel(new BorderLayout(0, 0));
                content.add(buildCenterPanel(), BorderLayout.CENTER);
                content.add(buildSouthPanel(bundle), BorderLayout.SOUTH);
                this.getContentPane().add(content);
            }

            private JPanel buildCenterPanel() {
                JPanel panel = new JPanel(new BorderLayout(0, 0));
                // Get plug-ins from the model
                JTabbedPane jTabbedPane = new JTabbedPane();
                // Add plug-ins to tabbed pane
                PluginInstance [] prefs = coreModel.getUtilitiesPlugins();
                for (int i = 0; i < prefs.length; i++) {
                    Plugin plugin = prefs[i].getPlugin();
                    JScrollPane jScrollPane = new JScrollPane(plugin.getJPanelView(), JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED, JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
                    jScrollPane.setBorder(new EmptyBorder(1, 1, 1, 1));
                    jTabbedPane.addTab(plugin.getName(), jScrollPane);
                }
                panel.add(jTabbedPane, BorderLayout.CENTER);
                return panel;
            }

            private JPanel buildSouthPanel(ResourceBundle bundle) {
                JPanel panel = new JPanel(new FlowLayout(FlowLayout.RIGHT));
                okJButton = new JButton(bundle.getString(UTILITIES_OK_JBUTTON_TEXT));
                cancelJButton = new JButton(bundle.getString(UTILITIES_CANCEL_JBUTTON_TEXT));
                applyJButton = new JButton(bundle.getString(UTILITIES_APPLY_JBUTTON_TEXT));
                applyJButton.setEnabled(coreModel.isUtilitiesApplyJButtonEnabled());
                panel.add(okJButton);
                panel.add(cancelJButton);
                panel.add(applyJButton);
                return panel;
            }

            /**
             * Getter for okButton
             */
            public JButton getOkButton() {
                return okJButton;
            }

            /**
             * Getter for applyButton
             */
            public JButton getApplyButton() {
                return applyJButton;
            }

            /**
             * Getter for cancelButton
             */
            public JButton getCancelButton() {
                return cancelJButton;
            }


            /**
             * This method is called whenever the model of plug-in is changed (observed object is changed).
             * An application calls an Observable object's notifyObservers method
             * to have all the object's observers notified of the change.
             * @param o the observable object.
             * @param arg an argument passed to the notifyObservers method.
             */
            public void update(java.util.Observable o, Object arg) {
                applyJButton.setEnabled(coreModel.isUtilitiesApplyJButtonEnabled());
            }
        }

        public static JFrame buildPreferencesFrame(CoreController controller) {
            PreferencesJFrame frame = new PreferencesJFrame(controller);
            controller.setPreferencesButtons(frame.getOkButton(),
                                             frame.getCancelButton(),
                                             frame.getApplyButton());
            controller.getModel().addObserver(frame);
            controller.getModel().setPreferencesApplyJButtonEnabled(false);
            return frame;
        }

        public static JFrame buildUtilitiesFrame(CoreController controller) {
            UtilitiesJFrame frame = new UtilitiesJFrame(controller);
            controller.setUtilitiesButtons(frame.getOkButton(),
                                           frame.getCancelButton(),
                                           frame.getApplyButton());
            controller.getModel().addObserver(frame);
            controller.getModel().setUtilitiesApplyJButtonEnabled(false);
            return frame;
        }

        public static JFrame buildDiagnosticsFrame(CoreController controller) {
            return new DiagnosticsGUI(controller).getFrame();
        }
}
