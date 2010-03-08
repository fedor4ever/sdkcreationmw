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


package com.nokia.epdt.plugins.s60.route;

import java.io.*;
import java.util.*;

/**
 * A class that writes coordinates to a file. The file must be
 * specified when an instance is created.
 *
 * @version 1.0
 */
public class CoordWriter {

    File spsFile;

    public CoordWriter(String filename) {
	this.spsFile = new File(filename);
	if (!this.spsFile.canWrite())
	    System.out.println("Writing prohibited");
    }

    public CoordWriter(File file) {
	this.spsFile = file;
	/*
	  if (!this.spsFile.canWrite())
	  System.out.println("Writing prohibited, fix");
	*/
    }

    /**
     * Computes the NMEA checksum. The checksum is the 8-bit XOR
     * of all characters in the sentence between $ and *.
     *
     * @param str The sentence to compute the checksum of
     * @return The checksum value
     * @exception Exception If for some reason the checksum computation fails
     */
    private static String compChecksum(String str) throws Exception {
	int check = 0;
	try {
	    byte[] chars = str.getBytes("US-ASCII");
	    for (int i=0; i < chars.length; i++)
		check ^= chars[i];

	} catch (Exception e) {
	    throw new Exception("NMEA checksum computation failed");
	    //System.out.println("Fatal error in conversion, exception " + e);
	}
	return Integer.toHexString(check).toUpperCase();
    }

    /**
     * Writes the given list of <code>Coordinate</code>s to the
     * file associated with this object.
     *
     * @param coords A list of <code>Coordinate</code>s to write
     * @param nmea Specifies whether to use NMEA-format (true) or SPS-format (false)
     * @exception Exception if an error occurs
     */
    public void write(LinkedList coords, boolean nmea) throws Exception {

	//iterate over list items
	//write each item (SPS)
	try {
	    FileOutputStream fstream = new FileOutputStream(spsFile);
	    Writer wout = new BufferedWriter(new OutputStreamWriter(fstream, "US-ASCII"));

	    int sec = 0;
	    int min = 0;
	    String minsec; // timestring to print (and modify) on each iteration
	    for (Iterator iter = coords.iterator(); iter.hasNext();) {
		Coordinate c = (Coordinate) iter.next();
		if (nmea) {

		    /* In order to have some movement, the coordinates are "sent"
		       10 seconds apart, starting from UTC 010000.00
		       More flexibility could be added if users want it. */
		    if (min < 10)
			minsec = "0" + min;
		    else
			minsec = Integer.toString(min);

		    if (sec < 10)
			minsec += "0" + sec;
		    else
			minsec += sec;

		    if ((sec + 10) == 60) {
			min++;
			sec = 0;
		    } else
			sec += 10;

		    // Sanity check
		    if (minsec.length() != 4)
			minsec = "0000";

		    //dates are hard-coded to 010704
		    //if this is restrictive, then changing it should be trivial

		    //lat should be ddmm.ss and lon dddmm.ss, see Coordinate.java
		    String gga = "GPGGA,01" + minsec + ".00," + c.getNmeaLatitude() + "," 
			+ (c.getLatitude() < 0 ? "S" : "N") + "," + c.getNmeaLongitude() + "," 
			+ (c.getLongitude() < 0 ? "W" : "E") + ",1,04," + c.getHorz() + ",1,M,,,,";
		    wout.write("$" + gga + "*" + compChecksum(gga) + "\r\n");

		    String gll = "GPGLL," + c.getNmeaLatitude() + "," 
			+ (c.getLatitude() < 0 ? "S" : "N") + "," + c.getNmeaLongitude() + "," 
			+ (c.getLongitude() < 0 ? "W" : "E") + ",01" + minsec + ".00,A,A";
		    wout.write("$" + gll + "*" + compChecksum(gll) + "\r\n");

		    String gsa = "GPGSA,A,3,01,02,03,04,,,,,,,,,1.0,2.0,3.0";
		    wout.write("$" + gsa + "*" + compChecksum(gsa) + "\r\n");

		    String rmc = "GPRMC,01" + minsec + ".00,A," + c.getNmeaLatitude() + "," 
			+ (c.getLatitude() < 0 ? "S" : "N") + "," + c.getNmeaLongitude() + "," 
			+ (c.getLongitude() < 0 ? "W" : "E") + "," + 1.94384*c.getSpeed() + ","  //we want the speed in knots from m/s
			+ c.getCourse() + ",010704,,,A";
		    wout.write("$" + rmc + "*" + compChecksum(rmc) + "\r\n");

		} else { // write SPS output
		    wout.write("Horizontal accuracy=" + c.getHorz() + ";\n");
		    wout.write("Vertical accuracy=" + c.getVert() + ";\n");
		    wout.write("TimeToFix min=" + c.getFixmin() + ";\n");
		    wout.write("TimeToFix max=" + c.getFixmax() + ";\n");
		    wout.write("Powerup time=" + c.getPowerup() + ";\n");

		    String lon = Double.toString(c.getLongitude());
		    if (lon.length() > 7) lon = lon.substring(0,7);
		    wout.write("Longitude=" + lon + ";\n");

		    String lat = Double.toString(c.getLatitude());
		    if (lat.length() > 7) lat = lat.substring(0,7);
		    wout.write("Latitude=" + lat + ";\n");

		    wout.write("Speed=" + c.getSpeed() + ";\n");

		    String course = Float.toString(c.getCourse());
		    if (course.length() > 7) course = course.substring(0,7);
		    wout.write("Course=" + course + ";\n");

		    if (c.isDeterministic())
			wout.write("Deterministic=" + c.getRandom() + ";\n");
		    else
			wout.write("Random=" + c.getRandom() + ";\n");
		    wout.write("\n");

		    //SPS can only take on coordinate/file
		    break;
		}
	    }
	    wout.close();
	    fstream.close();

	} catch (Exception e) {
	    System.out.println("Exception when opening stream: " + e);
	    throw new Exception("Cannot write to the given file\n" + e.getMessage());
	}
    }
}
