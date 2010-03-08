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

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Vector;

import java.util.Properties;

import com.nokia.epdt.core.config.epocini.EpocIniConfigurator.Property;

public class EpocIniConfiguratorTest
{
	EpocIniConfigurator configurator;
	
	public EpocIniConfiguratorTest(String aEpocRoot)
	{
		configurator = EpocIniConfigurator.createEpocIniConfigurator(aEpocRoot);
		
		String diag=configurator.getProperty("PlatSecDiagnostics");
		configurator.setProperty("PlatSecDiagnostics", "NewValueOfDiag");
		configurator.setProperty("FormerPlatSecDiagnostics", diag);
		configurator.setProperty("Pechatnikov", "Dima");
		
		Properties addedProps = new Properties();
		addedProps.put("addedKey1", "addedvalue1");
		addedProps.put("addedKey2", "addedvalue2");
		
		configurator.setProperties(addedProps);
		
		printValues("test_epoc.ini");
	}
	
	void printValues(String aFileName)
	{
		Vector props = configurator.getProperties();
		try{
			
	        PrintWriter writer = new PrintWriter(new FileOutputStream(new File(aFileName)));
	        for(int i= 0; i<props.size(); i++)
	        {	Property p = (Property)props.get(i);
	        	if(p.value.length()>0)
	        	{  	writer.write(p.key + " "+p.value+"\r\n");
	        	}
	        	else
	        	{	writer.write(p.key +"\r\n");
	        	}
	        }
	        
	        writer.write("\n\nResolutions:\n");
	        Vector resolutions = configurator.getProperties("configuration");
	        for(int i=0; i<resolutions.size(); i++)
	        {	Property p = (Property)resolutions.get(i);
	        	writer.write(p.key + " "+p.value+"\r\n");
	        }
	        
	        writer.flush();
	        writer.close();
       } catch (IOException e) {
            System.err.println("Can't open epoc.ini file");
        }
	}
	
	
	
}
