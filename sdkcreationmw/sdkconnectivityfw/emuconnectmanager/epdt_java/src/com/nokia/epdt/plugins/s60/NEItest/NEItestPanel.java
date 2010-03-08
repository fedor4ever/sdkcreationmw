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

package com.nokia.epdt.plugins.s60.NEItest;

import java.awt.LayoutManager;

import javax.swing.JPanel;
import javax.swing.JButton;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;



/**
 * Class NEItestPanel
 *
 * 
 * 
 */
public class NEItestPanel extends JPanel implements ActionListener{
    

    JButton sendButton = new JButton("Start Debug Agent");
    {
        sendButton.setActionCommand( "start");
        sendButton.addActionListener(this);
    
    }
    
    JButton killButton = new JButton("Kill Debug Agent");
    {
        killButton.setActionCommand( "kill");
        killButton.addActionListener(this);
    
    }
    
    NEIMessageHandler neiMessageHandler;
    /**
     * 
     */
    public NEItestPanel() {
        super();
        
        this.add(sendButton);
        this.add(killButton);
    }

    /**
     * @param isDoubleBuffered
     */
    public NEItestPanel(boolean isDoubleBuffered) {
        super(isDoubleBuffered);
        // TODO Auto-generated constructor stub
    }

    /**
     * @param layout
     */
    public NEItestPanel(LayoutManager layout) {
        super(layout);
        // TODO Auto-generated constructor stub
    }

    /**
     * @param layout
     * @param isDoubleBuffered
     */
    public NEItestPanel(LayoutManager layout, boolean isDoubleBuffered) {
        super(layout, isDoubleBuffered);
        // TODO Auto-generated constructor stub
    }
    
    
    public void actionPerformed(ActionEvent event)
    {
        if("start".equals(event.getActionCommand()))
        {
             neiMessageHandler.startTst();           

        
        }else if("kill".equals(event.getActionCommand()))
        {
              neiMessageHandler.killDebugAgent();           

            
        }
    }

    public void setNeiMessageHandler(NEIMessageHandler neiMessageHandler) {
        this.neiMessageHandler = neiMessageHandler;
    }
}
