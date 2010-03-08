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


package com.nokia.epdt.plugins.s60.taskman;

import java.text.MessageFormat;
import java.text.ParseException;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.StringTokenizer;
import com.nokia.epdt.core.messaging.Message;

/**
 * TaskManagerMessageParser
 *
 * TaskManager diagnostic data message (from ecmt to epdt)
 * -----------------------------------------------------
 * <cpu><b><uptime><b><usage><b>
 * <drives><b><drive-A><b><drive-size><b><drive-free><;>...<drive-Z><b><drive-size><b><drive-free><;><b>
 * <ram><b><free-RAM><total-RAM><total-ROM><b>
 * <type><b><name><b><id><b><mem-used><b><mem-reserved><b><stack><b><cpu-time><;>
 *
 * where:
 * <b>		= blank character
 * <cpu>	= CPU
 * <uptime>	= time since boot in microseconds
 * <usage>	= CPU usage percentage, 0...100
 *
 * <drives>	= DRIVES
 * <drive-A>	= A
 * <drive-size>= total capacity of drive
 * <drive-free>= current free capacity on drive
 * <drive-B>	= B
 * ...
 * <drive-Z>	= Z
 *
 * <ram>	= RAM
 * <free-RAM>	= current amount of free RAM
 * <total-RAM>	= total RAM on device
 * <total-ROM>	= total ROM on device
 *
 * <type>	= APPLICATIONS or THREADS
 * <name>	= name of application/thread (contains no blanks, no ';')
 * <id>		= id of thread or id of primary thread of application
 * <mem-used>	= amount of memory used by application/thread
 * <mem-reserved> = amount of memory reserved for application/thread
 * <stack>	= stack size of application/thread
 * <cpu-time>	= thread/application CPU usage in microseconds
 *
 * Example TaskManager diagnostic data message (from ecmt to epdt)
 * -------------------------------------------------------------
 * CPU 20200000 0 DRIVES A 0 0;B 0 0;C 40015954432 13840715264;D 40015954432 13840715264;E 1037824 951808;F 0 0;G 0 0;H 0 0;I 0 0;J 0 0;K 0 0;L 0 0;M 0 0;N 0 0;O 0 0;P 0 0;Q 0 0;R 0 0;S 0 0;T 0 0;U 0 0;V 0 0;W 0 0;X 0 0;Y 0 0;Z 4194304 0; RAM 264454144 269484032 4194304 APPLICATIONS Menu 36 111272 135168 8192 343750;EiksrvBackdrop 18 288496 360448 8192 781250;
 *
 * @version 1.0
 */
class TaskManagerMessageParser {
    private static MessageFormat applicationsValueMessageFormat = new MessageFormat("CPU {0,number,integer} {1,number,integer} DRIVES {2} RAM {3,number,integer} {4,number,integer} {5,number,integer} APPLICATIONS {6}");
    private static MessageFormat threadsValueMessageFormat = new MessageFormat("CPU {0,number,integer} {1,number,integer} DRIVES {2} RAM {3,number,integer} {4,number,integer} {5,number,integer} THREADS {6}");
    private static MessageFormat driveValueMessageFormat = new MessageFormat("{0} {1,number,integer} {2,number,integer}");
    private static MessageFormat applicationValueMessageFormat = new MessageFormat("{0} {1,number,integer} {2,number,integer} {3,number,integer} {4,number,integer} {5,number,integer}");
    private static MessageFormat threadValueMessageFormat = new MessageFormat("{0} {1,number,integer} {2,number,integer} {3,number,integer} {4,number,integer} {5,number,integer}");

    /** Creates a new instance of TaskManagerMessageParser */
    private TaskManagerMessageParser() {
        super();
    }

    /**
     * Parse a message
     */
    static TaskManagerMessage parseMessage(Message message) {
        String body = "";
        double uptime = 0.0;
        int usage = 0;
        ArrayList driveList = new ArrayList(0);
        int free = 0;
        int totalRam = 0;
        int totalRom = 0;
        ArrayList applicationList = new ArrayList(0);
        ArrayList threadList = new ArrayList(0);
        try {
            body = new String(message.getBody(), "UTF8");
            Object[] o = null;
            try {
                o = applicationsValueMessageFormat.parse(body);
                try {
                    //CPU
                    uptime = ((Number)o[0]).doubleValue();
                    usage = ((Number)o[1]).intValue();
                    // DRIVES
                    driveList = parseDrives((String) o[2]);
                    // RAM
                    free = ((Number)o[3]).intValue();
                    totalRam = ((Number)o[4]).intValue();
                    totalRom = ((Number)o[5]).intValue();
                    // APPLICATIONS
                    applicationList = parseApplications((String) o[6]);
                } catch (ClassCastException cce) {
                    // parsing failed
                    throw new ParseException(null, 0);
                }
            } catch (ParseException pe_0) {
                // parsing failed
                try {
                    o = threadsValueMessageFormat.parse(body);
                    try {
                        //CPU
                        uptime = ((Number)o[0]).doubleValue();
                        usage = ((Number)o[1]).intValue();
                        // DRIVES
                        driveList = parseDrives((String) o[2]);
                        // RAM
                        free = ((Number)o[3]).intValue();
                        totalRam = ((Number)o[4]).intValue();
                        totalRom = ((Number)o[5]).intValue();
                        // THREADS
                        threadList = parseThreads((String) o[6]);
                    } catch (ClassCastException cce) {
                        // parsing failed
                        throw new ParseException(null, 0);
                    }
                } catch (ParseException pe_1) {
                    // parsing failed
                }
            }
        } catch (UnsupportedEncodingException uee) {
            // parsing failed
        }
        Cpu cpu = Cpu.create(uptime, usage);
        Memory memory = Memory.create(free, totalRam, totalRom);
        TaskManagerMessage taskManagerMessage = TaskManagerMessage.create(cpu, driveList,  memory, applicationList, threadList);
        return taskManagerMessage;
    }

    /**
     * Parse a drives
     */
    private static ArrayList parseDrives(String s) {
        ArrayList result = new ArrayList(0);
        String name = "";
        long size = 0;
        long free = 0;
        for ( StringTokenizer st = new  StringTokenizer(s, ";"); st.hasMoreTokens(); ) {
            Object[] o = null;
            try {
                o = driveValueMessageFormat.parse(st.nextToken());
                try {
                    // DRIVE
                    name = (String) o[0];
                    size = ((Number)o[1]).longValue();
                    free = ((Number)o[2]).longValue();
                    result.add(Drive.create(name, size, free));
                } catch (ClassCastException cce) {
                    // parsing failed
                    throw new ParseException(null, 0);
                }
            } catch (ParseException pe) {
                // parsing failed
            }
        }
        return result;
    }

    /**
     * Parse a applications
     */
    private static ArrayList parseApplications(String s) {
        ArrayList result = new ArrayList(0);
        for ( StringTokenizer st = new  StringTokenizer(s, ";"); st.hasMoreTokens(); ) {
            try {
                Object[] o = applicationValueMessageFormat.parse(st.nextToken());
                try {
                    // DRIVE
                    String name = (String) o[0];
                    int id = ((Number)o[1]).intValue();
                    int memoryUsed = ((Number)o[2]).intValue();
                    int memoryReserved = ((Number)o[3]).intValue();
                    int stack = ((Number)o[4]).intValue();
                    long cpuUsage = ((Number)o[5]).longValue();
                    result.add(new Application(name, id, memoryUsed, memoryReserved, stack, cpuUsage));
                } catch (ClassCastException cce) {
                    // parsing failed
                    throw new ParseException(null, 0);
                }
            } catch (ParseException pe) {
                // parsing failed
            }
        }
        return result;
    }

    /**
     * Parse a threads
     */
    private static ArrayList parseThreads(String s) {
        ArrayList result = new ArrayList(0);
        String name = "";
        int id = 0;
        int memoryUsed = 0;
        int memoryReserved = 0;
        int stack = 0;
        int cpuUsage = 0;
        for ( StringTokenizer st = new  StringTokenizer(s, ";"); st.hasMoreTokens(); ) {
            Object[] o = null;
            try {
                o = threadValueMessageFormat.parse(st.nextToken());
                try {
                    // DRIVE
                    name = (String) o[0];
                    id = ((Number)o[1]).intValue();
                    memoryUsed = ((Number)o[2]).intValue();
                    memoryReserved = ((Number)o[3]).intValue();
                    stack = ((Number)o[4]).intValue();
                    cpuUsage = ((Number)o[5]).intValue();
                    result.add(Thread.create(name, id, memoryUsed, memoryReserved, stack, cpuUsage));
                } catch (ClassCastException cce) {
                    // parsing failed
                    throw new ParseException(null, 0);
                }
            } catch (ParseException pe) {
                // parsing failed
            }
        }
        return result;
    }

}
