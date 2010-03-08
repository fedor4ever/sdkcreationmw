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




package com.nokia.wtk.device;


import java.beans.PropertyChangeListener;
import java.beans.PropertyVetoException;
import java.io.IOException;
import java.io.Reader;
import java.util.Iterator;


/**
 * An interface for device property access.
 * This interface will be implemented by the hosting application.
 * DeviceProperty represents a means to expose configurable parameters
 * within a device simulation component.  Property values may be any of
 * the following types:
 * <ul>
 * <li><code>String</code></li>
 * <li><code>Boolean</code></li>
 * <li><code>Integer</code></li>
 * <li><code>Long</code></li>
 * <li><code>Short</code></li>
 * <li><code>Byte</code></li>
 * <li><code>Float</code></li>
 * <li><code>Double</code></li>
 * <li><code>String[]</code></li>
 * <li><code>int[]</code></li>
 * <li><code>java.io.File</code></li>
 * <li><code>java.net.URL</code></li>
 * <li><code>java.awt.Point</code></li>
 * <li><code>java.awt.Dimension</code></li>
 * <li><code>java.awt.Rectangle</code></li>
 * <li><code>java.awt.Polygon</code></li>
 * <li><code>java.awt.Font</code></li>
 * <li><code>java.awt.Color</code></li>
 * <li><code>javax.swing.KeyStroke</code></li>
 * </ul>
 * <p>
 * All device properties are contained hierarchically within instances
 * of DeviceProperty.Set.  The name of a DeviceProperty is used to denote
 * the hierarchical arrangement of individual properties, a "path" if you will.
 * A decimal point ("dot") is used as the delimiting character.  All other
 * characters, except whitespace and non-graphical characters, are considered
 * legal when creating path names.  Because property names are used to define
 * paths, a property name must not be allowed to conflict with the path
 * of another property.
 *
 */
//TODO: Future types: java.awt.Insets, byte[], java.util.Date, java.util.Locale, java.util.BitSet
public interface DeviceProperty {

    /**
     * Gets the name of this property.
     * @return          the fully-qualified property name.
     */
    public String getName();

    /**
     * Gets the type of this property.
     * @return          the property type.
     */
    public Class getType();

    /**
     * Gets the value of this property.
     * @return          the property value.
     */
    public Object getValue();

    /**
     * Gets the value of this property.
     * @return          the text representation of the property value.
     */
    public String getValueAsText();

    /**
     * Sets the value of this property.
     * @param value     the property value.
     * @exception       java.beans.PropertyVetoException
     *                  if the value is not allowed for the property.
     */
    public void setValue(Object value) throws PropertyVetoException;

    /**
     * Sets the value of this property.
     * @param value     the text representation of the property value to set.
     * @exception       java.beans.PropertyVetoException
     *                  if the value is not allowed for the property.
     */
    public void setValueAsText(String value) throws PropertyVetoException;

    /**
     * Sets the default value of this property.
     * The default value must either be of the same type as this property
     * or <code>null</code>.
     * @param value the default value.
     */
    public void setDefaultValue(Object value);

    /**
     * Assigns a special {@link PropertyEditor} class to this property.
     * Normally, <code>PropertyEditor</code>s will be determined using the property type,
     * but in some cases, this may be insufficient.  The <code>PropertyEditor</code>
     * is used to convert property values to and from a text representation.
     *
     * @param editorClass  The <code>PropertyEditor</code> class or <code>null</code>.
     */
    public void setEditorClass(Class editorClass);

    /**
     * Adds a {@link PropertyChangeListener} to this property.
     *
     * @param listener  the {@link PropertyChangeListener} to be added.
     */
    public void addPropertyChangeListener(PropertyChangeListener listener);

    /**
     * Removes a {@link PropertyChangeListener} from this property.
     *
     * @param listener  the {@link PropertyChangeListener} to be removed.
     */
    public void removePropertyChangeListener(PropertyChangeListener listener);

    /**
     * Fires a property change event for this property.
     * This is useful to force an event notification for those properties
     * whose values may otherwise change silently.
     * @param oldval    the old property value.
     * @param newval    the new property value.
     */
    public void firePropertyChange(Object oldval, Object newval);

    //
    // End of DeviceProperty methods
    //


    /**
     * Low-level property value implementation interface.
     * This is used by device simulators to implement properties that
     * must use specific set/get methods to access their property values.
     */
    public interface Value {

        /**
         * Gets the value of a property.
         * @return          the property value
         */
        public Object get();

        /**
         * Sets the value of a property.
         * A {@link PropertyVetoException} may be fired on behalf of this
         * property implementation if an {@link IllegalArgumentException}
         * is thrown by this method.
         * @param value     the value of the property
         * @exception       java.lang.IllegalArgumentException
         *                  if the value is not allowed for the property.
         */
        public void set(Object value);

    }   // End of DeviceProperty.Value


    /**
     * A set of device properties.
     * This interface will be implemented by the hosting application.
     * The hosting application will register an instance of this interface
     * with the device prior to calling {@link Device#openDevice}.  When
     * the <code>DeviceProperty.Set</code> instance is registered, the device
     * implementation is expected to add its own properties to the collection.
     */
    //TODO: Load&Save methods?
    public interface Set {

        /**
         * Adds a device property to this property set.
         * @param name      a fully-qualified property name.
         * @param type      the type of the property.
         * @param setget    the value set/get implementation of the property.
         *                  If <code>null</code>, then a default implementation
         *                  will be used.
         * @param defval    the default (and initial) property value
         *                  or <code>null</code>.
         * @param archive   if <code>true</code>, then property value archiving
         *                  will be enabled.
         * @return          The newly-created device property.
         */
        public DeviceProperty addProperty(String                name,
                                          Class                 type,
                                          DeviceProperty.Value  setget,
                                          Object                defval,
                                          boolean               archive);

        /**
         * Creates a standalone device property.
         * The property created cannot be added to a property set.
         * This is useful, for example, when deconstructing a Dimension-typed
         * property into two integer-typed properties for width and height.
         * In such cases, there is no need to be added to a property set.
         * @param name      a property name.
         *                  This is not a fully-qualified name
         *                  because it is not part of a property set.
         * @param type      the type of the property.
         * @param setget    the value set/get implementation of the property.
         *                  If <code>null</code>, then a default implementation
         *                  will be used.
         * @param defval    the default (and initial) property value
         *                  or <code>null</code>.
         * @return          The newly-created device property.
         */
        public DeviceProperty createProperty(String                 name,
                                             Class                  type,
                                             DeviceProperty.Value   setget,
                                             Object                 defval);

        /**
         * Creates a standalone property set.
         * @return          The newly-created property set.
         */
        public DeviceProperty.Set createPropertySet();

        /**
         * Loads a standalone property set from a resource location.
         * This is intended to facilitate the loading of device resource
         * configurations from either a file or a jarfile.
         * @param in        an input reader to load properties from.
         * @return          The newly-created property set.
         * @exception       java.io.IOException
         *                  if an error occurred while reading the property set.
         */
        public DeviceProperty.Set loadPropertySet(Reader in) throws IOException;

        /**
         * Removes a property from this property set.
         * @param name      the fully-qualified name of the property
         * @return          the property that was removed
         *                  or <code>null</code> if not defined
         */
        public DeviceProperty removeProperty(String name);

        /**
         * Removes a property subset from this property set.
         * @param name      the fully-qualified name of the property set
         * @return          the property set that was removed
         *                  or <code>null</code> if not defined
         */
        public DeviceProperty.Set removePropertySet(String name);

        /**
         * Gets a property from this property set.
         * @param name      the name of the property
         * @return          the property or <code>null</code> if not defined
         */
        public DeviceProperty getProperty(String name);

        /**
         * Gets a property subset from this property set.
         * This is used to obtain a subset of this set, against which
         * {@link PropertyChangeListener}s may be registered or unregistered.
         * @param name      the name of the property set.
         * @return          the property set
         *                  or <code>null</code> if not defined.
         */
        public DeviceProperty.Set getPropertySet(String name);

        /**
         * Gets the name of this property set.
         * @return          the fully-qualified property set name.
         */
        public String getName();

        /**
         * Returns an iterator over the properties in this set.
         * @return          a property iterator for this set.
         */
        public Iterator getProperties();

        /**
         * Returns an iterator over the property sets in this set.
         * @return          a property set iterator for this set.
         */
        public Iterator getPropertySets();

        /**
         * Adds a {@link PropertyChangeListener} to this property set.
         * The listener is registered for all properties in this set.
         *
         * @param listener  the {@link PropertyChangeListener} to be added.
         */
        public void addPropertyChangeListener(PropertyChangeListener listener);

        /**
         * Removes a {@link PropertyChangeListener} from this property set.
         *
         * @param listener  the {@link PropertyChangeListener} to be removed.
         */
        public void removePropertyChangeListener(PropertyChangeListener listener);

        /**
         * Sets the saveable state of this property set.
         * A saveable property set may be saved to a file.
         * By default, property sets are saveable.
         * @param saveable  if <code>true</code>, then this property set
         *                  may be saved to a file.
         */
        public void setSaveable(boolean saveable);

        /**
         * Determines whether this property set may be saved to a file.
         * @return <code>true</code> if this property set may be saved to a file.
         */
        public boolean isSaveable();

    }   // End of DeviceProperty.Set

}   // End of DeviceProperty
