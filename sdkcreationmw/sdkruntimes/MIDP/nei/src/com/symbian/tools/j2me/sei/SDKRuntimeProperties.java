/*
* Copyright (c) 2003 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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

import java.io.File;
import java.io.IOException;
import java.util.Properties;

import com.symbian.tools.j2me.sei.emulator.EmulatorResources;
import com.symbian.tools.j2me.sei.emulator.EmulatorLauncher;
import com.symbian.utils.Debug;

/**
 * <p>Title: SDK runtime properties</p>
 * <p>Description: Information about the SDK which the SEI needs </p>
 *
 * CUSTOMISATION NOTE: This Class is intended to be used for customization
 * The static variables are seen as the Class interface.
 * TODO: Licensees should use the static initializer to set the static variables
 * if their SDK requires those to be different then the given defaults.
 *
 * <p>Copyright: Copyright (c) 2004 By Symbian Ltd. All Rights Reserved</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 2.0
 */
public class SDKRuntimeProperties {

  //
  // Constants
  //

  /**
   * sdk.home passed by the win32 executable.
   */
  public static String SDK_HOME;

  /**
   * SEI home root. 3 folders above the sdk.home passed by the win32 executable.
   * Achieved by removing the product, platform and variant.
   */
  public static final String SDK_HOME_ROOT;

  /**
   * Epoc32 home. 3 folders above the SEI home.
   */
  public static String EPOC_HOME;

  /**
   * <Platform>
   */
  public static final String PLATFORM = System.getProperty("epoc.platform","winscw");

  /**
   * <Variant>
   */
  public static final String VARIANT = System.getProperty("epoc.variant","udeb");

  /**
   * <Product>
   */
  public static final String PRODUCT = System.getProperty("epoc.product","hmidps60v31");

  /**
   * System classes
   */
  public static final String SYSTEM_CLASSES;



  //
  // Life cycle
  //

  /**
   * No instances
   */
  private SDKRuntimeProperties(){

  }


  /**
   * CUSTOMISATION NOTE: This initialization method is intended to be used
   *  for customization.
   * TODO: Licensees should use this method to initialize the static variables
   * declared in this class.
   */
  static
  {
	//sdk home
	SDK_HOME = System.getProperty("sdk.home");
	if(SDK_HOME == null)
	{
	    try
	    {
		//Queries properties file
		String queries_file =
                    System.getProperty("sdk.tests.queries") == null ?
                    "queries.properties" : System.getProperty("sdk.tests.queries");
              Properties props = new Properties();
              props.load(EmulatorResources.class.getResourceAsStream(queries_file));
              SDK_HOME = props.getProperty("sdk.home");
	    }
	    catch(IOException e)
	    {
		Debug.printStackTrace(SDKRuntimeProperties.class, e);
	    }
	}
	//normalize the path
	try
	{
	    SDK_HOME = new File(SDK_HOME).getCanonicalFile().getAbsolutePath();
	}
	catch (IOException e)
	{
	    Debug.printStackTrace(SDKRuntimeProperties.class, e);
	}
	if(SDK_HOME == null)
	{
	    SDK_HOME = ".";
	}
	SDK_HOME = SDK_HOME.replace('\\','/');

	//epoc home
	EPOC_HOME = System.getProperty("epoc.root");
	if (EPOC_HOME == null)
	{
		EPOC_HOME = SDK_HOME + File.separator + "bin";
	}

	//SDK home root
	SDK_HOME_ROOT = SDK_HOME;

	//system classes
	SYSTEM_CLASSES = SDK_HOME_ROOT +
		File.separator + "lib" +
		File.separator + PRODUCT + ".jar";

	//print
	Debug.println(SDKRuntimeProperties.class, "epoc enviroment: \n" +
				  "sdk.home: " + SDK_HOME + "\n" +
				  PLATFORM + "/" +
				  VARIANT + "/" +
				  PRODUCT +
				  "\nepoc.home: " + EPOC_HOME +
				  "\nsystem classes: " + SYSTEM_CLASSES);


  }


}
