/*
* Copyright (c) 2003 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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

package com.symbian.tools.j2me.sei.emulator;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Properties;
import java.util.StringTokenizer;
import java.util.Vector;
import java.util.jar.Attributes;
import java.util.jar.JarFile;
import java.util.jar.Manifest;

import javax.swing.JOptionPane;

import com.symbian.io.JADFile;
import com.symbian.io.JARFile;
import com.symbian.io.VMVerbosePiper;
import com.symbian.tools.j2me.sei.SDKRuntimeProperties;
import com.symbian.tools.j2me.sei.emulator.commands.SEICommand;
import com.symbian.tools.j2me.sei.emulator.commands.VMLaunchArguments;
import com.symbian.tools.j2me.sei.emulator.event.EmulatorLauncherEvent;
import com.symbian.tools.j2me.sei.emulator.event.EmulatorLauncherListener;
import com.symbian.tools.j2me.sei.emulator.session.Session;
import com.symbian.tools.j2me.sei.preferences.UserPreferences;
import com.symbian.utils.Debug;
import com.symbian.utils.SEIManifest;
import com.symbian.utils.SystemExitHandler;
import com.symbian.utils.SystemExitListener;
import com.symbian.vm.MIDletInfo;
import com.symbian.vm.MIDletSuiteInfo;

/**
 * The template algorithm to launch an emulator using the SEI specification.
 * There are few steps to launch an emulator: 1> parse the command line 2>
 * initialize the emulator descriptor 3> initialize the jar manifest 4>
 * initialize the emulator jar 5> launch the executable
 * 
 * <p>
 * Copyright: Copyright (c) 2003 By Symbian Ltd. All Rights Reserved.
 * </p>
 * <p>
 * Company: Symbian Ltd
 * </p>
 * 
 * @author Roy Ben Hayun
 * @version 1.1 (CLDC-HI)
 */
public class EmulatorLauncher implements SystemExitListener {
	private static final String PROPERTIES_FILE = "version.properties";

	private static final String HELP_FILE = "help.properties";

	// Queries constants
	/**
	 * Delimiter used in the queries.properties file for separating between
	 * platform and variant
	 */
	private static final String PLATFORM_VARIANT_DELIM = "_";

	/**
	 * Queries properties file
	 */
	private static final String QUERIES_FILE = System
			.getProperty("sdk.tests.queries") == null ? "queries.properties"
			: System.getProperty("sdk.tests.queries");

	/**
	 * Device list key
	 */
	private static final String DEVICE_LIST_KEY = "device.list";

	/**
	 * The SEI arguments for -help command. Taken also from the queries property
	 * file.
	 */
	private static final String HELP_INFO = "sei.arguments";

	/**
	 * Version information key, taken from the version.properties file.
	 */
	private static final String VERSION_INFO = "VERSION_INFO";

	// State constants.
	public static final int CREATE_SEI_COMMAND = 0;

	public static final int ANSWER_QUERY = 1;

	public static final int READ_USER_PREFERENCES = 2;

	public static final int CREATE_VM_LAUNCH_ARGS = 3;

	public static final int INIT_JAD = 4;

	public static final int RESOLVE_APPLICATION = 5;

	public static final int INIT_JAR = 6;

	public static final int ADD_JAR_SIGNATURE = 7;

	public static final int INIT_SESSION = 8;

	public static final int ASSIGN_SESSION_PORTS = 9;

	public static final int SESSION_LAUNCH = 10;

	public static final int SESSION_DEBUG = 11;

	public static final int FINAL = 12;

	// Mode constants.
	public final static int INIT_MODE = 0;

	public final static int QUERY_MODE = 1;

	public final static int LAUNCH_MODE = 2;

	public final static int DEBUG_MODE = 3;

	// Members
	/**
	 * State indicator
	 */
	private int iState;

	/**
	 * Mode indicator
	 */
	private int iMode = INIT_MODE;

	/**
	 * Emulator listener
	 */
	private EmulatorLauncherListener iListener;

	/**
	 * User preferences
	 */
	protected UserPreferences iUserPreferences;

	/**
	 * SEI command
	 */
	protected SEICommand iSEICommand;

	/**
	 * VM launch arguments
	 */
	protected VMLaunchArguments iVMLaunchArguments;

	/**
	 * Jar file
	 */
	protected JARFile iJar;

	/**
	 * Jad file
	 */
	protected JADFile iJad;

	/**
	 * Abstract Debug/Launch Session
	 */
	private Session iSession;

	/**
	 * Executed MIDlet number in the Jad (starts from 1).
	 */
	private int iMIDletNumber;

	/**
	 * Need to keep this reference for tests shutdown
	 */
	private VMVerbosePiper iVMVerbosePiper;

	//
	// Life cycle
	//
	/**
	 * Constructor.
	 */
	public EmulatorLauncher() {
		SystemExitHandler.setExitListener(this);
	}

	// Methods
	/**
	 * Get Map with all MIDlets classes to names. Used by the view to enable
	 * Use-Case: debug 2 MIDlets.
	 * 
	 * @return Map with all MIDlets classes to names
	 */
	public Map getMIDlets() {
		return iJad.getMIDlets();
	}

	/**
	 * Get MIDlet info
	 * 
	 * @return MIDlet info
	 */
	public MIDletInfo getMIDletInfo() {
		return iVMLaunchArguments.getMIDletInfo();
	}

	/**
	 * Set listener
	 * 
	 * @param aListener
	 *            emulator launcher listener
	 */
	public void setListener(EmulatorLauncherListener aListener) {
		iListener = aListener;
	}

	/**
	 * Launch the emulator using the given SEI command line. This is the SEI
	 * execution algorithm template.
	 * 
	 * @return VM exit value
	 * 
	 * @param aArgs
	 *            command line arguments
	 */
	public void launch(String[] aArgs) {
		try {
			// parse command line
			changeState(CREATE_SEI_COMMAND);
			createSEICommand(aArgs);
			if (iSEICommand.isQuery()) {
				// handle query command
				changeMode(QUERY_MODE);
				changeState(ANSWER_QUERY);
				handleQueryCommand();
			} else {
				Debug.println(this, "+EmulatorLauncher.launch()"); // can't
				// print
				// before
				// queries
				// phase
				// change mode
				changeMode(iSEICommand.isDebugMode() ? DEBUG_MODE : LAUNCH_MODE);

				// create VM arguments
				changeState(CREATE_VM_LAUNCH_ARGS);
				createVMLaunchArguments();

				// read static user preferences
				changeState(READ_USER_PREFERENCES);
				readUserPreferences();
				if (!iSEICommand.isDebugMode()
						&& iUserPreferences.getKDPPreferences()
								.isAttachingToVMEnabled()) {
					// change to debug mode if not actually debugging and
					// attaching enabled
					iVMLaunchArguments.getVMArguments().setUnsuspendedMode();
					changeMode(DEBUG_MODE);
				}

				// initialize Jad
				changeState(INIT_JAD);
				initializeDescriptor();

				// resolve application name
				changeState(RESOLVE_APPLICATION);
				resolveApplication();

				// initialize Jar
				changeState(INIT_JAR);
				initializeJar();

				// sign suite
				if (iUserPreferences.getSecurityPreferences()
						.isAlwaysAddSignature()) {
					changeState(ADD_JAR_SIGNATURE);
					addJarSignature();
				} else {
					dispatchEvent(EmulatorLauncherEvent.SKIPPED_STATE,
							ADD_JAR_SIGNATURE);
				}

				// initialize Session
				changeState(INIT_SESSION);
				initializeSession();

				// assign remote session ports
				changeState(ASSIGN_SESSION_PORTS);
				assignSessionPorts();

				// launch midlet
				changeState(SESSION_LAUNCH);
				launchMIDlet();

				// start/enable debugging
				// TODO: find out if we need KDP agent in J9
				// in theory - not.
				// however we need to listen on a port and forward traffic to
				// IDE
				/*
				 * if (iMode == DEBUG_MODE) { changeState(SESSION_DEBUG);
				 * startKDPAgent(); } else {
				 * dispatchEvent(EmulatorLauncherEvent.SKIPPED_STATE,
				 * SESSION_DEBUG); }
				 */
				dispatchEvent(EmulatorLauncherEvent.SKIPPED_STATE,
						SESSION_DEBUG);
			}
			changeState(FINAL);
		} catch (Throwable ex) {
			Debug.printStackTrace(this, ex);
			handleLaunchException(ex);
		} finally {
			Debug.println(this, "-EmulatorLauncher.launch()");
			Debug
					.println(this,
							"<-- EmulatorLauncher launch-report --> \nSEI command line was: ");
			Debug.println(this, iSEICommand.getArguments());
		}
	}

	/**
	 * Run another MIDlet in the same VM. This method is invoked from the view
	 * when the user wants to debug another MIDlet from the same suite.
	 * 
	 * @param aMIDletName
	 *            MIDlet name
	 * 
	 * @throws Exception
	 *             if some error occured
	 */
	public void addMIDlet(String aMIDletName) throws Exception {
		iSession.startMIDlet(iVMLaunchArguments.getMIDletInfo()
				.getMIDletSuiteInfo().getMIDletInfo(aMIDletName));
	}

	/**
	 * Read user preferences. First tries loading from the user file. If fails,
	 * loads the defaults from the distribution unit.
	 * 
	 * @throws IOException
	 *             if some error occured
	 */
	protected void readUserPreferences() throws IOException {
		iUserPreferences = UserPreferences.load();

		// display view if user requested
		if (iUserPreferences.getUIPreferences().isShowLauncherView()) {
			dispatchEvent(EmulatorLauncherEvent.SHOW_VIEW, iState);
		}
		if (iUserPreferences.getCommandLinePreferences()
				.getBootStrapClasspath() != null) {
			iVMLaunchArguments.getVMArguments().setBootstrapClassPath(
					iUserPreferences.getCommandLinePreferences()
							.getBootStrapClasspath());
		}
		// init debug logs settings
		Debug.initLogSettings(iUserPreferences.getCommandLinePreferences());
	}

	/**
	 * Create the SEI Data object needed for execution. This method is the
	 * factory method for the SEI commands.
	 * 
	 * @param aArgs
	 *            command line arguments
	 */
	protected void createSEICommand(String[] aArgs) {
		// create SEI Command
		iSEICommand = new SEICommand(aArgs);
	}

	/**
	 * Create the VM launch argumentd data object needed for execution.
	 * 
	 * This method is the factory method for the KVM commands.
	 */
	protected void createVMLaunchArguments() {
		Debug.println(this, "+EmulatorLauncher.createVMLaunchArguments()");
		iVMLaunchArguments = new VMLaunchArguments(iSEICommand);
	}

	/**
	 * Initialize the descriptor. Never use the IDE descriptor file since we
	 * might change it.
	 * 
	 * @throws IOException
	 *             if some error occured
	 */
	protected void initializeDescriptor() throws IOException {
		Debug.println(this, "+EmulatorLauncher.initializeDescriptor()");
		// ensure all MUST properties exist
		// Dima: SystemAMS needs MIDlet name to be MIDlet specific
		// it's a bug - MIDlet name is arbitrary, can be always the same, but...
		Properties defaultPreferences = iUserPreferences.getSuitePreferences()
				.getDefaultJadProperties();
		String suiteName = iSEICommand.getApplication() == null ? iSEICommand
				.getDescriptor().getName() : iSEICommand.getApplication();
		defaultPreferences.put("MIDlet-Name", suiteName);

		iJad = new JADFile(defaultPreferences);

		// add J2ME permissions
		iJad.addJ2MEPermission(iUserPreferences.getSecurityPreferences()
				.getRequiredPermissions());

		// override with given properties
		if (iSEICommand.getDescriptor() != null) {
			iJad.overrideAttributes(new JADFile(iSEICommand.getDescriptor()));
		}
	}

	/**
	 * Resolve the executed MIDlet number
	 * 
	 * @throws IOException
	 *             if some error occured
	 * @throws IllegalStateException
	 *             if failed to resolve
	 */
	protected void resolveApplication() throws IOException,
			IllegalStateException {
		Debug.println(this, "+EmulatorLauncher.resolveApplication()");
		String className = iSEICommand.getApplication();
		if (className == null) {
			// Jad must contain MIDlets
			if (iJad.getMIDlets().size() == 0) {
				throw new IllegalStateException(
						"No application specified in Jad or command line");
			}
			// ask user to decide which to run
			iMIDletNumber = iJad
					.getMIDletNumber(displayMIDletsSelectionDialog(iJad));
		} else {
			// find the matching MIDlet index
			if (iJad.getMIDlets().size() == 0
					|| !iJad.containsMIDletClass(className)) {
				// add default name
				iJad.addDefaultMIDlet(className);
				iJad.flush();
			}
			iMIDletNumber = iJad.getMIDletNumber(className);
		}
	}

	/**
	 * Initialize the jar file
	 * 
	 * @throws IOException
	 *             if some error occured
	 */
	protected void initializeJar() throws IOException {
		Debug.println(this, "+EmulatorLauncher.initializeJar()");
		// remove non valid properties (i.e file size is invalid at this point)
		final String jarUrl = iJad.containsJarURL() ? iJad.getJarURL() : null;
		iJad.removeJarProperties();
		iJad.flush();

		if (jarUrl != null) {
			// take the given Jar file
			iJar = new JARFile(new File(
					iSEICommand.getDescriptor().getParent(), jarUrl));
		} else if (iSEICommand.getClassPath() != null
				&& iSEICommand.getClassPath().length > 0) {
			iJar = new JARFile(iUserPreferences.getSuitePreferences()
					.getFileTypes(), iSEICommand.getClassPath(), iJad.getFile());
		} else {
			throw new UnsupportedOperationException("No valid classpath given");
		}

		iJad.setJarSize(iJar.getFile().length());
		iJad.setJarURL(iJar.getFile().getName());
		iJad.flush();
	}

	/**
	 * Add Jar signature to Jad file
	 * 
	 * @throws IOException
	 *             if some error occured
	 * @throws ClassNotFoundException
	 *             if the JadTool.jar is not in the classpath
	 *             InterruptedException might be thrown by Process.waitFor()
	 */
	protected void addJarSignature() throws Exception {
		Debug.println(this, "+EmulatorLauncher.addJarSignature()");
		iJad = new JadToolLauncher().launch(iUserPreferences
				.getSecurityPreferences(), iJar, iJad);
	}

	/**
	 * Create Session
	 * 
	 * @exception SessionException
	 *                if session initialization failed
	 */
	protected void initializeSession() throws Exception {
		Debug.println(this, "+EmulatorLauncher.initializeSession()");
		iSession = Session.create(iSEICommand.getDeviceName());
		iSession.initialize(iUserPreferences);
	}

	/**
	 * Start MIDlet run session
	 * 
	 * @throws Exception
	 *             if some error occured
	 */
	protected void launchMIDlet() throws Exception {
		Debug.println(this, "+EmulatorLauncher.launchMIDlet()");
		// install
		MIDletSuiteInfo suiteInfo = iSession.installMIDletSuite(iJar, iJad,
				iUserPreferences.getSecurityPreferences().isTrusted());
		iVMLaunchArguments.setMIDletInfo(suiteInfo.getMIDletInfo(iJad
				.getMIDletName(iMIDletNumber)));

		// start
		iSession.startMIDlet(iVMLaunchArguments, iUserPreferences
				.getKDPPreferences().isVMLaunchingDeferred());

		// pipe output
		iVMVerbosePiper = new VMVerbosePiper(iSession.getSlaveInetAddress(),
				iSession.mapVmPort(iVMLaunchArguments.getVMArguments()
						.getVMConsolePort()), System.out, iSession
						.getProcess(iVMLaunchArguments.getMIDletInfo()),
				iUserPreferences.getKDPPreferences().getConnectionAttempts());
		iVMVerbosePiper.start();
	}

	/**
	 * Assign VM debug and verbose ports
	 * 
	 * @throws Exception
	 *             if some error occured
	 */
	protected void assignSessionPorts() throws Exception {
		Debug.println(this, "+EmulatorLauncher.assignSessionPorts()");
		
		String[] comm = (String[])iSEICommand.getArguments();
		for (int i = 0; i < comm.length; i++) {
			System.out.println(comm[i]);
		}
		
		/* Pranav: 
		 * Console IP address & port were part of the IDE VM arguments.  
		 * Those were being used for console/Diagnostic output. Not required...code removed from here..
		 */ 
		
		if (iMode == DEBUG_MODE) {
			// Emulator in server mode: Phone VM connects to the port in command line
			// Emulator in client mode: Phone VM listens on the port given
			
			//Pranav: Write VM arguments for MIDlet Debug from the iSEICommand
			String[] seiCommands = (String[])iSEICommand.getArguments();
			
			String parmaDebug = "";
			String paramJdwp = "";
			for (int i = 0; i < seiCommands.length; i++) {
				System.out.println(seiCommands[i]);
				if (seiCommands[i].startsWith("-Xdebug")) parmaDebug = seiCommands[i];
				else if (seiCommands[i].startsWith("-Xrunjdwp")) paramJdwp = seiCommands[i];
				
				if(!parmaDebug.equals("") && !paramJdwp.equals("")){
					break;
				}
			}
			iVMLaunchArguments.getVMArguments().setVMDebugPort(iSEICommand.getVMRemotePort(), parmaDebug.trim(), paramJdwp.trim());
			
			Debug.println(this, "debug port: " + iSEICommand.getVMRemotePort());
		}
	}

	//
	// Queries answering
	//
	/**
	 * Print to the System out the query response. This method loads the
	 * queries.properties file and prints the information.
	 * 
	 * @throws IOException
	 *             if could not load the properties file
	 */
	protected void handleQueryCommand() throws IOException {
		final String query = iSEICommand.getQuery();
		// a method for each query type, to enable future customization.
		if (query.equals(SEICommand.VERSION_ARG)) {
			printVersionInfo();
		} else if (query.equals(SEICommand.HELP_ARG)) {
			printHelpInfo();
		} else if (query.equals(SEICommand.QUERY_ARG)) {
			printQueryInfo();
		}
		// required for some IDEs to call System.exit() exlicitly
		SystemExitHandler.invokeSystemExit(0);
	}

	/**
	 * Print to the System out stream, the version info. (Hook method to enable
	 * future customization)
	 * 
	 * @throws IOException
	 *             if could not load the properties file
	 */
	protected void printVersionInfo() throws IOException {
		Properties props = new Properties();
		props.load(getClass().getResourceAsStream(PROPERTIES_FILE));

		// Get instance info from SDK directory name
		// it ends with _<INST_NUMBER> for all instances after the firts one:
		String instance = SDKRuntimeProperties.SDK_HOME
				.substring(SDKRuntimeProperties.SDK_HOME.lastIndexOf("_") + 1);
		System.out.print(props.getProperty("SDK_NAME"));
		try {
			Integer.parseInt(instance);
			System.out.println(" instance " + instance);
		} catch (NumberFormatException ex) { // The directory name does not
												// end with _<NUMBER>
			System.out.println();
		}
		// System.out.println(props.getProperty(VERSION_INFO));
		String profile = props.getProperty("PROFILE");
		String configuration = props.getProperty("CONFIGURATION");
		String[] defaultOptionalApis = (props.getProperty("OPTIONAL"))
				.split(",");
		StringBuffer optionalApis = new StringBuffer();
		File lib = new File(SDKRuntimeProperties.SDK_HOME, "lib");
		if (!lib.isDirectory()) {
			lib = new File(new File(SDKRuntimeProperties.EPOC_HOME)
					.getParentFile().getParentFile(), "lib");
		}
		
		if (lib.isDirectory()) {
			String[] jars = lib.list(new FilenameFilter() {
				public boolean accept(File dir, String name) {
					return (name.toLowerCase().endsWith(".jar") || name.toLowerCase().endsWith(".zip"));
				}
			});
			if (jars != null && jars.length > 0) {
				for (int i = 0; i < jars.length; i++) {

					try {
						JarFile jarFile = new JarFile(lib.getAbsolutePath() + File.separatorChar + jars[i]);
						Manifest manifest = jarFile.getManifest();
						if (manifest != null) {
							Attributes atr = manifest.getMainAttributes();
							String apiName = atr.getValue("API");
							String apiVersion = atr.getValue("API-Specification-Version");

							if ((apiName != null || !apiName.equals("")) && (apiVersion != null || !apiVersion.equals(""))) {
								if (!apiName.equalsIgnoreCase("CLDC") && !apiName.equalsIgnoreCase("MIDP")) {
									if (optionalApis.length() > 0) {
										optionalApis.append(", ");
									}
									optionalApis.append(apiName + "-" + apiVersion);
								}
							}

						}
					} catch (Exception e) {
						// TODO: handle exception
					}
				}

			}
		}
		System.out.println("Profile: " + profile);
		System.out.println("Configuration: " + configuration);
		System.out.println("Optional: " + optionalApis);
	}

	/**
	 * Print to the System out stream, the help info. (Hook method to enable
	 * future customization)
	 * 
	 * @throws IOException
	 *             if could not load the properties file
	 */
	protected void printHelpInfo() throws IOException {
		Properties props = new Properties();
		props.load(getClass().getResourceAsStream(HELP_FILE));
		System.out.println(props.getProperty(HELP_INFO));
	}

	/**
	 * Print to the System out stream, the query answer. (Hook method to enable
	 * future customization)
	 * 
	 * @throws IOException
	 *             if some error occured
	 */
	protected void printQueryInfo() throws IOException {
		try {

			// Check for the lib directory. If not avail, create it
			File lib = new File(SDKRuntimeProperties.SDK_HOME, "lib");
			if (!lib.isDirectory()) {
				lib = new File(new File(SDKRuntimeProperties.EPOC_HOME)
						.getParentFile().getParentFile(), "lib");
			}

			String[] jars = lib.list(new FilenameFilter() {
				public boolean accept(File dir, String name) {
					return (name.toLowerCase().endsWith(".jar") || name
							.toLowerCase().endsWith(".zip"));
				}
			});

			// Store file data
			String strLine = "";
			LinkedHashMap xQueryProps = new LinkedHashMap();

			if (jars != null && jars.length > 0) {

				BufferedReader br = getSelectedFileBufferedReader(QUERIES_FILE);

				// Read File Line By Line
				while ((strLine = br.readLine()) != null) {

					if (strLine.startsWith("#") || strLine.startsWith(" ")) {
						// move out of this condition
						xQueryProps.put(strLine, "");
					} else {
						if (strLine.indexOf(":") != -1) {
							xQueryProps.put(strLine.substring(0, strLine
									.indexOf(":")), strLine.substring(strLine
									.indexOf(":") + 2));

						} else {
							xQueryProps.put(strLine, "");
						}
					}
				}

				// Value of the bootpath property
				String absPath = lib.getAbsolutePath().replace(
						File.separatorChar, '/');
				StringBuffer sb = new StringBuffer((absPath.length() + 20)
						* jars.length);
				for (int i = 0; i < jars.length; i++) {
					if (i > 0)
						sb.append(',');
					sb.append(absPath).append('/').append(jars[i]);
				}
				String bootclasspath = sb.toString();

				String sdkHomePath = SDKRuntimeProperties.SDK_HOME.replace(
						File.separatorChar, '/');
				xQueryProps.put("kvem.class.path", sdkHomePath + "/"
						+ xQueryProps.get("kvem.class.path"));
				xQueryProps.put("sdk.home", sdkHomePath);
				
				// Get emulator resolution
				String epocPath = sdkHomePath + "/epoc32/data/epoc.ini";
				Vector emuResolution = getEmulatorResolution(epocPath);
				
				// Set bootpath for each device
				String deviceList = (String) xQueryProps.get("device.list");
				StringTokenizer t = new StringTokenizer(deviceList, ",");
				while (t.hasMoreTokens()) {
					String device = t.nextToken().trim();
					xQueryProps.put(device + ".bootclasspath", bootclasspath);
					xQueryProps.put(device + ".apis", bootclasspath);
					xQueryProps.put(device + ".screen.isTouch", emuResolution.get(0));
					xQueryProps.put(device + ".screen.width", emuResolution.get(1));
					xQueryProps.put(device + ".screen.height", emuResolution.get(2));
				}
			}

			// Print properties to standard output
			Iterator keySet = xQueryProps.keySet().iterator();
			while (keySet.hasNext()) {
				String key = (String) keySet.next();
				String value = (String) xQueryProps.get(key);
				System.out.print(key);
				if (!value.trim().equals("")) {
					System.out.print(": ");
					System.out.print(xQueryProps.get(key));
				}
				System.out.println("");
			}
		} catch (Exception e) {
			System.err.println("Error: " + e.getMessage());
			e.printStackTrace();
		}
	}

	/**
	 * Gives BufferedReader from the give file as parameter
	 * @param fileName file name
	 * @return br BufferedReader
	 */
	private BufferedReader getSelectedFileBufferedReader(String fileName) {
		// Read queries.properties file
		InputStream in = getClass().getResourceAsStream(fileName);
		DataInputStream data = new DataInputStream(in);
		BufferedReader br = new BufferedReader(new InputStreamReader(data));
		return br;
	}

	/**
	 * method gets current resolution of the emulator. it includes width, height and touchScreen
	 * It reads configuration entry from epoc.ini file. entry include
	 * 
	 * File Name Format: Parse file name for width, height and touchEnable
	 * epoc_360x640_touch.ini
	 * epoc_640x360_touch_qwerty.ini
	 * epoc_240x320.ini
	 * epoc_320x240.ini 
	 * 
	 * @param filePath epoc.ini file path
	 * @return param Vector includes touch, width and height
	 */
	private Vector getEmulatorResolution(String filePath){
		Vector param = new Vector();
		try{
			FileInputStream in = new FileInputStream(filePath);
			DataInputStream dataIn = new DataInputStream(in);
			BufferedReader bReader = new BufferedReader(new InputStreamReader(dataIn));
			
			String epocLine = "";
			String resolutionFileName = "";
			while ((epocLine = bReader.readLine()) != null) {
				if (!epocLine.equals("")){
				if (epocLine.substring(0, epocLine.indexOf(" ")).trim().equals(
						"configuration")) {
					
					//Get resolution file name
					resolutionFileName = epocLine.substring(epocLine.indexOf(" ")+1);
					
					// Search for touch enable/disable
					if (resolutionFileName.contains("touch")){
						param.add("true");
					} else {
						param.add("false");
					}
					
					// Get width and height
					resolutionFileName = resolutionFileName.substring(resolutionFileName.indexOf("_")+1);
					
					if (resolutionFileName.contains("_")){
						resolutionFileName = resolutionFileName.substring(0, resolutionFileName.indexOf("_"));
					} else if (resolutionFileName.contains(".")){
						resolutionFileName = resolutionFileName.substring(0, resolutionFileName.indexOf("."));
					}
					
					String width = resolutionFileName.substring(0, resolutionFileName.indexOf("x"));
					String height = resolutionFileName.substring(resolutionFileName.indexOf("x")+1);
					
					param.add(width);
					param.add(height);
					
					break;
				}
				}
			}
		} catch (Exception e) {
			System.err.println("Error: " + e.getMessage());
			e.printStackTrace();
			return null;
		}
		
		return param;
		
	}
	//
	// UI stuff
	//

	/**
	 * Handle error on launch. The default implementation displays an error
	 * dialog
	 * 
	 * @param aEx
	 *            exception to display stack trace
	 * 
	 * @exception SessionException
	 *                if some error occured
	 */
	protected void handleLaunchException(Throwable aEx) {
		cleanup();
		if (iListener != null) {
			iListener.handleEmulatorLauncherEvent(new EmulatorLauncherEvent(
					this, aEx, iMode, iState));
		}
	}

	/**
	 * Request the user to select a MIDlet from the list of MIDlets specified in
	 * the manifest.
	 * 
	 * @param aManifest
	 *            SEIManifest
	 * 
	 * @return selected MIDlet name
	 */
	private String displayMIDletsSelectionDialog(SEIManifest aManifest) {
		Object[] midlets = aManifest.getMIDlets().keySet().toArray();
		if (midlets.length > 1) {
			return (String) JOptionPane.showInputDialog(null,
					EmulatorResources.sChooseMIDletMessage,
					EmulatorResources.sChooseMIDletTitle,
					JOptionPane.INFORMATION_MESSAGE, null, midlets, midlets[0]);
		} else {
			// no need to display the dialog
			return (String) (midlets[0]);
		}
	}

	/**
	 * Change the state and notify the state listener
	 * 
	 * @param aState
	 *            new state
	 */
	private void changeState(int aState) {
		dispatchEvent(EmulatorLauncherEvent.EXIT_STATE, iState);
		iState = aState;
		dispatchEvent(EmulatorLauncherEvent.ENTER_STATE, iState);
	}

	/**
	 * Change the mode and notify the listener
	 * 
	 * @param aMode
	 *            new mode
	 */
	private void changeMode(int aMode) {
		iMode = aMode;
		dispatchEvent(EmulatorLauncherEvent.MODE_CHANGE, iState);
	}

	/**
	 * Dispatch launcher event
	 * 
	 * @param aEventType
	 *            launcher event type
	 * @param aState
	 *            event state
	 */
	private void dispatchEvent(int aEventType, int aState) {
		if (iListener != null) {
			iListener.handleEmulatorLauncherEvent(new EmulatorLauncherEvent(
					this, aEventType, iMode, aState));
		}
	}

	//
	// SystemExitListener implementation and exit handling
	//

	/**
	 * SystemExitHandler.invokeSystemExit was invoked
	 */
	public void systemExitInvoked() {
		cleanup();
	}

	/**
	 * Cleanup all resources
	 */
	private void cleanup() {
		// terminate session
		Debug.println(this, "+EmulatorLauncher.systemExitInvoked");
		if (iSession != null) {
			try {
				iSession.terminate();
			} catch (Exception e) {
				Debug.printStackTrace(this, e);
			}
		}
		iSession = null;

		// terminate verbose piper
		// NOTE: if the VM didnt exit already, this might crash it.
		if (iVMVerbosePiper != null) {
			// TODO: iVMVerbosePiper.closeConnection();
		}
		iVMVerbosePiper = null;
		Debug.println(this, "-EmulatorLauncher.systemExitInvoked");
	}

}
