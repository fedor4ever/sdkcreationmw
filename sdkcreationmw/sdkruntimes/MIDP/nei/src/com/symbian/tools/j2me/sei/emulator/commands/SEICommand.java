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

package com.symbian.tools.j2me.sei.emulator.commands;

import java.util.*;

import java.io.*;

import java.text.ParseException;

import com.symbian.utils.Utilities;
import com.symbian.tools.j2me.sei.*;

/**
 * SEI data encapsulation.
 *
 *
 * An SEICommand object would hold the arguments and indications passed
 * by the SEI String representation.
 * It is a place holder for the original string form and the parsed data.
 * The SEI String representation is not meant to be an executable
 * but a command line abstraction. therefor this class does not implement
 * the Executable interface.
 *
 * NOTE: by the SEI specification, the SEI must ignore any unknown command
 * line arguments.
 *
 * Role: This class is a Data class.
 *
 * <p>Copyright: Copyright (c) 2003 By Symbian Ltd. All Rights Reserved.</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 1.1 (CLDC-HI)
 */
public class SEICommand
{

	//
	// Constants
	//

	/**
	 * Properties file name
	 */
	private static final String DEF_SEI_ARGS = "seiarguments.properties";

	/**
	 * Class path delimiters
	 */
	protected static final String CLASS_PATH_DELIMITERS = ";,";

	/**
	 * Comma
	 */
	protected static final String COMMA = ",";

	/**
	 * Equals character
	 */
	protected static final String EQUALS = "=";

	/**
	 * Colon character
	 */
	protected static final String COLON = ":";

	/**
	 * M character. used for heap size in Megas
	 */
	private static final char M_CHAR = 'm';

	/**
	 * K character. used for heap size in Kilos
	 */
	private static final char K_CHAR = 'k';

	/**
	 * Kilo bytes
	 */
	private static final int KILO = 1024;

	/**
	 * Mega bytes
	 */
	private static final int MEGA = KILO * KILO;

	/**
	 *
	 */
	private static final String JDWP_DELIMITERS = ",=";

	//
	// Arguments constants
	//

	/**
	 * Version SEI argument
	 */
	public static String VERSION_ARG = "VERSION_ARG";

	/**
	 * Help SEI argument
	 */
	public static String HELP_ARG = "HELP_ARG";

	/**
	 * Emulator device information query
	 */
	public static String QUERY_ARG = "QUERY_ARG";

	/**
	 * Verbose SEI argument
	 */
	protected static String VERBOSE_ARG = "VERBOSE_ARG";

	/**
	 * All verbose type
	 */
	protected static String ALL_VERBOSE_TYPE = "ALL_VERBOSE_TYPE";

	/**
	 * Heap Allocation verbose type
	 */
	protected static String ALLOCATION_VERBOSE_TYPE = "ALLOCATION_VERBOSE_TYPE";

	/**
	 * Garbage collection verbose type
	 */
	protected static String GC_VERBOSE_TYPE = "GC_VERBOSE_TYPE";

	/**
	 * Detailed Garbage Collection verbose type
	 */
	protected static String DETAILED_GC_VERBOSE_TYPE = "DETAILED_GC_VERBOSE_TYPE";

	/**
	 * Class loading, creation and initialization verbose type
	 */
	protected static String CLASS_VERBOSE_TYPE = "CLASS_VERBOSE_TYPE";

	/**
	 * Detailed Class loading, creation and initialization verbose type
	 */
	protected static String DETAILED_CLASS_VERBOSE_TYPE = "DETAILED_CLASS_VERBOSE_TYPE";

	/**
	 * KVM verifier verbose type
	 */
	protected static String VERIFIER_VERBOSE_TYPE = "VERIFIER_VERBOSE_TYPE";

	/**
	 * Byte code execution verbose type
	 */
	protected static String BYTE_CODE_VERBOSE_TYPE = "BYTE_CODE_VERBOSE_TYPE";

	/**
	 * Thrown Exceptions verbose type
	 */
	protected static String EXCEPTIONS_VERBOSE_TYPE = "EXCEPTIONS_VERBOSE_TYPE";

	/**
	 * Threading status verbose type
	 */
	protected static String THREADING_VERBOSE_TYPE = "THREADING_VERBOSE_TYPE";

	/*
	 These options are not supported by HotSpot.
	 */
	//  /**
	//   * Stack maps verbose type
	//   */
	//  protected static final String STACKMAPS_VERBOSE_TYPE = "stackmaps";
	//
	//  /**
	//   * Methods entry and exit verbose type
	//   */
	//  protected static final String METHODS_VERBOSE_TYPE = "methods";
	//
	//  /**
	//   * Detailed Methods entry and exit verbose type
	//   */
	//  protected static final String DETAILED_METHODS_VERBOSE_TYPE = "methodsverbose";
	//
	//  /**
	//   * Stack frames pop and push verbose type
	//   */
	//  protected static final String STACK_FRAMES_VERBOSE_TYPE = "frames";
	//
	//  /**
	//   * Stack chunks creation verbose type
	//   */
	//  protected static final String STACK_CHUNKS_VERBOSE_TYPE = "stackchunks";
	//
	//  /**
	//   * Print events verbose type
	//   */
	//  protected static final String EVENTS_VERBOSE_TYPE = "events";
	//
	//  /**
	//   * Monitors enter and exit verbose type
	//   */
	//  protected static final String MONITORS_VERBOSE_TYPE = "monitors";
	//
	//  /**
	//   * Networking related methods verbose type
	//   */
	//  protected static String NETWORKING_VERBOSE_TYPE = "networking";
	/**
	 * Enable Runtime debugging argument (JDWP argument must also be specified)
	 */
	protected static String DEBUG_ARG = "DEBUG_ARG";

	/**
	 * JDWP session argument (xdebug argument must also be specified)
	 */
	protected static String JDWP_ARG = "JDWP_ARG";

	/**
	 * JDWP transport mechanism type
	 */
	protected static String JDWP_TRANSPORT_TYPE = "JDWP_TRANSPORT_TYPE";

	/**
	 * JDWP address type
	 */
	protected static String JDWP_ADDRESS_TYPE = "JDWP_ADDRESS_TYPE";

	/**
	 * Class path SEI argument
	 */
	private static String CLASS_PATH_ARG = "CLASS_PATH_ARG";

	/**
	 * Class path SEI argument
	 */
	private static final String BOOSTRAPCLASSES_ARG = "-bootstrapclasses";

	/**
	 * Properties SEI argument
	 */
	private static String PROPERTY_ARG = "PROPERTY_ARG";

	/**
	 * JDWP Server debug agent type
	 */
	private static String JDWP_SERVER_TYPE = "JDWP_SERVER_TYPE";

	/**
	 * JDWP suspend type
	 */
	private static String JDWP_SUSPEND_TYPE = "JDWP_SUSPEND_TYPE";

	/**
	 * Running device argument
	 */
	private static String DEVICE_ARG = "DEVICE_ARG";

	/**
	 * Heap size argument
	 */
	private static String HEAP_SIZE_ARG = "HEAP_SIZE_ARG";

	/**
	 * Descriptor SEI argument
	 */
	protected static String DESCRIPTOR_ARG = "DESCRIPTOR_ARG";

	/**
	 * Descriptor SEI argument
	 */
	protected static String OLD_DESCRIPTOR_ARG = "OLD_DESCRIPTOR_ARG";

	/**
	 * Set of all the Verbose parameters constants
	 */
	private static final Set sVerboseConstantsSet = new HashSet();

	/**
	 * Set of all the JDWP sub options parameters constants
	 */
	private static final Set sJDWPSubOptionsConstantsSet = new HashSet();

	/**
	 * Set of all the queries arguments
	 */
	private static final Set sQueriesConstantsSet = new HashSet();

	//
	// Members
	//

	/**
	 * The command line original arguments
	 */
	protected String[] iArgs;

	/**
	 * Currently parsed argument index
	 */
	protected int iIndex = 0;

	/**
	 * Array of parsed class path (could be Jars or foldes)
	 */
	private String[] iClassPath;

	/**
	 * Bootstrap classes
	 */
	private String iBoostrapClasses;

	/**
	 * Table of properties mapped by the property name.
	 * -D<property>=<value>
	 */
	private Map iSystemProperties = new Hashtable();

	/**
	 * Query argument. incase of a single argument command.
	 */
	private String iQuery;

	/**
	 * Device name
	 */
	private String iDeviceName;

	/**
	 * Heap size
	 */
	private int iHeapSize;

	/**
	 * Descriptor file
	 */
	private File iDescriptor;

	/**
	 * Verbose parameters indicators set
	 */
	private Set iVerboseIndicators = new HashSet();

	/**
	 * JDWP sub options indicators set
	 */
	private Map iJDWPOptions = new HashMap();

	/**
	 * Debug argument indicator
	 */
	private boolean iDebug;

	/**
	 * Entry class name.
	 */
	private String iApplication;

	/**
	 * Array of unrecognized arguments
	 * Could be usefull for subclasses to add their own specialized parsing.
	 */
	private String[] iUnrecognizedArgs = new String[0];

	//
	// Static initializer
	//

	static
	{
		//map SEI arguments to "licensed"-licensee argument
		try
		{
			Properties seiArgs = new Properties();
			InputStream is;
			if (System.getProperty("sdk.configuration.seiarguments.file") != null)
			{
				is = new FileInputStream(System
						.getProperty("sdk.configuration.seiarguments.file"));
			} else if (System
					.getProperty("sdk.configuration.seiarguments.bundle") != null)
			{
				is = SEICommand.class.getResourceAsStream(System
						.getProperty("sdk.configuration.seiarguments.bundle"));
			} else
			{
				is = SEICommand.class.getResourceAsStream(DEF_SEI_ARGS);
			}

			seiArgs.load(is);
			VERSION_ARG = seiArgs.getProperty(VERSION_ARG);
			HELP_ARG = seiArgs.getProperty(HELP_ARG);
			QUERY_ARG = seiArgs.getProperty(QUERY_ARG);
			VERBOSE_ARG = seiArgs.getProperty(VERBOSE_ARG);
			ALL_VERBOSE_TYPE = seiArgs.getProperty(ALL_VERBOSE_TYPE);
			ALLOCATION_VERBOSE_TYPE = seiArgs
					.getProperty(ALLOCATION_VERBOSE_TYPE);
			GC_VERBOSE_TYPE = seiArgs.getProperty(GC_VERBOSE_TYPE);
			DETAILED_GC_VERBOSE_TYPE = seiArgs
					.getProperty(DETAILED_GC_VERBOSE_TYPE);
			CLASS_VERBOSE_TYPE = seiArgs.getProperty(CLASS_VERBOSE_TYPE);
			DETAILED_CLASS_VERBOSE_TYPE = seiArgs
					.getProperty(DETAILED_CLASS_VERBOSE_TYPE);
			VERIFIER_VERBOSE_TYPE = seiArgs.getProperty(VERIFIER_VERBOSE_TYPE);
			BYTE_CODE_VERBOSE_TYPE = seiArgs
					.getProperty(BYTE_CODE_VERBOSE_TYPE);
			EXCEPTIONS_VERBOSE_TYPE = seiArgs
					.getProperty(EXCEPTIONS_VERBOSE_TYPE);
			THREADING_VERBOSE_TYPE = seiArgs
					.getProperty(THREADING_VERBOSE_TYPE);
			DEBUG_ARG = seiArgs.getProperty(DEBUG_ARG);
			JDWP_ARG = seiArgs.getProperty(JDWP_ARG);
			JDWP_TRANSPORT_TYPE = seiArgs.getProperty(JDWP_TRANSPORT_TYPE);
			JDWP_ADDRESS_TYPE = seiArgs.getProperty(JDWP_ADDRESS_TYPE);
			CLASS_PATH_ARG = seiArgs.getProperty(CLASS_PATH_ARG);
			PROPERTY_ARG = seiArgs.getProperty(PROPERTY_ARG);
			JDWP_SERVER_TYPE = seiArgs.getProperty(JDWP_SERVER_TYPE);
			JDWP_SUSPEND_TYPE = seiArgs.getProperty(JDWP_SUSPEND_TYPE);
			DEVICE_ARG = seiArgs.getProperty(DEVICE_ARG);
			HEAP_SIZE_ARG = seiArgs.getProperty(HEAP_SIZE_ARG);
			OLD_DESCRIPTOR_ARG = seiArgs.getProperty(OLD_DESCRIPTOR_ARG);
			DESCRIPTOR_ARG = seiArgs.getProperty(DESCRIPTOR_ARG);
			is.close();
		} catch (Exception e)
		{
			e.printStackTrace();
			throw new RuntimeException("could not initialize SEICommand: "
					+ e.getMessage());
		}

		//initialize verbose parameters set
		sVerboseConstantsSet.add(ALL_VERBOSE_TYPE);
		sVerboseConstantsSet.add(ALLOCATION_VERBOSE_TYPE);
		sVerboseConstantsSet.add(GC_VERBOSE_TYPE);
		sVerboseConstantsSet.add(DETAILED_GC_VERBOSE_TYPE);
		sVerboseConstantsSet.add(CLASS_VERBOSE_TYPE);
		sVerboseConstantsSet.add(DETAILED_CLASS_VERBOSE_TYPE);
		sVerboseConstantsSet.add(VERIFIER_VERBOSE_TYPE);
		sVerboseConstantsSet.add(BYTE_CODE_VERBOSE_TYPE);
		sVerboseConstantsSet.add(EXCEPTIONS_VERBOSE_TYPE);
		sVerboseConstantsSet.add(THREADING_VERBOSE_TYPE);
		/*
		 These options are not supported by HotSpot.
		 */
		//    sVerboseConstantsSet.add(STACKMAPS_VERBOSE_TYPE);
		//    sVerboseConstantsSet.add(METHODS_VERBOSE_TYPE);
		//    sVerboseConstantsSet.add(DETAILED_METHODS_VERBOSE_TYPE);
		//    sVerboseConstantsSet.add(STACK_FRAMES_VERBOSE_TYPE);
		//    sVerboseConstantsSet.add(STACK_CHUNKS_VERBOSE_TYPE);
		//    sVerboseConstantsSet.add(EVENTS_VERBOSE_TYPE);
		//    sVerboseConstantsSet.add(MONITORS_VERBOSE_TYPE);
		//    sVerboseConstantsSet.add(NETWORKING_VERBOSE_TYPE);
		//initialize debug sub options set
		sJDWPSubOptionsConstantsSet.add(JDWP_TRANSPORT_TYPE);
		sJDWPSubOptionsConstantsSet.add(JDWP_ADDRESS_TYPE);
		sJDWPSubOptionsConstantsSet.add(JDWP_SERVER_TYPE);
		sJDWPSubOptionsConstantsSet.add(JDWP_SUSPEND_TYPE);

		//initialize queries set
		sQueriesConstantsSet.add(VERSION_ARG);
		sQueriesConstantsSet.add(HELP_ARG);
		sQueriesConstantsSet.add(QUERY_ARG);
	}

	//
	// Constructors
	//

	/**
	 * Constructor.
	 *
	 * @param aArgs original command line arguments
	 */
	public SEICommand(String[] aArgs)
	{
		iArgs = aArgs;
		parse();
	}

	//
	// Methods
	//

	/**
	 * Return the String representation of the command
	 *
	 * @return The command String array arguments
	 */
	public String[] getArguments()
	{
		return iArgs;
	}

	/**
	 * Initialize data members by parsing the original line.
	 *
	 * NOTE: by the SEI specification, the SEI must ignore any unknown command
	 * line arguments.
	 */
	protected void parse()
	{

		if (iArgs.length == 0)
		{
			//this is for command line users. no IDE would launch emulator.exe without arguments.
			iQuery = HELP_ARG;
		} else
		{
			while (iIndex < iArgs.length)
			{
				if (!(parseAsQuery() || parseAsArgument() || (iIndex == iArgs.length - 1 && parseAsApplication())))
				{
					//argument was not recognized
					handleUnrecognizedArgument();
				}
				iIndex++;
			}
		}
	}

	/**
	 * Handle the current parsed argumentm which is unrecognized.
	 * The default implementation is storing all unrecognized arguments
	 * in a member array.
	 */
	protected void handleUnrecognizedArgument()
	{
		String[] tmp = new String[iUnrecognizedArgs.length + 1];
		System
				.arraycopy(iUnrecognizedArgs, 0, tmp, 0,
						iUnrecognizedArgs.length);
		iUnrecognizedArgs = tmp;
		iUnrecognizedArgs[iUnrecognizedArgs.length - 1] = iArgs[iIndex];
	}

	/**
	 * Attempt to parse the current argument as a query
	 *
	 * @return true if the current argument was identified as a query
	 */
	private boolean parseAsQuery()
	{
		if (sQueriesConstantsSet.contains(iArgs[iIndex]))
		{
			iQuery = iArgs[iIndex];
			return true;
		}
		return false;
	}

	/**
	 * Attempt to parse a recognized argument
	 *
	 * @return true if was recognized as an argument
	 */
	private boolean parseAsArgument()
	{
		String token = iArgs[iIndex];
		if (token.equals(CLASS_PATH_ARG))
		{
			iClassPath = parseTokens(iArgs[iIndex + 1], CLASS_PATH_DELIMITERS);
			iIndex++;
		} else if (token.startsWith(PROPERTY_ARG))
		{
			parseProperty(iSystemProperties, token.substring(PROPERTY_ARG
					.length()));
		} else if (token.startsWith(VERBOSE_ARG))
		{
			parseVerboseTypes(token);
		} else if (token.equalsIgnoreCase(DEBUG_ARG))
		{
			iDebug = true;
		} else if (token.startsWith(JDWP_ARG))
		{
			parseDebugOptions(token.substring(token.indexOf(COLON) + 1));
		} else if (token.startsWith(DEVICE_ARG))
		{
			iDeviceName = token.substring(token.indexOf(COLON) + 1);
		} else if (token.startsWith(HEAP_SIZE_ARG))
		{
			parseHeapSize(token.substring(token.indexOf(COLON) + 1));
		} else if (token.startsWith(DESCRIPTOR_ARG))
		{
			//Pranav: Added to solve eclipse problem to run the midlet using jad file path
			String descPath = token.substring(token.indexOf(COLON) + 1);
			if (descPath.startsWith("file:")){
				descPath = descPath.substring(descPath.indexOf("file:")+ 6);
			}
			iDescriptor = new File(descPath);
			//end
			//iDescriptor = new File(token.substring(token.indexOf(COLON) + 1));
		} else if (token.equals(OLD_DESCRIPTOR_ARG))
		{

			String fileName = iArgs[++iIndex];
			
			//Pranav: Added to solve eclipse problem to run the midlet using jad file path
			if (fileName.startsWith("file:")){
				fileName = fileName.substring(fileName.indexOf("file:")+ 6);
			}
			//end
			iDescriptor = new File(fileName);
		} else if (token.equals(BOOSTRAPCLASSES_ARG))
		{
			iBoostrapClasses = iArgs[iIndex + 1];
			iIndex++;
		} else
		{
			//was not parsed as a valid argument
			return false;
		}
		return true;
	}

	/**
	 * Validate the current argument has a valid class name form
	 *
	 * @return true if the current argument has a valid class name form
	 */
	private boolean parseAsApplication()
	{
		//cuould be nice to validate the name somehow..
		//but there is no trivial way in Java. (Class.forName wouldnt do the trick)
		if (Utilities.isValidClassName(iArgs[iArgs.length - 1]))
		{
			iApplication = iArgs[iArgs.length - 1];
			return true;
		}
		return false;
	}

	/**
	 * Parse a single property
	 *
	 * @param aToken property token of the form -D<property>=<value>
	 * @param aMap the map to put in
	 */
	private void parseProperty(Map aMap, String aToken)
	{
		int equalsIndex = aToken.indexOf(EQUALS);
		aMap.put(aToken.substring(0, equalsIndex), aToken
				.substring(equalsIndex + 1));
	}

	/**
	 * Parse heap size.
	 * The number can be written as a plain number to signify bytes.
	 * 'k' or 'm' may be appended to signify kilo-bytes or mega-bytes respectively
	 *
	 * @param aSizeToken string with heap size token
	 */
	private void parseHeapSize(String aSizeToken)
	{
		char c = aSizeToken.charAt(aSizeToken.length() - 1);
		if (c == M_CHAR)
		{
			iHeapSize = MEGA
					* Integer.parseInt(aSizeToken.substring(0, aSizeToken
							.length() - 2));
		} else
		{
			if (c == K_CHAR)
			{
				iHeapSize = KILO
						* Integer.parseInt(aSizeToken.substring(0, aSizeToken
								.length() - 2));

			} else
			{
				iHeapSize = Integer.parseInt(aSizeToken);
			}
		}
	}

	/**

	 /**
	 * Parse argument to an array of parameters
	 *
	 * @param aArg argument to parse
	 * @param aDelims delimiters
	 *
	 * @return String array of argument parameters
	 */
	private String[] parseTokens(String aArg, String aDelims)
	{
		StringTokenizer paramsTokenizer = new StringTokenizer(aArg, aDelims);
		Vector tmp = new Vector();
		while (paramsTokenizer.hasMoreTokens())
		{
			tmp.addElement(paramsTokenizer.nextToken());
		}
		String[] params = new String[tmp.size()];
		tmp.toArray(params);
		return params;
	}

	/**
	 * Parse verbose types
	 *
	 * @param token argument
	 */
	private void parseVerboseTypes(String aToken)
	{
		if (aToken.indexOf(COLON) == -1)
		{
			iVerboseIndicators.add(ALL_VERBOSE_TYPE);
		} else
		{
			String[] types = parseTokens(aToken
					.substring(aToken.indexOf(COLON) + 1), COMMA);

			for (int i = 0; i < types.length; i++)
			{
				if (sVerboseConstantsSet.contains(types[i]))
				{
					iVerboseIndicators.add(types[i]);
				}
			}
		}
	}

	/**
	 * Parse JDWP sub options
	 *
	 * @param aToken token containing jdwp options
	 */
	private void parseDebugOptions(String aToken)
	{
		StringTokenizer tokenizer = new StringTokenizer(aToken, JDWP_DELIMITERS);
		String token;
		while (tokenizer.hasMoreTokens())
		{
			token = tokenizer.nextToken();
			if (sJDWPSubOptionsConstantsSet.contains(token))
			{
				iJDWPOptions.put(token, tokenizer.nextToken());
			}
		}
	}

	/**
	 * Get the Class path
	 *
	 * @return String array of class paths
	 */
	public String[] getClassPath()
	{
		return iClassPath;
	}

	/**
	 * Get the Bootstrap classes
	 *
	 * @return Bootstrap classes
	 */
	public String getBootstrapClasses()
	{
		return iBoostrapClasses;
	}

	/**
	 * Get the entry MIDlet class name
	 *
	 * @return class name
	 */
	public String getApplication()
	{
		return iApplication;
	}

	/**
	 * Get the descriptor file
	 *
	 * @return descriptor file
	 */
	public File getDescriptor()
	{
		return iDescriptor;
	}

	/**
	 * Returns true if the command is a query command
	 *
	 * @return true if the command is a query command
	 */
	public boolean isQuery()
	{
		return iQuery != null;
	}

	/**
	 * Returns the command query argument
	 *
	 * @return query argument
	 */
	public String getQuery()
	{
		return iQuery;
	}

	/**
	 * Get the debug options map
	 *
	 * @return Map of debug options
	 */
	protected Map getDebugOptions()
	{
		return iJDWPOptions;
	}

	/**
	 * Get VM debug port.
	 * This is the port number for the agent to connect to CLIENT IDE
	 * That paramter comes from the IDE and is defined by the SEI specifications.
	 *
	 * @return int debug port
	 */
	public int getVMRemotePort()
	{
		//EclipseME uses "localhost:[port]"
		String arg = (String) iJDWPOptions.get(JDWP_ADDRESS_TYPE);
		return Integer.parseInt(arg.substring(arg.indexOf(COLON) + 1));
	}
	/**
	 * Get VM debug host.
	 * This is the host for the agent to connect to CLIENT IDE
	 * That paramter comes from the IDE and is defined by the SEI specifications.
	 *
	 * @return int debug port
	 */
	public String getVMRemoteHost()
	{
		String arg = (String) iJDWPOptions.get(JDWP_ADDRESS_TYPE);
		return arg.substring(0, arg.indexOf(COLON));
	}

	/**
	 * Indicates if the KDP is in server mode
	 *
	 * @return Indicates if the KDP is in server mode
	 */
	public boolean isEmulatorServerMode()
	{
		return ((String) iJDWPOptions.get(JDWP_SERVER_TYPE))
				.equalsIgnoreCase("y");

	}

	/**
	 * Returns a set of constant verbose types string
	 *
	 * @return set of constant verbose types string
	 */
	public Set getVerboseTypes()
	{
		return iVerboseIndicators;
	}

	/**
	 * Get the system properties
	 *
	 * @return Map of system properties
	 */
	public Map getSystemProperties()
	{
		return iSystemProperties;
	}

	/**
	 * Get the Array of unrecognized arguments.
	 * The returned array contains zero or more elements but never null.
	 *
	 * @return String Array of unrecognized arguments
	 */
	public String[] getUnrecognizedArguments()
	{
		return iUnrecognizedArgs;
	}

	/**
	 * Returns true if the IDE is in debug mode
	 *
	 * @return debug mode indicator
	 */
	public boolean isDebugMode()
	{
		return iDebug;
	}

	/**
	 * Return the device name
	 *
	 * @return String device name
	 */
	public String getDeviceName()
	{
		return iDeviceName;
	}
}
