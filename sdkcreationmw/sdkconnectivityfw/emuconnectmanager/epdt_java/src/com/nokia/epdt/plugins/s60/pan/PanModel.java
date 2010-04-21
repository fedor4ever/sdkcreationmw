/*
 * Copyright (c) 2000 - 2006 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of the License "Eclipse Public License v1.0"
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

import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.LinkedList;

import org.apache.log4j.Logger;

/**
 * PanModel
 * HCLT :
 * 
 * [hctl]
 * port= 1
 * baud_rate= 115200
 *
 * COM port:
 * -1 = disable
 * 0 - 15 = 1 - 16 enable
 *
 */
class PanModel {
	static Logger log = Logger.getLogger(PanModel.class);
	public static int BT_HCI_USB_INDEX = 2;
	static LinkedList hciList = new LinkedList();
	{
		hciList.add(0, "hci_bcsp.dll");
		hciList.add(1, "hci_h4.dll");
		hciList.add(BT_HCI_USB_INDEX, "hci_usb.dll");
	}

	private PropSets btProps = new PropSets();
	private String epoc32;

	public PanModel(String epocroot) throws IOException {
		this.epoc32 = epocroot;
		init();
	}

	private static String getResource(String key) {
		return PanResourceBundle.getInstance().getValue(key);
	}

	public boolean isBluetoothEnabled() {
		return getBluetoothComPort() >= 0;
	}

	public int getBluetoothHci() {
		String hci = btProps.getValue(getResource("bluetooth.esk.sectionName"),
				getResource("bluetooth.esk.dllKey"));
		return hciList.indexOf(hci);
	}

	public void setBluetoothHci(int index) {
		btProps.setValue(getResource("bluetooth.esk.sectionName"),
				getResource("bluetooth.esk.dllKey"), (String) hciList
						.get(index));
	}

	public int getBluetoothComPort() {
		String btPort = btProps.getValue(
				getResource("bluetooth.esk.sectionName"),
				getResource("bluetooth.esk.portKey"));
		return Integer.parseInt(btPort);
	}

	public void setBluetoothComPort(int comPort) {
		btProps
				.setValue(getResource("bluetooth.esk.sectionName"),
						getResource("bluetooth.esk.portKey"), Integer
								.toString(comPort));
	}

	/**
	 * The function reads innitial values from .esk files
	 *
	 */
	public void init() throws IOException { //Initialisation of btProps from .esk file:
		String btFileName = getBtEskFileName();

		FileReader btReader = null;
		try {
			btReader = new FileReader(btFileName);
			btProps.load(btReader);
		} catch (IOException ex) {
			throw (ex);
		} finally {
			try {
				if (btReader != null)
					btReader.close();
			} catch (IOException ex) {
				log.error("Error closing BT ESK configuration files:"
						+ ex.getMessage());
			}
		}
	}

	public void save() {
		String btFileName = getBtEskFileName();

		FileWriter btWriter = null;
		try {
			btWriter = new FileWriter(btFileName);
			btProps.save(btWriter);
		} catch (IOException ex) {
			log.error("Error saving BT ESK files:" + ex.getMessage());
		} finally {
			try {
				if (btWriter != null)
					btWriter.close();
			} catch (IOException ex) {
				log.error("error closing bt .esk configuration files:"
						+ ex.getMessage());
			}
		}

	}

	private String getBtEskFileName() {
		return epoc32 + getResource("eskFilePath") + "\\"
				+ getResource("bluetooth.eskFileName");
	}

}