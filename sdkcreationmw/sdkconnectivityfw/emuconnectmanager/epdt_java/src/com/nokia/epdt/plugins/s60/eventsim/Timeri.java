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

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.Timer;
import java.util.ArrayList;
/**
 * Class Timeri
 *
 *	Implements limited javax.swing.Timer functionality
 *	with possibility to modify ActionCommand
 *
 *	in Java 1.6 the functionality is implemented
 *	and this code is to be thrown away
 */
public class Timeri implements ActionListener
{
	private Timer timer;
	private String actionCommand="";
	private ArrayList listeners = new ArrayList();
	
	public Timeri(int _delay, ActionListener listener)
	{	super();
	 	timer = new Timer(_delay,this);
	 	timer.setRepeats(false);
	 	timer.setCoalesce(false);
	 	listeners.add(listener);
	}

	public void start()
	{ 	timer.start();
	}
	
	public void addListener(ActionListener listener)
	{ listeners.add(listener);	}
	
	public void setActionCommand(String _actionCommand)
	{	actionCommand = _actionCommand; 	}
	public void setDelay(int _delay)
	{ timer.setInitialDelay(_delay);}
	
	public void actionPerformed(ActionEvent origEvent)
	{	
		for(int i=0;i<listeners.size(); i++)
		{
			ActionEvent eventToSend = new ActionEvent(this,origEvent.getID(), actionCommand);
			((ActionListener)listeners.get(i)).actionPerformed(eventToSend);
		}
	}

}
