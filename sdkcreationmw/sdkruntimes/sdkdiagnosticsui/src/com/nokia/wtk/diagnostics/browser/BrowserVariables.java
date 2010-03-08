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






package com.nokia.wtk.diagnostics.browser;


import java.util.Vector;
import java.util.EventListener;
import javax.swing.event.EventListenerList;


import com.nokia.wtk.device.DeviceWMLBrowser;


/**
 * Browser context variables.
 * These are relevant to WML 1.x browsers.
 *
 */
public class BrowserVariables {

    /**
     * Creates a new <code>BrowserVariables</code>.
     */
    BrowserVariables() {
        this.variableNames = new Vector();
        this.variableValues = new Vector();
    }

    /**
     * Sets a new <code>DeviceWMLBrowser</code> instance.
     * @param wmlBrowser    the WML browser instance
     */
    void setWMLBrowser(DeviceWMLBrowser wmlBrowser) {
        this.wmlBrowser = wmlBrowser;
    }

    /**
     * Gets the current <code>DeviceWMLBrowser</code> instance.
     * @return          the current <code>DeviceWMLBrowser</code> or <code>null</code>.
     */
    private DeviceWMLBrowser getWMLBrowser() {
        return this.wmlBrowser;
    }

    /**
     * @returns <code>true</code> if the current device supports
     * the current push inbox messages operation.
     */
    public boolean isSetBrowserVariableSupported() {
        return Browser.isMethodSupported(getWMLBrowser(),
                                         "setContextVar",
                                         new Class[] {String.class, String.class});
    }

    /**
     * Assigns a value to a WML browser context variable.
     * If a variable does not exist, then it will be created.
     * If the value is an empty string or <code>null</code>,
     * then the variable will be removed.
     * <p>
     * This affects the actual current device browser and not
     * this model.  This model will only be affected indirectly
     * as a result of an event notification.
     *
     * @param name          the name of the context variable.
     * @param value         the value to assign.
     * @throws              com.nokia.wtk.diagnostics.browser.BrowserVariables.InvalidVariableNameException
     *                      if the variable name is badly formatted, per WML specifications.
     * @throws              com.nokia.wtk.diagnostics.browser.BrowserVariables.InvalidVariableValueException
     *                      if the variable value is badly formatted, per WML specifications.
     */
    public synchronized void setBrowserVariable(String name, String value)
                             throws InvalidVariableNameException,
                                    InvalidVariableValueException {
        if (!isValidVariableName(name)) {
            throw new InvalidVariableNameException(name);
        }
        if ((value != null) || (value.length() > 0)) {
            if (!isValidVariableValue(value)) {
                throw new InvalidVariableValueException(value);
            }
        }
        if (isSetBrowserVariableSupported()) {
            try { getWMLBrowser().setContextVar(name, value); }
            catch (UnsupportedOperationException ex) {}
        }
    }

    /**
     * Determine if the given string represents a valid WML variable name.
     * @param name          the input string
     * @return              <code>true</code> if the variable name is valid; false otherwise
     */
    private static boolean isValidVariableName(String name) {
        boolean valid = false;
        if ((name != null) && (name.length() > 0)) {
            valid = true;
            for (int index = 0; index < name.length(); index++) {
                char ch = name.charAt(index);
                // First character must be an ASCII letter or underscore
                // Remaining characters may also be ASCII numeric
                if (!((ch == '_')
                   || ((ch >= 'a') && (ch <= 'z'))
                   || ((ch >= 'A') && (ch <= 'Z'))
                   || ((ch >= '0') && (ch <= '9') && (index > 0)))) {
                    valid = false;
                    break;
                }
            }
        }
        return valid;
    }

    /**
     * Determine if the given string represents a valid WML variable value.
     * Basically, a variable value must be legal XML CDATA.
     * A <code>null</code> or empty value is also considered valid.
     * @param value         the value to test
     * @return              <code>true</code> if the variable name is valid; false otherwise
     */
    private static boolean isValidVariableValue(String value) {
        boolean valid = false;
        if ((value != null) && (value.length() > 0)) {
            valid = true;
            for (int index = 0; index < value.length(); index++) {
                char ch = value.charAt(index);
                // wierd compiler bug: cannot handle unicode escaped constant 000A or 000D
                // workaround: use regular character escapes (\t, \r, \n)
                if (!((ch == '\t')
                   || (ch == '\r')
                   || (ch == '\n')
                   || ((ch >= '\u0020') && (ch <= '\uD7FF'))
                   || ((ch >= '\uE000') && (ch <= '\uFFFD')))) {
                    valid = false;
                    break;
                }
            }
        } else {    // null or empty is considered valid, but handled elsewhere
            valid = true;
        }
        return valid;
    }


    /**
     * Called when a context variable has been set, changed or deleted.
     * @param name          the name of the context variable.
     * @param value         the value of the context variable.
     *                      (may be empty or <code>null</code> if deleted)
     */
    void setVariable(String name, String value) {
        synchronized (this) {
            int index = this.variableNames.indexOf(name);
            if (index >= 0) {
                if ((value != null) && (value.length() > 0)) {
                    this.variableValues.setElementAt(value, index);
                    // Event Notification:
                    fireVariableChanged(index);
                } else {    // Unset existing variable:
                    this.variableNames.remove(index);
                    this.variableValues.remove(index);
                    // Event Notification:
                    fireVariableRemoved(index);
                }
            } else {
                if ((value != null) && (value.length() > 0)) {
                    this.variableNames.add(name);
                    this.variableValues.add(value);
                    // Event Notification:
                    fireVariableAdded(getVariableCount() - 1);
                }
            }
        }
    }


    /**
     * Called when all context variables have been deleted.
     * This will occur as part of a 'newcontext' operation.
     */
    void removeAll() {
        synchronized (this) {
            this.variableNames.removeAllElements();
            this.variableValues.removeAllElements();
        }
        // Event Notification:
        fireAllVariablesRemoved();
    }

    /**
     * Gets the number of variables.
     * @return              the number of variables.
     */
    public int getVariableCount() {
        return this.variableNames.size();
    }

    /**
     * Gets a variable name at a specified list index.
     * @param index the list location.
     * @return              the variable name, or <code>null</code> if not defined.
     */
    public String getVariableNameAt(int index) {
        String v = null;
        if ((index >= 0) && (index < getVariableCount())) {
            v = (String)this.variableNames.elementAt(index);
        }
        return v;
    }

    /**
     * Gets a variable value at a specified list index.
     * @param index         the list location.
     * @return              the variable value, or <code>null</code> if not defined.
     */
    public String getVariableValueAt(int index) {
        String v = null;
        if ((index >= 0) && (index < getVariableCount())) {
            v = (String)this.variableValues.elementAt(index);
        }
        return v;
    }

    /**
     * Adds a listener to the list that's notified each time
     * a change to the variable list occurs.
     * @param l             the <code>VariableListListener</code>
     */
    public void addVariableListListener(VariableListListener l) {
        if (this.listenerList == null) {
            this.listenerList = new EventListenerList();
        }
        this.listenerList.add(VariableListListener.class, l);
    }

    /**
     * Removes a listener from the list that's notified each time
     * a change to the variable list occurs.
     * @param l             the <code>VariableListListener</code>
     */
    public void removeVariableListListener(VariableListListener l) {
        if (this.listenerList != null) {
            this.listenerList.remove(VariableListListener.class, l);
        }
    }

    /*
     * Notify all listeners that have registered interest for
     * notification on this event type.
     * @param index         the variable list index.
     */
    private void fireVariableAdded(int index) {
        if (this.listenerList != null) {
            // Following is guaranteed to return a non-null array:
            Object[] listeners = this.listenerList.getListenerList();
            // Process the listeners first to last, notifying
            // those that are interested in this event
            for (int i = 0; i < listeners.length; i += 2) {
                if (listeners[i] == VariableListListener.class) {
                    ((VariableListListener)listeners[i+1]).variableAdded(index);
                }
            }
        }
    }

    /*
     * Notify all listeners that have registered interest for
     * notification on this event type.
     * @param index         the variable list index.
     */
    private void fireVariableRemoved(int index) {
        if (this.listenerList != null) {
            // Following is guaranteed to return a non-null array:
            Object[] listeners = this.listenerList.getListenerList();
            // Process the listeners first to last, notifying
            // those that are interested in this event
            for (int i = 0; i < listeners.length; i += 2) {
                if (listeners[i] == VariableListListener.class) {
                    ((VariableListListener)listeners[i+1]).variableRemoved(index);
                }
            }
        }
    }

    /*
     * Notify all listeners that have registered interest for
     * notification on this event type.
     */
    private void fireAllVariablesRemoved() {
        if (this.listenerList != null) {
            // Following is guaranteed to return a non-null array:
            Object[] listeners = this.listenerList.getListenerList();
            // Process the listeners first to last, notifying
            // those that are interested in this event
            for (int i = 0; i < listeners.length; i += 2) {
                if (listeners[i] == VariableListListener.class) {
                    ((VariableListListener)listeners[i+1]).allVariablesRemoved();
                }
            }
        }
    }

    /*
     * Notify all listeners that have registered interest for
     * notification on this event type.
     * @param index         the variable list index.
     */
    private void fireVariableChanged(int index) {
        if (this.listenerList != null) {
            // Following is guaranteed to return a non-null array:
            Object[] listeners = this.listenerList.getListenerList();
            // Process the listeners first to last, notifying
            // those that are interested in this event
            for (int i = 0; i < listeners.length; i += 2) {
                if (listeners[i] == VariableListListener.class) {
                    ((VariableListListener)listeners[i+1]).variableChanged(index);
                }
            }
        }
    }


    /**
     * A listener for browser variable list events.
     */
    public interface VariableListListener extends EventListener {
        /**
         * Called when a variable has been added to the list.
         *
         * @param index     the variable list index.
         */
        public void variableAdded(int index);

        /**
         * Called when a variable has been removed from the list.
         *
         * @param index     the variable list index.
         */
        public void variableRemoved(int index);

        /**
         * Called when all variables have been removed from the list.
         */
        public void allVariablesRemoved();

        /**
         * Called when a variable has been changed.
         *
         * @param index     the variable list index.
         */
        public void variableChanged(int index);
    }   // End of VariableListListener


    /**
     * An exception for badly-formatted WML varable names.
     */
    public class InvalidVariableNameException extends Exception {
        InvalidVariableNameException(String message) {
            super(message);
        }
    }   // End of InvalidVariableNameException
    
    
    /**
     * An exception for badly-formatted WML varable values.
     */
    public class InvalidVariableValueException extends Exception {
        InvalidVariableValueException(String message) {
            super(message);
        }
    }   // End of InvalidVariableValueException


    private DeviceWMLBrowser wmlBrowser = null;
    private Vector variableNames = null;
    private Vector variableValues = null;
    private EventListenerList listenerList;
}   // End of BrowserVariables
