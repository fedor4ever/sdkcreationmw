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




package com.nokia.epdt.plugins.s60.emulator;

import java.util.Observable;
import java.text.MessageFormat;
import java.text.ParseException;

/**
 * EmulatorModel
 *
 * @version 1.0
 */
public class EmulatorModel extends Observable {
    private static EmulatorModel emulatorModel = null;
    boolean isValid = true;
    // memory
    // just in time
    private boolean justInTime = false; // if false disabled
    // panics
    private boolean panics = false; // if false disabled
    // debug info
    private boolean debugInfo = false; // if false disabled

    // previous values
    private boolean prevJustInTime = false; // if false disabled
    private boolean prevPanics = false; // if false disabled
    private boolean prevDebugInfo = false; // if false disabled

    /**
     * Creates a new instance of EmulatorModel
     */
    private EmulatorModel() {
        super();
    }
    
    /**
     * Returns a new EmulatorModel instance and creates a new one if necessary.
     */
    public static EmulatorModel getInstance() {
        if ( emulatorModel == null ) {
            emulatorModel = new EmulatorModel();
        }
        return emulatorModel;
    }
    
    /**
     * Validate a values of the model
     * @throws ValidateExeption
     */
    public void validate() throws ValidateException {
        this.isValid = true;
        StringBuffer errorMessage = new StringBuffer();
        String result = null;
        this.setChanged();
        this.notifyObservers();
        // Check is valid
        if ( !this.isValid ) {
            throw new ValidateException(errorMessage.toString());
        }
    }
    
    /**
     * Validate memory
     * @return null if memory number is not valid else return modified memory value
     */
    public String validateMemory(String str) {
        MessageFormat memoryFormat = new MessageFormat("{0,number,integer}");
        String result = null;
        Object[] o = null;
        try {
            o = memoryFormat.parse(str);
            for ( int i = 0, l = o.length; i < l; i++ ) {
                if ( ((Long)o[i]).intValue() < 8 || ((Long)o[i]).intValue() > 256 ) {
                    throw new ParseException(null, 0);
                }
            }
            result = String.valueOf(((Long)o[0]).intValue());
        } catch (ParseException pe) {
            this.isValid = false;
        }
        return result;
    }
    
    /** Getter for property justInTime.
     * @return Value of property justInTime.
     *
     */
    public boolean isJustInTime() {
        return justInTime;
    }
    
    /** Setter for property justInTime.
     * @param justInTime New value of property justInTime.
     *
     */
    public void setJustInTime(boolean justInTime) {
        this.justInTime = justInTime;
        this.setChanged();
        this.notifyObservers();
    }
    
    /** Getter for property panics.
     * @return Value of property panics.
     *
     */
    public boolean isPanics() {
        return panics;
    }
    
    /** Setter for property panics.
     * @param panics New value of property panics.
     *
     */
    public void setPanics(boolean panics) {
        this.panics = panics;
        this.setChanged();
        this.notifyObservers();
    }
    
    /** Getter for property debugInfo.
     * @return Value of property debugInfo.
     *
     */
    public boolean isDebugInfo() {
        return debugInfo;
    }
    
    /** Setter for property debugInfo.
     * @param debugInfo New value of property debugInfo.
     *
     */
    public void setDebugInfo(boolean debugInfo) {
        this.debugInfo = debugInfo;
        this.setChanged();
        this.notifyObservers();
    }
    
    /**
     * Setter for previous values
     */
    public void setPreviousValues() {
    	justInTime = prevJustInTime;
        panics = prevPanics;
        debugInfo = prevDebugInfo;    	
        setChangedAndNotifyObservers();
    }
    
    /**
     * Apply previous values
     */
    public void applyPreviousValues() {
    	prevJustInTime = justInTime;
        prevPanics = panics;
        prevDebugInfo = debugInfo;    	
    }

    /**
     * Marks this Observable object as having been changed; the hasChanged method will now return true.
     * If this object has changed, as indicated by the hasChanged method, then notify all of its observers
     * and then call the clearChanged method to indicate that this object has no longer changed.
     */
    public void setChangedAndNotifyObservers() {
        this.setChanged();
        this.notifyObservers();
    }
}
