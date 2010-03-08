/*
* Copyright (c) 2000 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.epdt.plugins.s60.pan;

import com.nokia.epdt.core.Constants;
import org.apache.log4j.Logger;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.LinkedList;

/**
 * PanModel
 * HCI :
 * [hci] hcidllfilename=
 *  hci_h4.dll
 *  hci_bcsp.dll
 *  hci_usb.dll
 * COM port:
 * -1 = disable
 * 0 - 15 = 1 - 16 enable
 *
 */
class PanModel{
	static Logger log = Logger.getLogger(PanModel.class);
	public static int BT_HCI_USB_INDEX = 2;
	static LinkedList hciList= new LinkedList();
	{
		hciList.add(0, "hci_bcsp.dll");
		hciList.add(1, "hci_h4.dll");
		hciList.add(BT_HCI_USB_INDEX, "hci_usb.dll");
	}
    
    private PropSets irdaProps = new PropSets();
    private PropSets btProps = new PropSets();
    private String epoc32;
    
    public PanModel(String epocroot) throws IOException
    {
    	this.epoc32 = epocroot;
    	init();
    }
    
    private static String getResource(String key)
    {	return PanResourceBundle.getInstance().getValue(key);  }
    
    public boolean isBluetoothEnabled(){ return getBluetoothComPort()>=0; }
    
    public int getBluetoothHci() {
    	String hci = btProps.getValue(getResource("bluetooth.esk.sectionName"), 
			getResource("bluetooth.esk.dllKey"));
    	return hciList.indexOf(hci); 
    }
    public void setBluetoothHci(int index)
    { 	btProps.setValue(getResource("bluetooth.esk.sectionName"), 
    			getResource("bluetooth.esk.dllKey"), (String)hciList.get(index)); }
    
    public int getBluetoothComPort()
    {
			String btPort =  btProps.getValue(getResource("bluetooth.esk.sectionName"), 
					getResource("bluetooth.esk.portKey"));
			return Integer.parseInt(btPort);
    }
    
    public void setBluetoothComPort(int comPort)
    {btProps.setValue(getResource("bluetooth.esk.sectionName"), 
			getResource("bluetooth.esk.portKey"), Integer.toString(comPort));}
    		
    public boolean isIrdaEnabled() { return getIrdaComPort() >=0; }
    
    public int getIrdaComPort()
    {return Integer.parseInt(
			irdaProps.getValue(getResource("irda.esk.sectionName"), 
					getResource("irda.esk.portKey"))); }
    public void setIrdaComPort(int comPort)
    {irdaProps.setValue(getResource("irda.esk.sectionName"), 
			getResource("irda.esk.portKey"), Integer.toString(comPort));}

    /**
     * The function reads innitial values from .esk files
     *
     */
    public void init() throws IOException
    {	//Initialisation of btProps and irdaProps from .esk files:
    	String btFileName = getBtEskFileName(); 
    	String irdaFileName = getIrdaEskFileName();
        
    	FileReader btReader =null;
    	FileReader irdaReader =null;
   		try
   		{
   			btReader = new FileReader(btFileName);
   			btProps.load(btReader);
   			irdaReader = new FileReader(irdaFileName);
   			irdaProps.load(irdaReader);
   		}
   		catch(IOException ex)
   		{
   			throw(ex);
   		}
   		finally
   		{
   			try{
   				if(btReader != null) btReader.close();
   				if(irdaReader != null)irdaReader.close();
   			}catch(IOException ex)
   			{log.error("error closing bt or IrDa .esk configuration files:"
   					+ ex.getMessage());}
   		}
    }
    public void save()
    {
    	String btFileName = getBtEskFileName();
    	String irdaFileName = getIrdaEskFileName();
        
    	FileWriter btWriter =null;
    	FileWriter irdaWriter =null;
   		try
   		{
   			btWriter = new FileWriter(btFileName);
   			btProps.save(btWriter);
   			irdaWriter = new FileWriter(irdaFileName);
   			irdaProps.save(irdaWriter);
   		}
   		catch(IOException ex)
   		{
   			log.error("Error saving BT or Irda ESK files:"+ex.getMessage());
   		}
   		finally
   		{
   			try{
   				if(btWriter != null) btWriter.close();
   				if(irdaWriter != null)irdaWriter.close();
   			}catch(IOException ex)
   			{log.error("error closing bt or IrDa .esk configuration files:"
   					+ ex.getMessage());}
   		}
    	
    }
    private String getBtEskFileName()
    {	return epoc32 + getResource("eskFilePath") 
		+ "\\"+ getResource("bluetooth.eskFileName");
    }
    private String getIrdaEskFileName()
    {	return epoc32 + getResource("eskFilePath") 
		+ "\\" + getResource("irda.eskFileName");
    }
}