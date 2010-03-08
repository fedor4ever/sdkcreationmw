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


package com.nokia.epdt.core.command;

import java.util.Properties;

import javax.swing.JFrame;
import javax.swing.SwingUtilities;

import org.apache.log4j.Logger;
import com.nokia.epdt.core.Constants;
import com.nokia.epdt.core.CoreController;
import com.nokia.epdt.core.CoreModel;
import com.nokia.epdt.core.connection.ConnectionStatus;
import com.nokia.epdt.core.connection.SimpleSocketConnection;

/**
 * CommandFactory
 */
public class CommandFactory implements Constants  {
    private static Logger log = Logger.getLogger(CommandFactory.class);
    private CoreController coreController = null;

    /**
     * Creates a new instance of CommandFactory
     */
    public CommandFactory(CoreController coreController) {
        this.coreController = coreController;
    }

    /**
     * Creates a new instance of Command
     * @param param parameters
     * @return Command object
     */
    public Command create(Properties param) {
        try {
            Object value = param.get(COMMAND_KEY);
            log.debug((String)value);
            if ( value.equals(SHOW_PREFERENCES_VALUE) ) {
                return new ShowPreferencesUI(param);
            } else if ( value.equals(HIDE_PREFERENCES_VALUE) ) {
                return new HidePreferencesUI();
            } else if ( value.equals(SHOW_DIAGNOSTICS_VALUE) ) {
                return new ShowDiagnosticsUI();
            } else if ( value.equals(HIDE_DIAGNOSTICS_VALUE) ) {
                return new HideDiagnosticsUI();
            } else if ( value.equals(SHOW_UTILITIES_VALUE) ) {
                return new ShowUtilitiesUI();
            } else if ( value.equals(HIDE_UTILITIES_VALUE) ) {
                return new HideUtilitiesUI();
            } else if ( value.equals(OPEN_VALUE) ) {
                return new OpenSimpleSocketConnectionCommand(param);
            } else if ( value.equals(EXIT_VALUE) ) {
                return new ExitCommand();
            } else {
                log.error("Request is incorrect");
            }
        } catch (Exception e) {
            log.error(e,e);
        }
        return getDefaultCommand();
    }

    /**
     * DefaultCommand getter
     */
    private static Command getDefaultCommand() {
        class DefaultCommand implements Command {
            public void execute() {}
        }
        return new DefaultCommand();
    }

    // Makes sure that the run() method is invoked on the event dispatch
    // thread. It's better to construct UI objects on the UI thread.
    private static abstract class UICommand implements Command, Runnable {
        public final void execute() {
            if (SwingUtilities.isEventDispatchThread()) {
                run();
            } else {
                // perhaps, we should use invokeAndWait instead?
                SwingUtilities.invokeLater(this);
            }
        }
    }

    private class ShowPreferencesUI extends UICommand {
        private boolean setLocation = false;
        private int x = 0;
        private int y = 0;
        ShowPreferencesUI(Properties params) {
            String top = params.getProperty(POS_TOP_KEY);
            String left = params.getProperty(POS_LEFT_KEY);
            if (left != null) {
                try {
                    x = Integer.parseInt(left);
                    setLocation = true;
                } catch (Exception e) {
                    log.error(e,e);
                }
            }
            if (top != null) {
                try {
                    y = Integer.parseInt(top);
                    setLocation = true;
                } catch (Exception e) {
                    log.error(e,e);
                }
            }
        }
        public void run() {
        	
        	// Debugging code. If you need to debug situation where -Dcommand=show_preferences
        	// is needed as VM argument and you need to connect to the emulator, it is not 
        	// possible to use -Dcommand=open (only one command can be used). This is why 
        	// we have to do this "open" here in the code level. Add other commands here if 
        	// needed.
        	//
        	//CommandFactory.OpenSimpleSocketConnectionCommand openCo =
        	//	new CommandFactory.OpenSimpleSocketConnectionCommand("127.0.0.1", "49359");
        	//openCo.execute();
        	//
        	// SHOW DIAGNOSTICS
        	//
        	// SHOW UTILITIES
        	//
        	
        	
        	
            // This code is invoked on the event dispatch thread
            JFrame ui = coreController.getModel().getPreferencesUI();
            if (setLocation) ui.setLocation(x,y);
            ui.setVisible(true);
            ui.toFront();
        }
    }

    private class ShowDiagnosticsUI extends UICommand {
        public void run() {
            // This code is invoked on the event dispatch thread
            JFrame ui = coreController.getModel().getDiagnosticsUI();
            ui.setVisible(true);
            ui.toFront();
        }
    }

    private class ShowUtilitiesUI extends UICommand {
        public void run() {
            // This code is invoked on the event dispatch thread
            JFrame ui = coreController.getModel().getUtilitiesUI();
            ui.setVisible(true);
            ui.toFront();
        }
    }

    private class HidePreferencesUI implements Command {
        public void execute() {
            // It's safe to hide the UI from an arbitrary thread
            coreController.getModel().hidePreferencesUI();
        }
    }

    private class HideDiagnosticsUI implements Command {
        public void execute() {
            // It's safe to hide the UI from an arbitrary thread
            coreController.getModel().hideDiagnosticsUI();
        }
    }

    private class HideUtilitiesUI implements Command {
        public void execute() {
            // It's safe to hide the UI from an arbitrary thread
            coreController.getModel().hideUtilitiesUI();
        }
    }

    private class ExitCommand implements Command {

        /**
         * An execute method that simply calls the command on the receiver.
         */
        public void execute() {
            CoreModel model = coreController.getModel();
            model.getPreferencesUI().setVisible(false);
            model.getDiagnosticsUI().setVisible(false);
            model.getUtilitiesUI().setVisible(false);
            coreController.exitProgramCall();
        }
    }

    private class OpenSimpleSocketConnectionCommand implements Command {
        private String host;
        private int port;

        /**
         * Creates a new instance of OpenSimpleSocketConnectionCommand
         * @param host
         * @param port
         */
        OpenSimpleSocketConnectionCommand(Properties param) {
            this(param.getProperty(HOST_KEY), param.getProperty(PORT_KEY));
        }

        /**
         * Creates a new instance of OpenSimpleSocketConnectionCommand
         * @param host
         * @param port
         */
        OpenSimpleSocketConnectionCommand(String host, String port) {
            this.host = host;
            this.port = Integer.parseInt(port);
        }

        /**
         * An execute method that simply calls the command on the receiver.
         */
        public void execute() {
            log.debug("Execute OpenSimpleSocketConnectionAction");
            CoreModel coreModel = coreController.getModel();
            if ( coreModel.getConnection() == null ||
                 coreModel.getConnectionStatus() == ConnectionStatus.IDLE ||
                 coreModel.getConnectionStatus() == ConnectionStatus.FAILED ) {
                SimpleSocketConnection ssc = new SimpleSocketConnection(host, port, coreController);
                log.debug("Open simple socket connection");
                ssc.open();
                if (coreModel.getConnectionStatus() == ConnectionStatus.ESTABLISHED) {
                    coreModel.setConnection(ssc);
                }
            }
        }
    }

    /**
     * Getter for OpenSimpleSocketConnectionCommand.
     * IT APPEARS THAT THIS METHOD IS NOT BEING USED.
     *
     * @param host connection host
     * @param port connection port
     * @param controller
     */
    public Command createOpenSimpleSocketConnectionCommand(String host, String port) {
        return new OpenSimpleSocketConnectionCommand(host, port);
    }
}
