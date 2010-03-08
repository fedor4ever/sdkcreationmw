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

import com.nokia.epdt.core.messaging.Message;


public class OutgoingMsg
{
	  public static final int BATTERYLOW_ID = 0;
	  public static final int BATTERYEMPTY_ID = 1;
	  public static final int GRIPOPEN_ID = 2;
	  public static final int GRIPCLOSED_ID = 3;
	  public static final int CAMERALENSOPEN_ID = 4;
	  public static final int CAMERALENSCLOSED_ID = 5;
	  public static final int ALARMNOTIFICATION_ID = 6;
	  public static final int CALENDARNOTIFICATION_ID = 7;
	  public static final int MEMORYNOTIFICATION_ID = 8;
	  public static final int DISKSPACENOTIFICATION_ID = 9;
	  public static final int SIMREMOVED_ID = 10;
	  public static final int SIMCHANGED_ID = 11;
	  public static final int SIMLOCKED_ID = 12;
	  public static final int SIMSMSMEMORYFULL_ID = 13;
	  public static final int HEADSETCONNECTED_ID = 14;
	  public static final int HEADSETREMOVED_ID = 15;
	  public static final int TTYCONNECTED_ID = 16;
	  public static final int TTYREMOVED_ID = 17;
	  public static final int LOOPSETCONNECTED_ID = 18;
	  public static final int LOOPSETREMOVED_ID = 19;
	  public static final int HANDSFREEMODEON_ID = 20;
	  public static final int HANDSFREEMODEOFF_ID = 21;
	  public static final int INCOMINGVOICECALL_ID = 22;
	  public static final int INCOMINGDATACALL_ID = 23;
	  public static final int INCOMINGFAXCALL_ID = 24;
	  public static final int REMOTEHANGUP_ID = 25;
	  public static final int CONNECTINGEXTERNALPOWERCHARGER_ID = 26;
	  public static final int REMOVINGEXTERNALPOWERCHARGER_ID = 27;
	  public static final int MMC_ID = 28;
	  public static final int BATTERYFULL_ID = 29;
	  public static final int SMS_MESSAGE_ID = 30;
	  public static final int MMS_MESSAGE_ID = 31;

	  private static final String BLANK = " ";
	  
	  private int event_id = -1;
	  private String description;
	  private EventSimPlugin plugin;
	  
	  public OutgoingMsg(int _id, String _description, EventSimPlugin _plugin)
	  {
		event_id = _id;
		description = _description;
		plugin = _plugin;
	  }
	/**
	   * Creates a new instance of Message for spesified eventId and message
	   * @param uid UID of the plugin
	   * @param eventId id of the event, use one of the following EventSimConstant value:

	   *
	   * @param message event specific message parameters, see the documentation of the event.
	   * @return Returns the EPDT <-> ECMT (Eventsim plugin) compliant message.
	   */
	  public void sendMessage()
	  {
		    String messageText = String.valueOf(event_id) + BLANK + description;
		    Message message = new Message(Constants.TARGET_PLUGIN_UID , messageText.getBytes());
			plugin.getMessageOutputQueue().put(message);
	  }
	  
}
