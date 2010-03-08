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

import javax.swing.JPanel;
import javax.swing.JLabel;
import javax.swing.JButton;
import javax.swing.SpringLayout;
import java.awt.Color;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import java.io.File;
import java.io.InputStream;
import java.io.FileOutputStream;

import com.nokia.epdt.core.CoreUtils;
import com.nokia.util.gui.SpringUtilities;

public class MsgPanel extends JPanel
{	
	 public static final String MSG_PANEL_TITLE = "Messaging";
	 private static final String SMS_MESSAGE_LABEL = "SMS message";
	 private static final String MMS_MESSAGE_LABEL = "MMS message";
	 private static final String SEND_SMS_COMMAND = "SEND SMS";
	 private static final String SEND_MMS_COMMAND = "SEND MMS";
	 private static final String SEND_LABEL = "Send";
	 private static int GAP = 7;
	 private static int TAB = 30;
	
	public class MsgPanelListener implements ActionListener
	{
		InputStream msgIs;
		String outgoingMsg;
		public void actionPerformed(ActionEvent event)
		{
			if(SEND_SMS_COMMAND.equals(event.getActionCommand()))
			{
				msgIs = this.getClass().getResourceAsStream("message.sms");
				outgoingMsg = CoreUtils.EpocRoot()+ "epoc32/WINSCW/C/smsin/message.sms";
			}
			else if(SEND_MMS_COMMAND.equals(event.getActionCommand()))
			{
				msgIs = this.getClass().getResourceAsStream("message.mms");
				outgoingMsg = CoreUtils.EpocRoot()+ "epoc32/WINSCW/C/mmsin/message.mms";
			}
			else
			{
				return;
			}
			copyFile(msgIs, outgoingMsg);
		}
	}


	private MsgPanelListener uiListener = new MsgPanelListener(); 
	private EventSimPlugin plugin;
	private JLabel smsL = new JLabel(SMS_MESSAGE_LABEL);
	private JLabel mmsL = new JLabel(MMS_MESSAGE_LABEL);
	private JButton sendSmsB = new JButton(SEND_LABEL);
	private JButton sendMmsB = new JButton(SEND_LABEL);
	
	
	public	MsgPanel(EventSimPlugin plugin)
	{	super();
		this.plugin=plugin;
	}
/**
 * the fn is supposed to be called by the plugin class
 * when it's ready with its data members: configurator, 
 * message and action interfaces, might be others.
 */	
	public void init()
	{	
		initLayout();
		initUiComponents();
	}
	
	private void initUiComponents()
	{
		sendSmsB.setActionCommand(SEND_SMS_COMMAND);
		sendSmsB.addActionListener(uiListener);
		
		sendMmsB.setActionCommand(SEND_MMS_COMMAND);
		sendMmsB.addActionListener(uiListener);
	}
	
	private void initLayout(){
		setLayout(new FlowLayout(FlowLayout.LEFT));
		
		JPanel mainPanel = new JPanel();
		//mainPanel.setBackground(Color.GREEN);
		add(mainPanel);
		mainPanel.setLayout(new SpringLayout());

		mainPanel.add(smsL);
		mainPanel.add(sendSmsB);
		mainPanel.add(mmsL);
		mainPanel.add(sendMmsB);
		
		SpringUtilities.makeCompactGrid(mainPanel, 
										2, 2,//rows cols
										TAB, TAB,//left and top gaps
										TAB, 2*GAP);//gaps between elements
		
	}
	
	private void copyFile(InputStream fis, String destName )
	{
			try
			{
			   //FileInputStream fis = new FileInputStream(new File(srcUri));
			   FileOutputStream fos = new FileOutputStream(new File(destName));
			 
			   byte[] buf = new byte[1024];
			   int i = 0;
			   while((i=fis.read(buf))!=-1)
			   {
			      fos.write(buf, 0, i);
			   }
			   fis.close();
			   fos.close();
			}
			catch (Throwable t)
			{
			   t.printStackTrace();
			}
	}
}
