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

package com.nokia.epdt;

import java.io.FileInputStream;
import java.net.URL;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.Locale;
import java.util.Map;
import java.util.Properties;
import java.util.ResourceBundle;

import javax.swing.UIManager;

import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import org.apache.log4j.xml.DOMConfigurator;

import com.nokia.epdt.core.Constants;
import com.nokia.epdt.core.CoreController;
import com.nokia.epdt.core.config.epocini.EpocIniConfigurator;

/**
 * Main
 * Singleton
 */
public class Main implements Constants {
	private Logger log;

	/**
	 * CoreController map
	 * key: String ID value: CoreController
	 */
	private Map coreControllerMap = new Hashtable();

	/**
	 * Causes this main to begin execution.
	 * The method accepts a single argument: an array of elements of type String.
	 * This array is the mechanism through which the framework passes information to main.
	 * An arguments let users affect the operation of the main without recompiling it.
	 * @param args An array of strings that contains the arguments
	 */
	public static void main(String[] args) {
		new Main();
	}

	/**
	 * Main instance creation
	 */
	private Main() {
		Properties config = new Properties();
		try {
			config.load(new FileInputStream(System.getProperty("configDir",
					CONFIG_PROPERTIES)));
		} catch (Exception e) {
			e.printStackTrace();
		}

		ResourceBundle bundle = null;
		try {
			Locale locale = new java.util.Locale(config
					.getProperty(EPDT_LANGUAGE), config
					.getProperty(Constants.EPDT_COUNTRY));
			bundle = ResourceBundle.getBundle(config
					.getProperty(EPDT_RESOURCE_BUNDLE), locale);
		} catch (Exception e) {
			e.printStackTrace();
		}

		String str = bundle.getString(LOGGING_CONFIGURATION);
		configureLogging(/*System.getProperty(LOG_LEVEL)*/"DEBUG", ClassLoader
				.getSystemResource(str));
		setLAF(config.getProperty(EPDT_LOOK_AND_FEEL));
		log = Logger.getLogger(Main.class);

		if (checkJava(System.getProperty("java.version"), config
				.getProperty(EPDT_JAVA_VERSION_START))) {
			start(bundle, config);
		} else {
			(new VersionJDialog(bundle)).setVisible(true);
		}
	}

	/**
	 * CoreController creation
	 * @param ID CoreControllers ID
	 * @return CoreController instance
	 */
	private CoreController createCoreController(String ID, Properties param,
			ResourceBundle bundle, EpocIniConfigurator configurator) {
		boolean exitOnClose = Boolean.getBoolean("exitonclose");
		CoreController cc = new CoreController(exitOnClose, ID, param, bundle,
				configurator);
		coreControllerMap.put(ID, cc);
		return cc;
	}

	/**
	 * Getter for CoreController
	 * @param ID
	 * @return CoreController instance
	 */
	public CoreController getCoreController(String ID) {
		return (CoreController) coreControllerMap.get(ID);
	}

	/**
	 * Getter for CoreController ID's
	 * @return ID Iterator
	 */
	public Iterator getControllerIDs() {
		return coreControllerMap.keySet().iterator();
	}

	/**
	 * Starting application
	 */
	private void start(ResourceBundle bundle, Properties config) {
		EpocIniConfigurator configurator = EpocIniConfigurator
				.createEpocIniConfigurator(System.getProperty(
						Constants.EPOC32_DIR, config
								.getProperty(Constants.EPOC32)));

		com.nokia.epdt.core.config.epocini.EpocIniConfiguratorTest newConfT = new com.nokia.epdt.core.config.epocini.EpocIniConfiguratorTest(
				System.getProperty(Constants.EPOC32_DIR, config
						.getProperty(Constants.EPOC32)));

		// Parse command line parameters and create request
		Properties param = new Properties();
		param.put(COMMAND_KEY, System.getProperty(COMMAND_KEY));
		param.put(HOST_KEY, System.getProperty(HOST_KEY));
		param.put(PORT_KEY, System.getProperty(PORT_KEY));
		createCoreController("INSTANCE_1", param, bundle, configurator);
	}

	/**
	 * Validation of right Java version
	 * @return success
	 */
	private static boolean checkJava(String javaVersion, String javaVersionStart) {
		boolean start = false;
		if (javaVersionStart != null) {
			String[] versions = javaVersionStart.split(",");
			for (int i = 0; i < versions.length; i++) {
				if (javaVersion.startsWith(versions[i])) {
					start = true;
				}
			}
		}
		return start;
	}

	/**
	 * Setting Look and feel
	 */
	private static void setLAF(String laf) {
		try {
			UIManager.setLookAndFeel(laf);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	/**
	 * Configuring used logging utility
	 */
	private static void configureLogging(String logLevel, URL configuration) {
		// Simple console logger
		boolean fileLogging = true;
		if (logLevel != null) {
			Properties p = new Properties();
			p.setProperty("log4j.rootLogger", logLevel + ", A1");
			if (!fileLogging) {
				p.setProperty("log4j.appender.A1",
						"org.apache.log4j.ConsoleAppender");
			} else {
				p.setProperty("log4j.appender.A1",
						"org.apache.log4j.FileAppender");
				p.setProperty("log4j.appender.A1.File", "Epdt_Java.log");
			}
			p.setProperty("log4j.appender.A1.layout",
					"org.apache.log4j.PatternLayout");
			p.setProperty("log4j.appender.A1.layout.ConversionPattern",
					"%d [%t] %-5p %c \n %m%n");
			PropertyConfigurator.configure(p);
		} else {
			// Default configuration file determines logger settings
			try {
				DOMConfigurator.configure(configuration);
			} catch (Throwable t) {
				t.printStackTrace();
			}
		}
	}
}
