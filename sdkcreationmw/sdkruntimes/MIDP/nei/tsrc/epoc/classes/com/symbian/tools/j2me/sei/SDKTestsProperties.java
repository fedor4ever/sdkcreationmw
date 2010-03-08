/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

package com.symbian.tools.j2me.sei;

public class SDKTestsProperties {
  public static final String NAIVE_SESSION_HOST = System.getProperty("sdk.tests.host.naive");
  public static final String WIN32_SESSION_HOST = System.getProperty("sdk.tests.host.win32");
  public static final String TARGET_SESSION_HOST = System.getProperty("sdk.tests.host.target");
  public static final String TEST_NAME = System.getProperty("sdk.tests.name");
  public static final String TEST_SESSION = System.getProperty("sdk.tests.session");
  public static final String KERNEL = System.getProperty("sdk.tests.kernel");
  public static final String SUITES_LOCATION_PREFIX = System.getProperty("sdk.tests.testsuites.location.prefix");
  public static final String IS_INTERACTIVE_SUITES = System.getProperty("sdk.tests.testsuites.interactive");
  public static final String INTERACTIVE_SUITES_LOCATION = System.getProperty("sdk.tests.testsuites.location.interactive");
  public static final String NON_INTERACTIVE_SUITES_LOCATION = System.getProperty("sdk.tests.testsuites.location.noninteractive");
  public static final String CONFIRMATION_PING_REMOTE_HOST = System.getProperty("sdk.tests.confirmation.remotehost");
  public static final String CONFIRMATION_PING_REMOTE_PORT = System.getProperty("sdk.tests.confirmation.remoteport");
  public static final String CONFIRMATION_LISTEN_LOCAL_PORT = System.getProperty("sdk.tests.confirmation.localport");
  public static final String RERUN_HOST = System.getProperty("sdk.tests.host");

  static {
    System.out.println("NAIVE_SESSION_HOST = " + NAIVE_SESSION_HOST);
    System.out.println("WIN32_SESSION_HOST = " + WIN32_SESSION_HOST);
    System.out.println("TARGET_SESSION_HOST = " + TARGET_SESSION_HOST);
    System.out.println("SUITES_LOCATION_PREFIX = " + SUITES_LOCATION_PREFIX);
    System.out.println("TEST_NAME = " + TEST_NAME);
    System.out.println("TEST_SESSION = " + TEST_SESSION);
    System.out.println("KERNEL = " + KERNEL);
    System.out.println("IS_INTERACTIVE_SUITES =  " + IS_INTERACTIVE_SUITES);
    System.out.println("INTERACTIVE_SUITES_LOCATION = " + INTERACTIVE_SUITES_LOCATION);
    System.out.println("NON_INTERACTIVE_SUITES_LOCATION = " + NON_INTERACTIVE_SUITES_LOCATION);
    System.out.println("CONFIRMATION_PING_REMOTE_HOST = " + CONFIRMATION_PING_REMOTE_HOST);
    System.out.println("CONFIRMATION_PING_REMOTE_PORT = " + CONFIRMATION_PING_REMOTE_PORT);
    System.out.println("CONFIRMATION_LISTEN_LOCAL_PORT = " + CONFIRMATION_LISTEN_LOCAL_PORT);
    System.out.println("RERUN_HOST = " + RERUN_HOST);
  }

  private SDKTestsProperties() {
  }
}
