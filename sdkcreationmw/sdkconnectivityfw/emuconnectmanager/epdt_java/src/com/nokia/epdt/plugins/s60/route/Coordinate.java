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

/**
 * Representation of a Coordinate, modeled according to the SPS-format
 * used by the SimPSY Configurator.
 *
 * There are a few helper methods for making Coordinate-handling
 * easier.
 *
 * @version 1.0
 */
public class Coordinate {
    
    private float horz;
    private float vert;
    private int fixmin;
    private int fixmax;
    private int powerup;
    private double longitude;	
    private double latitude;
    private float speed;
    private float course;
    private int random;
    private boolean deterministic;

    /**
     * Gets the value of horz
     *
     * @return the value of horz
     */
    public float getHorz() {
	return this.horz;
    }

    /**
     * Sets the value of horz
     *
     * @param argHorz Value to assign to this.horz
     */
    public void setHorz(float argHorz){
	this.horz = argHorz;
    }
    
    /**
     * Gets the value of vert
     *
     * @return the value of vert
     */
    public float getVert() {
	return this.vert;
    }

    /**
     * Sets the value of vert
     *
     * @param argVert Value to assign to this.vert
     */
    public void setVert(float argVert){
	this.vert = argVert;
    }

    /**
     * Gets the value of fixmin
     *
     * @return the value of fixmin
     */
    public int getFixmin() {
	return this.fixmin;
    }

    /**
     * Sets the value of fixmin
     *
     * @param argFixmin Value to assign to this.fixmin
     */
    public void setFixmin(int argFixmin){
	this.fixmin = argFixmin;
    }

    /**
     * Gets the value of fixmax
     *
     * @return the value of fixmax
     */
    public int getFixmax() {
	return this.fixmax;
    }

    /**
     * Sets the value of fixmax
     *
     * @param argFixmax Value to assign to this.fixmax
     */
    public void setFixmax(int argFixmax){
	this.fixmax = argFixmax;
    }

    /**
     * Gets the value of powerup
     *
     * @return the value of powerup
     */
    public int getPowerup() {
	return this.powerup;
    }

    /**
     * Sets the value of powerup
     *
     * @param argPowerup Value to assign to this.powerup
     */
    public void setPowerup(int argPowerup){
	this.powerup = argPowerup;
    }

    /**
     * Gets the value of longitude
     *
     * @return the value of longitude
     */
    public double getLongitude() {
	return this.longitude;
    }

    /**
     * Sets the value of longitude
     *
     * @param argLongitude Value to assign to this.longitude
     */
    public void setLongitude(double argLongitude){
	this.longitude = argLongitude;
    }

    /**
     * Gets the value of latitude
     *
     * @return the value of latitude
     */
    public double getLatitude() {
	return this.latitude;
    }

    /**
     * Sets the value of latitude
     *
     * @param argLatitude Value to assign to this.latitude
     */
    public void setLatitude(double argLatitude){
	this.latitude = argLatitude;
    }

    /**
     * Gets the value of speed
     *
     * @return the value of speed
     */
    public float getSpeed() {
	return this.speed;
    }

    /**
     * Sets the value of speed
     *
     * @param argSpeed Value to assign to this.speed
     */
    public void setSpeed(float argSpeed){
	this.speed = argSpeed;
    }

    /**
     * Gets the value of course
     *
     * @return the value of course
     */
    public float getCourse() {
	return this.course;
    }

    /**
     * Sets the value of course
     *
     * @param argCourse Value to assign to this.course
     */
    public void setCourse(float argCourse){
	this.course = argCourse;
    }

    /**
     * Gets the value of random
     *
     * @return the value of random
     */
    public int getRandom() {
	return this.random;
    }

    /**
     * Sets the value of random
     *
     * @param argRandom Value to assign to this.random
     */
    public void setRandom(int argRandom){
	this.random = argRandom;
    }

    /**
     * Gets the value of deterministic
     *
     * @return the value of deterministic
     */
    public boolean isDeterministic() {
	return this.deterministic;
    }

    /**
     * Sets the value of deterministic
     *
     * @param argDeterministic Value to assign to this.deterministic
     */
    public void setDeterministic(boolean argDeterministic){
	this.deterministic = argDeterministic;
    }
    

    /**
     * Creates a new <code>Coordinate</code> instance.
     *
     * @param h Horizontal accuracy
     * @param v Vertical accuracy
     * @param fmin Minimum time to fix
     * @param fmax Maximum time to fix
     * @param power Powerup time
     * @param lon Longitude
     * @param lat Latitude
     * @param speed Speed
     * @param course Course
     * @param rnd Randomness
     * @param d Whether "randomness" is determninistic, or truly random
     */
    public Coordinate(float h, float v, int fmin, int fmax, int power, 
		      double lon, double lat, float speed, float course, int rnd,
		      boolean d) {
	this.horz = h;
	this.vert = v;
	this.fixmin = fmin;
	this.fixmax = fmax;
	this.powerup = power;
	this.longitude = lon;
	this.latitude = lat;
	this.speed = speed;
	this.course = course;
	this.random = rnd;
	this.deterministic = d;
    }

    /**
     * Makes a real copy (not superficial; no references) of this object
     *
     * @return A real copy of this object
     */
    public Coordinate copy() {
	Coordinate c = new Coordinate(horz, vert, fixmin, fixmax,
				      powerup, longitude, latitude,
				      speed, course, random, deterministic);
	return c;
    }


    /**
     * Returns the latitude in NMEA format (ddmm.decimals)
     *
     * @return The latitude in NMEA format
     */
    public String getNmeaLatitude() {
	double lat = Math.abs(this.latitude);
	int degr = (int) Math.floor(lat);

	// calculate the minutes
	lat = (lat - (double) degr) * 60;

	// format degrees
	String nmeaLat = Integer.toString(degr);
	if (nmeaLat.length() == 1)
	    nmeaLat = "0" + nmeaLat;

	if (lat < 10)
	    nmeaLat += "0";
	nmeaLat += lat;

	// we don't want useless decimals
	if (nmeaLat.length() > 10)
	    nmeaLat = nmeaLat.substring(0,10);

	return nmeaLat;
    }

    /**
     * Returns the longitude in NMEA format (ddmm.decimals)
     *
     * @return The longitude in NMEA format
     */
    public String getNmeaLongitude() {
	double lon = Math.abs(this.longitude);
	int degr = (int) Math.floor(lon);

	// calculate the minutes
	lon = (lon - (double) degr) * 60;

	// format degrees
	String nmeaLon = Integer.toString(degr);
	if (nmeaLon.length() == 1)
	    nmeaLon = "00" + nmeaLon;
	else if (nmeaLon.length() == 2)
	    nmeaLon = "0" + nmeaLon;

	if (lon < 10)
	    nmeaLon += "0";
	nmeaLon += lon;

	// we don't want useless decimals
	if (nmeaLon.length() > 10)
	    nmeaLon = nmeaLon.substring(0,10);

	return nmeaLon;
    }
}
