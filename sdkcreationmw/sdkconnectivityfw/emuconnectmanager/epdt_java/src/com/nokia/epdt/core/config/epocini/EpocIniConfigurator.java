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

package com.nokia.epdt.core.config.epocini;

import org.apache.log4j.Logger;
import java.util.Vector;
import java.util.Iterator;
import java.util.Properties;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.PrintWriter;
import java.io.File;
import java.io.LineNumberReader;
import java.io.InputStreamReader;
import java.io.IOException;
/**
 * 
 * Class EpocIniConfigurator
 * there was a problem of settings save conflicts when different plugins 
 * tried to write to epoc.ini.
 * Therefore this common single EpocIniConfigurator was designed
 */

public class EpocIniConfigurator
{
	//Singleton pattern:
	private static EpocIniConfigurator instance;
	
	private static Logger log = Logger.getLogger(EpocIniConfigurator.class);
	
	public class Property
	{	public String key;
		public String value;
		public Property(String aKey, String aValue)
		{ key = aKey; value = aValue; }
	}
	 
	Vector props = new Vector();
	//Properties do not suite
	//- epoc.ini can contain multiple entries with the same name 
	//- the class does not maintain the order of insertion
	
	String epocroot = ""; 
	
	public static EpocIniConfigurator createEpocIniConfigurator(String aEpocroot)
	{
		if(instance ==null)
		{
			return new EpocIniConfigurator(aEpocroot);
		}
		else
		{
			return instance;
		}
	}
	
	private EpocIniConfigurator(String aEpocroot)
	{
		epocroot = aEpocroot;
        FileInputStream iniInputStream = null;
        try {
            iniInputStream = new FileInputStream(epocroot+"\\data\\epoc.ini");
            props = new Vector();
            
            LineNumberReader iniLineReader = new LineNumberReader(new InputStreamReader(iniInputStream) );
            String line;
            while( 	(line = iniLineReader.readLine()) != null)
            {
            	//log.debug(line);
            	line.trim();
            	int pos = line.lastIndexOf(' ');
            	if(pos>0)
            	{
            		props.add(new Property(line.substring(0, pos),line.substring(pos+1)) );
            	}
            	else if(line.length()>0){
            		props.add(new Property(line, ""));
            	}
            }

        } catch (IOException e) {
            log.error("Can't open epoc.ini file"+e.getMessage());
        } finally {
            if (iniInputStream != null) {
                try {
                    iniInputStream.close();
                } catch(IOException e) {};
            }
        }
	}
	
	public String getProperty(String aPropertyKey)
	{
		for(int i= 0; i<props.size(); i++)
		{	Property p = (Property)props.get(i);
			if(p.key.equals(aPropertyKey))
			{
				return p.value;
			}
		}
		return "";
	}
	/**
	 * Returns a vector of objects of Property type
	 * (inner class EpocIniConfigurator.Propery)
	 *  with the same name
	 * @param aPropertyKey
	 * @return Vector of EpocIniConfigurator.Propery
	 */
	public Vector getProperties(String aPropertyKey)
	{	Vector rVector = new Vector();
		for(int i= 0; i<props.size(); i++)
		{	Property p = (Property)props.get(i);
			if(p.key.equals(aPropertyKey))
			{
				rVector.add(p);
			}
		}
		return rVector;
	}

	
	/**
	 * Test function, should be commented out
	 * returns the whole props vector
	 */
	public Vector getProperties()
	{
		return props;
	}
	
	/**
	 * Assigns the property aKey a value of aValue
	 * if the property is not present - adds it 
	 * @param String aKey
	 * @param String aValue
	 */
	synchronized public void setProperty(String aKey, String aValue)
	{
		for(int i= 0; i<props.size(); i++)
		{	Property p = (Property)props.get(i);
			if(p.key.equals(aKey))
			{
				p.value = aValue;
				return;
			}
		}
		//we can't get here unless the key is not yet in props
		props.add(new Property(aKey, aValue));
	}
	
	/**
	 * Adds/Sets all properties in p into epoc.ini props
	 * @param Properties p
	 */
	synchronized public void setProperties(Properties p)
	{
		for(Iterator it = p.keySet().iterator(); it.hasNext();)
		{
			String key = (String)it.next();
			setProperty(key, p.getProperty(key));
		}
	}
	
	/**
	 * saves settings to Epoc.ini
	 *
	 */
	synchronized public void saveSettings()
	{
		try{
			
	        PrintWriter writer = new PrintWriter(
	        		new FileOutputStream(new File(epocroot+"\\data\\epoc.ini")));
	        for(int i= 0; i<props.size(); i++)
	        {	Property p = (Property)props.get(i);
	        	if(p.value.length()>0)
	        	{  	writer.write(p.key + " "+p.value+"\r\n");
	        	}
	        	else
	        	{	writer.write(p.key +"\r\n");
	        	}
	        }
	        writer.flush();
	        writer.close();
       } catch (IOException e) {
            log.error("Can't write to epoc.ini file: "+e.getMessage());
        }
	}
}
