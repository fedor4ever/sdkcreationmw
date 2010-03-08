/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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

import java.awt.Color;//needed for test code
import java.awt.Component;
import java.awt.Dimension;
import java.io.InputStream;
import org.apache.log4j.Logger;
import org.apache.log4j.Level;
import org.xml.sax.helpers.DefaultHandler;
import org.xml.sax.Attributes;
import org.xml.sax.SAXException;

import com.nokia.epdt.core.messaging.Message;
import com.nokia.util.gui.SpringUtilities;

import javax.xml.parsers.SAXParserFactory;
import javax.xml.parsers.SAXParser;
import javax.swing.JPanel;
import javax.swing.SpringLayout;
import javax.swing.JLabel;
import javax.swing.JButton;
import javax.swing.JFormattedTextField;
import javax.swing.text.NumberFormatter;
import javax.swing.Box;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.HashMap;

import javax.swing.ImageIcon;
/**
 * 
 * Class BasicEventsHandler
 * * parses XML file with the list of events
 * * creates UI for event sending
 * * handles event sending to ECMT server part
 * 
 *
 */
public class BasicEventsHandler
{
	private static String xmlResourceName = "/com/nokia/epdt/plugins/s60/eventsim/events.xml";
	//Constants for XML file definitions
	private static String XML_EVENT = "event";
	private static String XML_GROUP = "eventgroup";
	private static String XML_NAME = "name";
	private static String XML_ID = "id";
	private static String XML_DELAY = "delay";
	private static String SEND_BUTTON_TEXT = "Send";
	private static String DELAY_TEXTBOX_HASH_ENTRY = "DELAY_TEXTBOX_HASH_ENTRY";
	private static String ICON_HASH_ENTRY = "ICON_HASH_ENTRY";
	private static String TIME_NOTE = "Time is up!";
	
	private static int TAB = 20;
	private static int GAP = 5;
	
	
	private Logger log = Logger.getLogger(BasicEventsHandler.class);
	{
	log.setLevel(Level.DEBUG);
	}
	private EventSimPlugin plugin;
	private BasicEventsListener eventsListener = new BasicEventsListener();
	//hash containing delay edit boxes (to read on send key presses)
	HashMap delaysHash = new HashMap();
	/**
	 * delaysHash contains other hashes identified by ID of the event:
	 * 
	 * { <eventID>, {
	 * 				{DELAY_TEXTBOX_HASH_ENTRY, JFormattedTextField delayEditBox},
	 * 				{ICON_HASH_ENTRY, JLabel delayIcon}
	 * 			  }
	 * },
	 * { <eventID>, {
	 * 				{DELAY_TEXTBOX_HASH_ENTRY, JFormattedTextField delayEditBox},
	 * 				{ICON_HASH_ENTRY, JLabel delayIcon}
	 * 			  }
	 * }
	 */ 
	
	/**
	 * Class xmlParserHandler
	 * internal class for XML parsing
	 * creates UI
	 *
	 */
	public class xmlParserHandler extends DefaultHandler
	{
		//main panel with buttons for every group of events
		//need to store globally to be available from XML parsing func-s
		private JPanel tabPanel;
		//number of lines in buttonsPanel
		private int eventsNumber;

		public void startElement(String namespaceURI,
								String simpleNameNotUsed,
								String qualifiedName,
								Attributes attrs)
		throws SAXException
		{
			if(XML_GROUP.equals(qualifiedName))
			{	//start of the events group
				
				//every group of events corresponds to own tab (sub tab of a plugin)
				JPanel eventsGroupTab = new JPanel();
				eventsGroupTab.setLayout(new FlowLayout(FlowLayout.LEFT));
				MainPanel pluginMainPanel = ((MainPanel)plugin.getJPanelView());
				pluginMainPanel.getMainPane().
					addTab( attrs.getValue(XML_NAME),eventsGroupTab);

				//main panel of the group's tab
				//we need it to have constant definite size
				tabPanel = new JPanel();
				//tabPanel.setBackground(Color.BLUE );
				tabPanel.setAlignmentX(Component.LEFT_ALIGNMENT);
				tabPanel.setLayout(new SpringLayout());
				eventsGroupTab.add(tabPanel);

			}
			else if(XML_EVENT.equals(qualifiedName) && tabPanel != null)
			{//found new event in XML file
				
				log.debug("adding basic event "+qualifiedName+" "+
							" name="+ attrs.getValue(XML_NAME)+
							" id="+ attrs.getValue(XML_ID));
				
				eventsNumber++;
				JLabel eventLabel=new JLabel(attrs.getValue(XML_NAME));
				JButton sendButton=new JButton(SEND_BUTTON_TEXT);
				sendButton.setActionCommand(attrs.getValue(XML_ID));
				sendButton.addActionListener(eventsListener);
				sendButton.setAlignmentX(Component.LEFT_ALIGNMENT);
				//a panel for Send button for it not to be resized by some
				//great layout managers ignoring preferred sizes
				JPanel sendButtonPanel = new JPanel();
				sendButtonPanel.setLayout(new FlowLayout(FlowLayout.LEFT));
				sendButtonPanel.add(sendButton);
				sendButtonPanel.setAlignmentX(Component.LEFT_ALIGNMENT);
				//sendButtonPanel.setBackground(Color.ORANGE);
				tabPanel.add(eventLabel);

				//if there is no delay - just add the button for sending
				//if there is - create a panel, add there the send button
				//and edit box for delay with the label
				if(attrs.getValue(XML_DELAY) == null)
				{
					tabPanel.add(sendButtonPanel);
				}
				else
				{
					NumberFormatter numberFormatter = new NumberFormatter();
					numberFormatter.setMinimum(new Integer(0));
					JFormattedTextField delayEditBox = new JFormattedTextField(numberFormatter);
					delayEditBox.setValue(new Integer(0));
					delayEditBox.setColumns(4);

					java.net.URL imgURL = BasicEventsHandler.class.getResource("progress.gif");
					ImageIcon delayIcon = new ImageIcon(imgURL);
					JLabel delayIconLabel = new JLabel(delayIcon);
					delayIconLabel.setDisabledIcon(
							new ImageIcon(BasicEventsHandler.class.getResource("progress_static.png")));
					delayIconLabel.setEnabled(false);

					HashMap delayIconPair = new HashMap(2);
					delayIconPair.put(DELAY_TEXTBOX_HASH_ENTRY, delayEditBox);
					delayIconPair.put(ICON_HASH_ENTRY, delayIconLabel);
					
					delaysHash.put(attrs.getValue(XML_ID), delayIconPair );
					
					JLabel delayLabel = new JLabel(attrs.getValue(XML_DELAY));
					
					JPanel delayPanel = new JPanel();
					//delayPanel.setBackground(Color.GREEN);
					FlowLayout flowLayout = new FlowLayout(FlowLayout.LEFT);
					flowLayout.setHgap(0);
					delayPanel.setLayout(flowLayout);
					delayPanel.add(sendButtonPanel);
					delayPanel.add(Box.createRigidArea(new Dimension(GAP*2, GAP)));
					delayPanel.add(delayLabel);
					delayPanel.add(Box.createRigidArea(new Dimension(GAP*2, GAP)));
					delayPanel.add(delayEditBox);
					delayPanel.add(Box.createRigidArea(new Dimension(GAP*2, GAP)));
					delayPanel.add(delayIconLabel);					
					
					
					tabPanel.add(delayPanel);
					
				}
			}
		}
		
		public void endElement(String namespaceURI,
				String simpleNameNotUsed,
				String qualifiedName)
			throws SAXException
		{
			if( (XML_GROUP).equals(qualifiedName) && tabPanel != null )
			{
				SpringUtilities.makeCompactGrid(tabPanel, 
						eventsNumber, 2,//rows cols
						TAB*2, TAB,//left and top margins
						TAB, 0);//gaps between elements
				eventsNumber = 0;
				tabPanel = null;
			}
		}
	}
	 public class BasicEventsListener implements ActionListener
	{
		 //private static final String TIMER_ACTION = "timer:";
		 Timeri timer = new Timeri(0, this);
			public void actionPerformed(ActionEvent event)
			{
				//check if there is delay edit box&icon for the event:
				//if yes - init them
				HashMap editBoxIconPair = (HashMap)delaysHash.
										get(event.getActionCommand());
				JFormattedTextField textField = null;
				JLabel delayIconLabel = null;
				if(editBoxIconPair != null)
				{
					textField =
					(JFormattedTextField)editBoxIconPair.get(DELAY_TEXTBOX_HASH_ENTRY); 
					delayIconLabel = 	
					(JLabel)editBoxIconPair.get(ICON_HASH_ENTRY);
				}
				//There are two action types:
				// a. From buttons
				// b. From timer				
				if(event.getSource() instanceof JButton )
				{
						int delay = 0;
						String delayString = "";
						//if there is delay edit box with value>0
						//yes - set timer and send message
						//no - just send message
						if(textField !=null && delayIconLabel != null &&
						(delay = Integer.parseInt(textField.getText())) != 0)
						{	//want to put edit box content to int and
							//back to string to remove spaces, other
							//possible garbage
							delayString = String.valueOf(delay);
							//"start" delay progress GIF icon
							delayIconLabel.setEnabled(true);
							//start timer and exit
							//in a <delay> time it will notify us and we'll send msg
							//delay is set in milliseconds
							//field entry is set in min
							timer.setDelay(delay*1000*60);
							timer.setActionCommand(event.getActionCommand());
							timer.start();
						}
						//send message to Ecmt Server
						//action command should contain event ID for ECMT server
						Message message = new Message(Constants.TARGET_PLUGIN_UID ,
						 (event.getActionCommand()+" "+delayString).getBytes());
						plugin.getMessageOutputQueue().put(message);
						return;
				}
				if(delayIconLabel != null)
				{	//"stop" delay progress GIF icon
					delayIconLabel.setEnabled(false);
				}
			}
	}
	public BasicEventsHandler(EventSimPlugin _plugin)
	{
		super();
		plugin=_plugin;
	}

	public void init()
	{
		SAXParserFactory factory = SAXParserFactory.newInstance();
		xmlParserHandler parserHandler = new xmlParserHandler();
		try
		{
			SAXParser parser = factory.newSAXParser();  
			InputStream is = this.getClass().getResourceAsStream(xmlResourceName);
			parser.parse(is, parserHandler);
		}
		catch(Throwable e)
		{
			log.debug("Error parsing XML initialization file:\n" + e.getMessage());
		}
	}
}
