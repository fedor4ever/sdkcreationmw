/*
* Copyright (c) 1999 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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

package com.symbian.tools.j2me.sei.emulator.event;

import java.io.*;
import java.util.*;

import com.symbian.tools.j2me.sei.emulator.*;
import com.symbian.tools.j2me.sei.preferences.*;
import junit.framework.*;

/**
 * This unit test tests only the correct events sequence.
 * Not Use-Cases or functionality.
 */
public class TestEmulatorLauncherListener extends TestCase implements EmulatorLauncherListener{
  private static EmulatorLauncher sLauncher = new EmulatorLauncherStub();

  private static final EmulatorLauncherEvent[] EXPECTED_LAUNCH_EVENTS_TABLE
      = new EmulatorLauncherEvent[]
      {
      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.EXIT_STATE, EmulatorLauncher.INIT_MODE, 0),
      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.ENTER_STATE, EmulatorLauncher.INIT_MODE, EmulatorLauncher.CREATE_SEI_COMMAND),

      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.MODE_CHANGE, EmulatorLauncher.LAUNCH_MODE, EmulatorLauncher.CREATE_SEI_COMMAND),

      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.EXIT_STATE, EmulatorLauncher.LAUNCH_MODE, EmulatorLauncher.CREATE_SEI_COMMAND),
      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.ENTER_STATE, EmulatorLauncher.LAUNCH_MODE, EmulatorLauncher.CREATE_VM_LAUNCH_ARGS),

      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.EXIT_STATE, EmulatorLauncher.LAUNCH_MODE, EmulatorLauncher.CREATE_VM_LAUNCH_ARGS),
      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.ENTER_STATE, EmulatorLauncher.LAUNCH_MODE, EmulatorLauncher.READ_USER_PREFERENCES),

      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.EXIT_STATE, EmulatorLauncher.LAUNCH_MODE, EmulatorLauncher.READ_USER_PREFERENCES),
      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.ENTER_STATE, EmulatorLauncher.LAUNCH_MODE, EmulatorLauncher.INIT_JAD),

      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.EXIT_STATE, EmulatorLauncher.LAUNCH_MODE, EmulatorLauncher.INIT_JAD),
      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.ENTER_STATE, EmulatorLauncher.LAUNCH_MODE, EmulatorLauncher.RESOLVE_APPLICATION),

      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.EXIT_STATE, EmulatorLauncher.LAUNCH_MODE, EmulatorLauncher.RESOLVE_APPLICATION),
      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.ENTER_STATE, EmulatorLauncher.LAUNCH_MODE, EmulatorLauncher.INIT_JAR),

      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.SKIPPED_STATE, EmulatorLauncher.LAUNCH_MODE, EmulatorLauncher.ADD_JAR_SIGNATURE),

      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.EXIT_STATE, EmulatorLauncher.LAUNCH_MODE, EmulatorLauncher.INIT_JAR),
      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.ENTER_STATE, EmulatorLauncher.LAUNCH_MODE, EmulatorLauncher.INIT_SESSION),

      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.EXIT_STATE, EmulatorLauncher.LAUNCH_MODE, EmulatorLauncher.INIT_SESSION),
      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.ENTER_STATE, EmulatorLauncher.LAUNCH_MODE, EmulatorLauncher.ASSIGN_SESSION_PORTS),

      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.EXIT_STATE, EmulatorLauncher.LAUNCH_MODE, EmulatorLauncher.ASSIGN_SESSION_PORTS),
      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.ENTER_STATE, EmulatorLauncher.LAUNCH_MODE, EmulatorLauncher.SESSION_LAUNCH),

      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.SKIPPED_STATE, EmulatorLauncher.LAUNCH_MODE, EmulatorLauncher.SESSION_DEBUG),

      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.EXIT_STATE, EmulatorLauncher.LAUNCH_MODE, EmulatorLauncher.SESSION_LAUNCH),
      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.ENTER_STATE, EmulatorLauncher.LAUNCH_MODE, EmulatorLauncher.FINAL),
  };


  private static final EmulatorLauncherEvent[] EXPECTED_DEBUG_EVENTS_TABLE
      = new EmulatorLauncherEvent[]
      {
      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.EXIT_STATE, EmulatorLauncher.INIT_MODE, 0),
      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.ENTER_STATE, EmulatorLauncher.INIT_MODE, EmulatorLauncher.CREATE_SEI_COMMAND),

      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.MODE_CHANGE, EmulatorLauncher.DEBUG_MODE, EmulatorLauncher.CREATE_SEI_COMMAND),

      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.EXIT_STATE, EmulatorLauncher.DEBUG_MODE, EmulatorLauncher.CREATE_SEI_COMMAND),
      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.ENTER_STATE, EmulatorLauncher.DEBUG_MODE, EmulatorLauncher.CREATE_VM_LAUNCH_ARGS),

      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.EXIT_STATE, EmulatorLauncher.DEBUG_MODE, EmulatorLauncher.CREATE_VM_LAUNCH_ARGS),
      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.ENTER_STATE, EmulatorLauncher.DEBUG_MODE, EmulatorLauncher.READ_USER_PREFERENCES),

      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.EXIT_STATE, EmulatorLauncher.DEBUG_MODE, EmulatorLauncher.READ_USER_PREFERENCES),
      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.ENTER_STATE, EmulatorLauncher.DEBUG_MODE, EmulatorLauncher.INIT_JAD),

      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.EXIT_STATE, EmulatorLauncher.DEBUG_MODE, EmulatorLauncher.INIT_JAD),
      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.ENTER_STATE, EmulatorLauncher.DEBUG_MODE, EmulatorLauncher.RESOLVE_APPLICATION),

      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.EXIT_STATE, EmulatorLauncher.DEBUG_MODE, EmulatorLauncher.RESOLVE_APPLICATION),
      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.ENTER_STATE, EmulatorLauncher.DEBUG_MODE, EmulatorLauncher.INIT_JAR),

      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.SKIPPED_STATE, EmulatorLauncher.DEBUG_MODE, EmulatorLauncher.ADD_JAR_SIGNATURE),

      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.EXIT_STATE, EmulatorLauncher.DEBUG_MODE, EmulatorLauncher.INIT_JAR),
      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.ENTER_STATE, EmulatorLauncher.DEBUG_MODE, EmulatorLauncher.INIT_SESSION),

      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.EXIT_STATE, EmulatorLauncher.DEBUG_MODE, EmulatorLauncher.INIT_SESSION),
      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.ENTER_STATE, EmulatorLauncher.DEBUG_MODE, EmulatorLauncher.ASSIGN_SESSION_PORTS),

      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.EXIT_STATE, EmulatorLauncher.DEBUG_MODE, EmulatorLauncher.ASSIGN_SESSION_PORTS),
      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.ENTER_STATE, EmulatorLauncher.DEBUG_MODE, EmulatorLauncher.SESSION_LAUNCH),

      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.EXIT_STATE, EmulatorLauncher.DEBUG_MODE, EmulatorLauncher.SESSION_LAUNCH),
      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.ENTER_STATE, EmulatorLauncher.DEBUG_MODE, EmulatorLauncher.SESSION_DEBUG),

      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.EXIT_STATE, EmulatorLauncher.DEBUG_MODE, EmulatorLauncher.SESSION_DEBUG),
      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.ENTER_STATE, EmulatorLauncher.DEBUG_MODE, EmulatorLauncher.FINAL),
  };


  private static final EmulatorLauncherEvent[] QUERY_EVENTS
      = new EmulatorLauncherEvent[]
      {
      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.EXIT_STATE, EmulatorLauncher.INIT_MODE, 0),
      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.ENTER_STATE, EmulatorLauncher.INIT_MODE, EmulatorLauncher.CREATE_SEI_COMMAND),

      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.MODE_CHANGE, EmulatorLauncher.QUERY_MODE, EmulatorLauncher.CREATE_SEI_COMMAND),

      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.EXIT_STATE, EmulatorLauncher.QUERY_MODE, EmulatorLauncher.CREATE_SEI_COMMAND),
      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.ENTER_STATE, EmulatorLauncher.QUERY_MODE, EmulatorLauncher.ANSWER_QUERY),

      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.EXIT_STATE, EmulatorLauncher.QUERY_MODE, EmulatorLauncher.ANSWER_QUERY),
      new EmulatorLauncherEvent(sLauncher, EmulatorLauncherEvent.ENTER_STATE, EmulatorLauncher.QUERY_MODE, EmulatorLauncher.FINAL),
  };

  private static final EmulatorLauncherEvent[] VERSION_EVENTS = QUERY_EVENTS;

  //
  // Members
  //

  private EmulatorLauncherListener emulatorLauncherListener = null;
  private Properties iCommands = new Properties();
  private int sEventIndex;
  private static EmulatorLauncherEvent[] iEventsArray;


  public TestEmulatorLauncherListener(String name) throws IOException{
    super(name);
    iCommands.load(this.getClass().getResourceAsStream("commands.properties"));
  }

  //
  // Operations
  //

  public void setUp(){
    sLauncher = new EmulatorLauncherStub();
    sLauncher.setListener(this);
    sEventIndex = 0;
  }

  private String[] stringToArray(String aCommand){
    StringTokenizer tokenizer = new StringTokenizer(aCommand);
    String[] arr = new String[tokenizer.countTokens()];
    for(int i = 0; i < arr.length; i++){
      arr[i] = tokenizer.nextToken();
    }
    return arr;
  }

  public void testVersionEvent() {
    String command = iCommands.getProperty("version");
    iEventsArray = VERSION_EVENTS;
    sLauncher.launch(stringToArray(command));
    assertEquals("did not reach final event.", sEventIndex, iEventsArray.length);
  }

  public void testQueryEvent() {
    String command = iCommands.getProperty("query");
    iEventsArray = QUERY_EVENTS;
    sLauncher.launch(stringToArray(command));
    assertEquals("did not reach final event.", sEventIndex, iEventsArray.length);
  }

  public void testLaunchEvents() {
    String command = iCommands.getProperty("launch");
    iEventsArray = EXPECTED_LAUNCH_EVENTS_TABLE;
    sLauncher.launch(stringToArray(command));
    assertEquals("did not reach final event.", sEventIndex, iEventsArray.length);
  }

  public void testDebugEvents() {
    String command = iCommands.getProperty("debug");
    iEventsArray = EXPECTED_DEBUG_EVENTS_TABLE;
    sLauncher.launch(stringToArray(command));
  }


  public void handleEmulatorLauncherEvent(EmulatorLauncherEvent aEvent){
    try{
      assertEquals("incorrect event type", aEvent.getType(),
                   iEventsArray[sEventIndex].getType());
      assertEquals("incorrect launcher state", aEvent.getState(),
                   iEventsArray[sEventIndex].getState());
      assertEquals("incorrect launcher mode", aEvent.getMode(),
                   iEventsArray[sEventIndex].getMode());
      sEventIndex++;
    }
    catch(Exception e){
      e.printStackTrace();
    }
  }


}


/**
 * Stub launcher.
 * This unit test is not about functionality.
 */
class EmulatorLauncherStub extends EmulatorLauncher{

  protected void readUserPreferences() throws java.io.IOException {
    //use the defaults
    iUserPreferences = new UserPreferences();
  }

  protected void createSEICommand(String[] parm1) {
    super.createSEICommand(parm1);
  }

  protected void handleLaunchException(Throwable aEx){
    aEx.printStackTrace();
    Assert.fail("caught exception: " + aEx.getMessage());
  }


  protected void printVersionInfo() throws java.io.IOException {
  }
  protected void printQueryInfo() throws java.io.IOException {
  }
  protected void initializeSession() {
  }
  protected void initializeDescriptor() throws java.io.IOException {
  }
  protected void createVMLaunchArguments() {
  }
  protected void assignSessionPorts() throws Exception {
  }
  protected void startKDPAgent() throws java.lang.Exception {
  }
  protected void launchMIDlet() throws java.lang.Exception {
  }
  protected void initializeJar() throws java.io.IOException {
  }
  protected void resolveApplication() throws java.io.IOException, java.lang.IllegalStateException {
  }
  protected void addJarSignature() throws java.io.IOException, java.lang.ClassNotFoundException {
  }
  protected void printHelpInfo() throws java.io.IOException {
  }
  protected void handleQueryCommand() throws java.io.IOException {
  }

}
