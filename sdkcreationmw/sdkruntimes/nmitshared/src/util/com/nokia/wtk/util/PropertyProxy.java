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
import java.beans.PropertyChangeSupport;
import java.beans.PropertyChangeListener;

/**
 * A property that redirects all interface calls to another property.
 * The target property may be changed, as long as it has the same type
 * and name as the old target property.
 *
 */
public class PropertyProxy implements Property {

    private PropertyChangeListener forwarder;
    private PropertyChangeSupport pcs;
    private Property target;
    private String name;
    private Class type;

    /**
     * Creates PropertyProxy with specified name and class. You can later
     * assign the target property, but it must have the name and the type
     * that you have specified in the constructor.
     *
     * @param name the property name
     * @param type the property type
     */
    public PropertyProxy(String name, Class type) {
        if (name == null) throw new NullPointerException("name");
        if (type == null) throw new NullPointerException("type");
        this.name = name;
        this.type = type;
    }

    /**
     * Creates PropertyProxy and assignes the target property. You can
     * later change the target property but it must have the same name
     * and type as the property specified in the constructor.
     *
     * @param p the target property.
     */
    public PropertyProxy(Property p) {
        this(p.getName(), p.getType());
        setTargetProperty(p);
    }

    /**
     * Gets the target property
     *
     * @return the target property
     */
    public Property getTargetProperty() {
        return target;
    }

    /**
     * Sets the target property
     *
     * @param p the target property.
     */
    public void setTargetProperty(Property p) {
        if (p != target) {
            Object oldValue = null;
            Object newValue = null;

            // detach from the old target
            if (target != null) {
                oldValue = target.getValue();
                if (forwarder != null) {
                    target.removePropertyChangeListener(forwarder);
                }
            }

            // attach to the new target
            if (p == null) {
                target = null;
            } else {
                if (!name.equals(p.getName())) {
                    throw new IllegalArgumentException("property name mismatch");
                }
                if (!type.equals(p.getType())) {
                    throw new IllegalArgumentException("property type mismatch");
                }
                target = p;
                if (forwarder == null) forwarder = new Forwarder();
                target.addPropertyChangeListener(forwarder);
                newValue = target.getValue();
            }

            // notify the listeners that the value has changed
            if (!Utils.equals(oldValue, newValue)) {
                firePropertyChange(oldValue, newValue);
            }
        }
    }

    /**
     * Returns the name of this property. A property name cannot contain
     * the . (dot) character.
     *
     * @return The programmatic name of the property
     */
    public String getName() {
        return name;
    }

    /**
     * Returns the type of the property value.
     * @return the type of the property value
     */
    public Class getType() {
        return type;
    }

    /**
     * Returns the default value of this property.
     * @return The default value.
     */
    public Object getDefaultValue() {
        return (target != null) ? target.getDefaultValue() : null;
    }

    /**
     * Determines whether a property may be included in an archive.
     * This does not imply or override any implicit requirements for archiveability.
     * @return <code>true</code> if the property may be included in an archive.
     */
    public boolean isArchiveable() {
        return (target != null) ? target.isArchiveable() : false;
    }

    /**
     * Returns <code>true</code> if this property is readonly. This
     * prevents PropertySet from setting value of this property. It's
     * also assumed that a read-only property never fires property change
     * events.
     *
     * @return <code>true</code> if this property is readonly
     */
    public boolean isReadOnly() {
        return (target != null) ? target.isReadOnly() : true;
    }

    /**
     * Reads the value of a property.
     * @return the property value or <code>null</code> if not defined
     */
    public Object getValue() {
        return (target != null) ? target.getValue() : null;
    }

    /**
     * Writes the value of a property.
     *
     * @param value the value of the property
     *
     * @throws      UnsupportedOperationException
     *              If the value of this property cannot be set
     * @throws      ClassCastException
     *              If the type of the object does not match the expectation
     * @throws      IllegalArgumentExceptions
     *              IF the type is right but the value is not acceptable
     */
    public void setValue(Object value) {
        if (target != null) {
            target.setValue(value);
        }
    }

    /**
     * Returns the text representation of the property value. If property
     * is not defined, returns <code>null</code>
     *
     * @return  the text representation of the property value.
     */
    public String getValueAsText() {
        return (target != null) ? target.getValueAsText() : null;
    }

    /**
     * Sets the value of this property.
     * @param value the text representation of the property value to set.
     *
     * @throws      UnsupportedOperationException
     *              If this method is not suported by the property
     * @throws      IllegalArgumentException
     *              If the text cannot be converted into the object
     */
    public void setValueAsText(String value) {
        if (target != null) {
            target.setValueAsText(value);
        }
    }

    /**
     * Adds property change listener to this property.
     * The property must notify the listeners every time after its value has
     * changed. This property <strong>must be</strong> the source of such
     * property change events.
     *
     * @param l the listener to add
     */
    public void addPropertyChangeListener(PropertyChangeListener l) {
        if (pcs == null) {
            pcs = new PropertyChangeSupport(this);
        }
        pcs.addPropertyChangeListener(l);
    }

    /**
     * Removes property change listener from this property
     *
     * @param l the listener to remove
     */
    public void removePropertyChangeListener(PropertyChangeListener l) {
        if (pcs != null) {
            pcs.removePropertyChangeListener(l);
        }
    }

    /**
     * Notifies the listeners of this property that the property value has
     * changed.
     *
     * @param oldValue the old value of the property
     * @param newValue the new value of the property
     */
    private void firePropertyChange(Object oldValue, Object newValue) {
        if (pcs != null) {
            pcs.firePropertyChange(name, oldValue, newValue);
        }
    }

    /**
     * The internal property change listener that forwards property change
     * events to the listeners of this property.
     */
    private class Forwarder implements PropertyChangeListener {
        public void propertyChange(PropertyChangeEvent e) {
            firePropertyChange(e.getOldValue(), e.getNewValue());
        }
    }
}
