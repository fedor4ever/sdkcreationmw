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



package com.nokia.epdt.plugins.s60.eventsim;

import javax.swing.JPanel;
import java.awt.Image;
import javax.swing.ImageIcon;

import com.nokia.epdt.core.CoreController;
import com.nokia.epdt.core.plugin.CustomPlugin;
import com.nokia.epdt.core.plugin.PluginType;
import com.nokia.epdt.core.messaging.MessageInterface;
import com.nokia.epdt.core.messaging.MessageListener;
import com.nokia.epdt.core.messaging.Message;
import com.nokia.epdt.core.messaging.MessageQueue;
import com.nokia.epdt.core.action.Action;
import com.nokia.epdt.core.action.ActionInterface;
import com.nokia.epdt.core.action.ActionListener;
import com.nokia.epdt.core.action.ActionEventQueue;
import org.apache.log4j.Logger;

/* com.nokia.epdt.core.config.ini.epoc */
import com.nokia.epdt.core.config.epocini.EpocIniConfigurator;
import com.nokia.epdt.core.config.epocini.EpocIniConstants;
import com.nokia.epdt.plugins.s60.eventsim.Constants;
import com.nokia.epdt.plugins.s60.eventsim.ReceivedMsg;



/*
 * Provides "bootstrapping" and message handling code for the plugin.
 *
 * @version 1.0
 */
public class EventSimPlugin extends CustomPlugin {

  public static final int UID = 0x101FD985;
  public static final String PLUGIN_NAME = "Events";
  private static Logger log = Logger.getLogger(EventSimPlugin.class);
  private MessageQueue messageOutputQueue = null;
  private ActionEventQueue actionEventQueue = null;
  private EpocIniConfigurator configurator;
  
  private MainPanel mainPanel = null;
  
  /**
  * A ActionListener object is used to receive synchronously delivered actions.
  */
  private class EventSimPluginActionListener implements ActionListener {
    
    public void actionPerformed(Action e) {
      if (e.getName() == Action.UTILITIES_OK) {
    	  mainPanel.applyChanges();
    	  
          actionEventQueue.put(new Action(
              Action.UTILITIES_REMOVE_PLUGIN_MODEL_CHANGED,
              getPluginUID(), Action.CORE, new int[0]));
          actionEventQueue.put(new Action(
              Action.UTILITIES_HIDE_PLUGIN, getPluginUID(),
              Action.CORE, new int[0]));
      }
      else if (e.getName() == Action.UTILITIES_APPLY) {
    	  mainPanel.applyChanges();
      }
    }
  }

public MessageQueue getMessageOutputQueue()
{
	return messageOutputQueue;
}

public EpocIniConfigurator getConfigurator()
{
	return configurator;
}

  /**
  * A MessageListener object is used to receive asynchronously delivered
  * messages.
  */
  private class EventSimPluginMessageListener implements MessageListener {

    public void messageReceived(Message message) {

        ReceivedMsg msg = new ReceivedMsg(message);
        //forward msg to panels for them to get updated
        mainPanel.msgReceived(msg);
    }
  }
  /**
   * Creates a new <code>eventsimPlugin</code> instance. Creates the
   * JPanel's, the widgets and the action listeners.
   *
   */
  public EventSimPlugin()
  {	  super();
      mainPanel = new MainPanel(this);
  }

  /**
   * Getter for property pluginUID.
   * @return Value of property pluginUID.
   */
  public Integer getPluginUID() {
      return new Integer(EventSimPlugin.UID);
  }

  /**
   * Getter for property pluginType.
   * @return Value of property pluginType.
   */
  public PluginType getPluginType() {
      return PluginType.UTILITIES;
  }

  /**
   * Getter for property name.
   * @return Value of property name.
   */
  public String getName() {
      return PLUGIN_NAME;
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
      return (JPanel)this.mainPanel;
  }

  /**
   * Bind the plugin's with the message interface.
   * @param m the message interface which is used to send and receive messages
   */
  public void bindMessageInterface(MessageInterface m) {
      m.setMessageListener(new EventSimPlugin.EventSimPluginMessageListener(),
    		  getPluginUID());
      this.messageOutputQueue = m.getMessageOutputQueue();
      if(isInitialized())
      {	mainPanel.init();
      }
  }

  /**
   * Bind the plugin's with the action interface.
   * @param a the action interface which is used to mediate and receive actions
   */
  public void bindActionInterface(ActionInterface a) {
      a.setActionListener(new EventSimPlugin.EventSimPluginActionListener(),
    		  getPluginUID(), ActionListener.PLUGIN);
      this.actionEventQueue = a.getActionEventQueue();
      if(isInitialized())
      {	mainPanel.init();
      }
  }

  public void bindCore(CoreController controller) {
  configurator = controller.getEpocIniConfigurator();
  if(isInitialized())
  {	mainPanel.init();
  }
}

  /**
   * Notifies the core that a plugin model is changed
   */
  public void pluginModelChangedAction() {
      actionEventQueue.put(new Action(
          Action.UTILITIES_PLUGIN_MODEL_CHANGED,
          getPluginUID(), Action.CORE, new int[0]));
  }
  /**
   * The fn tells if the plugin is actually usable
   * (by it's own views, for instance)
   */
  private boolean isInitialized()
  {
	  return 	configurator != null &&
	  			messageOutputQueue != null &&
	  			actionEventQueue != null;
  }
  
}