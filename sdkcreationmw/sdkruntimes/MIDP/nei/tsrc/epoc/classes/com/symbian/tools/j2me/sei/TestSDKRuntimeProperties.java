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

package com.symbian.tools.j2me.sei;

import junit.framework.*;

import java.io.*;

/**
 * This is actually just a copy of the runtime properties resolving but it
 * ensures that if it is changed, we will notice it immediatly when running
 * the tests
 */
public class TestSDKRuntimeProperties extends TestCase {
  //
  // Constants
  //

  public static final String SHADOW_SDK_HOME;
  public static final String SHADOW_SDK_HOME_ROOT;
  public static final String SHADOW_EPOC_HOME;
  public static final String SHADOW_PLATFORM;
  public static final String SHADOW_VARIANT;
  public static final String SHADOW_PRODUCT;
  public static final String SHADOW_SYSTEM_CLASSES;

  static {
	String tmp = System.getProperty("sdk.home");
	//sdk home
	SHADOW_SDK_HOME = System.getProperty("sdk.home");

	//epoc home
	SHADOW_EPOC_HOME = new File(tmp).getParentFile().getParentFile().
		getParentFile().getParentFile().getParentFile().getParentFile().
		getAbsolutePath();

	//product
	int index = tmp.lastIndexOf(File.separator);
	SHADOW_PRODUCT = tmp.substring(index + 1);
	tmp = tmp.substring(0, index);

	//variant
	index = tmp.lastIndexOf(File.separator);
	SHADOW_VARIANT = tmp.substring(index + 1);
	tmp = tmp.substring(0, index);

	//platform
	index = tmp.lastIndexOf(File.separator);
	SHADOW_PLATFORM = tmp.substring(index + 1);

	//SDK home
	SHADOW_SDK_HOME_ROOT = tmp.substring(0, index);

	//system classes
	SHADOW_SYSTEM_CLASSES = SHADOW_SDK_HOME_ROOT +
		File.separator + SHADOW_PLATFORM +
		File.separator + SHADOW_VARIANT +
		File.separator + SHADOW_PRODUCT +
		File.separator + "lib" +
		File.separator + SHADOW_PRODUCT + ".jar";
  }

  public void testSDKRuntimeProperties() {
	assertEquals(SHADOW_EPOC_HOME, SDKRuntimeProperties.EPOC_HOME);
	assertEquals(SHADOW_PLATFORM, SDKRuntimeProperties.PLATFORM);
	assertEquals(SHADOW_PRODUCT, SDKRuntimeProperties.PRODUCT);
	assertEquals(SHADOW_SDK_HOME, SDKRuntimeProperties.SDK_HOME);
	assertEquals(SHADOW_SYSTEM_CLASSES, SDKRuntimeProperties.SYSTEM_CLASSES);
	assertEquals(SHADOW_SDK_HOME_ROOT, SDKRuntimeProperties.SDK_HOME_ROOT);
	assertEquals(SHADOW_VARIANT, SDKRuntimeProperties.VARIANT);
  }

}
