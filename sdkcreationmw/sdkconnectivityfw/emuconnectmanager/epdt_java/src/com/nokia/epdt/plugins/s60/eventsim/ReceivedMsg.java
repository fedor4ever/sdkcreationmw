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

import java.io.UnsupportedEncodingException;
import java.text.MessageFormat;
import java.text.ParseException;
import java.util.StringTokenizer;
import java.util.Vector;

import com.nokia.epdt.core.messaging.Message;
import com.nokia.epdt.plugins.s60.eventsim.Constants;


public class ReceivedMsg
{
	private static MessageFormat messageFormat = new MessageFormat("28 {0} WINS_DRIVES {1}");
    private static String charsetName = "UTF8";

	public static final String MMC_REMOVED = "REMOVED";
	public static final String MMC_INSERTED = "INSERTED";
	public static final String MMC_CORRUPTED = "CORRUPTED";
	public static final String MMC_UNKNOWN = "UNKNOWN";
	 
    private String mmcState = MMC_UNKNOWN;
    //The following field mmcDrives is not used
    //and left for historical reasons
    //who knows, history can repeat in the future
    private Vector mmcWinsDrives = new Vector();
	/**
     * Parse message from communication format to EventSimMessage object
     *
     * @param message Message from ECMT (EventSimPlugin)
     * @return EPDT (EventSimPlugin) formated message in EventSimMessage instance
     * @see EventSimMessage
     */
    public ReceivedMsg(Message message) {

    	try {
    		String body = new String(message.getBody(), charsetName);
              Object[] o = null;
              try {
                o = messageFormat.parse(body);
                String state = (String) o[0];
                if (state.compareTo(Constants.MMC_REMOVE_EVENTSIM_MESSAGE) == 0) {
            		  mmcState = MMC_REMOVED;
            	  }
            	  else if (state.compareTo(Constants.MMC_INSERT_EVENTSIM_MESSAGE) == 0) {
            		  mmcState = MMC_INSERTED;
            	  }
            	  else if (state.compareTo(Constants.MMC_CORRUPTED_EVENTSIM_MESSAGE) == 0) {
            		  mmcState = MMC_CORRUPTED;
            	  }
                //mmcDrives is left for historical reasons
                if (o[1] != null) {
                  String mmcDrives = (String) o[1];
                	mmcWinsDrives = parseStringToVector(mmcDrives);
                }
              }
              catch (ParseException pe) {
                // parsing failed
              }

            }
            catch (UnsupportedEncodingException uee) {

            }
          }


    /**
     * Parse string to vector
     * @param s
     * @return Vector
     */
    private static Vector parseStringToVector(String s) {
    	Vector activeCalls = new Vector();

        for ( StringTokenizer st = new  StringTokenizer(s, " "); st.hasMoreTokens(); ) {
        	activeCalls.add(st.nextToken());
        }
    	return activeCalls;
    }

    public String getMmcState()
	{
		return mmcState;
	}


}
