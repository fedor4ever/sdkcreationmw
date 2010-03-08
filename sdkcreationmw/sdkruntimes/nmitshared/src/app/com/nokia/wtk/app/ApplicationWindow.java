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

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Frame;
import java.awt.Image;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import java.beans.PropertyChangeEvent;

import javax.swing.BorderFactory;
import javax.swing.Icon;
import javax.swing.ImageIcon;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JMenuBar;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.WindowConstants;

import com.nokia.wtk.util.MessageWriters;
import com.nokia.wtk.util.PropertySet;
import com.nokia.wtk.util.Utils;

/**
 * An application window for containing an application UI component.
 *
 */
public class ApplicationWindow implements MessageWriters {

    private abstract class Property extends PropertySet.BasicProperty {

        Property(String path, Class type, boolean archive) {
            super(path, type, archive);
        }

        /**
         * Adds a PropertyChangeListener to the listener list.
         * @param listener The PropertyChangeListener to be added
         */
        public void addPropertyChangeListener(PropertyChangeListener l) {
            if (propChange == null) createPropertyChangeSupport();
            propChange.addPropertyChangeListener(getName(), l);
        }

        /**
         * Removes a PropertyChangeListener from the listener list.
         * @param listener The PropertyChangeListener to be removed
         */
        public void removePropertyChangeListener(PropertyChangeListener l) {
            if (propChange != null) {
                propChange.removePropertyChangeListener(getName(), l);
            }
        }

        /**
         * Fires a property change event
         */
        public void firePropertyChange(boolean oldValue, boolean newValue) {
            if (oldValue != newValue) {
                firePropertyChange(Utils.getBoolean(oldValue),
                                   Utils.getBoolean(newValue));
            }
        }

        /**
         * Fires a property change event
         */
        public void firePropertyChange(Object oldValue, Object newValue) {
            if (propChange != null) {
                // create our own property change event to make
                // this property the source of the event
                propChange.firePropertyChange(new PropertyChangeEvent(
                            this, getName(), oldValue, newValue));
            }
        }
    }

    /**
     * Creates ApplicationWindow
     */
    public ApplicationWindow() {
        // Application Window Location:
        propLocation =
            new Property(Location_Property, Point.class, true) {
                public void setValue(Object value) {
                    setWindowLocation((Point)value);
                }
                public Object getValue() {
                    return getWindowLocation();
                }
            };

        // Application Window Size:
        propSize =
            new Property(Size_Property, Dimension.class, true) {
                public void setValue(Object value) {
                    setWindowSize((Dimension)value);
                }
                public Object getValue() {
                    return getWindowSize();
                }
            };

        // Application Window Visible:
        propVisible =
            new Property(Visible_Property, Boolean.class, false) {
                public void setValue(Object value) {
                    setWindowVisible(((Boolean)value).booleanValue());
                }
                public Object getValue() {
                    return Utils.getBoolean(isWindowVisible());
                }
            };

        // Application Window Iconified:
        propIconified =
            new Property(Iconified_Property, Boolean.class, false) {
                public void setValue(Object value) {
                    setWindowIconified(((Boolean)value).booleanValue());
                }
                public Object getValue() {
                    return Utils.getBoolean(isWindowIconified());
                }
            };
    }

    /**
     * Get the set of properties to be exposed by this class.
     * @param propertySet the PropertySet to which the properties are to be added
     */
    public PropertySet getProperties() {
        if (propertySet == null) {
            propertySet = new PropertySet();
            propertySet.addProperty(propLocation);
            propertySet.addProperty(propSize);
            propertySet.addProperty(propVisible);
            propertySet.addProperty(propIconified);
        }
        return propertySet;
    }

    /**
     * Creates PropertyChangeSupport
     */
    private synchronized void createPropertyChangeSupport() {
        if (propChange == null) {
            propChange = new PropertyChangeSupport(this);
        }
    }

    /**
     * Add a PropertyChangeListener to the listener list.
     * @param listener The PropertyChangeListener to be added
     */
    public void addPropertyChangeListener(PropertyChangeListener listener) {
//SYSOUT.println("ApplicationWindow.addPropertyChangeListener(" + listener + ")");
        if (listener == null) {
            Thread.dumpStack();
        }
        if (listener != null) {
            if (propChange == null) createPropertyChangeSupport();
            propChange.addPropertyChangeListener(listener);
        }
    }

    /**
     * Remove a PropertyChangeListener from the listener list.
     * @param listener The PropertyChangeListener to be removed
     */
    public void removePropertyChangeListener(PropertyChangeListener listener) {
//SYSOUT.println("ApplicationWindow.removePropertyChangeListener(" + listener + ")");
        if ((listener != null) && (propChange != null)) {
            propChange.removePropertyChangeListener(listener);
        }
    }

    /**
     * Set the location of the window.
     * @param location the location of the upper left corner of the window
     */
    public void setWindowLocation(Point location) {
//SYSOUT.println("ApplicationWindow.setWindowLocation(" + location + ")");
        getFrame().setLocation(location);
    }

    /**
     * Get the location of the window.
     * @return the location of the upper left corner of the window
     */
    public Point getWindowLocation() {
        return getFrame().getLocation();
    }

    /**
     * Set the size of the window.
     * @param size the size of the window
     */
    public void setWindowSize(Dimension size) {
//SYSOUT.println("ApplicationWindow.setWindowSize(" + size + ")");
        getFrame().setSize(size);
    }

    /**
     * Get the size of the window.
     * @return the size of the window
     */
    public Dimension getWindowSize() {
        return getFrame().getSize();
    }

    /**
     * Show or hide the window.
     * @param visible if <code>true</code>, then show the window, and bring
     * forward. Otherwise, nothing will happen, the window cannot be hidden this way.
     */
    public void setWindowVisible(boolean visible) {
//SYSOUT.println("ApplicationWindow.setWindowVisible(" + visible + ")");
        boolean wasVisible = appFrameVisible;
        if (visible != wasVisible) {
            appFrameVisible = visible;

            // In addition, if visible=true, bring window to front
            // and request focus. If visible=false, don't hide the window
            // but rather let it stay up until application exits
            if (visible) {
                getFrame().setVisible(true);
                getFrame().toFront();
                getFrame().requestFocus();
            }
            // no need to fire prop change, making visible will make component
            // listener below fire prop change
        }
    }

    /**
     * Get the visibility status of the window.
     * @return <code>true</code> if the window is visible.
     */
    public boolean isWindowVisible() {
        return appFrameVisible;
    }

    /**
     * Iconify or de-iconify(restore) the window.
     * @param iconified if <code>true</code>, then iconify the window.
     *                  Otherwise, de-iconify and show the window.
     */
    public void setWindowIconified(boolean iconified) {
        if (iconified == isWindowIconified()) {
            // do not make this happen if window is de-iconified anyway,
            // because it might be the result of applying values. Since
            // de-iconifying here makes the window visible, we want to acvoid
            // making this call when the window is not iconified anyway
            return;
        }
//SYSOUT.println("ApplicationWindow.setWindowIconified(" + iconified + ")");
        getFrame().setState(iconified ? Frame.ICONIFIED : Frame.NORMAL);
        if (!iconified) {
            setWindowVisible(true);
        }
    }

    /**
     * Get the iconified status of the window.
     * @return <code>true</code> if the window is iconified.
     */
    public boolean isWindowIconified() {
        return (getFrame().getState() == Frame.ICONIFIED);
    }

    /**
     * Set the title to appear in the window decorator.
     * @param title the text of the title to set
     */
    public void setWindowTitle(String title) {
        getFrame().setTitle(title);
    }

    /**
     * Set the icon image to appear in the window decorator.
     * @param icon the icon image to set
     */
    public void setWindowIcon(ImageIcon icon) {
        if (icon != null) {
            Image image = icon.getImage();
//SYSOUT.println("ApplicationWindow.setWindowIcon: icon=" + icon + ", image=" + image);
            getFrame().setIconImage(image);
        }
    }

    /** Returns new ImageIcon containing Image used as icon. **/
    public ImageIcon getWindowIcon() {
        return new ImageIcon(getFrame().getIconImage());
    }

    /**
     * Brings window to front
     */
    public void toFront() {
        if (isWindowVisible()) {
            getFrame().toFront();
        } else {
            setWindowVisible(true);
        }
    }

    /**
     * Set the application UI component.
     * @param c the application UI component
     */
    public void setApplicationComponent(Component c) {
        if (appContainer == null) {
            appContainer = new JPanel(new BorderLayout());
            appContainer.setPreferredSize(new Dimension(580,400));
            appContainer.setBorder(LookFactory.emptyBorder());
            getFrame().getContentPane().add(appContainer, BorderLayout.CENTER);
        } else if (appComponent != null) {
            // Remove existing component:
            appContainer.remove(appComponent);
        }
        appComponent = c;
        if (appComponent != null) {
            // Add the new component if non-null:
            appContainer.add(appComponent, BorderLayout.CENTER);
        }
        appContainer.validate();
        getFrame().pack();
    }

    /**
     * Set the status UI component.
     * @param c the application UI component
     */
    public void setStatusComponent(Component c) {
        if (statusContainer == null) {
            statusContainer = new JPanel(new BorderLayout());
            statusContainer.setBorder(BorderFactory.createCompoundBorder(LookFactory.emptyBorder(),
                                                                         LookFactory.loweredBorder()));
            getFrame().getContentPane().add(statusContainer, BorderLayout.SOUTH);
        } else if (statusComponent != null) {
            // Remove existing component:
            statusContainer.remove(statusComponent);
        }
        statusComponent = c;
        if (statusComponent != null) {
            // Add the new component if non-null:
            statusContainer.add(statusComponent, BorderLayout.CENTER);
        }
        statusContainer.validate();
        getFrame().pack();
    }

    /**
     * Sets the menubar for this window.
     * @param menubar the application menu bar component
     */
    public void setMenuBar(JMenuBar menubar) {
        getFrame().setJMenuBar(menubar);
    }

    /**
     * Gets the menubar set for this window.
     * @return the application menu bar component
     */
    public JMenuBar getMenuBar() {
        return (JMenuBar)getFrame().getJMenuBar();
    }

    /**
     * @deprecated
     * Creates a modal or non-modal dialog with this window as the parent.
     * @param title the String to display in the dialog's title bar
     * @param modal <code>true</code> for a modal dialog,
     *              <code>false</code> for one that allows others windows
     *              to be active at the same time
     * @return the dialog component
     * @see ApplicationDialogs
     */
     public JDialog createDialog(String title, boolean modal) {
        //TODO: Provide a mechanism to place the dialog centered on the screen.
        return new JDialog(getFrame(), title, modal);
    }

    /**
     * @deprecated
     * Creates a modal option dialog with this window as the parent.
     * @param title the String to display in the dialog's title bar
     * @return the dialog component
     * @see ApplicationDialogs
     */
    public JDialog createOptionDialog(JOptionPane options, String title) {
        //TODO: Provide a mechanism to place the dialog centered on the screen.
        return options.createDialog(getFrame(), title);
    }

    /**
     * Brings up a modal dialog where the number of choices is determined
     * by the <code>optionType</code> parameter.
     *
     * @param message   The Object to display
     * @param title     the title string for the dialog
     * @param optionType an int designating the options available on the dialog:
     *                   YES_NO_OPTION, or YES_NO_CANCEL_OPTION
     * @return an int indicating the option selected by the user
     * @see JOptionPane
     */
    public int showConfirmDialog(Object message, String title, int optionType) {
        return showConfirmDialog(message, title, optionType, JOptionPane.QUESTION_MESSAGE);
    }

    /**
     * Brings up a modal dialog where the number of choices is determined
     * by the <code>optionType</code> parameter, where the <code>messageType</code>
     * parameter determines the icon to display.
     * The <code>messageType</code> parameter is primarily used to supply
     * a default icon from the look and feel.
     *
     * @param parentComponent Determines the Frame in which the dialog is displayed.
     *                  If null, or if the parentComponent has no Frame, a
     *                  default Frame is used.
     * @param message   The Object to display
     * @param title     the title string for the dialog
     * @param optionType an int designating the options available on the dialog:
     *                   YES_NO_OPTION, or YES_NO_CANCEL_OPTION
     * @param messageType an int designating the kind of message this is,
     *                    primarily used to determine the icon from the pluggable
     *                    look and feel: ERROR_MESSAGE, INFORMATION_MESSAGE,
     *                    WARNING_MESSAGE, QUESTION_MESSAGE, or PLAIN_MESSAGE.
     * @return an int indicating the option selected by the user
     * @see JOptionPane
     */
    public int showConfirmDialog(Object message, String title, int optionType, int messageType) {
        return showConfirmDialog(message, title, optionType, messageType, null);
    }

    /**
     * Brings up a modal dialog with a specified icon, where the number of
     * choices is determined by the <code>optionType</code> parameter.
     * The <code>messageType</code> parameter is primarily used to supply
     * a default icon from the look and feel.
     *
     * @param message   The Object to display
     * @param title     the title string for the dialog
     * @param optionType an int designating the options available on the dialog:
     *                   YES_NO_OPTION, or YES_NO_CANCEL_OPTION
     * @param messageType an int designating the kind of message this is,
     *                    primarily used to determine the icon from the pluggable
     *                    look and feel: ERROR_MESSAGE, INFORMATION_MESSAGE,
     *                    WARNING_MESSAGE, QUESTION_MESSAGE, or PLAIN_MESSAGE.
     * @param icon      the icon to display in the dialog
     * @return an int indicating the option selected by the user
     * @see JOptionPane
     */
    public int showConfirmDialog(Object message, String title, int optionType,
                                 int messageType, Icon icon) {
        return showOptionDialog(message, title, optionType, messageType, icon, null, null);
    }

    /**
     * Brings up a modal dialog with a specified icon, where the initial
     * choice is dermined by the <code>initialValue</code> parameter and
     * the number of choices is determined by the <code>optionType</code>
     * parameter.
     * <p>
     * If <code>optionType</code> is YES_NO_OPTION, or YES_NO_CANCEL_OPTION
     * and the <code>options</code> parameter is null, then the options are
     * supplied by the look and feel.
     * <p>
     * The <code>messageType</code> parameter is primarily used to supply
     * a default icon from the look and feel.
     *
     * @param message   The Object to display
     * @param title     the title string for the dialog
     * @param optionType an int designating the options available on the dialog:
     *                   YES_NO_OPTION, or YES_NO_CANCEL_OPTION
     * @param messageType an int designating the kind of message this is,
     *                    primarily used to determine the icon from the pluggable
     *                    look and feel: ERROR_MESSAGE, INFORMATION_MESSAGE,
     *                    WARNING_MESSAGE, QUESTION_MESSAGE, or PLAIN_MESSAGE.
     * @param icon      the icon to display in the dialog
     * @param options   an array of objects indicating the possible choices
     *                  the user can make. If the objects are components, they
     *                  are rendered properly. Non-String objects are
     *                  rendered using their <code>toString</code> methods.
     *                  If this parameter is null, the options are determined
     *                  by the look and feel.
     * @param initialValue the object that represents the default selection
     *                     for the dialog
     * @return an int indicating the option chosen by the user,
     *         or CLOSED_OPTION if the user closed the Dialog
     * @see JOptionPane
     */
    public int showOptionDialog(Object      message,
                                String      title,
                                int         optionType,
                                int         messageType,
                                Icon        icon,
                                Object[]    options,
                                Object      initialValue) {
        return JOptionPane.showOptionDialog(getFrame(),
                                            message,
                                            title,
                                            optionType,
                                            messageType,
                                            icon,
                                            options,
                                            initialValue);
    }

    /**
     * Get the actual frame component that will contain the application UI.
     */
    public JFrame getFrame() {
        if (appFrame == null) {
            appFrame = new JFrame();
            appFrame.setDefaultCloseOperation(WindowConstants.DO_NOTHING_ON_CLOSE);

            // Detect window events and notify interested
            // parties through propertyChange events:
            appFrame.addWindowListener(new WindowAdapter() {
                /** Handle 'Window Closing' Event */
                public void windowClosing(WindowEvent e) {
                    // Just hide the window:
                    appFrameVisible = false;
                    propVisible.firePropertyChange(true, false);
                }
                public void windowIconified(WindowEvent e) {
                    propIconified.firePropertyChange(false, true);
                }
                public void windowDeiconified(WindowEvent e) {
                    propIconified.firePropertyChange(true, false);
                }

                public void windowActivated(WindowEvent e) {
                    FocusSystem f = AppEnvironment.
                                        getAppEnvironment().getFocusSystem();
                    if (f != null) {
                        f.applicationWindowActivated(ApplicationWindow.this);
                        f = null;
                    }
                }
                public void windowDeactivated(WindowEvent e) {
                    FocusSystem f = AppEnvironment.
                                        getAppEnvironment().getFocusSystem();
                    if (f != null) {
                        f.applicationWindowDeactivated(ApplicationWindow.this);
                        f = null;
                    }
                }
            });

            // Detect window component events and notify interested
            // parties through propertyChange events:
            appFrame.addComponentListener(new ComponentAdapter() {
                private Point location = appFrame.getLocation();
                private Dimension size = appFrame.getSize();
                public void componentResized(ComponentEvent e) {
                    Dimension oldSize = size;
                    size = appFrame.getSize();
                    propSize.firePropertyChange(oldSize, size);
                }
                public void componentMoved(ComponentEvent e) {
                    Point oldLocation = location;
                    location = appFrame.getLocation();
                    propLocation.firePropertyChange(oldLocation, location);
                }
                public void componentShown(ComponentEvent e) {
                    // make sure appFrameVisible gets set to true in case
                    // of "backdoor" use, i.e. when caller calls show() or
                    // setVisible(true) on the frame directly instead of
                    // going calling setWindowVisible().
                    appFrameVisible = true;
                    propVisible.firePropertyChange(false, true);
                }
                public void componentHidden(ComponentEvent e) {
                    propVisible.firePropertyChange(true, false);
                }
            });

            // Set default frame bounds:
            appFrame.setBounds(defaultAppFrameBounds);

            // Set default icon. Since frame is not shown when created,
            // programmer can still set it to something else
            this.setWindowIcon(AppEnvironment.getIcon(AppEnvironment.appIcon));
        }
        return appFrame;
    }

    /** The title of the application window. */
    private static final String Title_Property = "title";
    /** The name of the image file resource to be used as the icon in the application window. */
    private static final String Icon_Property = "icon";
    /** The location of the upper left corner of the application window. */
    private static final String Location_Property = "location";
    /** The size of the application window. */
    private static final String Size_Property = "size";
    /** The visibility state of the application window. */
    public  static final String Visible_Property = "visible";
    /** The iconified state of the application window. */
    private static final String Iconified_Property = "iconified";

    private static final Rectangle defaultAppFrameBounds = new Rectangle(0, 0, 600, 600);

    private Rectangle appFrameBounds = defaultAppFrameBounds.getBounds();

    private Property propLocation;
    private Property propSize;
    private Property propVisible;
    private Property propIconified;

    private PropertyChangeSupport propChange = null;
    private PropertySet propertySet;
    private JFrame appFrame;
    private JPanel appContainer;
    private JPanel toolContainer;
    private Component toolComponent;
    private Component appComponent;
    private JPanel statusContainer;
    private Component statusComponent;
    private boolean appFrameVisible = false;
}
