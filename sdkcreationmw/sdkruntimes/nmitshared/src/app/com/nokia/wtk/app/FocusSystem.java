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


package com.nokia.wtk.app;

/**
 * Title:        Nokia Mobile Internet Toolkit
 * Description:  A class to centralize all issues of following focus, like
 * which window is active and which Saveable has the focus now. It serves to
 * encapsulate and concentrate these focussing issues so that when JAVA
 * changes mechanisms -- as it will in 1.4 -- only one file will need to be
 * changed.
 * Company:      Nokia
 * @version 1.0
 */

import java.awt.AWTEvent;
import java.awt.Component;
import java.awt.Window;
import java.awt.event.AWTEventListener;
import java.awt.event.FocusEvent;
import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;

import javax.swing.JMenuItem;
import javax.swing.SwingUtilities;

import com.nokia.wtk.util.ArrayUtils;

public class FocusSystem {

    private PropertyChangeSupport focusSupport = new PropertyChangeSupport(this);

    // These three must be kept in sync with each other. The classes
    // array is an array of distinct classes to look for. The currentObjects
    // is equally long, and for each position j, currentObjects[j] contains
    // either null or a value of type classes[j] that has been found in
    // the last examination of the hierarchy.
    // foundObjects[j] stores the temporary results of the new examination of the
    // hierarchy, and each position j contains either null or an object of
    // the type classes[j]. When foundObjects has been processed, its values
    // are stored in cirrentObjects[j]
    private Class[] classes = new Class[0];
    private Object[] foundObjects = new Object[0];
    private Object[] currentObjects = new Object[0];

    private ApplicationWindow currentWindow;
    private Component focussedComponent;

    // don't want instantiations from outside, really
    FocusSystem() {
        startFocusListener();
    }

    private void findObjects(Component deepest) {
        int all = 0;

        for(Component c=deepest; c != null; c=c.getParent()) {
            for (int j = 0; j < classes.length; j++) {
                if (classes[j].isInstance(c) && (foundObjects[j] == null)) {
                    foundObjects[j] = c;
                    all++;
                    if (all == foundObjects.length) {
                        return;
                    }
                }
            }
        }
    }

    private void handleFocusSwitch(FocusEvent fe) {
        Window w = (Window) SwingUtilities.getRoot((Component) fe.getSource());
        if (w == null) {
            return;
        }
        Component c = (Component) fe.getSource();

        int j;
        if (c instanceof JMenuItem) {
            return;
        }
        focussedComponent = c;
        for (j = 0; j < foundObjects.length; j++) {
            foundObjects[j] = null;
        }
        findObjects(c);
        // Do notifications in the order the listener have been added
        for (j = 0; j < classes.length; j++) {
            if ((foundObjects[j] != null) || (currentObjects[j] != null)) {
                Object prev = currentObjects[j];
                Object now = foundObjects[j];
                currentObjects[j] = foundObjects[j];
                focusSupport.firePropertyChange(classes[j].getName(), prev, now);
            }
        }
    }

    private void startFocusListener() {
        java.awt.Toolkit.getDefaultToolkit().addAWTEventListener(
            new AWTEventListener() {

                final public void eventDispatched(AWTEvent event) {
                    FocusEvent fe = (FocusEvent) event;    // guaranteed to not crash
                    if (fe.getID() == FocusEvent.FOCUS_GAINED) {
                        handleFocusSwitch(fe);
                    }
                }
            },
            AWTEvent.FOCUS_EVENT_MASK);
    }

    /**
     * Return the application window that has the focus. This call is useful
     * for ApplicationDialogs.
     * @return the ApplicationWindow that has the focus. May return null
     * if there isn't an ApplicationWindow with the focus.
     */
    public ApplicationWindow getActiveWindow() {
        return currentWindow;
    }

    /**
     * Return the acomponent that has the focus, excluding menus.
     */
    public Component getActiveComponent() {
        return this.focussedComponent;
    }

    /**
     * Adds a focus listener for a specific class as provided in the parameter.
     * Every time the focus changes, the hierarchy from the deepest component
     * that has the focus up to the top-level window will be traversed until the
     * first occurance an instance of the supplied class is found. When found,
     * the property change will be called with the name of the class as the
     * property string, the value of the previous object of the class that had
     * the focus (may be null) and the object of the new class that has focus.
     * <b>Multiple listeners can be added for the same class. Once the the hierarchy
     * has been examined after a focus switch, the listener will be notified in
     * the order that they have been added, i.e. the first listener added will
     * be notified first. A listener will not be notified if the old and new
     * objects are both null. </b>
     */
    public void addFocusListenerForClass(Class aClass, PropertyChangeListener p) {
        if (aClass == null) return;
        if (!focusSupport.hasListeners(aClass.getName())) {
            synchronized (classes) {
                classes = (Class[]) ArrayUtils.add(classes, aClass);

                // Array Utils does not allow adding a null object. Add this and
                // replace with null later
                int len = foundObjects.length;
                foundObjects = ArrayUtils.add(foundObjects, (Object) this);
                currentObjects = ArrayUtils.add(currentObjects, (Object) this);
                foundObjects[len] = null;
                currentObjects[len] = null;
            }
        }
        focusSupport.addPropertyChangeListener(aClass.getName(), p);
    }

    /**
     * Remove a PropertyChangeListener for a specific class. Other listeners
     * for the same class will remain active.
     */
    public synchronized void removeFocusListenerForClass(Class aClass,
                                                PropertyChangeListener p) {
        focusSupport.removePropertyChangeListener(aClass.getName(), p);
        if (!focusSupport.hasListeners(aClass.getName())) {
            int i = ArrayUtils.indexOf(classes, aClass);
            synchronized (classes) {
                ArrayUtils.remove(classes, i);
                ArrayUtils.remove(foundObjects, i);
                ArrayUtils.remove(currentObjects, i);
            }
        }
    }

    /**
     * Method private to this package to let ApplicationWindow let us know
     * about gaining focus.
     */
    void applicationWindowActivated(ApplicationWindow appWin) {
        currentWindow = appWin;
    }
    /**
     * Method private to this package to let ApplicationWindow let us know
     * about losing focus.
     */
    void applicationWindowDeactivated(ApplicationWindow appWin) {
        if (appWin == currentWindow) {
            // A window is announcing loss of focus. If it is not the window
            // we currently know to have the focus, we don't care, because
            // applicationWindowActivated should have taken care of it anyway.
            currentWindow = null;
        }
    }

}
