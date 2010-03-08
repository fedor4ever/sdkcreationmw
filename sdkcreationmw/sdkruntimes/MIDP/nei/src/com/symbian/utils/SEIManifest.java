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

package com.symbian.utils;

import java.util.*;
import java.util.jar.*;

import java.io.*;





/**
 * The J2SE Manifest class is a a bizarre creature..
 * This class encapsulates the same operations but much simpler.
 *
 * <p>Copyright: Copyright (c) 2003 By Symbian Ltd. All Rights Reserved.</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 1.1 (CLDC-HI)
 */
public class SEIManifest
{



  //
  // Constants
  //


  /**
   * Default midlet name
   */
  private static final String DEFAULT_MIDLET_NAME = "SDKMIDlet";

  /**
   * MIDlet property
   */
  private static final String MIDLET_PROPERTY = "MIDlet-";


  //
  // Members
  //

  /**
   * Replaces the java.util.Manifest main attributes
   */
  private Map iMainAttributes = new HashMap();

  /**
   * Table of MIDlets names mapped by class name
   * Class -> Name
   */
  private Map iMIDletsClasses2Names = new HashMap();

  /**
   * Table of MIDlets names mapped by class name
   * Class -> Number
   */
  private Map iMIDletsClasses2Numbers = new HashMap();


  //
  // Constructor
  //


  /**
   * Default constructor.
   */
  protected SEIManifest()
  {
    iMainAttributes.put("Generated-By", "Symbian SEI"); //signature
  }


  /**
   * Create a SEI manifest with the given SEI Manifest attributes
   *
   * @param aManifest manifest to copy attributes from
   *
   * @throws IOException if some error occured
   */
  public SEIManifest(SEIManifest aManifest) throws IOException
  {
    this();
    overrideAttributes(aManifest);
  }

  //
  // Operations
  //



  /**
   * Add attribute
   *
   * @param aKey attribute key
   * @param aValue attribute value
   */
  public void addAttribute(String aKey, String aValue){
    getMainAttributes().put(aKey, aValue);

  }

  /**
   * Get Map of main attributes
   *
   * @return Map of main attributes
   */
  public Map getMainAttributes(){
    return iMainAttributes;
  }


  /**
   * Override the current attributes with the SEI Manifest.
   *
   * @param aManifest SEI Manifest
   */
  public void overrideAttributes(SEIManifest aManifest)
  {
	  	// Pranav - Add code to remove sign related attributes from the jad file to make it work for netbeans and eclipse IDEs
		Map parsedAttr = aManifest.getMainAttributes();

		Iterator iter = parsedAttr.keySet().iterator();

		Vector keys = new Vector();
		while (iter.hasNext()) {
			try {
				String key = (String) iter.next();
				if (key.startsWith("MIDlet-Certificate-")
						|| key.equals("MIDlet-Jar-RSA-SHA1")) {
					keys.add(key);
				}
			} catch (Exception e) {
				// Move silently - ignore
			}
		}

		System.out.println(keys);

		for (int length = 0; length < keys.size(); length++) {
			String key = ((String)keys.elementAt(length)).trim();
			if (parsedAttr.containsKey(key)){
				parsedAttr.remove(key);
			}
		}
		getMainAttributes().putAll(parsedAttr);
		//getMainAttributes().putAll(aManifest.getMainAttributes());
		// End Pranav
    parseMIDletsProperties();
  }


  /**
   * Utility method for getting the default name used for midlets
   *
   * @return default MIDlet name
   */
  public String getDefaultMIDletName(String aClassName)
  {
    String defaultName = aClassName.substring(aClassName.lastIndexOf('.')+1);
    String name = defaultName;
    int number = 0;
    while (iMIDletsClasses2Names.containsValue(name))
    {
      name = defaultName + "-" + (++number);
    }
    return name;
  }



  /**
   * parse MIDlet-<n> property
   */
  protected void parseMIDletsProperties()
  {
    int num = 1; //starts from 1.
    int colonIndex;
    String line = (String)getMainAttributes().get(MIDLET_PROPERTY + num);
    String className;
    while(line != null)
    {
      line = line.trim();
      //[name], [icon], [class]
      className = line.substring(line.lastIndexOf(",") + 1).trim();
      iMIDletsClasses2Names.put
          (className, line.substring(0, line.indexOf(",")).trim());
      iMIDletsClasses2Numbers.put(className, new Integer(num));
      line = (String)getMainAttributes().get(MIDLET_PROPERTY + ++num);
    }

  }


  /**
   * Get MIDlet name by class name
   *
   * @param aClassName MIDlet full class name
   *
   * @return MIDlet name
   */
  public String getMIDletName(String aClassName)
  {
    return (String)iMIDletsClasses2Names.get(aClassName);
  }


  /**
   * Get MIDlet number by class name
   *
   * @param aMIDletClassName MIDlet full class name
   *
   * @return MIDlet number
   */
  public int getMIDletNumber(String aClassName)
  {
    return ((Integer)iMIDletsClasses2Numbers.get(aClassName)).intValue();
  }


  /**
   * Get MIDlet name by MIDlet number
   *
   * @param aNumber MIDlet number
   *
   * @return MIDlet name
   *
   * @exception IllegalArgumentException if the MIDlet could not be found
   */
  public String getMIDletName(int aNumber){
    Object className;
    Integer number = new Integer(aNumber);
    for(Iterator iter = iMIDletsClasses2Numbers.keySet().iterator();
        iter.hasNext(); ){
      className = iter.next();
      if(iMIDletsClasses2Numbers.get(className).equals(number)){
         return (String)iMIDletsClasses2Names.get(className);
      }
    }
    throw new IllegalArgumentException("Could not find MIDlet #" + aNumber);
  }

  /**
   * Get Map with all MIDlets classes to names
   *
   * @return Map with all MIDlets classes to names
   */
  public Map getMIDlets()
  {
    return iMIDletsClasses2Names;
  }


  public boolean containsMIDletClass(String aClassName)
  {
    return iMIDletsClasses2Names.containsKey(aClassName);
  }

  /**
   * Add a default midlet property
   *
   * @param aClassName MIDlet class name
   */
  public void addDefaultMIDlet(String aClassName)
  {
    int number = iMIDletsClasses2Names.size() + 1;
    getMainAttributes().put
        (MIDLET_PROPERTY + number, getDefaultMIDletName(aClassName) + ", , " + aClassName);
    parseMIDletsProperties();
  }




  /**
   * Read the SEI manifest as a Properties file.
   *
   * @param is Input stream
   *
   * @throws IOException if some error occured
   */
  public void read(InputStream is) throws IOException
  {
    Properties props = new Properties();
    props.load(is);
    iMainAttributes.putAll(props);
    //Debug.printAttributes(this.getClass().getName(), this);
  }


  /**
   * Write the SEI manifest as a Properties with colon.
   *
   * @param is Input stream
   *
   * @throws IOException if some error occured
   */
  public void write(OutputStream out) throws IOException
  {
    PrintStream ps = new PrintStream(out);
    String key;
    for(Iterator iter = iMainAttributes.keySet().iterator(); iter.hasNext(); ){
      key = (String)iter.next();
      ps.println(key + ": " + iMainAttributes.get(key));
    }
    ps.flush();
  }





}
