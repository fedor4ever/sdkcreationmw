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



/**
 * EventSimConstants, constants for the EventSimPlugin
 *
 * @version 2.0
 */
public class Constants
{
	public static final int TARGET_PLUGIN_UID = 0x101FD983;
	 public static final String ACTIVECALLS_TYPE_EVENTSIM_MESSAGE =
	      "ACTIVECALLS";
	  public static final String MMC_TYPE_EVENTSIM_MESSAGE = "MMC";
	 // public static final String MMC_EDIT_EVENTSIM_MESSAGE = "ED";
	  public static final String MMC_SET_EVENTSIM_MESSAGE = "SET";
	  public static final String MMC_WHAT_EVENTSIM_MESSAGE = "WHAT";
	  public static final String MMC_VIRTUAL_EVENTSIM_MESSAGE = "VIRTUAL";
	  public static final String MMC_NONE_EVENTSIM_MESSAGE = "NOT_SET";
	  public static final String MMC_WINS_DRIVES_EVENTSIM_MESSAGE =
	      "WINS_DRIVES";
	  public static final String MMC_REMOVE_EVENTSIM_MESSAGE = "REMOVE";
	  public static final String MMC_INSERT_EVENTSIM_MESSAGE = "INSERT";
	  public static final String MMC_CORRUPTED_EVENTSIM_MESSAGE = "CORRUPTED";

	  public static final String OK_TYPE_EVENTSIM_MESSAGE = "OK";
	  public static final String ERROR_TYPE_EVENTSIM_MESSAGE = "ERROR";
  
  public static final String EMULATOR_NEEDS_TO_RESTART = "The emulator must be restarted in order to activate the MMC emulation type change.";
  public static final String EVENTSIM_MESSAGE_TITLE = "Event Simulation";


  //Messaging constants
  public static final String SMS_TARGET_FILE_NAME = "message.sms";
  public static final String MMS_TARGET_FILE_NAME = "message.mms";
  
  public static final String _EPOC_DRIVE_E = "_EPOC_DRIVE_E";
  public static final String _EPOC_LocDrv_1 = "_EPOC_LocDrv_1";
  /**
   * Creates a new instance of EventSimConstants
   */
  private Constants() {
    super();
  }

}