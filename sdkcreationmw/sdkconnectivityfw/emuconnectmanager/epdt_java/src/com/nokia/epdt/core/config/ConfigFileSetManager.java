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



package com.nokia.epdt.core.config;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import org.apache.log4j.Logger;

/**
 *
 * ConfigFileSetManager
 */
public class ConfigFileSetManager {
    private static Logger log = Logger.getLogger(ConfigFileSetManager.class);
	private static ConfigFileSetManager instance = null;
	private List listenerList = new ArrayList();
	private Map fileMap = new HashMap(); 
	
	private ConfigFileSetManager() {}
	
	/**
	 * Getter for instance
	 */
	public static ConfigFileSetManager getInstance() {
		if(instance == null) 
			instance = new ConfigFileSetManager();
		return instance; 
	}
	
	/**
	 * Setter for file rows
	 * @param filePath
	 * @param rowList
	 */
	public void setFileRows(Object sender, String filePath, List rowList) {
		fileMap.put(filePath, rowList);
		notifyListeners(sender, filePath);
	}	

	/**
	 * Getter for file rows
	 * @param filePath
	 * @return file rows
	 */
	public List getFileRows(String filePath) {
		List s = (List) fileMap.get(filePath);
		if(s == null) {
			if(readFile(filePath))
				s = (List) fileMap.get(filePath);				
			else
				return null;
		}
		List d = new ArrayList(s);
		return d;
	}
	
	/**
	 * Getter for file paths
	 */
	public List getFilePathNames() {
		List l = new ArrayList();
		Iterator i = fileMap.keySet().iterator();
		while(i.hasNext()) {
			l.add(i.next());
		}
		return l;
	}
	
	/**
	 * File reading routine
	 * @param filePath
	 * @return success
	 */
	public boolean readFile(String filePath) {
		if(log.isDebugEnabled())
			log.debug("Reading file : "+filePath);
		File file = new File(filePath);
        if (file.exists()) {
			if(file.isFile()) {
				List list = new ArrayList();
				try {
			        BufferedReader in = new BufferedReader(new FileReader(file));
			        String str;
			        while ((str = in.readLine()) != null) {
			        	//System.out.println(str);
			        	list.add(str);
			        }
			        in.close();
			    } catch (Exception e) {
			    	log.warn("File reading failed", e);
			    }
			    if(list.size() > 0) {
			    	fileMap.put(filePath, list);
			        return true;
			    }
			}
        }
		return false;
	}

	/**
	 * File writing routine
	 * @param filePath 
	 * @return success
	 */
	public boolean writeFile(String filePath) {		
		// writing backup file
		renameFile(filePath);
		
		if(log.isDebugEnabled())
			log.debug("Writing file : "+filePath);
		try {
	        BufferedWriter out = new BufferedWriter(new FileWriter(filePath, false));
	        List l = (List) fileMap.get(filePath);
	        Iterator i = l.iterator();
	        
	        while(i.hasNext()) {
	        	String str = (String)i.next();
	        	out.write(str);	        		        	
	        }	        
			out.close();
			return true;
		} catch (Exception e) {
	    	log.warn("File writing failed", e);
	    }
		return false;
	}	
	
	/**
	 * File renaming procedure 
	 * @param filePath
	 */
	private static void renameFile(String filePath) {
		File file = new File(filePath);
	    String newFileName = file.getParentFile()+"/~"+file.getName();
	    File file2 = new File(newFileName);
	    boolean success = file.renameTo(file2);
	    if (!success) {
	    	log.warn("Could not create file : "+newFileName);
	    }
	}
	
	/**
	 * Notifying listeners
	 * @param key changed file name
	 */
	private void notifyListeners(Object sender, String key) {
		Iterator i = listenerList.iterator();
		while(i.hasNext()) {
			((ConfigFileSetManagerListener) i.next()).fileChanged(sender, key);
		}
	}
}
