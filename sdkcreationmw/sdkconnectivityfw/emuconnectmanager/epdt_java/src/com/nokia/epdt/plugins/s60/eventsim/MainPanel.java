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

import java.awt.LayoutManager;
import javax.swing.JPanel;
import javax.swing.JTabbedPane;
import java.awt.BorderLayout;

public class MainPanel extends JPanel
{	
	private EventSimPlugin eventSimPlugin;
	JTabbedPane mainPane;
	MmcPanel mmcPanel;
	BasicEventsHandler basicEventsPanel;
	MsgPanel msgPanel;
	
	public MainPanel(EventSimPlugin plugin)
	{	super();
		eventSimPlugin = plugin;
		
		mainPane = new JTabbedPane();
		mmcPanel = new MmcPanel(plugin);
		basicEventsPanel = new BasicEventsHandler(plugin);
		msgPanel = new MsgPanel(plugin);

		this.setLayout(new BorderLayout());
		this.add(mainPane);
		
		mainPane.setTabLayoutPolicy(JTabbedPane.SCROLL_TAB_LAYOUT);
	}
	
	public void init()
	{
		mmcPanel.init();
		mainPane.addTab(MmcPanel.MMC_PANEL_TITLE, mmcPanel);
		basicEventsPanel.init();
		msgPanel.init();
		mainPane.addTab(MsgPanel.MSG_PANEL_TITLE, msgPanel);
		
	}
	public void applyChanges()
	{
		mmcPanel.applyChanges();
	}
	public void msgReceived(ReceivedMsg msg)
	{
		mmcPanel.msgReceived(msg);
	}

	public JTabbedPane getMainPane()
	{
		return mainPane;
	}

}
