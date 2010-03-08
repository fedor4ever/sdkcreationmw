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


package com.nokia.epdt.plugins.s60.taskman;

/* java.beans */
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

/* java.util */
import java.util.List;
import java.util.ArrayList;

/* javax.swing */
import javax.swing.JPanel;
import javax.swing.JMenuItem;
import javax.swing.JComponent;
import javax.swing.JPopupMenu;
import javax.swing.SwingUtilities;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.PropertySet;
import com.nokia.wtk.util.PropertyEditors;
import com.nokia.wtk.util.gui.MenuButton;

/* com.nokia.wtk.diagnostics.views */
import com.nokia.wtk.diagnostics.views.InterViewCommunicator;
import com.nokia.wtk.diagnostics.views.ViewAndControlsInterface;
import com.nokia.wtk.diagnostics.views.AbstractViewFactoryInterface;

/* com.nokia.epdt.core... */
import com.nokia.epdt.core.plugin.CustomPlugin;
import com.nokia.epdt.core.plugin.PluginType;
import com.nokia.epdt.core.messaging.Message;
import com.nokia.epdt.core.messaging.MessageQueue;
import com.nokia.epdt.core.messaging.MessageListener;
import com.nokia.epdt.core.messaging.MessageInterface;

/**
 * TaskManagerPlugin
 */
public class TaskManagerPlugin extends CustomPlugin
    implements ViewAndControlsInterface,
               AbstractViewFactoryInterface,
               TaskManagerConstants {

    private static final Integer UID = new Integer(0x10204578);
    private static final int TARGET_UID = 0x10203452;
    private TaskManagerModel taskManagerModel = new TaskManagerModel();
    private MessageQueue messageOutputQueue;
    private JPanel largeComponent;
    private JComponent smallComponent;
    private MenuButton menuButton;

    /**
     * A MessageListener object is used to receive asynchronously delivered messages.
     */
    private class TaskManagerPluginMessageListener implements MessageListener, Runnable {
        private List messageList = new ArrayList();
        private boolean updateScheduled = false;

        public void messageReceived(Message m) {
            if (taskManagerModel.getUpdateSpeedValue() != 0) {
                TaskManagerMessage tm = TaskManagerMessageParser.parseMessage(m);
                synchronized (messageList) {
                    messageList.add(tm);
                    if (!updateScheduled) {
                        updateScheduled = true;
                        SwingUtilities.invokeLater(this);
                    }
                }
            }
        }

        public void run() {
            TaskManagerMessage [] messages;
            synchronized (messageList) {
                updateScheduled = false;
                messages = new TaskManagerMessage[messageList.size()];
                messageList.toArray(messages);
                messageList.clear();
            }

            for (int i=0; i<messages.length; i++) {
                TaskManagerMessage tm = messages[i];

                // Update CPU/memory model
                taskManagerModel.setCpu(tm.getCpu());
                taskManagerModel.setMemory(tm.getMemory());

                // Take these only from the last messsage
                if (i == (messages.length-1)) {
                    taskManagerModel.setDriveList(tm.getDriveList());
                    taskManagerModel.setApplicationList(tm.getApplicationList());
                    taskManagerModel.setThreadList(tm.getThreadList());
                }

                // Notify observers on each message to update CPU/memory graphs
                taskManagerModel.setChangedAndNotifyObservers();
            }
        }
    }

    /**
     * Creates a new instance of TaskManagerPlugin
     */
    public TaskManagerPlugin() {
    }

    /**
     * Getter for property pluginUID.
     * @return Value of property pluginUID.
     */
    public Integer getPluginUID() {
        return TaskManagerPlugin.UID;
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
        return getResString(PLUGIN_NAME);
    }

    /**
     * Getter for property jTaskManagerelView.
     * @return plug-in module user interface
     */
    public JPanel getJPanelView() {
        if (largeComponent == null) {
            largeComponent = new TaskManagerPluginJPanel(this);
        }
        return largeComponent;
    }

    /**
     * Bind the plugin's with the message interface.
     * @param m the message interface which is used to send and receive messages
     */
    public void bindMessageInterface(MessageInterface m) {
        m.setMessageListener(new TaskManagerPluginMessageListener(), UID);
        messageOutputQueue = m.getMessageOutputQueue();
        updateSpeedChanged();
        taskManagerModel.getUpdateSpeedProperty().addPropertyChangeListener(
            new UpdateSpeedChangeListener()
        );
    }

    private class UpdateSpeedChangeListener implements PropertyChangeListener {
        public void propertyChange(PropertyChangeEvent e) {
            updateSpeedChanged();
        }
    }

    void updateSpeedChanged() {
        messageOutputQueue.put(TaskManagerMessageFactory.createMessage(TARGET_UID, taskManagerModel.getUpdateSpeedValue(), 0));
    }

    /**
     * Returns the plugin data model
     * @return the plugin data model
     */
    TaskManagerModel getTaskManagerModel() {
        return taskManagerModel;
    }

    /**
     * Reads a string from the resource bundle
     * @param key the resource key
     * @return the string from the resource bundle, null if none
     */
    private static String getResString(String key) {
        return TaskManagerResourceBundle.getInstance().getValue(key);
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
        return taskManagerModel.getProperties();
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
        if (smallComponent == null) {
            smallComponent = new TaskManagerPluginSmallJPanel(taskManagerModel);
        }
        return smallComponent;
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

    /**
     * Registers with {@link InterViewCommunicator} to receive inter-views
     * notifications. This method is invoked when view is being added to
     * diagnostics.
     *
     * @param comm the communicator to register with
     */
    public void bindToCommunicator(InterViewCommunicator comm) {
    }

    /**
     * Unregisters from {@link InterViewCommunicator}. This method is invoked
     * when view is being removed from diagnostics
     *
     * @param comm the communicator to unregister from
     */
    public void unbindFromCommunicator(InterViewCommunicator comm) {
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
        if (menuButton == null) {
            menuButton = new MenuButton(getViewTitle());
            //Misc.setOpaque(menuButton, false);
            JPopupMenu popup = menuButton.getPopupMenu();
            JMenuItem [] menuItems = PropertyEditors.createRadioMenuItems(
                taskManagerModel.getUpdateSpeedProperty(),
                TaskManagerModel.getUpdateSpeedLabels(),
                TaskManagerModel.getUpdateSpeedValues());
            for (int i=0; i<menuItems.length; i++) {
                popup.add(menuItems[i]);
            }
        }
        return menuButton;
    }

    /**
     * This method returns the object that encapsulates the contents of
     * the view. Typically the object that implements the detailed view
     * returns a reference to itself.
     */
    public JComponent getPodContent() {
        return getJPanelView();
    }
}
