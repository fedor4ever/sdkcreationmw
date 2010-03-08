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
import java.beans.PropertyEditor;

/* java.util */
import java.util.List;
import java.util.Iterator;
import java.util.ArrayList;

/**
 * A set of static utilities related to properties and property sets
 *
 */
public abstract class PropUtils {

    /**
     * Prevents instantiation
     */
    protected PropUtils() {
        throw new Error("PropUtils");
    }

    /**
     * Prints a stack trace
     * @param x a throwable object containing the stack trace to print
     */
    private static void printStackTrace(Throwable x) {
        if (Trace.isVerbose()) {
            x.printStackTrace();
        }
    }

    /**
     * Creates the property set with the specified name, removing all
     * conflicting properties in the process.
     *
     * @param ps the containing property set
     * @param path the name of the property set to create
     * @return the created property set
     */
    public static PropertySet createPath(PropertySet ps, String path) {

        // normalize the node name
        int len = path.length();
        while (len > 0 && path.charAt(len-1) == '.') len--;
        if (len > 0) {
            // make sure that all property sets in the hierarchy exist and
            // don't conflict with existing properties.
            path = path.substring(0, len);
            int lastdot = -1;
            do {
                int nextdot = path.indexOf('.', lastdot+1);
                if (nextdot < 0) nextdot = len;
                String node = path.substring(lastdot+1, nextdot);
                ps.removeProperty(node);
                ps = ps.createPropertySet(node);
                lastdot = nextdot;
            } while (lastdot < len);
        }
        return ps;
    }

    /**
     * Gets a property by name, checking its type.
     * @param ps the property set
     * @param name the name of the property to search for
     * @param t the expected property type, can be <code>null</code>
     * @return the Property if it exists and its type matches
     *  the expected one; or <code>null</code> if property is not defined
     *  or type does not match. It expected type is <code>null</code>,
     *  any property will match
     */
    public static Property getProperty(PropertySet ps, String name, Class t) {
        Property p = ps.getProperty(name);
        if ((p != null) && (t == null || t.equals(p.getType()))) {
            return p;
        }
        return null;
    }

    /**
     * Makes sure that the specified named property exists in the property
     * set. If the property does not exist, it's created. If the property
     * exists but has a wrong type, it's replaced with the new property.
     *
     * @param ps        property set
     * @param s         property name
     * @param type      property type
     *
     * @return the requested property
     */
    public static Property makeProperty(PropertySet ps, String s, Class type) {
        return makeProperty(ps, s, type, null);
    }

    /**
     * Makes sure that the specified named property exists in the property
     * set. If the property does not exist, it's created. If the property
     * exists but has a wrong type, it's replaced with the new property.
     * If the property has a <code>null</code> value, it's assigned the
     * specified default value.
     *
     * @param ps        property set
     * @param name      property name
     * @param type      property type
     * @param defVal    default property value
     *
     * @return the requested property
     */
    public static Property makeProperty(PropertySet ps, String name,
                                        Class type, Object defVal) {
        Property p = ps.getProperty(name);
        if (p != null) {
            if (p.getType() != type) {
                ps.removeProperty(name);
                p = ps.addProperty(name, type, defVal, true);
            } else if (defVal != null && p.getValue() == null) {
                p.setValue(defVal);
            }
        } else {
            p = ps.addProperty(name, type, defVal, true);
        }
        return p;
    }

    /**
     * Makes sure that the specified property of type String exists in the
     * property set. If the property does not exist, it's created. If the
     * property exists but has a wrong type, it's replaced with the new
     * property.
     *
     * @param ps        property set
     * @param name      property name
     *
     * @return the requested property
     */
    public static Property makeStringProperty(PropertySet ps, String name) {
        return makeProperty(ps, name, String.class, null);
    }

    /**
     * Makes sure that the specified property of type String exists in the
     * property set. If the property does not exist, it's created. If the
     * property exists but has a wrong type, it's replaced with the new
     * property. If the property has a <code>null</code> value, it's
     * assigned the specified default value.
     *
     * @param ps        property set
     * @param name      property name
     * @param defVal    default property value
     *
     * @return the requested property
     */
    public static Property makeStringProperty(PropertySet ps, String name,
                                              String defVal) {
        return makeProperty(ps, name, String.class, defVal);
    }

    /**
     * Makes sure that the specified property of type Boolean exists in the
     * property set. If the property does not exist, it's created. If the
     * property exists but has a wrong type, it's replaced with the new
     * property.
     *
     * @param ps        property set
     * @param name      property name
     *
     * @return the requested property
     */
    public static Property makeBooleanProperty(PropertySet ps, String name) {
        return makeProperty(ps,name, Boolean.class, null);
    }

    /**
     * Makes sure that the specified property of type Boolean exists in the
     * property set. If the property does not exist, it's created. If the
     * property exists but has a wrong type, it's replaced with the new
     * property. If the property has a <code>null</code> value, it's
     * assigned the specified default value.
     *
     * @param ps        property set
     * @param name      property name
     * @param defVal    default property value
     *
     * @return the requested property
     */
    public static Property makeBooleanProperty(PropertySet ps, String name,
                                               boolean defVal) {
        return makeProperty(ps,name,Boolean.class,Utils.getBoolean(defVal));
    }

    /**
     * Makes sure that the specified property of type Integer exists in the
     * property set. If the property does not exist, it's created. If the
     * property exists but has a wrong type, it's replaced with the new
     * property.
     *
     * @param ps        property set
     * @param name      property name
     *
     * @return the requested property
     */
    public static Property makeIntProperty(PropertySet ps, String name) {
        return makeProperty(ps, name, Integer.class, null);
    }

    /**
     * Makes sure that the specified property of type Integer exists in the
     * property set. If the property does not exist, it's created. If the
     * property exists but has a wrong type, it's replaced with the new
     * property. If the property has a <code>null</code> value, it's
     * assigned the specified default value.
     *
     * @param ps        property set
     * @param name      property name
     * @param defVal    default property value
     *
     * @return the requested property
     */
    public static Property makeIntProperty(PropertySet ps, String name,
                                           int defVal) {
        // NOTE: not using makeProperty() in order to avoid creating
        // unnecessary Integer object
        Property p = ps.getProperty(name);
        if (p != null) {
            if (p.getType() != Integer.class) {
                ps.removeProperty(name);
                p = ps.addIntProperty(name, defVal, true);
            } else if (p.getValue() == null) {
                p.setValue(new Integer(defVal));
            }
        } else {
            p = ps.addIntProperty(name, defVal, true);
        }
        return p;
    }

    /**
     * Sets the value of a property of type Boolean.
     * @param p the property to update
     * @param value the value of the property
     * @return <code>true</code> if the value was set, <code>false</code>
     * if it wasn't
     */
    public static boolean setValue(Property p, boolean value) {
        return setValue(p, Utils.getBoolean(value));
    }

    /**
     * Sets the value of a property of type Integer.
     * @param p the property to update
     * @param value the value of the property
     * @return  <code>true</code> if the property value has changed,
     *          <code>false</code> otherwise
     */
    public static boolean setValue(Property p, int value) {
        if (p != null) {
            Object curValue = p.getValue();
            try {
                if (curValue == null) {
                    p.setValue(new Integer(value));
                    return true;
                } else if (curValue instanceof Integer) {
                    int cutIntValue = ((Integer)curValue).intValue();
                    if (cutIntValue != value) {
                        p.setValue(new Integer(value));
                        return true;
                    }
                }
            } catch (Exception x) {
                printStackTrace(x);
            }
        }
        return false;
    }

    /**
     * Sets the value of a property.
     * @param p the property to update
     * @param value the value of the property
     * @return <code>true</code> if the value was set, <code>false</code>
     * if it wasn't
     */
    public static boolean setValue(Property p, Object value) {
        if (p != null && !p.isReadOnly()) {
            try {
                p.setValue(value);
                return true;
            } catch (Exception x) {
                printStackTrace(x);
            }
        }
        return false;
    }

    /**
     * Returns the property value.
     *
     * @param ps the property set
     * @param name property name
     * @return the property value, <code>null</code> if property is undefined
     * @deprecated  this call is here for historical reasons. There's no good
     *              reason to use it anymore. It's absolutely equivalent to
     *              <code>ps.getValue(name)</code>
     */
    public static Object getValue(PropertySet ps, String name) {
        return ps.getValue(name);
    }

    /**
     * Returns a value of an integer property
     *
     * @param p         the property to query
     * @param defValue  the default value
     * @return          the value of the property, or the default value if
     *                  the property is undefined or is not an integer.
     */
    public static int getValue(Property p, int defValue) {
        if (p != null) {
            Object value = p.getValue();
            if (value instanceof Integer) {
                return ((Integer)value).intValue();
            }
        }
        return defValue;
    }

    /**
     * Returns a value of a boolean property
     *
     * @param p         the property to query
     * @param defValue  the default value
     * @return          the value of the property, or the default value if
     *                  the property is undefined or is not a boolean.
     */
    public static boolean getValue(Property p, boolean defValue) {
        if (p != null) {
            Object value = p.getValue();
            if (value instanceof Boolean) {
                return ((Boolean)value).booleanValue();
            }
        }
        return defValue;
    }

    /**
     * Returns a value of a string property
     *
     * @param p the property to query
     * @param defValue the default value
     *
     * @return  the requested value, or default value if it's undefined or
     *          is not a String
     */
    public static String getValue(Property p, String defValue) {
        if (p != null) {
            Object value = p.getValue();
            if (value instanceof String) {
                return (String)value;
            }
        }
        return defValue;
    }

    /**
     * Extracts the property name from the full path.
     * @param path the full property path
     * @return the property name (last path component of the path)
     */
    public static String getPropertyName(String path) {
        int dot = path.lastIndexOf('.');
        return ((dot < 0) ? path : path.substring(dot+1));
    }

    /**
     * Extracts the node name from the full path.
     * @param path the full property path
     * @return  the property node name (name of the containing property set),
     *          <code>null</code> if there's no parent node
     */
    public static String getNodeName(String path) {
        int dot = path.lastIndexOf('.');
        return ((dot < 0) ? null : path.substring(0,dot));
    }

    /**
     * Compares two node names, ignoring training dot if present.
     * Also, empty string is equivalent to null
     *
     * @param node1 the first node name to compare
     * @param node2 the second node name to compare
     * @return  <code>true</code> if two strings represent the same node
     */
    public static boolean equalNode(String node1, String node2) {
        if (node1 == node2) return true;
        if (node1 == null) return (node2.length() == 0);
        if (node2 == null) return (node1.length() == 0);

        int len1 = node1.length();
        int len2 = node2.length();
        if (len1 == 0 && len2 == 0) return true;

        // now we know they both are not null and at least one is not empty
        if (node1.endsWith(".")) {
            if (node2.endsWith(".")) {
                return node1.equals(node2);
            } else if (len1 == (len2+1)) {
                return node1.startsWith(node2);
            } else {
                return false;
            }
        } else if (node2.endsWith(".")) {
            if (node1.endsWith(".")) {
                return node2.equals(node1);
            } else if (len2 == (len1+1)) {
                return node2.startsWith(node1);
            } else {
                return false;
            }
        } else if (len1 == len2) {
            return node1.equals(node2);
        } else {
            return false;
        }
    }

    /**
     * Very generic filter. {@link PropertyHistory} uses it to determine
     * which values should go into the history and which shouldn't
     */
    public interface Filter {
        public boolean accept(Object value);
    }

    //=======================================================================
    //          H I S T O R Y
    //=======================================================================

    static final String HISTORY_LIMIT_PROP = "limit";
    static final String HISTORY_DATA_PROP = "data";

    /**
     * Listens for the changes in a property and updates a history of property
     * values. The history would typically be represented as a dropdown list
     */
    public static class PropertyHistory extends PropertySetAdapter {

        private Filter filter;
        private Class propType;
        private String propName;
        private Object propValue;
        private Property limitProp;
        private HistoryProp historyProp;
        private PropertySet ps;
        private PropertySet history;
        private PropertyEditor editor;
        private List historyList = new ArrayList();
        private int suspendHistoryListener = 0;

        private static final int DEFAULT_LIMIT = 20;

        public PropertyHistory(PropertySet ps, String name, PropertySet hist) {
            this(ps, name, null, hist);
        }

        public PropertyHistory(PropertySet set, String name, Filter f, PropertySet h) {
            Property prop = set.getProperty(name);
            ps = set;
            propName = name;
            propType = prop.getType();
            propValue = prop.getValue();
            history = h;
            filter = f;

            // must have property editor for this class
            editor = PropertyEditors.getEditor(propType);
            if (editor == null) throw new IllegalArgumentException(propType.getName());

            // create limit property if it does not exist
            limitProp = makeIntProperty(h, HISTORY_LIMIT_PROP, DEFAULT_LIMIT);

            // create history property
            Object historyData = h.getValue(HISTORY_DATA_PROP);
            if (!(historyData instanceof String[])) historyData = null;
            historyProp = new HistoryProp((String[])historyData);
            h.addProperty(historyProp);

            suspendHistoryListener++;
            cleanupHistoryPropertySet();
            resetHistoryPropertySet();
            suspendHistoryListener--;

            ps.addPropertySetListener(this);
            history.addPropertySetListener(this);
        }

        /**
         * Invoked after a property within the property set hierarchy
         * has changed. In case of "bulk" updates, only one such
         * notification will be fired after the update is complete.
         *
         * @param src the source of the event
         */
        public void propertySetChanged(PropertySet src) {
            if (src == history && suspendHistoryListener == 0) {
                cleanupHistoryPropertySet();
            }

            Object newValue = ps.getProperty(propName).getValue();
            if (!Utils.equals(newValue,propValue)) {

                // the "watched" property may have changed, update
                // the history
                propValue = newValue;
                if (filter == null || filter.accept(newValue)) {

                    // translate value into text
                    editor.setValue(newValue);
                    String s = editor.getAsText();

                    // add it to the history
                    int index = historyList.indexOf(s);
                    if (index != 0) {
                        if (index < 0) {
                            historyList.add(0,s);
                        } else {
                            historyList.remove(index);
                            historyList.add(0,s);
                        }

                        // make sure history does not grow too big
                        int max = Math.max(getValue(limitProp,DEFAULT_LIMIT),0);
                        while (historyList.size() >= max) {
                            historyList.remove(historyList.size()-1);
                        }
                    }
                }
            }

            // reset the property set
            resetHistoryPropertySet();
        }

        /**
         * Makes sure that the contents of the history property set makes
         * sense and removes the garbage from it.
         */
        private void cleanupHistoryPropertySet() {
            int max = getValue(limitProp, 0);
            if (max <= 0) setValue(limitProp, DEFAULT_LIMIT);
            String[] strings = historyProp.getHistoryData();
            historyList.clear();

            // remove the strings that cannot be decoded by the property editor
            if (strings != null && strings.length > 0) {
                ArrayUtils.add(historyList, strings);
                int n = Math.min(strings.length, max);
                Iterator i = historyList.iterator();
                while (i.hasNext()) {
                    String s = (String)i.next();
                    try { editor.setAsText(s); }
                    catch (Exception x) { i.remove(); }
                }
            }

            // if history is empty, add current value to the history
            if (historyList.isEmpty()) {
                Object value = ps.getValue(propName);
                if (value != null) {
                    if (filter == null || filter.accept(value)) {
                        editor.setValue(value);
                        historyList.add(editor.getAsText());
                    }
                }
            }
        }

        /**
         * Clears the property set holding the history properties and
         * re-fills it with current history
         */
        private void resetHistoryPropertySet() {

            // temporarily deregister history listener
            if (suspendHistoryListener++ == 0) {
                history.removePropertySetListener(this);
            }

            try {
                // reset the history property set
                int n = historyList.size();
                historyProp.setHistoryData(historyList);
                history.removeAll(true);
                history.addProperty(limitProp);
                history.addProperty(historyProp);
            } finally {

                // re-register history listener
                if ((--suspendHistoryListener) == 0) {
                    history.addPropertySetListener(this);
                }
            }
        }

        /**
         * History data property
         */
        private static class HistoryProp extends PropertySet.StandaloneProperty {

            /**
             * Old value of the property, before it was changed last time.
             *
             * This is a workaround (see also setValue method) for a problem
             * with a PropertySet that contains properties that depend on each
             * other. Here is an examples. Let's say we have properties A and
             * B. Any change of property A updates the value of property B
             * (in our case it's the value and its history). Consider two
             * property sets, X and Y, which is a deep copy of X. Any change
             * of property A in property set X updates the value of B, while
             * property set Y does not have such mechanism (for a good reason).
             * We change the value of A in property set Y and call
             * X.applyValues(Y). Now, iff property B happens to be after
             * property A in the property set X, then here is what happens:
             * <ul>
             * <li>  X.applyValues updates property A in property set X. This
             *       changes the value of property B in property set X. Note
             *       that property B in property set Y still has the old
             *       value.
             * <li>  after that, applyValues applies property B (remember
             *       that property B is AFTER property A). This restores
             *       property B back to its original value.
             * </ul>
             * The bottom line - property A has changed but the value of
             * property B remained the same. oops... The workaround is to
             * ignore the second update of property B which resets it back
             * to its original value.
             */
            private String[] oldValue;

            /**
             * Creates the history data property.
             * @param value the initial value.
             */
            HistoryProp(String[] value) {
                super(HISTORY_DATA_PROP,String[].class,value,true);
                oldValue = value;
            }

            /**
             * Returns the value of the property
             * @return the current value of the property
             */
            public String [] getHistoryData() {
                return (String[])this.getValue();
            }

            /**
             * Sets the property value.
             * @param list the new property value
             */
            public void setHistoryData(List list) {
                if (!ArrayUtils.equals(list,getHistoryData())) {
                    this.setValue(list.toArray(new String[list.size()]));
                }
            }

            /**
             * Sets the property value.
             * @param newValue the property value
             */
            public void setValue(Object newValue) {
                // see comments for oldValue field which explains the
                // purpose of this hack...
                if (newValue != oldValue &&
                   !PropertySet.valueEqual(this.getValue(), newValue)) {

                    // apply this change
                    oldValue = getHistoryData();
                    super.setValue(newValue);
                }
            }
        }
    }
}
