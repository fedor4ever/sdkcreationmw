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



package com.nokia.wtk.util.gui;

/* java.lang.ref */
import java.lang.ref.Reference;
import java.lang.ref.WeakReference;

/* java.beans */
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

/* java.util */
import java.util.Set;
import java.util.Map;
import java.util.HashSet;
import java.util.HashMap;
import java.util.Iterator;

/* java.awt.event */
import java.awt.event.ActionEvent;

/* javax.swing */
import javax.swing.Action;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.Utils;
import com.nokia.wtk.util.Trace;
import com.nokia.wtk.util.ArrayUtils;

/**
 * Redirects all requests to another {@link Action}. Also supports
 * "sticky" properties which do not get propagated to the target
 * object. These are useful for example when it's necessary to create
 * an action that looks and acts like another action but must have
 * a different icon.
 *
 */
public class ActionProxy implements Action {

    public static final String ENABLED_KEY = "enabled";

    /**
     * {@link PropertyChangeListener} that forwards property change events
     * from the target action to the registered listeners.
     */
    private PropertyChangeListener forwarder;

    /**
     * The target action. All interface requests are redirected to this
     * object unles it's <code>null</code>. The exception to this rule
     * are the requests for "sticky" properties that are associated with
     * this object rather than the target.
     */
    private Action target;

    /**
     * Maps property names to values, including both target action properties
     * and the "sticky" properties.
     */
    private Map valueMap;

    /**
     * Set of "sticky" property keys.
     */
    private Set stickySet;

    /**
     * List of property change listeners.
     */
    private PropertyChangeListener [] listeners;

    /**
     * Creates an <code>ActionProxy</code> with no target.
     */
    public ActionProxy() {
    }

    /**
     * Creates an <code>ActionProxy</code> that redirects all interface calls
     * to the specified target action.
     *
     * @param a the target action
     */
    public ActionProxy(Action a) {
        setTarget(a);
    }

    /**
     * Gets the target action
     *
     * @return the target action
     */
    public Action getTarget() {
        return target;
    }

    /**
     * Sets the target action.
     *
     * @param a the target action
     */
    public void setTarget(Action a) {
        if (target != a) {
            if (target != null) {
                if (forwarder != null) {
                    target.removePropertyChangeListener(forwarder);
                }
            }
            target = a;

            if (listeners != null) {
                if (valueMap != null && !valueMap.isEmpty()) {

                    // fire property change events for non-sticky properties
                    Iterator entries = valueMap.entrySet().iterator();
                    while (entries.hasNext()) {
                        Map.Entry entry = (Map.Entry)entries.next();
                        String key = (String)entry.getKey();
                        if (!isSticky(key)) {
                            Object oldVal = entry.getValue();
                            Object newVal = null;
                            if (target != null) newVal = target.getValue(key);
                            firePropertyChange(key, oldVal, newVal);
                            if (newVal == null) {
                                entries.remove();
                            } else {
                                entry.setValue(newVal);
                            }
                        }
                    }
                }

                // add property change listener to the target action
                if (target != null) {
                    if (forwarder == null) {
                        forwarder = new PCF(this);
                    }
                    target.addPropertyChangeListener(forwarder);
                }
            } else if (valueMap != null) {
                if (stickySet != null) {
                    valueMap.keySet().retainAll(stickySet);
                } else {
                    valueMap.clear();
                }
            }
        }
    }

    /**
     * Changes "sticky" status of one of the object's properties.
     * A "sticky" value sticks to this <code>ActionProxy</code> object and
     * is not propagated to the target action. Similarly, changes of the
     * property with the same name in the target object are not propagated
     * to property change listeners registered with this object.
     *
     * @param key    a <code>String</code> containing the key
     * @param sticky <code>true</code> to make this property sticky,
     *               <code>false</code> to "unstick" it.
     */
    public void setSticky(String key, boolean sticky) {
        if (sticky) {

            // add it to the map
            if (stickySet == null) stickySet = new HashSet(7);
            stickySet.add(key);

        } else if (stickySet != null && stickySet.remove(key)) {

            // when "unsticking" the property, we may need to fire a property
            // change event
            Object oldValue = valueMap.get(key);
            Object newValue = null;
            if (target != null) newValue = target.getValue(key);
            putValueInternal(key, newValue);
            firePropertyChange(key, oldValue, newValue);
            if (stickySet.isEmpty()) stickySet = null;
        }
    }

    /**
     * Returns <code>true</code> if this is a key of a "sticky" property.
     * A "sticky" value sticks to this <code>ActionProxy</code> object and
     * is not propagated to the target action. Similarly, changes of the
     * property with the same name in the target object are not propagated
     * to property change listeners registered with this object.
     *
     * @param key    a <code>String</code> containing the key
     * @return <code>true</code> if this is a key of a "sticky" property.
     */
    public boolean isSticky(String key) {
        return (stickySet != null && stickySet.contains(key));
    }

    //======================================================================
    //          I N T E R F A C E
    //======================================================================

    /**
     * Performs the action.
     * @param e the action event
     */
    public void actionPerformed(ActionEvent e) {
        if (target != null) {
            target.actionPerformed(e);
        }
    }

    /**
     * Gets one of this object's properties using the associated key.
     *
     * @param key the property key
     * @return the property value
     */
    public Object getValue(String key) {
        if (isSticky(key)) {
            return ((valueMap == null) ? null : valueMap.get(key));
        } else if (target != null) {
            Object value = target.getValue(key);
            if (value != null) {
                putValueInternal(key, value);
            }
            return value;
        } else {
            return null;
        }
    }

    /**
     * Sets one of this object's properties
     * using the associated key. If the value has
     * changed, a <code>PropertyChangeEvent</code> is sent
     * to listeners.
     *
     * @param key    a <code>String</code> containing the key
     * @param value  an <code>Object</code> value
     */
    public void putValue(String key, Object value) {
        if (isSticky(key)) {
            Object oldValue = ((valueMap == null) ? null : valueMap.get(key));
            putValueInternal(key,value);
            firePropertyChange(key, oldValue, value);
        } else if (target != null) {
            putValueInternal(key,value);
            target.putValue(key, value);
        }
    }

    /**
     * Sets the enabled state of the <code>Action</code>.  When enabled,
     * any component associated with this object is active and
     * able to fire this object's <code>actionPerformed</code> method.
     * If the value has changed, a <code>PropertyChangeEvent</code> is sent
     * to listeners.
     *
     * @param  b true to enable this <code>Action</code>, false to disable it
     */
    public void setEnabled(boolean b) {
        if (isSticky(ENABLED_KEY)) {
            Object oldValue = ((valueMap == null) ? null : valueMap.get(ENABLED_KEY));
            Object newValue = Utils.getBoolean(b);
            putValueInternal(ENABLED_KEY,newValue);
            firePropertyChange(ENABLED_KEY, oldValue, newValue);
        } else if (target != null) {
            target.setEnabled(true);
        }
    }

    /**
     * Returns the enabled state of the <code>Action</code>. When enabled,
     * any component associated with this object is active and
     * able to fire this object's <code>actionPerformed</code> method.
     *
     * @return true if this <code>Action</code> is enabled
     */
    public boolean isEnabled() {
        if (isSticky(ENABLED_KEY)) {
            Object value = ((valueMap == null) ? null : valueMap.get(ENABLED_KEY));
            if (value != null) return ((Boolean)value).booleanValue();
        } else if (target != null) {
            boolean enabled = target.isEnabled();
            putValueInternal(ENABLED_KEY, Utils.getBoolean(enabled));
            return enabled;
        }
        return false;
    }

    /**
     * Adds a <code>PropertyChange</code> listener. Containers and attached
     * components use these methods to register interest in this
     * <code>Action</code> object. When its enabled state or other property
     * changes, the registered listeners are informed of the change.
     *
     * @param l a <code>PropertyChangeListener</code> object
     */
    public void addPropertyChangeListener(PropertyChangeListener l) {
        if (l == null) throw new NullPointerException();
        if (listeners == null) {
            listeners = new PropertyChangeListener[] {l};
            if (target != null) {
                if (forwarder == null) {
                    forwarder = new PCF(this);
                }
                target.addPropertyChangeListener(forwarder);
           }
        } else {
            listeners = (PropertyChangeListener[])ArrayUtils.add(listeners,l);
        }
    }

    /**
     * Removes a <code>PropertyChange</code> listener.
     *
     * @param l a <code>PropertyChangeListener</code> object
     * @see #addPropertyChangeListener
     */
    public void removePropertyChangeListener(PropertyChangeListener l) {
        if (listeners != null) {
            if (listeners.length == 1 && listeners[0] == l) {
                listeners = null;
                if (forwarder != null && target != null) {
                    target.removePropertyChangeListener(forwarder);
                }
            } else if (listeners.length > 1) {
                listeners = (PropertyChangeListener[])ArrayUtils.remove(listeners,l);
            }
        }
    }

    //======================================================================
    //          I M P L E M E N T A T I O N
    //======================================================================

    /**
     * Sets the <code>Value</code> associated with the specified key.
     *
     * @param key  the <code>String</code> that identifies the stored object
     * @param value the <code>Object</code> to store using this key
     */
    private void putValueInternal(String key, Object value) {
        if (value != null) {
            if (valueMap == null) valueMap = new HashMap(7);
            valueMap.put(key, value);
        } else if (valueMap != null) {
            valueMap.remove(key);
        }
    }

    /**
     * Fires a property change event
     *
     * @param name the property name
     * @param oldv the old value of the property
     * @param newv the new value of the property
     */
    protected void firePropertyChange(String name, Object oldv, Object newv) {
        if (listeners != null && !Utils.equals(oldv,newv)) {
            firePropertyChange(new PropertyChangeEvent(this,name,oldv,newv));
        }
    }

    /**
     * Fires a property change event
     *
     * @param e the event to fire
     */
    protected void firePropertyChange(PropertyChangeEvent e) {
        PropertyChangeListener [] l = listeners;
        if (l != null) {
            for (int i=0; i<l.length; i++) {
                try { l[i].propertyChange(e); }
                catch (Exception x) { Trace.printStackTrace(x); }
            }
        }
    }

    //======================================================================
    //          P R O P E R T Y    L I S T E N E R
    //======================================================================

    /**
     * {@link PropertyChangeListener} that forwards property change events
     * from the target action to the registered listeners.
     */
    private static class PCF implements PropertyChangeListener {

        private Reference ref;

        PCF(ActionProxy a) {
            ref = new WeakReference(a);
        }

        public void propertyChange(PropertyChangeEvent e) {
            ActionProxy proxy = (ActionProxy)ref.get();
            if (proxy != null) {
                String name = e.getPropertyName();
                // ignore changes of "sticky" properties
                if (!proxy.isSticky(name)) {
                    Object oldv = e.getOldValue();
                    Object newv = e.getNewValue();
                    proxy.putValueInternal(name, newv);
                    proxy.firePropertyChange(name, oldv, newv);
                }
            } else {
                // the proxy is gone, deregister the listener
                Action a = (Action)e.getSource();
                a.removePropertyChangeListener(this);
            }
        }
    }
}
