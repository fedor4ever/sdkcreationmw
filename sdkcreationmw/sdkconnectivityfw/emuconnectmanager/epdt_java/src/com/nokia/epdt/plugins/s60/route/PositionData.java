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

import java.util.*;

import com.nokia.epdt.core.messaging.Message;

/**
 * The data model for the Route plugin. A singleton-class, it stores
 * all the (geographical) coordinates as <code>Coordinate</code>
 * objects.
 *
 * @version 1.0
 */
class PositionData extends Observable {

    private static PositionData posData = null;
    private LinkedList coords;

    private String filename = null;

    /** Creates a new instance of PositionData */
    private PositionData() {
        super();
        coords = new LinkedList();
    }

    /**
     * Retrieves the instance (singleton) of this class
     *
     * @return The <code>PositionData</code> vinstance
     */
    public static PositionData getInstance() {
        if (posData == null)
            posData = new PositionData();
        return posData;
    }

    /**
     * Gets the value of filename
     *
     * @return the value of filename
     */
    public String getFilename() {
        return this.filename;
    }

    /**
     * Sets the value of filename
     *
     * @param argFilename Value to assign to this.filename
     */
    public void setFilename(String argFilename){
        this.filename = argFilename;
    }

    /**
     * Updates the angle of the previous coordinate to point at the
     * next coordinate to be added. Performed just before the new
     * coordinate is added to the coords-list.
     *
     * @param c The new coordinate to be added
     */
    private void updatePreviousAngle(Coordinate c) {
        try {
            Coordinate p = (Coordinate) coords.getLast();
            double dx = c.getLongitude() - p.getLongitude();
            double dy = c.getLatitude() - p.getLatitude();
            double angle = Math.atan(dx/dy);
            // 			System.out.println("lon: " + dx + " lat: " + dy + " angle: " +angle);

            if (dx > 0) {
                if (Double.isNaN(angle))
                    angle = Math.PI * 1.5;
                else if (dy < 0)
                    angle = Math.PI - angle; //angle is negative here
                else
                    angle = Math.PI * 2 - angle;
            } else {
                if (Double.isNaN(angle))
                    angle = Math.PI * 0.5;
                else if (dy < 0)
                    angle = Math.PI - angle;
                else
                    angle = -1*angle;
            }
            // 			System.out.println("angler: " + angle + " angled: " + Math.toDegrees(angle));

            p.setCourse((float) Math.toDegrees(angle));
        } catch (NoSuchElementException e) {}
    }


    /**
     * Retrieve the coordinates
     *
     * @return The internal representation of the coordinates
     * (reference, not copy)
     */
    public LinkedList getCoords() {
        return coords;
    }

    // ********** These are called by CoordPanel **********

    /**
     * Adds a coordinate to the model and notifies the observers.
     *
     * @param c The coordinate to add
     */
    public void addCoordinate(Coordinate c) {
        updatePreviousAngle(c);

        coords.add(c);
        this.setChanged();
        double[] newVals = new double[3];
        newVals[0] = 1f;
        newVals[1] = c.getLongitude();
        newVals[2] = c.getLatitude();
        this.notifyObservers(newVals);
    }

    /**
     * Replaces the last coordinate added with a new one and notifies
     * the observers.
     *
     * @param c The new coordinate replacing the old
     */
    public void replaceCoordinate(Coordinate c) {
        try {
            coords.removeLast();
            updatePreviousAngle(c);
        } catch (NoSuchElementException e) {}
        coords.add(c);
        this.setChanged();
        double[] newVals = new double[3];
        newVals[0] = -1f;
        newVals[1] = c.getLongitude();
        newVals[2] = c.getLatitude();
        this.notifyObservers(newVals);
    }


    // ********** These are called by PlotCanvas **********

    /**
     * Adds a coordinate just from the longitude and latitude, filling
     * in the blanks with either defaults or the settings for the
     * previous coordinate. Notifies the observers.
     *
     * @param lon Longitude
     * @param lat Latitude
     */
    public void addPoint(double lon, double lat) {
        Coordinate c;
        if (coords.size() == 0) {
            c = new Coordinate(1.5f, 2.5f,
                               1, 2, 3, lon, lat,
                               15f, 247f, 3, true);
        } else {
            Coordinate cPrev = (Coordinate) coords.getLast();
            c = cPrev.copy();
            c.setLongitude(lon);
            c.setLatitude(lat);
        }

        updatePreviousAngle(c);
        coords.add(c);
        this.setChanged();
        this.notifyObservers(c);
    }


    /**
     * Removes the last coordinate form the model and notifies the
     * observers.
     *
     */
    public void rmCoordinate() {
        try {
            coords.removeLast();
            this.setChanged();
            this.notifyObservers(coords.getLast());
        } catch (NoSuchElementException e) {}
    }


    /**
     * Removes all coordinate and notifies the observers.
     *
     */
    public void rmAll() {
        coords.clear();
        this.setChanged();
        this.notifyObservers();
    }

    /**
     * Creates a message to be sent to the emulator. The message simply contains
     * which file for the SimPSY Configurator to read. The full pathname is given,
     * as otherwise it would have to be added in the emulator plugin.
     *
     * @param uid The uid of the sending plugin
     * @return The message created
     */
    public Message createMessage(int uid) {
        StringBuffer sb = new StringBuffer();
        sb.append("PSYFILE");
        sb.append(" ");
        sb.append("c:\\documents\\SimPsyData\\" + this.filename);
        return new Message(uid , sb.toString().getBytes());
    }
}
