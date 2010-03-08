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
package com.nokia.sdk.s60.midp.disclaimer;

import java.io.File;

import javax.swing.JDialog;
import javax.swing.JFrame;

/**
 * Class contains information about the SDK disclaimer details at the SDK emulator startup
 * @author Pranav Gothadiy
 *
 */
public class Disclaimer extends JDialog {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	/**
	 * Application startup main method, entry point of the disclaimer.
	 * @param args 
	 */
	public static String ROOT_PATH = "";

	public Disclaimer(JFrame frame, String str) {
		super(frame, str);

	}

	public static void main(String[] args) {

		// Get the root of the application
		ROOT_PATH = getRootPath();

		// Create a dialog box to show the disclaimer
		Disclaimer frame = new Disclaimer(new JFrame(), "Dialog Box");

		// Show disclaimer window
		new DisclaimerUI(frame);

	}

	/**
	 * It will give the root path of the application
	 * @return
	 */
	private static String getRootPath() {
		File fi = new File("temp.txt");
		try {
			String path = fi.getAbsolutePath();
			path = path.substring(0, path.lastIndexOf("\\") + 1);
			fi.delete();
			return path;
		} catch (Exception e) {
			return "";
		}
	}

}
