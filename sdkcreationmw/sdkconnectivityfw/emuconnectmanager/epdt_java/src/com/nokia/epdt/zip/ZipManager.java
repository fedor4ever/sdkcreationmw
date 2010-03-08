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



package com.nokia.epdt.zip;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.IOException;
import java.util.Enumeration;
import java.util.Vector;
import java.util.Properties;
import java.util.ResourceBundle;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;

import com.nokia.epdt.core.CoreUtils;

/**
 * ZipManager
 * 
 * Unzips a language ZIP file from 
 * <epocroot>epoc32\tools\ecmt\language\<sdk_id>-<LanguageID>_<CountryID>.zip
 * (ex 200640-en_us.zip)
 * TO epocroot directory
 * 
 * The string <LanguageID>_<CountryID> is expeted as -Dlanguage property
 * 
 * <epocroot>epoc32\tools\ecmt\language\200640-undo.zip MUST EXIST
 */
public class ZipManager {  
    
    protected static final String LANGUAGE = "language";
    protected static final String SECOND = "second";
    
    private static final String PAK_PATH = "epoc32\\tools\\ecmt\\language\\";
	
    private static Logger log = Logger.getLogger(ZipManager.class);
    private ResourceBundle resourceBundle;
    private int emulatorInstances = 1;

    UnZipView viewFrame = new UnZipView();
    
    public ZipManager() {
	resourceBundle = ResourceBundle.getBundle("com.nokia.epdt.zip.zipmanager");
    }
    /**
     * Implementing Runnable
     */
    public void run()
    {	
    	// starting gui
    	viewFrame.setVisible(true);
    	
    	String language       = System.getProperty(LANGUAGE);
    	String secondInstance = System.getProperty(SECOND);
    	// checking commandline values and exiting if not sufficent
    	if (language == null) {
		    viewFrame.showErrorMessage(resourceBundle.getString("INVALID_PARAMS"));
		    viewFrame.dispose();
		    System.exit(-1);
    	}
		// check if 2nd instance should also be changed
		if ((secondInstance != null) && !secondInstance.equals("")) {
		    emulatorInstances = 2;
		}
		try{
			String epocroot = CoreUtils.EpocRoot();
			StringBuffer sdkId=new StringBuffer();
			//TODO: cleanup: 
			//for every zip file in PAK_PATH != selected language zip
			//delete all existing zip entires in destination folder (epocroot)
			//or save current language ID and read it - not to remove every zip content
			
			//Unzipping
			viewFrame.setLabelText(resourceBundle.getString("DECOMPRESSING"));

			//Vector of strings with directories names -
			//epocroots of instances where we change the language
			Vector roots = new Vector();
			roots.add(epocroot);
		    if (emulatorInstances == 2)
		    {	//TODO: we might consider supporting more than 2 instances
		    	//in the future
		    	String instanceEpocroot = getInstancePath(epocroot, 2);
		    	roots.add(instanceEpocroot);
		    }
		    for(int i=0; i<roots.size(); i++)
		    {	
		    	String root = (String)roots.get(i);
				if(!checkLanguageZips(root+PAK_PATH, sdkId))
				{	//Error:
					viewFrame.showErrorMessage(
							resourceBundle.getString("INCONSISTENT_ZIP_WARNING") );
					viewFrame.dispose();
				    System.exit(-1);
				}
			    decompress(	root+PAK_PATH+sdkId+"-undo.zip", root);
			    //Decompress language package
			    decompress(	root+PAK_PATH+sdkId+"-"+language+".zip", root);
		    }
		    viewFrame.showInformationMessage(resourceBundle.getString("SUCCESS"));
		}catch(Exception e){
			viewFrame.showErrorMessage(resourceBundle.getString("FAILURE") + "\n"+ e.getMessage());
			log.error("Decompress exception: "+e.getMessage());
		}
		viewFrame.dispose();
    	System.exit(0);
    }

    public static void main(String[] args) {
	ZipManager zm = new ZipManager();
	
	configureDebugLogging();
	
	zm.run();
    }
    /**
     * The function tries to find the path for the Nth instance epocroot
     * which should be something like 
     * 		epocrot\..\*_N 		for C++
	 * and 	epocrot\..\..\*_N 	for MIDP SDK
	 * 
	 *  where N - instance number
     */
    private String getInstancePath(String epocRootName, int instanceNumber)
    {
    	File sdkDir = new File(epocRootName);
    	if(sdkDir.exists() && sdkDir.isDirectory())
    	{	
    		for(int i=0; i<2; i++)
    		{
		    	//i=0: Try C++ SDK set up
		    	//i=1: 	if failed to find the instance &return, consider MIDP setup
		    	sdkDir = sdkDir.getParentFile();
		    	if(sdkDir.exists() && sdkDir.isDirectory())//paranoija?
		    	{
		    		File[] instDirs = sdkDir.listFiles();
	    			for(int j=0; j<instDirs.length; j++)
	    			{
	    				if(instDirs[j].isDirectory() && 
	    					instDirs[j].getName().endsWith("_"+String.valueOf(instanceNumber)))
	    				{
	    					if(i==0){
	    						//C++ SDK
	    						return instDirs[j].getAbsolutePath()+"\\";
	    					}else
	    					{	//MIDP SDK
	    						return instDirs[j].getAbsolutePath()+"\\bin\\";
	    					}
	    				}
	    			}
		    	}
    		}
    	}	
    	return null;
    }
    /**
     * The function: 
     * 1. Checks if names of all Zips in the folder start from
     * 		SDK ID ended with "-" char (if yes - returns true)
     * 2. Sets the SDK ID parameter
     * @param path
     * @param sdkID
     * @return true if the folder is consistent
     */
    private boolean checkLanguageZips(String path, StringBuffer sdkId)
    {
    	File pakDir = new File(path);
    	if(pakDir.isDirectory())
    	{
    		File[] dirList = pakDir.listFiles();
    		if(dirList.length>0)
    		{	//Get the first file, set the ID from its name:	
    			sdkId.replace(0, sdkId.length(), 
    					dirList[0].getName().substring(0, dirList[0].getName().indexOf('-')));
	    		for(int i=0; i<dirList.length; i++)
	    		{//Check if other zips have the same ID:
	    			if(!dirList[i].getName().startsWith(sdkId.toString()))
	    			{return false;
	    			}
	    		}
	    		return true;
    		}
    	}
    	return false;
    }
    /**
     * Extracting entrys from zip file 
     *
     * @param pathToZipFile path where to look for zip file
     * @param pathToDestination path where to decompress file entrys from zip file
     */
    public void decompress(	String zipFileName,
    							String destDirName) throws Exception
    {	
        File sourceFile = new File(zipFileName);
        ZipFile zipFile = new ZipFile(sourceFile);
        File destDir = new File(destDirName);
        byte[] b = new byte[2048];
        FileOutputStream output = null;
        InputStream input = null;
        int counter = 0;
	    int numEntries = zipFile.size();  	    
        // Extract files
	    for (Enumeration e = zipFile.entries(); e.hasMoreElements();)
	    {
            ZipEntry zipEntry = (ZipEntry) e.nextElement();
            counter++;
        	if (! zipEntry.isDirectory() )
        	{//Unzip files only, not directories
			    File file = new File(destDir.getAbsolutePath() + "/" + zipEntry.getName());
			    File dir = file.getParentFile();
			    if (!dir.exists()) {
			    	dir.mkdirs();
			    }
			    output = new FileOutputStream(file);
			    input = zipFile.getInputStream(zipEntry);
	            try
	            {
				    int len;
				    while( (len = input.read(b, 0, b.length)) != -1)
				    {
				    	output.write(b, 0, len);
				    }
	            }catch(IOException ex)
	            { 	log.error(ex.getMessage());
	            	viewFrame.showErrorMessage("Error unzipping: "+ ex.getMessage());
	            }
	            finally{
		            output.close();
		            input.close();
	            }
        	}    
		    viewFrame.setProgressPercent(counter*100/numEntries);
		}
    }
    /**
     * Deleting files that match zip file entrys
     * 
     * @param zipFile path where to look for zip file
     * @param pathToDeleteRoot root path where to look for deletion candidates
     */
    public void deleteEntryFiles(String zipFileName, String dirName) throws Exception 
    {
	    // finding zip file 
    	ZipFile zipFile = new ZipFile(new File(zipFileName)); 
	    File dir = new File(dirName);
		int numEntries = zipFile.size();
		Enumeration enumeration = zipFile.entries();
		int counter = 0;
		// going through all zip entrys
		while (enumeration.hasMoreElements()) 
		{
		    ZipEntry entry = (ZipEntry) enumeration.nextElement();
		    // processing files only
		    if (!entry.isDirectory()) {			
				File f = new File(dir.getAbsolutePath()+"/"+entry.getName());
				// delete existing file
				if (f.exists()) {
				    f.delete();
				} 
		    }
		    counter++;
		    viewFrame.setProgressPercent(counter*100/numEntries);
		 }
    }
    
    /**
     * Configuring used logging utility
     */
    private static void configureDebugLogging() {
        	Properties p = new Properties();
            p.setProperty("log4j.rootLogger", "DEBUG , A1");
           	p.setProperty("log4j.appender.A1", "org.apache.log4j.FileAppender");
           	p.setProperty("log4j.appender.A1.File", "Epdt_Java.log");
            p.setProperty("log4j.appender.A1.layout", "org.apache.log4j.PatternLayout");
            p.setProperty("log4j.appender.A1.layout.ConversionPattern", "%d [%t] %-5p %c \n %m%n");
            PropertyConfigurator.configure(p);
    }
    
}

