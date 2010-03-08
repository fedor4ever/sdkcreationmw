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
import javax.swing.JRadioButton;
import javax.swing.JLabel;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.BoxLayout;
import javax.swing.SpringLayout;
import javax.swing.BorderFactory;

import javax.swing.Box;

import com.nokia.epdt.core.config.epocini.EpocIniConstants;
import com.nokia.epdt.plugins.s60.eventsim.Constants;
import com.nokia.util.gui.SpringUtilities;

import java.awt.Dimension;
import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import java.io.File;

public class MmcPanel extends JPanel
{	
	 public class MmcPanelListener implements ActionListener
	{
		public void actionPerformed(ActionEvent event)
		{
			if( event.getActionCommand().equals(MMC_VIRTUAL_RADIO) )
			{ 	useVirtual();
				plugin.pluginModelChangedAction();
			}
			else if(event.getActionCommand().equals(MMC_DRIVE_RADIO) )
			{ 	useMmc();
				plugin.pluginModelChangedAction();
			}
			else if(event.getActionCommand().equals(MMC_INSERT_BUTTON) )
			{	EventSendInsertMmc();
			}
			else if(event.getActionCommand().equals(MMC_REMOVE_BUTTON) )
			{	EventSendRemoveMmc();
			}
			else if(event.getActionCommand().equals(DRIVE_COMBO) )
			{
				plugin.pluginModelChangedAction();
			}
		}
	}

	static final String MMC_PANEL_TITLE = "Multimedia Card (MMC)";
	 static final String MMC_VIRTUAL_RADIO = "Use Virtual MMC 1024KB";
	 static final String MMC_STATUS_LABEL = "MMC status:"; //MMC Status
	 static final String SEND_EVENT = "Send event:";
	 static final String MMC_INSERT_BUTTON = "Insert MMC";//insert and remove buttons
	 static final String MMC_REMOVE_BUTTON = "Remove MMC";
	 static final String MMC_DRIVE_RADIO = "Use local drive as a memory card";//removable drive radio title
	 static final String MMC_SELECT_DRIVE_LABEL = "Select drive:";
	 static final String EPOC_DRIVE_E = "_EPOC_DRIVE_E";
	 private static final String DRIVE_COMBO = "driveCombo";
	 static int GAP = 7;
	 static int TAB = 30;
	
	private MmcPanelListener uiListener = new MmcPanelListener(); 
	private EventSimPlugin plugin;
	private JRadioButton virtualMmcRadio = new JRadioButton(MMC_VIRTUAL_RADIO);
	private JRadioButton driveMmcRadio = new JRadioButton(MMC_DRIVE_RADIO);
	private JLabel mmcStatus = new JLabel("Unknown");
	private JButton insertMmc = new JButton(MMC_INSERT_BUTTON);
	private JButton removeMmc = new JButton(MMC_REMOVE_BUTTON);
	private JComboBox driveCombo;
	
	public	MmcPanel(EventSimPlugin plugin)
	{	super();
		this.plugin=plugin;
	}
/**
 * the fn is supposed to be called by the plugin class
 * when it's ready with its data members: configurator, 
 * message and action interfaces, might be others.
 */	
	public void init()
	{	//Get all available file system roots (= drives)
		//initialize drive combo with the list obtained
		File[] roots = File.listRoots();
		String[] drives = new String[roots.length];
		for(int i=0; i< roots.length; i++)
		{
			String absPath = roots[i].getAbsolutePath();
			if(absPath.length()>=2 && absPath.indexOf(':')== 1)
			{	drives[i]=absPath.substring(0, 2);//Take the first two chars
			}
			else
			{	continue;
			}
		}
		
		driveCombo = new JComboBox(drives);
		//good way to set combobox width:
		driveCombo.setPrototypeDisplayValue("AAAA");
		
		String drive = plugin.getConfigurator().getProperty(EPOC_DRIVE_E);
		
		if(drive == null || "".equals(drive ) )
		{ useVirtual();
		}
		else
		{
			useMmc();
			//get the drive letter from Epoc configurator
			//and set it in Combo
			for(int i=0; i< driveCombo.getModel().getSize(); i++)
			{
				if(drive.equals((String)driveCombo.getModel().getElementAt(i) ) )
				{
					driveCombo.setSelectedIndex(i);
				}
			}
		}
		initLayout();
		initUiComponents();
	}
	
	private void initUiComponents()
	{
		virtualMmcRadio.setActionCommand(MMC_VIRTUAL_RADIO);
		virtualMmcRadio.addActionListener(uiListener);
		driveMmcRadio.setActionCommand(MMC_DRIVE_RADIO);
		driveMmcRadio.addActionListener(uiListener);
		insertMmc.setActionCommand(MMC_INSERT_BUTTON);
		insertMmc.addActionListener(uiListener);
		removeMmc.setActionCommand(MMC_REMOVE_BUTTON);
		removeMmc.addActionListener(uiListener);
		driveCombo.setActionCommand(DRIVE_COMBO);
		driveCombo.addActionListener(uiListener);
	}
	
	private void initLayout(){
		setLayout(new BoxLayout(this, BoxLayout.PAGE_AXIS));
		this.setBorder(BorderFactory.createEmptyBorder(TAB, TAB, TAB, TAB));
		
		add(virtualMmcRadio);
		virtualMmcRadio.setAlignmentX(Component.LEFT_ALIGNMENT);
		
		JPanel virtualMmcPanel = new JPanel();
		add(virtualMmcPanel);
		virtualMmcPanel.setAlignmentX(Component.LEFT_ALIGNMENT);
		virtualMmcPanel.setLayout(new SpringLayout());
		virtualMmcPanel.setMaximumSize(new Dimension(300, 100));
		virtualMmcPanel.add(new JLabel(MMC_STATUS_LABEL));
		virtualMmcPanel.add(mmcStatus);
		virtualMmcPanel.add(new JLabel(SEND_EVENT));
		virtualMmcPanel.add(insertMmc);
		virtualMmcPanel.add(Box.createRigidArea(new Dimension(5, 5)));
		virtualMmcPanel.add(removeMmc);
		SpringUtilities.makeCompactGrid(virtualMmcPanel, 
										3, 2,//rows cols
										TAB, GAP,//left and top gaps
										GAP, GAP);//gaps between elements

		add(driveMmcRadio);
		driveMmcRadio.setAlignmentX(Component.LEFT_ALIGNMENT);
		
		JPanel drivePanel = new JPanel();
		drivePanel.setMaximumSize(new Dimension(300, 30));
		drivePanel.setAlignmentX(Component.LEFT_ALIGNMENT);
		drivePanel.setLayout(new SpringLayout());
		add(drivePanel);
		drivePanel.add( new JLabel(MMC_SELECT_DRIVE_LABEL) );
		drivePanel.add(driveCombo);
		driveCombo.setMaximumSize(insertMmc.getSize());
		SpringUtilities.makeCompactGrid(drivePanel, 
				1, 2,//rows cols
				TAB, GAP,//left and top margins
				GAP, GAP);//gaps between elements
	}
	private void useVirtual()
	{
		virtualMmcRadio.setSelected(true);
		driveMmcRadio.setSelected(false);
		insertMmc.setEnabled(true);
		removeMmc.setEnabled(true);
		driveCombo.setEnabled(false);
	}
	
	private void useMmc()
	{
		virtualMmcRadio.setSelected(false);
		driveMmcRadio.setSelected(true);
		insertMmc.setEnabled(false);
		removeMmc.setEnabled(false);
		driveCombo.setEnabled(true);
	}
	
	  /**
	   *
	   */
	  public void EventSendRemoveMmc() {
		  new OutgoingMsg(	OutgoingMsg.MMC_ID,
		  			Constants.MMC_REMOVE_EVENTSIM_MESSAGE,
		  			plugin).sendMessage();
	  }

	  /**
	   *
	   */
	  public void EventSendInsertMmc() {
		  new OutgoingMsg(	OutgoingMsg.MMC_ID,
				  			Constants.MMC_INSERT_EVENTSIM_MESSAGE,
				  			plugin).sendMessage();
	  }
	  
	  public void applyChanges()
	  {
		  if(virtualMmcRadio.isSelected())
		  {
		      // using EpocIniConfigurator to modify epoc.ini files
			  plugin.getConfigurator().setProperty(EPOC_DRIVE_E, "");
			  plugin.getConfigurator().saveSettings();
		  }
		  else
		  {
		      // using EpocIniConfigurator to modify epoc.ini files
			  plugin.getConfigurator().setProperty(EPOC_DRIVE_E,
					  (String)driveCombo.getSelectedItem());
			  plugin.getConfigurator().saveSettings();
		  }
	  }
	  
	  public void msgReceived(ReceivedMsg msg)
	  {
		  mmcStatus.setText(msg.getMmcState());
	  }
	
}
