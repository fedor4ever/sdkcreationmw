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



package com.nokia.wtk.util;

/* java.beans */
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

/* java.awt */
import java.awt.Frame;
import java.awt.Point;
import java.awt.Image;
import java.awt.Dimension;

/* java.awt.event */
import java.awt.event.WindowEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentAdapter;

/* java.beans */
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeSupport;
import java.beans.PropertyChangeListener;

/* javax.swing */
import javax.swing.JFrame;
import javax.swing.ImageIcon;
import javax.swing.SwingUtilities;

/**
 * Implements a set of properties for a frame window
 *
 */
public class FrameProperties {

    /* property names */
    public static final String TITLE        = "title";
    public static final String LOCATION     = "location";
    public static final String SIZE         = "size";
    public static final String VISIBLE      = "visible";
    public static final String ICONIFIED    = "iconified";
    public static final String RESIZABLE    = "resizable";

    /** the actual frame window */
    private JFrame window;

    /* property change support */
    private PropertySet propertySet;
    private PropertyChangeSupport propChange;

    /* properties */
    private BooleanWindowProperty propResizable;
    private BooleanWindowProperty propIconified;
    private BooleanWindowProperty propVisible;
    private WindowProperty propLocation;
    private WindowProperty propTitle;
    private WindowProperty propSize;
    private String title;

    /**
     * Creates <code>FrameProperties</code>
     *
     * @param frame the frame to attach to
     */
    public FrameProperties(JFrame frame) {

        // Create properties:
        propResizable = new ResizableProperty();
        propIconified = new IconifiedProperty();
        propLocation = new LocationProperty();
        propVisible = new VisibleProperty();
        propTitle = new TitleProperty();
        propSize = new SizeProperty();

        // Add listeners to the actual frame
        window = frame;
        window.addWindowListener(new WindowAdapter() {
            public void windowIconified(WindowEvent e) {
                propIconified.firePropertyChange(true);
            }
            public void windowDeiconified(WindowEvent e) {
                propIconified.firePropertyChange(false);
            }
        });

        // Detect window repositioning and resizing notify interested
        // parties through propertyChange events:
        window.addComponentListener(new ComponentAdapter() {
            private Dimension size = window.getSize();
            private Point location = window.getLocation();
            public void componentResized(ComponentEvent e) {
                Dimension oldSize = size;
                size = window.getSize();
                propSize.firePropertyChange(oldSize, size);
            }
            public void componentMoved(ComponentEvent e) {
                Point oldLocation = location;
                location = window.getLocation();
                propLocation.firePropertyChange(oldLocation, location);
            }
            public void componentShown(ComponentEvent e) {
                propVisible.firePropertyChange(true);
            }
            public void componentHidden(ComponentEvent e) {
                propVisible.firePropertyChange(false);
            }
        });

        // This only works under JDK 1.4 or better. In older JDKs Frame
        // did not fire title property change events. But we register the
        // listener anyway in case if we are dealing with a subclass of
        // Frame that fixes this problem for earlier JDKs
        title = window.getTitle();
        window.addPropertyChangeListener("title",new PropertyChangeListener(){
            public void propertyChange(PropertyChangeEvent e) {
                String oldTitle = title;
                title = (String)e.getNewValue();
                propTitle.firePropertyChange(oldTitle, title);
            }
        });
    }

    /**
     * Gets the set of properties to be exposed by this class.
     * @return the set of properties to be exposed by this class.
     */
    public PropertySet getProperties() {
        if (propertySet == null) {
            PropertySet ps = new PropertySet();
            ps.addProperty(propResizable);
            ps.addProperty(propIconified);
            ps.addProperty(propLocation);
            ps.addProperty(propTitle);
            ps.addProperty(propSize);
            ps.addProperty(propVisible);
            propertySet = ps;
        }
        return propertySet;
    }

    /*
     * Creates PropertyChangeSupport
     */
    private synchronized void createPropertyChangeSupport() {
        if (propChange == null) {
            propChange = new PropertyChangeSupport(this);
        }
    }

    /**
     * Returns the PropertyChangeSupport used by this class
     * @return the PropertyChangeSupport used by this class
     */
    protected PropertyChangeSupport getPropertyChangeSupport() {
        if (propChange == null) createPropertyChangeSupport();
        return propChange;
    }

    /**
     * Set the location of the window.
     * @param location the location of the upper left corner of the window
     */
    public void setWindowLocation(Point location) {
        // NOTE: component listener will fire the property change event
        window.setLocation(location);
    }

    /**
     * Set the location of the window.
     * @param x the x coordinate of the upper left corner of the window
     * @param y the y coordinate of the upper left corner of the window
     */
    public void setWindowLocation(int x, int y) {
        // NOTE: component listener will fire the property change event
        window.setLocation(x,y);
    }

    /**
     * Get the location of the window.
     * @return the location of the upper left corner of the window
     */
    public Point getWindowLocation() {
        return window.getLocation();
    }

    /**
     * Get the x coordinate of the window.
     * @return the x coordinate of the window
     */
    public int getWindowX() {
        return window.getX();
    }

    /**
     * Get the x coordinate of the window.
     * @return the x coordinate of the window
     */
    public int getWindowY() {
        return window.getY();
    }

    /**
     * Set the size of the window.
     * @param size the size of the window
     */
    public void setWindowSize(Dimension size) {
        // NOTE: component listener will fire the property change event
        window.setSize(size);
    }

    /**
     * Set the size of the window.
     * @param width the width of the window
     * @param height the height of the window
     */
    public void setWindowSize(int width, int height) {
        // NOTE: component listener will fire the property change event
        window.setSize(width,height);
    }

    /**
     * Get the size of the window.
     * @return the size of the window
     */
    public Dimension getWindowSize() {
        return window.getSize();
    }

    /**
     * Get the width of the window.
     * @return the width of the window
     */
    public int getWindowWidth() {
        return window.getWidth();
    }

    /**
     * Get the height of the window.
     * @return the height of the window
     */
    public int getWindowHeight() {
        return window.getHeight();
    }

    /**
     * Show or hide the window.
     * @param visible if <code>true</code>, then show the window.
     *                Otherwise, hide the window.
     */
    public void setWindowVisible(boolean visible) {
        // NOTE: component listener will fire the property change event
        window.setVisible(visible);
    }

    /**
     * Get the visibility status of the window.
     * @return <code>true</code> if the window is visible.
     */
    public boolean isWindowVisible() {
        return window.isVisible();
    }

    /**
     * Switches between iconic and normal state.
     * @param iconic if <code>true</code>, then iconify the window.
     *                Otherwise, restore it to normal state.
     */
    public void setWindowIconified(boolean iconic) {
        // NOTE: window listener will fire the property change event
        window.setState(iconic ? Frame.ICONIFIED : Frame.NORMAL);
    }

    /**
     * Returns the state of the window (iconic or normal).
     * @return <code>true</code> if window is in iconic state;
     *         <code>false</code> if window is in normal state.
     */
    public boolean isWindowIconified() {
        return (window.getState() == Frame.ICONIFIED);
    }

    /**
     * Sets whether this window is resizable by the user.
     * @param resizable <code>true</code> if this window is resizable;
     *                   <code>false</code> otherwise.
     */
    public void setWindowResizable(boolean resizable) {
        if (isWindowResizable() != resizable) {
            window.setResizable(resizable);
            propResizable.firePropertyChange(resizable);
        }
    }

    /**
     * Indicates whether this frame is resizable by the user.
     * @return <code>true</code> if the user can resize this window;
     *         <code>false</code> otherwise.
     */
    public boolean isWindowResizable() {
        return window.isResizable();
    }

    /**
     * Sets the title to appear in the window decorator.
     * @param title the text of the title to set
     */
    public void setWindowTitle(String title) {
        window.setTitle(title);

        // prior JDK 1.4 Frame did not fire title property change events
        String oldTitle = this.title;
        this.title = title;
        propTitle.firePropertyChange(oldTitle, title);
    }

    /**
     * Returns the title that appear in the window decorator.
     * @return the title that appear in the window decorator.
     */
    public String getWindowTitle() {
        return window.getTitle();
    }

    /**
     * Set the icon image to appear in the window decorator.
     * @param icon the icon image to set
     */
    public void setWindowIcon(ImageIcon icon) {
        if (icon != null) {
            Image image = icon.getImage();
            window.setIconImage(image);
        }
    }

    /**
     * Gets the actual frame window
     * @return the actual frame window
     */
    public JFrame getFrame() {
        return window;
    }

    /**
     * Brings this window to the front.
     * Places this window at the top of the stacking order and
     * shows it in front of any other windows.
     */
    public void toFront() {
        getFrame().toFront();
    }

    /**
     * Releases all resources, including the frame.
     * Do not call this object afterwards.
     */
    public void dispose() {
        // do not block the current thread, it may cause a deadlock
        if (SwingUtilities.isEventDispatchThread()) {
            disposeImpl();
        } else {
            SwingUtilities.invokeLater(new Runnable() {
                public void run() { disposeImpl();  }
            });
        }
    }

    /**
     * <code>dispose()</code> implementation, invoked on GUI thread.
     */
    private void disposeImpl() {
        getFrame().getContentPane().removeAll();
        getFrame().dispose();
    }

    //=======================================================================
    //      P R O P E R T I E S
    //=======================================================================

    /**
     * A base class for frame properties
     */
    private abstract class WindowProperty extends PropertySet.BasicProperty {

        WindowProperty(String name, Class type, boolean archive) {
            super(name, type, archive);
        }

        /**
         * Adds a PropertyChangeListener to the listener list.
         * @param l The PropertyChangeListener to be added
         */
        public void addPropertyChangeListener(PropertyChangeListener l) {
            if (propChange == null) createPropertyChangeSupport();
            propChange.addPropertyChangeListener(getName(), l);
        }

        /**
         * Removes a PropertyChangeListener from the listener list.
         * @param l The PropertyChangeListener to be removed
         */
        public void removePropertyChangeListener(PropertyChangeListener l) {
            if (propChange != null) {
                propChange.removePropertyChangeListener(getName(), l);
            }
        }

        /**
         * Fires a property change event
         *
         * @param oldValue the old value of the property
         * @param newValue the new value of the property
         */
        public void firePropertyChange(Object oldValue, Object newValue) {
            if (propChange != null) {
                if (!Utils.equals(oldValue, newValue)) {
                    String name = getName();
                    if (propChange.hasListeners(name)) {
                        // create our own property change event to make
                        // this property the source of the event
                        propChange.firePropertyChange(new PropertyChangeEvent(
                            this,name,oldValue,newValue));
                    }
                }
            }
        }
    }

    /**
     * A base class for boolean frame properties
     */
    private abstract class BooleanWindowProperty extends WindowProperty {

        BooleanWindowProperty(String name, boolean archive) {
            super(name, Boolean.class, archive);
        }

        /**
         * Fires a property change event for the boolean property
         *
         * @param oldValue the old value of the property
         * @param newValue the new value of the property
         */
        public void firePropertyChange(boolean oldValue, boolean newValue) {
            if (oldValue != newValue) {
                firePropertyChange(Utils.getBoolean(oldValue),
                                   Utils.getBoolean(newValue));
            }
        }

        /**
         * Fires a property change event for the boolean property
         *
         * @param newValue  the new value of the property. The old value
         *                  is presumed to be different from the new value
         */
        public void firePropertyChange(boolean newValue) {
            firePropertyChange(Utils.getBoolean(!newValue),
                               Utils.getBoolean(newValue));
        }
    }

    /**
     * Window Title property
     */
    private class TitleProperty extends WindowProperty {
        TitleProperty() {
            super(TITLE, String.class, true);
        }
        public void setValue(Object value) {
            setWindowTitle((String)value);
        }
        public Object getValue() {
            return getWindowTitle();
        }
    }

    /**
     * Window Size property
     */
    private class SizeProperty extends WindowProperty {
        SizeProperty() {
            super(SIZE, Dimension.class, true);
        }
        public void setValue(Object value) {
            setWindowSize((Dimension)value);
        }
        public Object getValue() {
            return getWindowSize();
        }
    }

    /**
     * Window Location property
     */
    private class LocationProperty extends WindowProperty {
        LocationProperty() {
            super(LOCATION, Point.class, true);
        }
        public void setValue(Object value) {
            setWindowLocation((Point)value);
        }
        public Object getValue() {
            return getWindowLocation();
        }
    }

    /**
     * Window Visible property
     */
    private class VisibleProperty extends BooleanWindowProperty {
        VisibleProperty() {
            super(VISIBLE, false);
        }
        public void setValue(Object value) {
            setWindowVisible(((Boolean)value).booleanValue());
        }
        public Object getValue() {
            return Utils.getBoolean(isWindowVisible());
        }
    }

    /**
     * Window Iconified property
     */
    private class IconifiedProperty extends BooleanWindowProperty {
        IconifiedProperty() {
            super(ICONIFIED, false);
        }
        public void setValue(Object value) {
            setWindowIconified(((Boolean)value).booleanValue());
        }
        public Object getValue() {
            return Utils.getBoolean(isWindowIconified());
        }
    }

    /**
     * Resizable property
     */
    private class ResizableProperty extends BooleanWindowProperty {
        ResizableProperty() {
            super(RESIZABLE, true);
        }
        public void setValue(Object value) {
            setWindowResizable(((Boolean)value).booleanValue());
        }
        public Object getValue() {
            return Utils.getBoolean(isWindowResizable());
        }
    }
}
