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

/* java.lang.ref */
import java.lang.ref.Reference;
import java.lang.ref.WeakReference;

/* java.io */
import java.io.File;
import java.io.Reader;
import java.io.Writer;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.Serializable;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;

/* java.util */
import java.util.Map;
import java.util.Date;
import java.util.List;
import java.util.Stack;
import java.util.Vector;
import java.util.Random;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Hashtable;
import java.util.Properties;
import java.util.NoSuchElementException;

/* java.beans */
import java.beans.PropertyEditor;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeSupport;
import java.beans.PropertyChangeListener;

import javax.xml.parsers.*;
import org.xml.sax.*;
import org.xml.sax.helpers.DefaultHandler;

/**
 * A PropertySet is a collection of properties or other PropertySets.
 * A PropertySet may be archived to a file to be loaded at a later time.
 * To effect this, an XML-derived format is used to maintain the archived
 * state. Also, a PropertyEditor must exist for the property type to allow
 * conversion of property values to and from a text representation.
 * A property may be unconditionally excluded from archive operations
 * by disabling its "archiveability" flag.
 */
public final class PropertySet implements Serializable {

    /**
     * Creates a new PropertySet.
     */
    public PropertySet() {
        this(true);
    }

    /**
     * Creates a new PropertySet.
     * @param archiveEnabled If <code>true</code>, then enable archiving.
     *                       Otherwise, disable archiving.
     */
    private PropertySet(boolean archiveEnabled) {
        this.archiveEnabled = archiveEnabled;
    }

    /**
     * Compares this property set with another one.
     * @param ps the property set to compare with
     * @return <code>true</code> if the property sets contain the same
     *         hierarchy of values.
     */
    public boolean equalsTo(PropertySet ps) {
        return equalsTo(ps, false);
    }

    /**
     * Compares this property set with another one.
     * @param ps the property set to compare with
     * @param ignoreNonArchiveable <code>true</code> to ignore the values
     *    of non-archiveable properties. Note that even if this parameter
     *    if <code>true</code>, the properties must still exist for the
     *    property sets to be equal, it's just their values that are being
     *    ignored.
     * @return <code>true</code> if the property sets contain the same
     *         hierarchy of values.
     */
    public boolean equalsTo(PropertySet ps, boolean ignoreNonArchiveable) {
        // trivial cases
        if (ps ==  null || ps.archiveEnabled != archiveEnabled) {
            return false;
        }
        if (ps == this || (isEmpty() && ps.isEmpty())) {
            return true;
        }

        // compare number of entries
        if (getElementNameList().size() != ps.getElementNameList().size()) {
            return false;
        }

        // compare the individual entries
        Iterator i1 = getElementMapEntries();
        Iterator i2 = ps.getElementMapEntries();
        while (i1.hasNext() && i2.hasNext()) {
            Map.Entry e1 = (Map.Entry)i1.next();
            Map.Entry e2 = (Map.Entry)i2.next();
            if (!e1.getKey().equals(e2.getKey())) {
                // element names don't match
                return false;
            }
            Object o1 = e1.getValue();
            Object o2 = e2.getValue();
            if ((o1 instanceof PropertySet) && (o2 instanceof PropertySet)) {
                PropertySet ps1 = (PropertySet)o1;
                PropertySet ps2 = (PropertySet)o2;
                if (!ps1.equalsTo(ps2)) {
                    // child property sets don't match
                    return false;
                }
            } else if ((o1 instanceof Property) && (o2 instanceof Property)) {
                Property p1 = (Property)o1;
                Property p2 = (Property)o2;
                if (p1.isReadOnly() != p2.isReadOnly() ||
                    p1.isArchiveable() != p2.isArchiveable() ||
                    !p1.getName().equals(p2.getName()) ||
                    !p1.getType().equals(p2.getType())) {
                    // properties don't match
                    return false;
                } else if (!ignoreNonArchiveable || p1.isArchiveable()) {
                    if (!valueEqual(p1.getValue(), p2.getValue())) {
                        // property values don't match
                        return false;
                    }
                }
            } else {
                // object classes don't match
                return false;
            }
        }
        return !(i1.hasNext() || i2.hasNext());
    }

    /**
     * Adds a set of properties, defined in another PropertySet,
     * to this PropertySet.
     *
     * @param other another PropertySet to include in this one
     */
    public void addProperties(PropertySet other) {
        addProperties(other, false);
    }

    /**
     * Adds a set of properties, defined in another PropertySet, to this
     * PropertySet.
     *
     * @param other another PropertySet to include in this one
     * @param applyValues if this parameter is <code>true</code>, the existing
     *        property values are applied to the new properties added to this
     *        property set. Typical use - replacing auto-created properties
     *        with "real" ones.
     *
     *        This parameters has no effect on readonly properties
     */
    public void addProperties(PropertySet other, boolean applyValues) {
        Iterator i = other.getElementNames();
        while (i.hasNext()) {
            String name = (String)i.next();
            Object elem = other.getNamedElement(name);
            if (elem instanceof Property) {
                Property p = (Property)elem;
                addProperty(p, applyValues);
            } else if (elem instanceof PropertySet) {
                PropertySet subNode;
                Object existingNode = getNamedElement(name);
                if (existingNode instanceof PropertySet) {
                    subNode = (PropertySet)existingNode;
                } else {
                    subNode = new PropertySet(true);
                    addNamedElement(name, subNode);
                }
                subNode.addProperties((PropertySet)elem, applyValues);
            }
        }
    }

    /**
     * Creates a property set within this property set.
     *
     * @param node name of the property set (node).
     *             May end with a single '.' character
     */
    public PropertySet createPropertySet(String node) {
        addProperties(node, null, false);
        return getPropertySet(node);
    }

    /**
     * Adds a set of properties, defined in another PropertySet,
     * to this PropertySet relative to the specified node.
     *
     * @param node      name of the parent node for the specified properties.
     *                  May end with a single '.' character
     * @param other     another PropertySet to include in this one
     */
    public void addProperties(String node, PropertySet other) {
        addProperties(node, other, false);
    }

    /**
     * Adds a set of properties, defined in another PropertySet,
     * to this PropertySet relative to the specified node.
     *
     * @param node        name of the parent node for the specified properties.
     *                    May end with a single '.' character
     * @param other       another PropertySet to include in this one
     * @param applyValues if this parameter is <code>true</code>, the existing
     *                    property values are applied to the new properties
     *                    added to this property set. Typical use - replacing
     *                    auto-created properties with "real" ones.
     *
     *                    This parameters has no effect on readonly properties
     */
    public void addProperties(String node, PropertySet other, boolean applyValues) {
        if (node != null && node.length() > 0 && !isValidNodeName(node)) {
            throw new IllegalArgumentException(node);
        }
        // if other property set is null, we just create the node.
        // THIS BEHAVIOR IS INTENTIONAL AND IT IS BEING USED
        if (other == null || !other.isEmpty()) {
            PropertySet subNode = ((node) == null ? this : createPath(node));
            if (other != null) subNode.addProperties(other, applyValues);
        }
    }

    /**
     * Adds a property set to this PropertySet relative to the specified node.
     * Notice the difference between adding properties from another property
     * set and adding the property set itself. If this property set already
     * contains a property set with the same name that we are adding, the
     * existing property set object (and all the properties it contains) will
     * be removed and replaced with the specified property set.
     *
     * @param node        path of the property set within this property set
     * @param ps          another PropertySet to add to this one
     */
    public void addPropertySet(String node, PropertySet ps) {
        addPropertySet(node, ps, false);
    }

    /**
     * Adds a property set to this PropertySet relative to the specified node.
     * Notice the difference between adding properties from another property
     * set and adding the property set itself. If this property set already
     * contains a property set with the same name that we are adding, the
     * existing property set object (and all the properties it contains) will
     * be removed and replaced with the specified property set.
     *
     * @param path        path of the property set within this property set
     * @param ps          another PropertySet to add to this one
     * @param applyValues if this parameter is <code>true</code>, the existing
     *                    property values are applied to the new properties
     *                    added to this property set. Typical use - replacing
     *                    auto-created properties with "real" ones.
     *
     *                    This parameters has no effect on readonly properties
     */
    public void addPropertySet(String path, PropertySet ps, boolean applyValues) {
        if (ps == this) throw new IllegalArgumentException();

        int end = path.length(); // exclusive
        while (end > 0 && path.charAt(end-1) == '.') end--;
        int start = end - 1;
        while (start >= 0 && path.charAt(start) != '.') start--;

        String name;
        PropertySet node;
        if (start >= 0) {
            name = path.substring(start+1, end);
            node = createPath(path.substring(0, start));
        } else {
            name = path.substring(0, end);
            node = this;
        }

        if (applyValues) {
            PropertySet existingPropertySet = node.getPropertySet(name);
            if (existingPropertySet != null) {
                ps.applyValues(existingPropertySet, false);
            }
        }
        node.addNamedElement(name, ps);
    }

    /**
     * Adds an object property to the PropertySet.
     * Duplicate property names are not supported.
     *
     * @param path the location of the property within this property set
     *             (does not include the property name itself)
     * @param p the property to add
     */
    public void addProperty(String path, Property p) {
        addProperty(path, p, false);
    }

    /**
     * Adds an object property to the PropertySet.
     * Duplicate property names are not supported.
     *
     * @param path the location of the property within this property set
     *             (does not include the property name itself)
     * @param p the property to add
     * @param applyValue if this parameter is <code>true</code>,
     *        the value of the existing property (if such exists) is applied
     *        the the new property prior adding it to the property set.
     *        Typical use - replacing auto-created properties with "real" ones.
     *        Has no effect if the new property is readonly
     */
    public void addProperty(String path, Property p, boolean applyValue) {

        if (path != null && path.length() > 0 && !isValidNodeName(path)) {
            throw new IllegalArgumentException(path);
        }

        // don't use getElementPath(path) because path does not include
        // the element name, i.e. getElementPath(path) would return the
        // property set one level up
        try { createPath(path).addProperty(p,applyValue); }
        catch (Exception x) {
            printStackTrace(x); // path conflict
        }
    }

    /**
     * Adds an object property to this property set.
     * Duplicate property names are not supported.
     *
     * @param p the property to add
     */
    public void addProperty(Property p) {
        addNamedElement(p.getName(), p);
    }

    /**
     * Adds an object property to this property set.
     * @param p the property to add
     * @param applyValue if this parameter is <code>true</code>,
     *        the value of the existing property (if such exists) is applied
     *        the the new property prior adding it to the property set.
     *        Typical use - replacing auto-created properties with "real" ones.
     *        Has no effect if the new property is readonly
     */
    public void addProperty(Property p, boolean applyValue) {
        if (applyValue && !p.isReadOnly()) {
            Property oldProp = getProperty(p.getName());
            if (oldProp != null) {
                // copy value from existing property the the new one prior to
                // adding the new property to the property set to avoid
                // producing unnecessary property change events
                try { p.setValue(oldProp.getValue()); }
                catch (Exception x) {
                    printStackTrace(x);
                }
            }
        }
        addProperty(p);
    }

    /**
     * Adds a standalone non-archiveable String property to this PropertySet.
     *
     * @param name    the full name of the property
     * @param value   the value of the property
     *
     * @return the created property descriptor
     */
    public Property addStringProperty(String name, String value) {
        return addStringProperty(name, value, false);
    }

    /**
     * Adds a standalone String property to this PropertySet.
     *
     * @param name    the full name of the property
     * @param value   the value of the property
     * @param archive if <code>true</code> then explicitly enable archiving
     *                for the property. Otherwise, explicitly disable archiving
     *                for the property.
     *
     * @return the created property descriptor
     */
    public Property addStringProperty(String name, String value, boolean archive) {
        return addProperty(name, String.class, value, archive);
    }

    /**
     * Adds a standalone non-archiveable integer property to this PropertySet.
     *
     * @param name    the full name of the property
     * @param value   the value of the property
     *
     * @return the created property descriptor
     */
    public Property addIntProperty(String name, int value) {
        return addIntProperty(name, value, false);
    }

    /**
     * Adds a standalone integer property to this PropertySet.
     *
     * @param name    the full name of the property
     * @param value   the value of the property
     * @param archive if <code>true</code> then explicitly enable archiving
     *                for the property. Otherwise, explicitly disable archiving
     *                for the property.
     *
     * @return the created property descriptor
     */
    public Property addIntProperty(String name, int value, boolean archive) {
        return addProperty(name, Integer.class, new Integer(value), archive);
    }

    /**
     * Adds a standalone non-archiveable boolean property to this PropertySet.
     *
     * @param name    the full name of the property
     * @param value   the value of the property
     *
     * @return the created property descriptor
     */
    public Property addBooleanProperty(String name, boolean value) {
        return addBooleanProperty(name, value, false);
    }

    /**
     * Adds a standalone boolean property to this PropertySet.
     *
     * @param name    the full name of the property
     * @param value   the value of the property
     * @param archive if <code>true</code> then explicitly enable archiving
     *                for the property. Otherwise, explicitly disable archiving
     *                for the property.
     *
     * @return the created property descriptor
     */
    public Property addBooleanProperty(String name, boolean value, boolean archive) {
        return addProperty(name, Boolean.class, Utils.getBoolean(value), archive);
    }

    /**
     * Adds a standalone non-archiveable property to this PropertySet.
     * Duplicate property names are not supported.
     * The default value of the property is <code>null</code>
     *
     * @param name    the full name of the property
     * @param type    the type of the property
     *
     * @return the created property descriptor
     */
    public Property addProperty(String name, Class type) {
        return addProperty(name, type, null, false);
    }

    /**
     * Adds a standalone property to this PropertySet.
     * Duplicate property names are not supported.
     * The default value of the property is <code>null</code>
     *
     * @param name    the full name of the property
     * @param type    the type of the property
     * @param archive if <code>true</code> then explicitly enable archiving
     *                for the property. Otherwise, explicitly disable archiving
     *                for the property.
     *
     * @return the created property descriptor
     */
    public Property addProperty(String name, Class type, boolean archive) {
        return addProperty(name, type, null, archive);
    }

    /**
     * Adds a standalone property to this PropertySet.
     * Duplicate property names are not supported.
     *
     * @param name    the full name of the property
     * @param type    the type of the property
     * @param archive if <code>true</code> then explicitly enable archiving
     *                for the property. Otherwise, explicitly disable archiving
     *                for the property.
     *
     * @return the created property descriptor
     */
    public Property addProperty(String name, Class type, Object defVal, boolean archive) {
        if (!isValidElementName(name)) {
            throw new IllegalArgumentException(name);
        }

        String pname = PropUtils.getPropertyName(name);
        Property p = new StandaloneProperty(pname,type,defVal,archive);
        getElementPath(name).addNamedElement(pname,p);
        return p;
    }

    /**
     * Adds a resource property to the PropertySet.
     * Duplicate property names are not supported.
     * <p>
     * @param name      the full (expanded) name of the resource
     * @param type      the type of the property
     * @param textValue the initial property value as text
     * @param defVal    the default value of the property if
     *                  <code>textVal</code> is invalid
     * @param archive   if <code>true</code> then explicitly enable archiving
     *                  for the property. Otherwise, explicitly disable
     *                  archiving for the property.
     *
     * @return the created property descriptor
     */
    public Property addProperty(String name, Class type, String textValue, Object defVal, boolean archive) {
        if (!isValidElementName(name)) {
            throw new IllegalArgumentException(name);
        }

        if (textValue != null) {
            PropertyEditor editor = PropertyEditors.getEditor(type);
            if (editor != null) {
                editor.setAsText(textValue);
                defVal = editor.getValue();
            }
        }

        String pname = PropUtils.getPropertyName(name);
        Property p = new StandaloneProperty(pname,type,defVal,archive);
        getElementPath(name).addNamedElement(pname,p);
        return p;
    }

    /**
     * Adds a standalone property to this PropertySet.
     *
     * @param path the full name of the property
     * @return the created property descriptor
     */
    public Property addProperty(PropertySet ps, String path) {
        Property p = ps.getProperty(path);
        if (p != null) addProperty(PropUtils.getNodeName(path), p);
        return p;
    }

    /**
     * Removes a property or a property set from this PropertySet.
     *
     * @param path  the full path to the property or a property set
     *              within this property set
     * @return the removed property or property set.
     *         If no property or property set with this name was found,
     *         returns <code>null</code>
     */
    public Object remove(String path) {
        return removeElement(path, null);
    }

    /**
     * Removes a property from this PropertySet.
     *
     * @param path the full path to the property within this property set
     * @return the removed property.
     *         If property was not found, returns <code>null</code>
     */
    public Property removeProperty(String path) {
        return (Property)removeElement(path, Property.class);
    }

    /**
     * Removes a subset from this PropertySet.
     * @return the removed PropertySet.
     *         If subset was not found, returns <code>null</code>
     */
    public PropertySet removePropertySet(String path) {
        return (PropertySet)removeElement(path, PropertySet.class);
    }

    /**
     * Finds a contained element (Property or PropertySet) by name.
     * This method will recurse through any contained PropertySet
     * elements as necessary.
     *
     * @param name the dot-delimited name of the element for which to search.
     * @aram elementClass type of the element to remove, <code>null</code>
     *                    if it doesn't matter
     *
     * @return removed Property or PropertySet or <code>null</code> if the name
     *         is not defined
     */
    private Object removeElement(String name, Class elementClass) {
        if (name != null) {
            String elementName;
            PropertySet node;

            int dot = name.lastIndexOf('.');
            if (dot >= 0) {
                Object mustBeNode = findNamedElement(name.substring(0, dot));
                if (!(mustBeNode instanceof PropertySet)) return null;

                node = (PropertySet)mustBeNode;
                elementName = name.substring(dot+1);
            } else {
                node = this;
                elementName = name;
            }

            Object elem = node.getNamedElement(elementName);
            if (elementClass == null || elementClass.isInstance(elem)) {
                return node.removeNamedElement(elementName);
            }
        }
        return null;
    }

    /**
     * Removes all elements from the property set
     */
    public void removeAll(boolean recurse) {
        if (!isEmpty()) {
            List c = getElementNameList();
            String [] names = (String[])c.toArray(new String[c.size()]);
            for (int i=names.length-1; i>=0; i--) {
                String name = names[i];
                Object elem = getNamedElementMap().get(name);
                if (elem instanceof PropertySet) {
                    if (recurse) {
                        PropertySet ps = (PropertySet)elem;
                        ps.removeAll(recurse);
                        removeNamedElement(name);
                    }
                } else if (elem instanceof Property) {
                    removeNamedElement(name);
                } else if (Trace.isVerbose()) {
                    printStackTrace(new Exception("unexpected element: "+elem));
                }
            }
        }
    }

    /**
     * Recursively removes all elements from the property set
     */
    public void removeAll() {
        removeAll(true);
    }

    /**
     * Gets a deep copy of this PropertySet.
     * The properties will be an exact copy of the current set of properties.
     *
     * @return the copied properties
     */
    public PropertySet getCopy() {
        return getCopy(new PropertySet(), false);
    }

    /**
     * Gets a deep copy of this PropertySet.
     * The properties will be an exact copy of the current set of properties.
     *
     * @param shallow If <code>true</code>, then only copy the hierarhical
     *                structure (the property sets), don't clone the
     *                properties. Otherwise, copy the properties, too
     *
     * @return the copied properties
     */
    public PropertySet getCopy(boolean shallow) {
        return getCopy(new PropertySet(), shallow);
    }

    /**
     * Gets a deep copy of this PropertySet.
     * The properties will be an exact copy of the current set of properties.
     *
     * @param copy    The destination PropertySet to copy properties into.
     * @param shallow If <code>true</code>, then only copy the hierarhical
     *                structure (the property sets), don't clone the
     *                properties. Otherwise, copy the properties, too
     * @return the destination PropertySet
     */
    private PropertySet getCopy(PropertySet copy, boolean shallow) {
        for (Iterator i = getElementNames(); i.hasNext();) {
            String name = (String)i.next();
            Object elem = getNamedElement(name);
            if (elem instanceof Property) {
                Property p = (Property)elem;
                if (shallow) {
                    copy.addProperty(p);
                } else {    // Ownerless copy:
                    copy.addProperty(name,
                                     p.getType(), null,
                                     p.getValue(),
                                     p.isArchiveable());
                }
            } else if (elem instanceof PropertySet) {
                PropertySet pSrc = (PropertySet)elem;
                PropertySet pDst = copy.createPropertySet(name);
                pSrc.getCopy(pDst, shallow);
            }
        }
        return copy;
    }

    /**
     * Gets the PropertySet that will contain a named element.
     * Subordinate PropertySets will be created as required
     * to accomplish this task.
     *
     * @param name the element name
     *
     * @return the deepest node in the path
     *         or <code>null</code> if the path would conflict
     *         with an existing property name
     */
    private PropertySet getElementPath(String name) {
        int dotPosn = name.lastIndexOf('.');
        if (dotPosn >= 0) {
            try { return createPath(name.substring(0, dotPosn)); }
            catch (Exception ex) {
                printStackTrace(ex);   // probably a path conflict
                return null;
            }
        } else {
            return this;
        }
    }

    /**
     * Gets the PropertySet represented by a pathname.
     * Subordinate PropertySets will be created as required
     * to accomplish this task;
     * @param pathname the relative pathname to create or retrieve
     * @return the deepest node in the path
     * @exception IllegalArgumentException if the pathname conflicts with
     *                                     existing property names
     */
    private PropertySet createPath(String pathname) {
        if (pathname == null) return this;
        PropertySet p = null;
        int dotPosn = pathname.indexOf('.');
        if (dotPosn >= 0) {
            // Create or find subnode
            PropertySet subNode;
            if (dotPosn == 0) {
                subNode = this;
            } else {
                String name = pathname.substring(0, dotPosn);
                Object elem = getNamedElement(name);
                if (elem instanceof PropertySet) {
                    subNode = (PropertySet)elem;
                } else if (elem == null) {
                    subNode = new PropertySet(true);
                    addNamedElement(name,subNode);
                } else {
                    throw new IllegalArgumentException("Path Conflict: "+pathname);
                }
            }
            p = subNode.createPath(pathname.substring(dotPosn+1));
        } else if (pathname.length() == 0) {
            p = this;
        } else {
            Object elem = getNamedElement(pathname);
            if (elem instanceof PropertySet) {
                p = (PropertySet)elem;
            } else if (elem == null) {
                p = new PropertySet(true);
                addNamedElement(pathname, p);
            } else {
                throw new IllegalArgumentException("Path Conflict: "+pathname);
            }
        }
        return p;
    }

    /**
     * Verifies a valid node name.
     * This will be called prior to adding a new name to the collection.
     * @param name a nodename
     * @return <code>true</code> if the name is considered valid
     */
    private boolean isValidNodeName(String name) {
        return isValidName(name, true);
    }

    /**
     * Verifies a valid element name.
     * This will be called prior to adding a new name to the collection.
     * @param name an element name
     * @return <code>true</code> if the name is considered valid
     */
    private boolean isValidElementName(String name) {
        return isValidName(name, false);
    }

    /**
     * Verifies a valid element name.
     * This will be called prior to adding a new name to the collection.
     * @param name an element name
     * @param node <code>true</code> if this is a node name
     * @return <code>true</code> if the name is considered valid
     */
    private boolean isValidName(String name, boolean node) {
        boolean valid = ((name != null) && (name.length() > 0));
        if (valid) {
            // Cannot start with a delimiter:
            valid = !name.startsWith(".");
        }
        if (valid && !node) {
            // Cannot end with a delimiter unless it's a node name:
            valid = !name.endsWith(".");
        }
        if (valid) {
            // Cannot contain contiguous delimiters:
            valid = (name.indexOf("..") < 0);
        }
        if (valid) {
            // Cannot start with invalid identifier start character:
            char start = name.charAt(0);
            valid = (Character.isUnicodeIdentifierStart(start) ||
                     Character.isDigit(start));
        }
        if (valid) {
            for (int i = 1; valid && (i < name.length()); i++) {
                char c = name.charAt(i);
                // Cannot contain whitespace:
                if (Character.isWhitespace(c)) {
                    valid = false;
                    break;
                }
                // Cannot contain invalid identifier characters (except delimiter):
                if (Character.isIdentifierIgnorable(c) && (c != '.')) {
                    valid = false;
                    break;
                }
            }
        }
        return valid;
    }

    /**
     * Adds a named element to the PropertySet or replaces the existing one.
     * @param name the name of the element (already validated)
     * @param elem either a <code>Property</code> or a <code>PropertySet</code>
     */
    private void addNamedElement(String name, Object elem) {
        addNamedElement(name, elem, getElementNameList().size());
    }

    /**
     * Adds a named element to the PropertySet or replaces the existing one.
     * @param name the name of the element (already validated)
     * @param elem either a <code>Property</code> or a <code>PropertySet</code>
     * @param index the index of the new element. This parameter is ignored
     *              if existing entry is being replaced.
     */
    private void addNamedElement(String name, Object elem, int index) {
        Object prevElem = getNamedElementMap().get(name);
        if (prevElem == elem) {  // Nothing to do
            return;
        } else if (prevElem != null) {    // Name already exists
            index = getElementNameList().indexOf(name);
            getElementNameList().remove(index);
            getNamedElementMap().remove(name);
            cleanupAfter(name, prevElem);
        }

        // Add named element:
        getElementNameList().add(index, name);
        getNamedElementMap().put(name, elem);

        // Add forwarding PropertyChangeListener:
        if (elem instanceof PropertySet) {
            PropertySet ps = (PropertySet)elem;
            PropertySetChangeForwarder fwd = getPropertySetChangeForwarder();
            ps.addBulkChangeListener(fwd);
            ps.addPropertySetListener(fwd);
            ps.addPropertyChangeListener(fwd);
            firePropertySetAdded(name);
        } else if (elem instanceof Property) {
            Property p = (Property)elem;
            // Do not add property change listeners to readonly properties
            if (!p.isReadOnly()) {
                p.addPropertyChangeListener(getPropertyChangeForwarder());
            }
            firePropertyAdded(name);
        }
    }

    /**
     * Removes a named element from the PropertySet.
     * @param name the name of the element (already validated)
     * @return the removed element, <code>null</code> if element with such name
     *         did not exist in this property set
     */
    private Object removeNamedElement(String name) {
        if (elementNames != null) {
            elementNames.remove(name);
            Object elem = getNamedElementMap().remove(name);
            cleanupAfter(name, elem);
            return elem;
        } else {
            return null;
        }
    }

    /**
     * Returns the name of the immediate subset of this property set
     * @param subSet a child property set
     * @return the property set name, <code>null</code> if the element
     *          wasn't found
     */
    private String getChildName(PropertySet subSet) {
        if (namedElements != null) {
            // NOTE: this is quite inefficient. Perhaps, we should have
            // separate maps for properties and child property sets?
            Iterator entries = namedElements.entrySet().iterator();
            while (entries.hasNext()) {
                Map.Entry entry = (Map.Entry)entries.next();
                if (entry.getValue() == subSet) {
                    return (String)entry.getKey();
                }
            }
        }
        return null;
    }

    /**
     * Cleans up after an element removed from this property set
     * @param name the element name
     * @param elem the removed element, <code>null</code> is ignored
     */
    private void cleanupAfter(String name, Object elem) {
        if (elem != null) {
            if (archiveMap != null) archiveMap.remove(name);
            if (elem instanceof PropertySet) {
                PropertySet ps = ((PropertySet)elem);
                if (propertySetChangeForwarder != null) {
                    ps.removeBulkChangeListener(propertySetChangeForwarder);
                    ps.removePropertySetListener(propertySetChangeForwarder);
                    ps.removePropertyChangeListener(propertySetChangeForwarder);
                }
                firePropertySetRemoved(ps,name);
            } else {    // must be a property
                Property p = ((Property)elem);
                if (propertyChangeForwarder != null) {
                    p.removePropertyChangeListener(propertyChangeForwarder);
                }
                firePropertyRemoved(p,p.getName());
            }
        }
    }

    /**
     * Returns an enumeration of element names in the PropertySet.
     * @return an enumeration of element names in the PropertySet
     * (never <code>null</code>).
     */
    private Iterator getElementNames() {
        if (isEmpty()) {
            return EmptyIterator.getInstance();
        } else {
            return getElementNameList().iterator();
        }
    }

    /**
     * Returns all keys (element names) in this PropertySet in the order
     * in which they have been created. Does not recurse.
     *
     * @return all keys in this PropertySet, empty array if none.
     */
    public String [] getKeys() {
        if (isEmpty()) {
            return new String[0];
        } else {
            String [] keys = new String[getElementNameList().size()];
            return (String[])getElementNameList().toArray(keys);
        }
    }

    /**
     * Returns an enumeration of named elements in the PropertySet.
     * @return an enumeration of named elements in the PropertySet
     * (never <code>null</code>).
     */
    private Iterator getNamedElements() {
        if (isEmpty()) {
            return EmptyIterator.getInstance();
        } else {
            return getNamedElementMap().values().iterator();
        }
    }

    /**
     * Returns an enumeration of name-element entries in the PropertySet.
     * The iterator returns <code>Map.Entry</code> objects
     * @return an enumeration of name-element entries in the PropertySet
     * (never <code>null</code>).
     */
    private Iterator getElementMapEntries() {
        if (isEmpty()) {
            return EmptyIterator.getInstance();
        } else {
            return new ElementMapIterator();
        }
    }

    /**
     * This iterator returns Map.Entry objects in the order they were
     * added to the property set. If we were using the map iterator,
     * the order would not be guaranteed.
     *
     * This iterator does not support removal of the current element.
     */
    private class ElementMapIterator implements Iterator {
        private Iterator namesIterator;
        private Entry lastEntry;
        ElementMapIterator() {
            namesIterator = getElementNames();
        }
        public void remove() {
            namesIterator.remove();
            String name = lastEntry.getName();
            Object elem = getNamedElementMap().remove(name);
            cleanupAfter(name, elem);
        }
        public boolean hasNext() {
            return namesIterator.hasNext();
        }
        public Object next() {
            lastEntry = new Entry((String)namesIterator.next());
            return lastEntry;
        }

        /**
         * Map.Entry implementation. Does not support setValue operation.
         */
        private class Entry implements Map.Entry {
            private String name;
            Entry(String name) {
                this.name = name;
            }
            String getName() {
                return name;
            }
            public Object getKey() {
                return name;
            }
            public Object getValue() {
                return getNamedElementMap().get(name);
            }
            public Object setValue(Object value) {
                throw new UnsupportedOperationException("setValue");
            }
            public boolean equals(Object obj) {
                if (obj instanceof Map.Entry) {
                    Map.Entry that = (Map.Entry)obj;
                    return Utils.equals(this.getKey(),that.getKey()) &&
                           Utils.equals(this.getValue(),that.getValue());
                }
                return false;
            }
            public int hashCode() {
                return Utils.hashCode(this.getKey()) ^
                       Utils.hashCode(this.getValue());
            }
        }
    }

    /**
     * @return the ordered set of element names.
     */
    private List getElementNameList() {
        if (elementNames == null) {
            elementNames = new Vector(2, 6);
        }
        return elementNames;
    }

    /**
     * @return the set of named elements.
     */
    private Map getNamedElementMap() {
        if (namedElements == null) {
            namedElements = new Hashtable(2);
        }
        return namedElements;
    }

    /**
     * Checks if property set is empty.
     * @return <code>true</code> if this property set contains no properties
     * or child property sets.
     */
    public boolean isEmpty() {
        return ((namedElements != null) ? namedElements.isEmpty() : true);
    }

    /**
     * Returns the number of entries in the PropertySet. This includes
     * both properties and the child property sets.
     * @return the number of entries in the PropertySet.
     */
    public int getSize() {
        return ((namedElements != null) ? namedElements.size() : 0);
    }

    /**
     * Checks for the existence of a property.
     * @param name the name of the property
     * @return <code>true</code> if the property is defined
     */
    public boolean containsProperty(String name) {
        return (!isEmpty() && getProperty(name) != null);
    }

    /**
     * Checks for the existence of a property of the specified type.
     * @param name the name of the property
     * @param type the expected type, can be <code>null</code>
     * @return <code>true</code> if the property is defined and it has the
     *          expected type; <code>false</code> otherwise
     */
    public boolean containsProperty(String name, Class type) {
        if (!isEmpty()) {
            Property p = getProperty(name);
            if (p != null) {
                if (type == null) {
                    return true;
                } else {
                    return type.equals(p.getType());
                }
            }
        }
        return false;
    }

    /**
     * Checks for the existence of a child property set.
     * @param name the name of the property
     * @return <code>true</code> if the property is defined
     */
    public boolean containsPropertySet(String name) {
        return (!isEmpty() && getPropertySet(name) != null);
    }

    /**
     * Finds a contained element (Property or PropertySet) by name.
     * This method will recurse through any contained PropertySet elements as necessary.
     * @param name the dot-delimited name of the element for which to search.
     * @return a Property or a PropertySet or <code>null</code> if the name is not defined
     */
    private Object findNamedElement(String name) {
        Object elem = null;
        if (name != null) {
            int dotPosn = name.indexOf('.');
            if (dotPosn >= 0) {
                // Search on topmost name component:
                elem = getNamedElement(name.substring(0, dotPosn));
                if (elem instanceof PropertySet) {
                    // Search on remaining name component(s):
                    elem = ((PropertySet)elem).findNamedElement(name.substring(dotPosn+1));
                } else {
                    elem = null;
                }
            } else {
                // Simple name; simple search:
                elem = getNamedElement(name);
            }
        }
        return elem;
    }

    /**
     * Gets a contained element (Property or PropertySet) by name.
     * @param name the name of the element for which to search.
     * @return a Property or a PropertySet or <code>null</code> if the name is not defined
     */
    private Object getNamedElement(String name) {
        return (name != null && !isEmpty()) ? getNamedElementMap().get(name) : null;
    }

    /**
     * Gets a property by name.
     * @param name the name of the property to search for
     * @return the Property or <code>null</code> if not defined
     */
    public Property getProperty(String name) {
        if (name == null) throw new NullPointerException("name");
        if (name.length() == 0) throw new IllegalArgumentException("name");
        Object elem = findNamedElement(name);
        return (elem instanceof Property) ? (Property)elem : null;
    }

    /**
     * Gets a property subset by name.
     * @param name the name of the property subset to search for
     * @return the PropertySet for the property subset or <code>null</code>
     * if the property subset is not defined
     */
    public PropertySet getPropertySet(String name) {
        // allow node names end with "."
        if (name == null) throw new NullPointerException("name");
        if (name.endsWith(".")) name = name.substring(0,name.length()-1);
        if (name.length() == 0) throw new IllegalArgumentException("name");
        Object elem = findNamedElement(name);
        return (elem instanceof PropertySet) ? (PropertySet)elem : null;
    }

    /**
     * Returns an enumeration of elements in the PropertySet.
     * @param recurse if <code>true</code> then recurse through subsets
     * @return an enumeration of elements in the PropertySet
     */
    public PropertySetIterator getElements(boolean recurse) {
        return new PropertySetIterator(this, recurse, PropertySetIterator.TYPE_BOTH);
    }

    /**
     * Returns an enumeration of properties in the PropertySet.
     * @param recurse if <code>true</code> then recurse through subsets
     * @return an enumeration of elements in the PropertySet
     */
    public PropertySetIterator getProperties(boolean recurse) {
        return new PropertySetIterator(this, recurse, PropertySetIterator.TYPE_PROPERTY);
    }

    /**
     * Returns an enumeration of property sets in this PropertySet.
     * @param recurse if <code>true</code> then recurse through subsets
     * @return an enumeration of elements in the PropertySet
     */
    public PropertySetIterator getPropertySets(boolean recurse) {
        return new PropertySetIterator(this, recurse, PropertySetIterator.TYPE_PROPERTY_SET);
    }

    /**
     * Gets the value of a property. If property returns <code>null</code>,
     * returns the specified default value.
     * @param p the property (must not be <code>null</code>)
     * @param defValue the default value of the property if not defined
     * @return the property value or the supplied default if not defined
     */
    private static Object getValue(Property p, Object defValue) {
        Object value = p.getValue();
        return ((value == null) ? defValue : value);
    }

    /**
     * Gets the value of a property.
     * @param name the name of the property
     * @return the property value or <code>null</code> if not defined
     */
    public Object getValue(String name) {
        return getValue(name, null);
    }

    /**
     * Gets the value of a property.
     * @param name the name of the property
     * @param defValue the default value of the property if not defined
     * @return the property value or the supplied default if not defined
     */
    public Object getValue(String name, Object defValue) {
        Property p = getProperty(name);
        return ((p == null) ? defValue : getValue(p, defValue));
    }

    /**
     * Gets the value of a <code>String</code> property.
     * This is a convenience method for a common property type.
     * @param name the name of the property
     * @param defValue the default value of the property if not defined
     * @return the property value or the supplied default if not defined
     */
    public String getStringValue(String name, String defValue) {
        Object v = getValue(name, defValue);
        return (v instanceof String) ? ((String)v) : defValue;
    }

    /**
     * Gets the value of a <code>String</code> property.
     * This is a convenience method for a common property type.
     * @param name the name of the property
     * @return the property value or <code>null</code> if not defined
     */
    public String getStringValue(String name) {
        return getStringValue(name, null);
    }

    /**
     * Gets the value of an integer property.
     * This is a convenience method for a common property type.
     * @param name the name of the property
     * @param defValue the default value of the property if not defined
     * @return the property value or the supplied default if not defined
     */
    public int getIntValue(String name, int defValue) {
        Object v = getValue(name, null);
        return (v instanceof Integer) ? ((Integer)v).intValue() : defValue;
    }

    /**
     * Gets the value of an boolean property.
     * This is a convenience method for a common property type.
     * @param name the name of the property
     * @param defValue the default value of the property if not defined
     * @return the property value or the supplied default if not defined
     */
    public boolean getBooleanValue(String name, boolean defValue) {
        Object v = getValue(name, null);
        return (v instanceof Boolean) ? ((Boolean)v).booleanValue() : defValue;
    }

    /**
     * Sets the value of a property.
     * @param name the name of the property
     * @param value the value of the property
     * @return <code>true</code> if the value was set, <code>false</code>
     * if it wasn't, most likely because the value is read-only
     */
    public boolean setValue(String name, Object value) {
        Property p = getProperty(name);
        if (p != null && !p.isReadOnly()) {
            // the setValue call may have side effects that change
            // more properties in the same property set. Setting the
            // bulkChangeInProgress may reduce the number of property
            // set changed notifications.
            beginBulkChange();
            try {
                p.setValue(value);
                return true;
            } catch (Exception x) {
                printStackTrace(x);
            } finally {
                endBulkChange();
            }
        }
        return false;
    }

    /**
     * Sets the value of a boolean property. The value is stored as a
     * {@link Boolean} object.
     *
     * @param name the name of the property
     * @param value the value of the property
     *
     * @return <code>true</code> if the value was set, <code>false</code>
     * if it wasn't, most likely because the value is read-only
     */
    public boolean setBooleanValue(String name, boolean value) {
        return setValue(name, Utils.getBoolean(value));
    }

    /**
     * Sets the value of an integer property. The value is stored as an
     * {@link Integer} object.
     *
     * @param name the name of the property
     * @param value the value of the property
     *
     * @return <code>true</code> if the value was set, <code>false</code>
     * if it wasn't, most likely because the value is read-only
     */
    public boolean setIntValue(String name, int value) {
        return setValue(name, new Integer(value));
    }

    /**
     * Utility function that compares two values. If values are arrays,
     * compares individual elements of the arrays
     */
    public static boolean valueEqual(Object o1, Object o2) {
        if (Utils.equals(o1,o2)) return true;
        if ((o1 instanceof Object[]) && (o2 instanceof Object[])) {
            Object [] a1 = (Object[])o1;
            Object [] a2 = (Object[])o2;
            if (a1.length == a2.length) {
                int n = a1.length;
                for(int i=0; i<n; i++) {
                    if (!valueEqual(a1[i],a2[i])) {
                        return false;
                    }
                }
                return true;
            }
        }
        return false;
    }

    /**
     * Gets the forwarding PropertyChangeListener instance for property sets.
     * This listener will simply forward all PropertyChangeEvents to the
     * registered listeners.
     *
     * @return a PropertyChangeListener instance for this PropertySet
     */
    private PropertySetChangeForwarder getPropertySetChangeForwarder() {
        if (propertySetChangeForwarder == null) {
            propertySetChangeForwarder = new PropertySetChangeForwarder(this);
        }
        return propertySetChangeForwarder;
    }

    /**
     * Gets the forwarding PropertyChangeListener instance for properties.
     * This listener will simply forward all PropertyChangeEvents to the
     * registered listeners.
     *
     * @return a PropertyChangeListener instance for this PropertySet
     */
    private PropertyChangeListener getPropertyChangeForwarder() {
        if (propertyChangeForwarder == null) {
            propertyChangeForwarder = new PropertyChangeForwarder(this);
        }
        return propertyChangeForwarder;
    }

    /**
     * Returns the instance of PropertyChangeSupport used by this property
     * set. Creates one if necessary.
     *
     * @return a PropertyChangeSupport instance for this PropertySet
     */
    private PropertyChangeSupport getPropertyChangeSupport() {
        if (pcs == null) {
            pcs = new PropertyChangeSupport(this);
        }
        return pcs;
    }

    /**
     * Adds a PropertyChangeListener to the PropertySet.
     * The listener is registered for all properties. The source of the
     * property change events received by this listener will be this
     * property set.
     *
     * @see #removePropertyChangeListener(PropertyChangeListener)
     *
     * @param listener  The PropertyChangeListener to be added
     */
    public void addPropertyChangeListener(PropertyChangeListener listener) {
        getPropertyChangeSupport().addPropertyChangeListener(listener);
    }

    /**
     * Removes a PropertyChangeListener from the PropertySet.
     * This removes a PropertyChangeListener that was registered
     * for all properties.
     * @see #addPropertyChangeListener(PropertyChangeListener)
     *
     * @param listener  The PropertyChangeListener to be removed
     */
    public void removePropertyChangeListener(PropertyChangeListener listener) {
        if (pcs != null) {
            pcs.removePropertyChangeListener(listener);
        }
    }

    /**
     * Adds a PropertyChangeListener for a property with a specific name.
     * The source of the property change events received by this listener
     * will be this property set.
     *
     * @param name  The name of the property to listen on.
     * @param listener  The PropertyChangeListener to be added
     *
     * @see #removePropertyChangeListener(String,PropertyChangeListener)
     */
    public void addPropertyChangeListener(String name, PropertyChangeListener listener) {
        getPropertyChangeSupport().addPropertyChangeListener(name, listener);
    }

    /**
     * Removes a PropertyChangeListener for a property with a specific name.
     *
     * @param name  The name of the property that was listened on.
     * @param listener  The PropertyChangeListener to be removed
     *
     * @see #addPropertyChangeListener(String,PropertyChangeListener)
     */
    public void removePropertyChangeListener(String name, PropertyChangeListener listener) {
        if (pcs != null) {
            pcs.removePropertyChangeListener(name, listener);
        }
    }

    /**
     * This method gets called when a bound property is changed.
     * @param evt A PropertyChangeEvent object describing the event source and the property that has changed.
     */
    private void firePropertyChange(PropertyChangeEvent evt) {
        if (evt.getSource() != this) throw new IllegalArgumentException();
        if (pcs != null) {
            pcs.firePropertyChange(evt);
        }
    }

    /**
     * Adds {@link PropertySetListener} to this property set
     */
    public void addPropertySetListener(PropertySetListener listener) {
        if (listener == null) throw new NullPointerException();
        if (propertySetListeners == null) {
            propertySetListeners = new PropertySetListener[] {listener};
        } else {
            propertySetListeners = (PropertySetListener[])
                ArrayUtils.add(propertySetListeners,listener);
        }
    }

    /**
     * Removes {@link PropertySetListener} to this property set
     */
    public void removePropertySetListener(PropertySetListener listener) {
        if (propertySetListeners != null) {
            if (propertySetListeners.length == 1) {
                if (propertySetListeners[0] == listener) {
                    propertySetListeners = null;
                }
            } else {
                propertySetListeners = (PropertySetListener[])
                    ArrayUtils.remove(propertySetListeners, listener);
            }
        }
    }

    /**
     * Adds {@link BulkChangeListener} to this property set
     */
    public void addBulkChangeListener(BulkChangeListener listener) {
        if (listener == null) throw new NullPointerException();
        if (bulkChangeListeners == null) {
            bulkChangeListeners = new BulkChangeListener[] {listener};
        } else {
            bulkChangeListeners = (BulkChangeListener[])
                ArrayUtils.add(bulkChangeListeners,listener);
        }
    }

    /**
     * Removes {@link BulkChangeListener} to this property set
     */
    public void removeBulkChangeListener(BulkChangeListener listener) {
        if (bulkChangeListeners != null) {
            if (bulkChangeListeners.length == 1) {
                if (bulkChangeListeners[0] == listener) {
                    bulkChangeListeners = null;
                }
            } else {
                bulkChangeListeners = (BulkChangeListener[])
                    ArrayUtils.remove(bulkChangeListeners, listener);
            }
        }
    }

    /**
     * Notifies the listeners that a child property set is added
     * @param path the full path of the new property set, relative to ps
     */
    private void firePropertySetAdded(String path) {
        PropertySetListener [] listeners = propertySetListeners;
        if (listeners != null) {
            for (int i=listeners.length-1; i>=0; i--) {
                listeners[i].propertySetAdded(this,path);
            }
        }
    }

    /**
     * Notifies the listeners that a child property set is removed
     * @param child the property set that has been removed
     * @param path the full path of the removed property set, relative to ps
     */
    private void firePropertySetRemoved(PropertySet child, String path) {
        PropertySetListener [] listeners = propertySetListeners;
        if (listeners != null) {
            for (int i=listeners.length-1; i>=0; i--) {
                listeners[i].propertySetRemoved(this,child,path);
            }
        }
    }

    /**
     * Notifies the listeners that a property is added
     * @param path the full path of the new property, relative to ps
     */
    private void firePropertyAdded(String path) {
        PropertySetListener [] listeners = propertySetListeners;
        if (listeners != null) {
            for (int i=listeners.length-1; i>=0; i--) {
                listeners[i].propertyAdded(this,path);
            }
        }
    }

    /**
     * Notifies the listeners that a property is removed
     * @param p the property that has been removed
     * @param path the full path of the removed property, relative to ps
     */
    private void firePropertyRemoved(Property p, String path) {
        PropertySetListener [] listeners = propertySetListeners;
        if (listeners != null) {
            for (int i=listeners.length-1; i>=0; i--) {
                listeners[i].propertyRemoved(this,p,path);
            }
        }
    }

    /**
     * Notifies the listeners that something within a property
     * set hierarhy has changed
     */
    public void firePropertySetChanged() {
        PropertySetListener [] listeners = propertySetListeners;
        if (listeners != null) {
            PropertySet src = PropertySet.this;
            for (int i=listeners.length-1; i>=0; i--) {
                listeners[i].propertySetChanged(src);
            }
        }
    }

    /**
     * Determines whether this Property may be included in an archive store
     * operation.
     * @return <code>true</code> if the property may be included in a store
     * operation.
     */
    private boolean isArchiveable(Property p) {
        if (!p.isArchiveable()) {
            return false;
        } else {
            if (archiveMap != null) {
                Boolean flag = (Boolean)archiveMap.get(p.getName());
                if (flag != null) {
                    return flag.booleanValue();
                }
            }
            return true;
        }
    }

    /**
     * Determines whether this Property may be included in an archive store
     * operation.
     * @return <code>true</code> if the property may be included in a store
     * operation.
     */
    private boolean isArchiveable(String name, PropertySet ps) {
        if (!ps.isArchiveable()) {
            return false;
        } else {
            if (archiveMap != null) {
                Boolean flag = (Boolean)archiveMap.get(name);
                if (flag != null) {
                    return flag.booleanValue();
                }
            }
            return true;
        }
    }

    /**
     * Determines whether this PropertySet may be included in an archive store operation.
     * @return <code>true</code> if the property may be included in a store operation.
     */
    public boolean isArchiveable() {
        boolean storeable = archiveEnabled && !isEmpty();
        if (storeable) {
            storeable = false;
            for (Iterator i = getElementNames(); !storeable && i.hasNext(); ) {
                String name = (String)i.next();
                Object elem = getNamedElement(name);
                if (elem instanceof Property) {
                    storeable = isArchiveable((Property)elem);
                } else if (elem instanceof PropertySet) {
                    storeable = ((PropertySet)elem).isArchiveable();
                }
            }
        }
        return storeable;
    }

    /**
     * Allow or disallow including the property in an archive store
     * operation.
     *
     * @param name the property name
     * @param f <code>true</code> if all the specified property in this
     *          property set may be included in a store operation,
     *          <code>false</code> to disallow storing this property
     *          in an archive
     */
    public void setArchiveable(String name, boolean f) {
        Object elem = null;
        int dotPosn = name.lastIndexOf('.');
        if (dotPosn >= 0) {
            String nodeName = name.substring(0, dotPosn);
            if (!containsProperty(nodeName)) {
                String propName = name.substring(dotPosn+1);
                if (f) {
                    PropertySet ps = getPropertySet(nodeName);
                    if (ps != null) {
                        ps.setArchiveable(propName, f);
                    }
                } else {
                    // Must create property set
                    createPropertySet(nodeName).setArchiveable(propName,f);
                }
            }
        } else {
            // Simple name
            if (f) {
                if (archiveMap != null) {
                    archiveMap.remove(name);
                }
            } else {
                if (archiveMap == null) {
                    archiveMap = new Hashtable();
                }
                archiveMap.put(name, Utils.getBoolean(f));
            }
        }
    }

    /**
     * Allow or disallow including specific property in an archive store
     * operation.
     *
     * @param f <code>true</code> if all the properties in this property
     *          set may be included in a store operation, <code>false</code>
     *          to disallow storing this property set in an archive
     */
    public void setArchiveable(boolean f) {
        archiveEnabled = f;
    }

    /**
     * Applies a single value to each property in this property set.
     * Only those properties that are mutable will be written to.
     * @param value the value to apply to each property
     */
    public void applyValue(Object value) {
        beginBulkChange();
        try {
            for (Iterator i = getElementNames(); i.hasNext(); ) {
                String name = (String)i.next();
                Object elem = getNamedElement(name);
                if (elem instanceof Property) {
                    Property p = (Property)elem;
                    if (!p.isReadOnly()) {
                        try { p.setValue(value); }
                        catch (Exception x) { printStackTrace(x); }
                    }
                } else if (elem instanceof PropertySet) {
                    ((PropertySet)elem).applyValue(value);
                }
            }
        } finally {
            endBulkChange();
        }
    }

    /**
     * Applies any default property values to this property set.
     * Only those properties that are mutable will be written to.
     */
    public void applyDefaultValues() {
        beginBulkChange();
        try {
            for (Iterator i = getElementNames(); i.hasNext(); ) {
                String name = (String)i.next();
                Object elem = getNamedElement(name);
                if (elem instanceof Property) {
                    Property pDst = (Property)elem;
                    if (!pDst.isReadOnly()) {

                        // Get and assign the default property value
                        // if defined:
                        Object value = pDst.getDefaultValue();
                        if (value != null) {
                            try { pDst.setValue(value); }
                            catch (Exception x) { printStackTrace(x); }
                        }
                    }
                } else if (elem instanceof PropertySet) {
                    ((PropertySet)elem).applyDefaultValues();
                }
            }
        } finally {
            endBulkChange();
        }
    }

    /**
     * Marks the beginning of a bulk change operation. If anything has
     * changed during the bulk change operation, only one propertySetChanged
     * event will be fired,
     *
     * @param bulk the objects that actually performs the bulk change.
     */
    private void beginBulkChange() {
        if (++bulkChangeInProgress == 1) {
            BulkChangeListener[] listeners = bulkChangeListeners;
            if (listeners != null) {
                for (int i = listeners.length - 1; i >= 0; i--) {
                    listeners[i].bulkChangeStarted(this);
                }
            }
        }
    }

    /**
     * Marks the end of a bulk change operation. If anything has changed
     * during the bulk change operation, only one propertySetChanged
     * event will be fired,
     *
     * @param bulk the objects that actually performs the bulk change.
     */
    private void endBulkChange() {
        bulkChangeInProgress--;
        if (bulkChangeInProgress == 0) {
            boolean changed = bulkChangePending;
            bulkChangePending = false;

            // Notify listeners that all changes have been completed:
            if (changed) {
                firePropertySetChanged();
            }

            // notify bulk change listeners that the bulk change is finished
            BulkChangeListener[] listeners = bulkChangeListeners;
            if (listeners != null) {
                for (int i = listeners.length - 1; i >= 0; i--) {
                    listeners[i].bulkChangeFinished(this, changed);
                }
            }
        }
    }

    /**
     * Modifies the property set. These modifications result in a
     * single "propertySetChanged" event when modification is complete.
     * No "propertySetChanged" event is fired for the individual changes
     * during the modification.
     *
     * @param bulk the objects that actually performs the bulk change.
     */
    public void bulkChange(BulkChange bulk) {
        try {
            beginBulkChange();
            bulk.change(this);
        } catch (Throwable x) {
            printStackTrace(x);
        } finally {
            endBulkChange();
        }
    }

    /**
     * Applies the property values from another set of properties
     * into the corresponding and mutable properties of this set.
     * Only those properties that exist in this set and are mutable
     * will be copied.
     *
     * @param source a set of properties to copy values from.
     */
    public void applyValues(PropertySet source) {
        applyValues(source, false);
    }

    /**
     * Applies the property values from another set of properties
     * into the corresponding and mutable properties of this set.
     * Only those properties that exist in this set and are mutable
     * will be copied.
     *
     * @param source a set of properties to copy values from.
     * @param construct if <code>true</code>, then allow construction of
     *                  new properties that may not already be defined
     *                  in this PropertySet.
     */
    public void applyValues(PropertySet source, boolean construct) {
        if (source != null && source != this && !source.isEmpty()) {
            beginBulkChange();
            try {
                // Copy all property values:
                int index = 0;
                Iterator i = source.getElementNames();
                while (i.hasNext()) {
                    String name = (String)i.next();
                    Object elem = source.getNamedElement(name);
                    if (elem instanceof Property) {
                        Property pSrc = (Property)elem;
                        Property pDst = getProperty(name);
                        if (pDst == null && construct) {
                            // construct new property
                            pDst = new StandaloneProperty(pSrc);
                            addNamedElement(name, pDst, index);
                        } else if (pDst != null && pDst != pSrc) {
                            Object value = pSrc.getValue();
                            if (value == null) value = pDst.getDefaultValue();
                            try { pDst.setValue(value); }
                            catch (Exception x) { printStackTrace(x); }
                        }
                        index++;
                    } else if (elem instanceof PropertySet) {

                        // Recursively invoke child property sets to let them
                        // fire bulk property change events too
                        PropertySet pSrc = (PropertySet)elem;
                        if (!pSrc.isEmpty()) {
                            PropertySet pDst = getPropertySet(name);
                            if (pDst == null && construct) {
                                // construct new property set
                                pDst = new PropertySet();
                                addNamedElement(name, pDst, index);
                                // addNamedElement notifies the listeners
                                // whichmay replace the property set that
                                // we have created with a new one. Make
                                // sure that we pick up the right one
                                pDst = getPropertySet(name);
                            }
                            if (pDst != null && (construct || !pDst.isEmpty())) {
                                pDst.applyValues(pSrc, construct);
                            }
                            index++;
                        }
                    }
                }
            } finally {
                endBulkChange();
            }
        }
    }

    /**
     * Applies the values of all mutable properties from a set of
     * text properties.
     *
     * @param textProp a set of text properties to get property values from.
     */
    public void applyTextValues(Properties textProp) {
        if (textProp != null && !textProp.isEmpty()) {
            beginBulkChange();
            try {
                // Parse and restore the property values:
                Iterator entries = getElementMapEntries();
                while (entries.hasNext()) {
                    Map.Entry entry = (Map.Entry)entries.next();
                    Object elem = entry.getValue();
                    if (elem instanceof Property) {

                        // Set property in this property set:
                        Property pDst = (Property)elem;
                        if (!pDst.isReadOnly()) {
                            String name = pDst.getName();
                            String textValue = textProp.getProperty(name);
                            if (textValue != null) {
                                try {
                                    pDst.setValueAsText(textValue);
                                } catch (Exception y) {
                                    printStackTrace(y);
                                }
                            }
                        }

                    } else if (elem instanceof PropertySet) {

                        // Apply properties to the child property set:
                        PropertySet pDst = (PropertySet)elem;
                        String prefix = ((String)entry.getKey()) + '.';
                        Properties pSrc = Utils.extract(textProp, prefix);
                        if (pSrc != null) {
                            pDst.applyTextValues(pSrc);
                        }
                    }
                }
            } finally {
                endBulkChange();
            }
        }
    }

    /**
     * Loads <code>PropertySet</code> from an {@link InputStream}.
     * @param in an input stream to load properties from.
     * @exception java.io.IOException if an error occurred while reading
     * the input.
     */
    public void load(InputStream in) throws IOException {
        beginBulkChange();
        try {
            Archiver.loadPropertySet(this, new InputStreamReader(in), true);
        } finally {
            endBulkChange();
        }
    }

    /**
     * Loads <code>PropertySet</code> from a {@link Reader}.
     * @param in a reader to load properties from.
     * @exception java.io.IOException if an error occurred while reading
     * the input.
     */
    public void load(Reader in) throws IOException {
        beginBulkChange();
        try {
            Archiver.loadPropertySet(this, in, true);
        } finally {
            endBulkChange();
        }
    }

    /**
     * Loads <code>PropertySet</code> from a {@link Reader}.
     * @param in a reader to load properties from.
     * @param construct if <code>true</code>, then allow construction
     *                  of new properties that may not already be defined
     *                  in this PropertySet.
     * @exception java.io.IOException if an error occurred while reading
     * the input.
     */
    public void load(Reader in, boolean construct) throws IOException {
        beginBulkChange();
        try {
            Archiver.loadPropertySet(this, in, construct);
        } finally {
            endBulkChange();
        }
    }

    /**
     * Loads a set of property values from a file.
     * Normally, values will be assigned to existing
     * properties that are already defined when the
     * corresponding property is found in the file.
     * If specified, then new properties and property
     * groups that may not already be defined in the
     * current PropertySet may also be created as
     * required.
     *
     * @param fin an input file to load properties from.
     * @param construct if <code>true</code>, then allow construction
     *                  of new properties that may not already be defined
     *                  in this PropertySet.
     * @return <code>true</code> if no error occurred when reading from
     *          the file. If the file does not exist, then that is not
     *          considered an error.
     */
    public boolean load(File fin, boolean construct) {
        boolean success = false;
        if ((fin != null) && fin.exists()) {
            try {
                Reader reader = new FileReader(fin);
                load(reader, construct);
                reader.close();
                success = true;
            }
            catch (IOException ex) {
                // dumpt it?
            }
        }
        return success;
    }

    /**
     * Saves all archiveable properties into an XML file.
     *
     * @param fout an output file to send properties to.
     * @param title an identifying title to include in the header comment
     *              of the archive.
     * @return <code>true</code> if no error occurred when writing
     *          to the file.
     */
    public boolean save(File fout, String title) {
        return save(fout, title, false);
    }

    /**
     * Saves properties into an XML file.
     *
     * @param fout an output file to send properties to.
     * @param title an identifying title to include in the header comment
     *              of the archive.
     * @param all if <code>true</code> then include non-archiveable properties
     * @return <code>true</code> if no error occurred when writing
     *          to the file.
     */
    public boolean save(File fout, String title, boolean all) {
        boolean success = true;
        if ((all || archiveEnabled) && (!fout.exists() || fout.canWrite())) {
            try {

                // first write to a temporary file so that the actual output
                // file does not get corrupted if our process gets killed
                // while we are writing the file. Very real situation, I've
                // experienced that quite a few times.
                String s1 = "wtk";
                String s2 = ".tmp";
                File tmp = null;
                FileWriter w = null;

                // NOTE: we are creating temp file in the same directory as
                // the target file to ensure that both temporary files and
                // the target file are on the same device (volume, drive,
                // partition, whatever). Othwise, we won't be able to rename
                // the files
                String dir = fout.getParent();
                for (int i=0; i<1000; i++) {
                    String fname;
                    if (dir == null) {
                        fname = s1+nextCounter()+s2;
                    } else {
                        fname = dir+File.separator+s1+nextCounter()+s2;
                    }
                    try {
                        w = new FileWriter(fname);
                        tmp = new File(fname);
                        break;
                    } catch (IOException x) { /* ignore */ }
                }

                success = false;
                if (w != null) {
                    if ((title == null) || (title.length() == 0)) {
                        title = fout.getName();
                    } else {
                        title = fout.getName() + ": " + title;
                    }
                    Archiver.savePropertySet(this, w, title, all, false);
                    w.flush();
                    w.close();

                    // now rename the temporary file into the actual output
                    // file. Since rename is an atomic operation on most file
                    // systems, we cannot lose both old and new contents
                    // of the prefs file. Either one of them or both will
                    // survive under (almost) any curcumstances
                    for (int i=0; i<1000 && !success; i++) {
                        // first assume that the output file does not exist
                        success = tmp.renameTo(fout);
                        if (!success) {
                            // it propably exists, try to replace it
                            File f = new File(dir,s1+nextCounter()+s2);
                            if (fout.renameTo(f)) {
                                success = tmp.renameTo(fout);
                                f.delete();
                            }
                        }
                    }
                }
            }
            catch (IOException ex) {
                success = false;
            }
        }
        return success;
    }

    /**
     * Saves all archiveable properties to a file.
     *
     * @param out an output stream to send properties to.
     * @param title an identifying title to include in the header comment
     * of the archive.
     */
    public void save(OutputStream out, String title) throws IOException {
        save(out, title, false);
    }

    /**
     * Writes properties in XML format to a stream.
     *
     * @param out an output stream to send properties to.
     * @param title an identifying title to include in the header comment
     * of the archive.
     * @param all if <code>true</code> then include non-archiveable properties
     */
    public void save(OutputStream out, String title, boolean all)
    throws IOException {
        if (all || archiveEnabled) {
            Writer w = new OutputStreamWriter(out);
            save(w, title, all);
            w.flush();
        }
    }

    /**
     * Writes all archiveable properties in XML format to a stream.
     *
     * @param w the writer to send properties to.
     * @param title an identifying title to include in the header comment
     * of the archive.
     */
    public void save(Writer w, String title) throws IOException {
        save(w, title, false);
    }

    /**
     * Saves properties to a file.
     *
     * @param w the writer to send properties to.
     * @param title an identifying title to include in the header comment
     * of the archive.
     * @param all if <code>true</code> then include non-archiveable properties
     */
    public void save(Writer w, String title, boolean all) throws IOException {
        if (all || archiveEnabled) {
            if ((title == null) || (title.length() == 0)) {
                title = "Untitled Preferences";
            }
            Archiver.savePropertySet(this, w, title, all, false);
        }
    }

    /**
     * Dumps all archiveable properties in archive format.
     *
     * @param out a writer to dump properties to. If <code>null</code>,
     *      then <code>System.out</code> will be used.
     */
    public void dump(Writer out, String label) {
        dump(out, label, false);
    }

    /**
     * Dumps properties in archive format with additional debug information.
     *
     * @param out a writer to dump properties to. If <code>null</code>, then
     *            <code>System.out</code> will be used.
     * @param all if <code>true</code> then include non-archiveable properties
     */
    public void dump(Writer out, String label, boolean all) {
        if (out == null) {
            out = new PrintWriter(System.out,true);
        }
        if (label == null) {
            label = "DEBUG";
        }
        try {
            Archiver.savePropertySet(this, out, label, all, true);
        } catch (Exception x) {
            printStackTrace(x);
        } finally {
            try { out.flush(); }
            catch (Exception x) {
                printStackTrace(x);
            }
        }
    }

    /**
     * Dumps the names and values of all properties.
     *
     * @param out a writer to dump properties to.
     *            If <code>null</code>, then <code>System.out</code> will be used.
     */
    public void dump(PrintWriter out) {
        dump(out, false);
    }

    /**
     * Dumps the names and values of all properties.
     *
     * @param out a writer to dump properties to.
     *            If <code>null</code>, then <code>System.out</code> will be used.
     * @param verbose if <code>true</code>, then show all information.
     *                Otherwise, just show name, type and value.
     */
    public void dump(PrintWriter out, boolean verbose) {
        // Gather the archiveable properties and convert their values to text:
        // Gather the archiveable properties and convert their values to text:
        if (out == null) {
            out = new PrintWriter(System.out,true);
        }
        out.println();
        out.println("Dump of all Property values:");
        out.println("...................................................");
        dump(null, this, 0, out, verbose);
        out.println("...........................End of PropertySet Dump.");
        out.flush();
    }

    /**
     * Dumps a property set.
     *
     * @param name the name of the property set.
     * @param ps the property set to dump.
     * @param indent the amount of indenting to apply.
     * @param out a writer to dump properties to.
     * @param verbose if <code>true</code>, then show all information.
     *                Otherwise, just show name, type and value.
     */
    private static void dump(String name, PropertySet ps, int indent, PrintWriter out, boolean verbose) {
        for (int i = indent; i > 0; i--) {
            out.print("  ");
        }
        out.print("PropertySet: ");
        if (name != null) {
            out.print("\"");
            out.print(name);
            out.print("\" ");
        }
        if (verbose) {
            out.print("[archive=");
            out.print(ps.isArchiveable());
            out.print("]");
        }
        out.println();
        for (Iterator i = ps.getElementMapEntries(); i.hasNext(); ) {
            Map.Entry entry = (Map.Entry)i.next();
            Object elem = entry.getValue();
            if (elem instanceof PropertySet) {
                dump((String)entry.getKey(), (PropertySet)elem, indent+1, out, verbose);
            } else {
                dump((Property)elem, indent+1, out, verbose);
            }
        }
    }

    /**
     * Dumps a property.
     *
     * @param p the property to dump
     * @param indent the amount of indenting to apply.
     * @param out a writer to dump properties to.
     *            If <code>null</code>, then <code>System.out</code> will be used.
     * @param verbose if <code>true</code>, then show all information.
     *                Otherwise, just show name, type and value.
     */
    private static void dump(Property p, int indent, PrintWriter out, boolean verbose) {
        for (int i = indent; i > 0; i--) {
            out.print("  ");
        }
        out.print("Property: \"");
        out.print(p.getName());
        out.print("\" [type=");
        out.print(p.getType().getName());
        if (verbose) {
            out.print(", archive=");
            out.print(p.isArchiveable());
            if (p.isReadOnly()) {
                out.print(", readonly");
            }
        }
        out.print("] = ");
        Object value = p.getValue();
        if (value == null) {
            out.println("<undefined>");
        } else {
            String text = p.getValueAsText();
            if (text != null) {
                out.println(text);
            } else {
                out.println(value.toString());
            }
        }
    }

    /**
     * Prints a stack trace if <code>MessageWriters.verbose</code> is
     * <code>true</code>
     */
    private static void verboseStackTrace(Throwable x) {
        if (Trace.isVerbose()) {
            printStackTrace(x);
        }
    }

    /**
     * Prints a stack trace
     */
    private static void printStackTrace(Throwable x) {
        Trace.Debug.printStackTrace(x);
    }

    /**
     * Serialization support
     *
     * @param out the stream to write this property set to
     * @throws IOException if an I/O error occurs
     */
    private void writeObject(ObjectOutputStream out) throws IOException {
        out.defaultWriteObject();
        ByteArrayOutputStream bos = new ByteArrayOutputStream();
        save(bos, doNotWriteHeader, true);
        byte [] data = bos.toByteArray();
        out.writeInt(data.length);
        out.write(data);
    }

    /**
     * Serialization support
     *
     * @param in the stream to read the property set
     * @throws IOException if an I/O error occurs
     * @throws ClassNotFoundException one of the exceptions thrown by
     *  {@link ObjectInputStream}
     */
    private void readObject(ObjectInputStream in)
    throws IOException, ClassNotFoundException {
        removeAll(true);
        in.defaultReadObject();
        byte [] buf = new byte[in.readInt()];
        in.readFully(buf);
        load(new ByteArrayInputStream(buf));
    }

    /**
     * An object that performs some work on ProperytSet
     */
    public interface BulkChange {
        public void change(PropertySet ps);
    }

    /**
     * Receives notifications about bulk change events. This interface is
     * primarily for internal use, but I can imagine that it may be useful
     * for external clients as well, so it's made public.
     */
    public interface BulkChangeListener {
        public void bulkChangeStarted(PropertySet ps);
        public void bulkChangeFinished(PropertySet ps, boolean changed);
    }

    /**
     * Inner Class: A proxy class for creating property descriptors on
     *              behalf of other objects.
     */
    public static class StandaloneProperty extends AbstractProperty {

        private PropertyChangeSupport propChange = null;

        /**
         * Creates a new StandaloneProperty which is a copy of another
         * property.
         *
         * @param p the source property
         */
        public StandaloneProperty(Property p) {
            this(p.getName(),p.getType(),p.getDefaultValue(),p.isArchiveable());
            setValue(p.getValue());
        }

        /**
         * Creates a new StandaloneProperty of type String.
         *
         * @param name      the name of the property (not including the path).
         * @param value     the default value of the property.
         * @param archive   if <code>true</code> then explicitly enable
         *                  archiving for the property. Otherwise, explicitly
         *                  disable archiving for the property.
         */
        public StandaloneProperty(String name, String value, boolean archive) {
            this(name, String.class, value, archive);
        }

        /**
         * Creates a new boolean StandaloneProperty.
         *
         * @param name      the name of the property (not including the path).
         * @param value     the default value of the property.
         * @param archive   if <code>true</code> then explicitly enable
         *                  archiving for the property. Otherwise, explicitly
         *                  disable archiving for the property.
         */
        public StandaloneProperty(String name, boolean value, boolean archive) {
            this(name, Boolean.class, Utils.getBoolean(value), archive);
        }

        /**
         * Creates a new StandaloneProperty instance.
         *
         * @param name      the name of the property (not including the path).
         * @param type      the type of the property.
         * @param value     the default value of the property.
         * @param archive   if <code>true</code> then explicitly enable
         *                  archiving for the property. Otherwise, explicitly
         *                  disable archiving for the property.
         */
        public StandaloneProperty(String name, Class type, Object value, boolean archive) {
            super(name, type, value, archive);
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
         * Adds a PropertyChangeListener to the listener list.
         * @param listener The PropertyChangeListener to be added
         */
        public void addPropertyChangeListener(PropertyChangeListener listener) {
            if (listener == null) {
                throw new NullPointerException("listener");
            }
            if (propChange == null) {
                createPropertyChangeSupport();
            }
            propChange.addPropertyChangeListener(listener);
        }

        /**
         * Removes a PropertyChangeListener from the listener list.
         * @param listener The PropertyChangeListener to be removed
         */
        public void removePropertyChangeListener(PropertyChangeListener listener) {
            if (listener != null && propChange != null) {
                propChange.removePropertyChangeListener(listener);
            }
        }

        /**
         * This method gets called when a bound property is changed.
         * @param oldValue the old value of the property
         * @param newValue the new value of the property
         */
        protected void firePropertyChange(Object oldValue, Object newValue) {
            if (propChange != null && propChange.hasListeners(this.getName())) {
                propChange.firePropertyChange(this.getName(), oldValue, newValue);
            }
        }
    }   // End of StandaloneProperty


    /**
     * Inner class: XML Archiver for property values.
     */
    private static class Archiver {
        /**
         * Saves property values as XML.
         * @param p the property set to save
         * @param out the output writer to save to
         * @param title a title to include in the header comment
         * @param all if <code>true</code> then ignore "archiveable" flags
         * @param debug if <code>true</code> then write additional debug
         * information
         */
        private static void savePropertySet(PropertySet p,
                                            Writer out,
                                            String title,
                                            boolean all,
                                            boolean debug)
        throws IOException {
            IndentWriter w = new IndentWriter(out);

            w.println("<?xml version=\"1.0\" standalone=\"yes\"?>");
            if (title != doNotWriteHeader) {
                w.println("<!-- " + title + " -->");
                w.println("<!-- " + staticArchiveHeader + " -->");
                w.println("<!-- Last Modified: " + new Date().toString() + " -->");
                w.println();
            }
            writePropertySet(p, null, w, all, debug);
            w.println();
        }

        /**
         * Writes a set of property values as XML.
         * @param p the set of property values
         * @param name the name or title of the property set
         * @param w describes how the children should be written
         * @param all if <code>true</code> then ignore "archiveable" flags
         * @param debug if <code>true</code> then write additional debug
         * information
         */
        private static void writePropertySet(PropertySet p,
                                             String name,
                                             IndentWriter w,
                                             boolean all,
                                             boolean debug)
        throws IOException {
            if (!p.isEmpty()) {
                PropertySet rootNode = p;
                w.write("<");
                w.write(xmlPropertySetElementName);
                if ((name != null) && (name.length() > 0)) {
                    w.write(" ");
                    w.write(xmlPropertyNameAttributeName);
                    w.write("=\"");
                    w.write(StringUtils.xmlize(name));
                    w.write("\"");
                }

                if (!p.archiveEnabled) {
                    w.write(" ");
                    w.write(xmlPropertyArchiveAttribute);
                    w.write("=\"");
                    w.write(Boolean.FALSE.toString());
                    w.write("\"");
                }

                w.write(">");
                w.indent();

                // Write data entries
                for (Iterator i = p.getElementNames(); i.hasNext(); ) {
                    String elemName = (String)i.next();
                    Object elem = p.getNamedElement(elemName);
                    if (elem instanceof Property) {
                        Property pDescr = (Property)elem;
                        if (all || p.isArchiveable(pDescr)) {
                            boolean serialized = false;
                            // Get the property value:
                            String textValue = pDescr.getValueAsText();
                            if ((textValue == null) && all) {
                                Object obj = pDescr.getValue();
                                if (obj instanceof Serializable) {
                                    try {
                                        byte [] bytes = IOUtils.serialize(obj);
                                        textValue = BASE64.encode(bytes);
                                        serialized = true;
                                    } catch (Exception x) {
                                        verboseStackTrace(x);
                                        textValue = null;
                                    }
                                }
                            }
                            if (textValue != null) {
                                w.write("<");
                                w.write(xmlPropertyElementName);
                                w.write(" ");
                                w.write(xmlPropertyNameAttributeName);
                                w.write("=\"");
                                w.write(StringUtils.xmlize(elemName));
                                w.write("\"");
                                String type = null;
                                Class cls = pDescr.getType();
                                if (cls != null) {
                                    if (serialized) {
                                        type = serializedPrefix + cls.getName();
                                        if (!pDescr.isArchiveable()) {
                                            w.write(" ");
                                            w.write(xmlPropertyArchiveAttribute);
                                            w.write("=\"");
                                            w.write(Boolean.FALSE.toString());
                                            w.write("\"");
                                        }
                                    } else {
                                        String c = cls.getName();
                                        String a = (String)classAliasMap.get(c);
                                        type = ((a!=null) ? a : c);
                                    }
                                }
                                if (type != null) {
                                    w.write(" ");
                                    w.write(xmlPropertyTypeAttributeName);
                                    w.write("=\"");
                                    w.write(StringUtils.xmlize(type));
                                    w.write("\"");
                                }
                                w.write(" ");
                                w.write(xmlPropertyValueAttributeName);
                                w.write("=\"");
                                w.write(StringUtils.xmlize(textValue));
                                w.write("\"/>");
                                if (debug && !p.isArchiveable(pDescr)) {
                                    w.write(" <!-- NOARCHIVE -->");
                                }
                                w.println();
                            }
                        }
                    }
                }

                // Write child property sets
                for (Iterator i = p.getElementNames(); i.hasNext(); ) {
                    String elemName = (String)i.next();
                    Object elem = p.getNamedElement(elemName);
                    if (elem instanceof PropertySet) {
                        PropertySet ps = (PropertySet)elem;
                        if (all || p.isArchiveable(elemName,ps)) {
                            PropertySet subset = (PropertySet)elem;
                            writePropertySet(subset, elemName, w, all, debug);
                        }
                    }
                }

                w.unindent();
                w.write("</");
                w.write(xmlPropertySetElementName);
                w.write(">");
                w.println();
            }
        }

        /**
         * Loads property values from XML file.
         * @param p the property set to load into
         * @param in the input stream to load from
         * @param construct if <code>true</code>, then allow construction of
         *                  new properties that may not already be defined
         *                  in this PropertySet.
         */
        static void loadPropertySet(PropertySet p, Reader in, boolean construct) throws IOException {
            try {
                XMLReader parser = SAXParserFactory.newInstance().newSAXParser().getXMLReader();
                final PropertySet rootNode = p;
                final boolean constructionAllowed = construct;
                DefaultHandler docHandler = new DefaultHandler() {
                    // Track element nodes as document is parsed.
                    private Stack stack = new Stack();

                    // OldArchiveFormat (generated by v2.0 and earlier)
                    // implies that groups have no real meaning and both
                    // group and property names may have embedded 'dot'
                    // delimiters.  The first property element in
                    // OldArchiveFormat always had an embedded 'dot'
                    // delimiter and there was no immediate named group
                    // element, so it is an easy test to detect
                    // OldArchiveFormat for backward compatibility.
                    private boolean isOldArchiveFormat = false;

                    public void startElement(String uri, String localName, String qname,
                        Attributes attrs) throws SAXException {
                        if (stack.isEmpty()) {
                            // Root Element:
                            stack.push(rootNode);
                        } else {
                            PropertySet currentNode = (PropertySet)stack.peek();
                            if (xmlPropertySetElementName.equals(qname)) {
                                // PropertySet Element:
                                String name = attrs.getValue(xmlPropertySetNameAttributeName);
                                if (name.indexOf('.') >= 0) {
                                    isOldArchiveFormat = true;
                                }

                                PropertySet subNode;
                                if (isOldArchiveFormat) {
                                    subNode = rootNode.getPropertySet(name);
                                    if (subNode == null) {
                                        subNode = rootNode.createPath(name);
                                    }
                                } else {
                                    subNode = currentNode.getPropertySet(name);
                                    if (subNode == null) {
                                        subNode = new PropertySet(true);
                                        currentNode.addNamedElement(name,subNode);
                                    }
                                }

                                // restore the archive flag
                                String a = attrs.getValue(xmlPropertyArchiveAttribute);
                                subNode.archiveEnabled = ((a == null) ||
                                        Boolean.TRUE.toString().equals(a));

                                stack.push(subNode);

                            } else if (xmlPropertyElementName.equals(qname)) {
                                // Property Element:
                                String name = attrs.getValue(xmlPropertyNameAttributeName);
                                if (name.indexOf('.') >= 0) {
                                    isOldArchiveFormat = true;
                                }
                                String type = attrs.getValue(xmlPropertyTypeAttributeName);
                                String text = attrs.getValue(xmlPropertyValueAttributeName);
                                PropertySet node = (isOldArchiveFormat ? rootNode : currentNode);
                                Property pDescr = node.getProperty(name);
                                Object value = null;

                                // Construct a new property if required and allowed:
                                if ((pDescr == null) && constructionAllowed) {
                                    String a = attrs.getValue(xmlPropertyArchiveAttribute);
                                    boolean archive = ((a == null) ||
                                        Boolean.TRUE.toString().equals(a));
                                    try {
                                        Class cls;
                                        if (type.startsWith(serializedPrefix)) {
                                            // this is a serialized object
                                            type = type.substring(serializedPrefix.length());
                                            cls = Class.forName(type);
                                            byte [] buf = BASE64.decode(text);
                                            value = IOUtils.deserialize(buf);
                                        } else {

                                            // replace names of primitive types
                                            // found in the preferences file with
                                            // their java.lang.* analogs to maintain
                                            // compatibility with the old preferences
                                            // files
                                            cls = resolveClass(type);
                                        }

                                        // create a standalone property
                                        pDescr = node.addProperty(
                                            name,    // name
                                            cls,     // type
                                            null,    // empty value
                                            archive  // archiveable
                                            );
                                    } catch (Throwable ex) {
                                        verboseStackTrace(ex);
                                    }
                                }

                                if (pDescr != null) {
                                    try {
                                        if (value != null) {
                                            pDescr.setValue(value);
                                        } else {
                                            pDescr.setValueAsText(text);
                                        }
                                    } catch (Exception ex) {
                                        printStackTrace(ex);
                                    }
                                }
                            }
                        }
                    }

                    public void endElement(String uri, String localName, String qname)
                        throws SAXException {
                        if (!isOldArchiveFormat) {
                            if (xmlPropertySetElementName.equals(qname)) {
                                stack.pop();
                            }
                        }
                    }

                    // treat validation errors as fatal
                    public void error (SAXParseException err) throws SAXParseException {
                        throw err;
                    }

                    // dump warnings too
                    public void warning (SAXParseException err) throws SAXParseException {
                        throw err;
                    }
                };
                parser.setContentHandler(docHandler);
                parser.setErrorHandler(docHandler);
                parser.parse(new InputSource(in));
            } catch (SAXParseException ex) {
                Trace.Debug.println("*** PropertySet.Archiver.loadPropertySet: ParseException"
                                    + ": PublicId=" + ex.getPublicId()
                                    + ", SystemId=" + ex.getSystemId()
                                    + ", line=" + ex.getLineNumber()
                                    + ", column=" + ex.getColumnNumber()
                                    + ": " + ex.getMessage());
                verboseStackTrace(ex);
            } catch (SAXException ex) {
                // Ignore
            } catch (FactoryConfigurationError ex) {
                Trace.Debug.println("*** " + ex);
                verboseStackTrace(ex);
            } catch (ParserConfigurationException ex) {
                Trace.Debug.println("*** " + ex);
                verboseStackTrace(ex);
            }
        }


        // Constants used for XML save/restore:
        private final static String xmlPropertySetElementName = "property-set";
        private final static String xmlPropertyElementName = "property";
        private final static String xmlPropertyNameAttributeName = "name";
        private final static String xmlPropertyArchiveAttribute = "archive";
        private final static String xmlPropertySetNameAttributeName = xmlPropertyNameAttributeName;
        private final static String xmlPropertyTypeAttributeName = "type";
        private final static String xmlPropertyValueAttributeName = "value";
    }   // End of Archiver


    /**
     * An abstract property descriptor implementation. It's useful as a base
     * class of all property implementations.
     */
    abstract public static class BasicProperty implements Property {

        private String name;
        private boolean archiveable;
        private Class type;
        private Object defaultValue = null;

        /**
         * Creates a new Property by name.
         *
         * @param name The programmatic name of the property.
         */
        protected BasicProperty(String name, Class type) {
            this(name, type, true);
        }

        /**
         * Creates a new Property by name.
         *
         * @param name The programmatic name of the property.
         */
        protected BasicProperty(String name, Class type, boolean archiveable) {

            if (name.indexOf('.') >= 0) {
                throw new IllegalArgumentException("Illegal Property Name: " + name);
            }

            if (type == null) {
                throw new IllegalArgumentException("Type Required");
            }

            this.name = name;
            this.type = type;
            this.archiveable = archiveable;
        }

        /**
         * Creates a new Property which is a copy of another Property.
         * @param p the other Property
         */
        protected BasicProperty(Property p) {
            this(p.getName(), p.getType(), p.isArchiveable());
            this.defaultValue = p.getDefaultValue();
        }

        /**
         * Returns a string representation of this Property.
         * This method is for debugging purposes.
         * @return a string representation of this Property
         */
        public String toString() {
            StringBuffer sbuf = new StringBuffer();
            sbuf.append("Property[");
            sbuf.append("name=\"").append(getName()).append("\",");
            sbuf.append("type=\"").append(getType().getName()).append("\",");
            sbuf.append("archive=\"").append(archiveable).append("\"");
            sbuf.append("]");
            return sbuf.toString();
        }

        /**
         * Gets the base name of this property.
         *
         * @return The programmatic name of the property/method/event
         */
        final public String getName() {
            return name;
        }

        /**
         * Sets the default value of this property.
         * @param value The default value.
         */
        public final void setDefaultValue(Object value) {
            if ((value == null) || (getType().isInstance(value))) {
                defaultValue = value;
            }
        }

        /**
         * Returns the default value of this property.
         * @return The default value.
         */
        public final Object getDefaultValue() {
            return defaultValue;
        }

        /**
         * Gets the expected type of a property value.
         * @return the type of the property value
         */
        public final Class getType() {
            return type;
        }

        /**
         * Sets the explicit type of a property value.
         * @param type the type of the property value
         */
        public final void setType(Class type) {
            if (type != null) {
                this.type = type;
            }
        }

        /**
         * Explicitly specifies whether a property may be read from and
         * written to an archive. As an implicit requirement, the property's
         * read and write methods must be accessible and also a PropertyEditor
         * must be defined which can successfully perform text conversion on
         * a value during a load and store operations.
         *
         * @param f If <code>true</code> then include the property in
         *          archive operations.
         *          If <code>false</code> then exclude the property from
         *          archive operations.
         */
        public void setArchiveable(boolean f) {
            archiveable = f;
        }

        /**
         * Determines whether a property may be included in an archive.
         * This does not imply or override any implicit requirements for
         * archiveability.
         *
         * @return <code>true</code> if the property may be included in
         * an archive.
         */
        public boolean isArchiveable() {
            return archiveable;
        }

        /**
         * Returns <code>true</code> if this property is readonly. This
         * prevents PropertySet from setting value of this property.
         *
         * @return <code>true</code> if this property is readonly.
         */
        public boolean isReadOnly() {
            return false;
        }

        /**
         * Returns {@link PropertyEditor} for this property
         */
        protected PropertyEditor getPropertyEditor() {
            return PropertyEditors.getEditor(type);
        }

        /**
         * Returns the text representation of the property value. If property
         * is not defined, returns <code>null</code>
         *
         * @return  the text representation of the property value.
         */
        public String getValueAsText() {
            Object value = getValue();
            if (value != null) {
                PropertyEditor editor = getPropertyEditor();
                if (editor != null) {
                    editor.setValue(value);
                    return editor.getAsText();
                }
            }
            return null;
        }

        /**
         * Sets the value of this property.
         * @param text  the text representation of the property value to set.
         */
        public void setValueAsText(String text) {
            PropertyEditor editor = getPropertyEditor();
            if (editor != null) {
                editor.setAsText(text);
                setValue(editor.getValue());
            } else {
                throw new UnsupportedOperationException();
            }
        }

        /**
         * Writes the value of a property. The default implementation does
         * nothing.
         * @param value the value of the property
         */
        public void setValue(Object value) {
        }

    }   // End of Property


    /**
     * A base class for properties that store their own value
     */
    public abstract static class AbstractProperty extends BasicProperty {

        private Object value;

        protected AbstractProperty(String name, Class type) {
            this(name, type, true);
        }

        protected AbstractProperty(String name, Class type, boolean archive) {
            this(name, type, null, archive);
        }

        protected AbstractProperty(String name, Class type, Object value, boolean archive) {
            super(name, type, archive);
            if (value != null && !type.isInstance(value)) {
                throw new ClassCastException(value.getClass().getName());
            }
            checkType(value);
            setDefaultValue(value);
            this.value = value;
        }

        /**
         * Checks if we cast the value to the type of this property
         */
        protected void checkType(Object newValue) {
            if (newValue != null && !getType().isInstance(newValue)) {
                String className = newValue.getClass().getName();
                if (Trace.isVerbose()) {
                    String msg = "ClassCastException: " + this +
                                 " is given a value of type " + className;

                    Trace.Debug.println(msg);
                }
                throw new ClassCastException(className);
            }
        }

        /**
         * Gets the property value.
         * @return the property value
         */
        public Object getValue() {
            return value;
        }

        /**
         * Sets the property value.
         * @param newValue the property value
         */
        public void setValue(Object newValue) {
            checkType(newValue);
            Object oldValue = value;
            value = newValue;
            if (!valueEqual(oldValue, newValue)) {
                firePropertyChange(oldValue, newValue);
            }
        }

        /**
         * This method gets called when a bound property is changed.
         * @param oldValue the old value of the property
         * @param newValue the new value of the property
         */
        abstract protected void firePropertyChange(Object oldValue, Object newValue);
    }

    /**
     * A readonly property descriptor.
     */
    public static class ReadonlyProperty extends BasicProperty {
        private Object value;
        public ReadonlyProperty(String name, int value) {
            this(name, Integer.class, new Integer(value));
        }
        public ReadonlyProperty(String name, boolean value) {
            this(name, Boolean.class, Utils.getBoolean(value));
        }
        public ReadonlyProperty(String name, Object value) {
            this(name, value.getClass(), value, false);
        }
        public ReadonlyProperty(String name, Class type, Object value) {
            this(name, type, value, false);
        }
        public ReadonlyProperty(String name, Class type, Object value, boolean archive) {
            super(name, type, archive);
            if (value != null && !type.isInstance(value)) {
                throw new ClassCastException(value.getClass().getName());
            }
            this.value = value;
            setDefaultValue(value);
        }
        public Object getValue() {
            return value;
        }
        public void setValueAsText(String text) {
        }
        public boolean isReadOnly() {
            return false;
        }
        public void addPropertyChangeListener(PropertyChangeListener l) {
        }
        public void removePropertyChangeListener(PropertyChangeListener l) {
        }
    }

    /**
     * Inner class: PropertySet iterator.
     * @todo there must be a more efficient way to implement the iterator
     */
    public static final class PropertySetIterator implements Iterator {

        static final int TYPE_PROPERTY_SET = 1;
        static final int TYPE_PROPERTY = 2;
        static final int TYPE_BOTH = TYPE_PROPERTY | TYPE_PROPERTY_SET;

        private int type;
        private boolean recurse;
        private boolean removed = false;
        private Iterator entries;
        private PropertySet ps;

        private Object currentObject;
        private String currentObjectName;
        private String currentNode;
        private PropertySetIterator currentChild;

        private Object nextObject;
        private String nextObjectName;
        private String nextNode;
        private PropertySetIterator nextChild;

        /**
         * Creates a new Iterator for a PropertySet.
         * @param ps the property set
         * @param recurse if <code>true</code> then recurse through subsets
         */
        private PropertySetIterator(PropertySet ps, boolean recurse, int type) {
            this(null, ps, recurse, type);
        }

        /**
         * Creates a new Iterator for a child PropertySet.
         * @param name the name of the property set
         * @param ps the property set
         * @param recurse if <code>true</code> then recurse through subsets
         */
        private PropertySetIterator(String name, PropertySet ps, boolean recurse, int type) {
            this.type = type;
            this.ps = ps;
            this.recurse = recurse;
            entries = ps.getElementMapEntries();
            findNextElement();
        }

        private boolean isRemoved() {
            if (removed) {
                return true;
            } else if (currentChild != null) {
                return currentChild.isRemoved();
            } else {
                return false;
            }
        }

        private void invalidateNextElement() {
            nextNode = null;
            nextChild = null;
            nextObject = null;
            nextObjectName = null;
        }

        private void switchToNextElement() {
            currentNode = nextNode;
            currentChild = nextChild;
            currentObject = nextObject;
            currentObjectName = nextObjectName;
            invalidateNextElement();
        }

        /**
         * Attempts to descend into a property set and find the next
         * object there. Returns <code>true</code> in case of success,
         * <code>false</code> if there's nothing there
         */
        private boolean findNextElementRecursively(PropertySet set, String name) {

            // try to descend into a property set
            PropertySetIterator i = new PropertySetIterator(set,recurse,type);
            nextObject = i.nextObject;
            if (nextObject != null) {
                nextChild = i;
                nextNode = name;
                nextObjectName = nextNode + "." + i.nextObjectName;
                return true;
            } else {
                invalidateNextElement();
                return false;
            }
        }

        private boolean findNextElement2() {
            invalidateNextElement();
            while (entries.hasNext()) {
                Map.Entry entry = (Map.Entry)entries.next();
                nextObject = entry.getValue();
                nextObjectName = (String)entry.getKey();
                if (nextObject instanceof PropertySet) {
                    if ((type & TYPE_PROPERTY_SET) != 0) {
                        return true;
                    } else if (recurse) {
                        PropertySet set = (PropertySet)nextObject;
                        if (findNextElementRecursively(set,nextObjectName)) {
                            return true;
                        }
                    }
                } else {
                    if ((type & TYPE_PROPERTY) != 0) {
                        return true;
                    }
                }
            }
            nextObject = null;
            nextObjectName = null;
            return false;
        }

        private boolean findNextElementAfterRemove() {
            invalidateNextElement();
            if (removed) {
                removed = false;
                return findNextElement2();
            } else if (currentChild.findNextElementAfterRemove()) {
                grabNextNodeFromTheCurrentNode();
                return true;
            } else {
                return findNextElement2();
            }
        }

        /**
         * Copies the next node information from the current node.
         * The caller must make sure that the next node is under the
         * current node.
         */
        private void grabNextNodeFromTheCurrentNode() {
            nextNode = currentNode;
            nextChild = currentChild;
            nextObject = nextChild.nextObject;
            nextObjectName = nextNode + "." + nextChild.nextObjectName;
        }

        /**
         * Switches to the new element in the iteration
         */
        private boolean findNextElement() {
            // check if existing child can find the next object
            removed = false;
            switchToNextElement();
            if (currentChild != null) {
                if (currentChild.findNextElement()) {
                    grabNextNodeFromTheCurrentNode();
                    return true;
                }

                // check if we need to recurse
            } else if ((currentObject instanceof PropertySet) && recurse) {
                PropertySet set = (PropertySet)currentObject;
                if (findNextElementRecursively(set, currentObjectName)) {
                    return true;
                }
            }

            // our children could not find anything for us
            // continue scanning objects at this level
            return findNextElement2();
        }

        /**
         * Removes the current element.
         */
        private void removeInternal() {
            if (currentChild != null) {
                currentChild.removeInternal();
            } else if (!removed) {
                removed = true;
                entries.remove();
            } else {
                throw new IllegalStateException();
            }
        }

        /**
         * Removes the current element.
         */
        public void remove() {
            // since the current element has been removed, we may have
            // to repeat the search for the next element in case if the
            // would-be next element has been removed together with the
            // property set that contained it. Perhaps, this could be
            // optimized but for now we always repeat the search
            removeInternal();
            findNextElementAfterRemove();
        }

        /**
         * Returns <code>true</code> if the iteration has more elements.
         *
         * @return <code>true</code> if the iterator has more elements.
         */
        public boolean hasNext() {
            return (nextObject != null);
        }

        /**
         * Returns the next element in the interation.
         */
        public Object next() {
            if (nextObject == null) throw new NoSuchElementException();
            findNextElement();
            return currentObject;
        }

        /**
         * Gets the name of the current element.
         * @return the current element name
         */
        public String getCurrentName() {
            if (currentObject == null) throw new NoSuchElementException();
            return currentObjectName;
        }

        /**
         * Gets the current element.
         * @return the current element
         */
        public Object getCurrentElement() {
            return currentObject;
        }

    }   // End of Iterator

    /**
     * Property change forwarder for a {@link Property}
     */
    private final static class PropertyChangeForwarder extends PropertyChangeProxy {

        PropertyChangeForwarder(PropertySet ps) {
            super(ps);
        }

        /**
         * Receives a property change notification and forwards it to the
         * target property set. If target property set has been garbage
         * collected, unregisters from the property.
         */
        public void propertyChange(PropertyChangeEvent e) {
            PropertySet ps = getTarget();
            if (ps == null) {
                try {
                    Property property = (Property)e.getSource();
                    property.removePropertyChangeListener(this);
                } catch (Exception x) {
                    printStackTrace(x);
                }
            } else {
                PropertyChangeEvent newEvent = new PropertyChangeEvent(ps,
                    e.getPropertyName(), e.getOldValue(), e.getNewValue());
                newEvent.setPropagationId(e.getPropagationId());
                ps.firePropertyChange(newEvent);

                // also fire "PropertySetChanged" event unless bulk update
                // is in progress
                if (ps.bulkChangeInProgress > 0) {
                    ps.bulkChangePending = true;
                } else {
                    ps.firePropertySetChanged();
                }
            }
        }
    }

    /**
     * Internal listener for a {@link PropertySet}
     */
    private
    final
    class       PropertySetChangeForwarder
    extends     PropertyChangeProxy
    implements  PropertySetListener,
                BulkChangeListener {

        PropertySetChangeForwarder(PropertySet ps) {
            super(ps);
        }

        /**
         * Returns the weakly referenced target property set,
         * deregisters from the source of the event if the target
         * has been garbage collected
         *
         * @param src the source of the event
         */
        private PropertySet getTarget(PropertySet src) {
            PropertySet target = getTarget();
            if (target == null) {
                src.removeBulkChangeListener(this);
                src.removePropertySetListener(this);
                src.removePropertyChangeListener(this);
            }
            return target;
        }

        /**
         * Receives a property change notification and forwards it to the
         * target property set. If target property set has been garbage
         * collected, unregisters from the property.
         */
        public void propertyChange(PropertyChangeEvent e) {
            PropertySet target = getTarget();
            PropertySet src = (PropertySet)e.getSource();
            if (target != null) {
                // keep the event name relative to the source of the event
                // NOTE: the source of the event MUST have a valid non-empty
                // name, otherwise it couldn't be our child property set
                String name = getChildName(src);
                PropertyChangeEvent newEvent = new PropertyChangeEvent(target,
                    name + "." + e.getPropertyName(),
                    e.getOldValue(), e.getNewValue());
                newEvent.setPropagationId(e.getPropagationId());
                target.firePropertyChange(newEvent);
            }
        }

        /**
         * Invoked when a child property set is added
         * @param ps the property set listener is attached to
         * @param path the full path of the new property set, relative to ps
         */
        public void propertySetAdded(PropertySet ps, String path) {
            PropertySet target = getTarget(ps);
            if (target != null && target.propertySetListeners != null) {
                target.firePropertySetAdded(getChildName(ps) + '.' + path);
            }
        }

        /**
         * Invoked when a child property set is removed
         * @param ps the property set listener is attached to
         * @param child the property set that has been removed
         * @param path the full path of the removed property set, relative to ps
         */
        public void propertySetRemoved(PropertySet ps, PropertySet child, String path) {
            PropertySet target = getTarget(ps);
            if (target != null && target.propertySetListeners != null) {
                String name = getChildName(ps);
                target.firePropertySetRemoved(child, name + '.' + path);
            }
        }

        /**
         * Invoked when a property is added
         * @param ps the property set listener is attached to
         * @param path the full path of the new property, relative to ps
         */
        public void propertyAdded(PropertySet ps, String path) {
            PropertySet target = getTarget(ps);
            if (target != null && target.propertySetListeners != null) {
                target.firePropertyAdded(getChildName(ps) + '.' + path);
            }
        }

        /**
         * Invoked when a property is removed
         * @param ps the property set listener is attached to
         * @param p the property that has been removed
         * @param path the full path of the removed property, relative to ps
         */
        public void propertyRemoved(PropertySet ps, Property p, String path) {
            PropertySet target = getTarget(ps);
            if (target != null && target.propertySetListeners != null) {
                target.firePropertyRemoved(p,getChildName(ps) + '.' + path);
            }
        }

        /**
         * Invoked when something within a property set hierarhy has changed
         * @param ps the property set listener is attached to
         */
        public void propertySetChanged(PropertySet ps) {
            PropertySet target = getTarget(ps);
            if (target != null) {
                if (target.bulkChangeInProgress > 0) {
                    target.bulkChangePending = true;
                } else {
                    target.firePropertySetChanged();
                }
            }
        }
        public void bulkChangeStarted(PropertySet ps) {
            PropertySet target = getTarget(ps);
            if (target != null) {
                target.beginBulkChange();
            }
        }
        public void bulkChangeFinished(PropertySet ps, boolean changed) {
            PropertySet target = getTarget(ps);
            if (target != null) {
                target.endBulkChange();
            }
        }
    }

    /**
     * This class forwards property change events to the PropertySet
     * which created it. It keeps weak reference to the target property set,
     * otherwise properties would end up having strong references to all
     * the property sets which contain them. This is a problem with static
     * properties which never go away by definition. Without this weak
     * reference, no property set that contains a static property would
     * ever go away, as well as all the other properties that they contain +
     * all the owner objects references by the properties they contain +
     * all the objects referenced by those objects, etc. You get the idea.
     *
     */
    private static abstract class PropertyChangeProxy
    implements PropertyChangeListener {

        /**
         * A weak reference to the target {@link PropertySet}
         */
        private Reference ref;

        /**
         * creates a <code>PropertyChangeProxy</code> weakly referencing the
         * specified property set.
         */
        PropertyChangeProxy(PropertySet ps) {
            ref = new WeakReference(ps);
        }

        /**
         * Returns the target property or <code>null</code> if target has
         * been gabage-collected.
         */
        protected final PropertySet getTarget() {
            Reference localRef = ref;
            if (localRef != null) {
                PropertySet ps = (PropertySet)localRef.get();
                if (ps == null) ref = null;
                return ps;
            }
            return null;
        }
    }

    /**
     * Convert a nibble to a hex character
     * @param	nibble	the nibble to convert.
     */
    private static char toHex(int nibble) {
        return hexDigit[(nibble & 0xF)];
    }

    /**
     * Returns the next value of the synchronized counter
     * @return the next value of the synchronized counter
     */
    private static synchronized int nextCounter() {
        if (counter < 0) counter = new Random().nextInt() & 0xffff;
        counter++;
        return ((counter < 0) ? counter = 0 : counter);
    }

    /** A table of hex digits */
    private static final char[] hexDigit = {
        '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'
    };

    private static int counter = -1;

    private static final String staticArchiveHeader = "This is a generated file! Do not edit.";
    private static final String serializedPrefix = "serialized:";
    private static final String doNotWriteHeader = "x"; // any non-empty string

    private static Map primitiveTypeMap;
    private static Map aliasClassMap;
    private static Map classAliasMap;

    static {
        // NOTE: the hash table sizes are chosen to optimize the performance.
        // With these sizes each hash entry falls nicely into its own slot.
        aliasClassMap = new HashMap(60);
        classAliasMap = new HashMap(42);
        primitiveTypeMap = new HashMap(12);

        createAlias( "byte",        Byte        .class );
        createAlias( "char",        Character   .class );
        createAlias( "double",      Double      .class );
        createAlias( "float",       Float       .class );
        createAlias( "int",         Integer     .class );
        createAlias( "long",        Long        .class );
        createAlias( "short",       Short       .class );
        createAlias( "boolean",     Boolean     .class );
        createAlias( "string",      String      .class );
        createAlias( "string[]",    String[]    .class );
        createAlias( "int[]",       int[]       .class );
        createAlias( "byte[]",      byte[]      .class );

        // avoid loading GUI classes... hardcode the names
        createAlias( "point",       "java.awt.Point" );
        createAlias( "size",        "java.awt.Dimension" );
        createAlias( "rectangle",   "java.awt.Rectangle" );
        createAlias( "color",       "java.awt.Color" );
        createAlias( "polygon",     "java.awt.Polygon" );
        createAlias( "font",        "java.awt.Font" );
        createAlias( "keystroke",   "javax.swing.KeyStroke" );
        createAlias( "keystroke[]", "[Ljavax.swing.KeyStroke;" );

        primitiveTypeMap.put( Byte      .TYPE, Byte      .class );
        primitiveTypeMap.put( Character .TYPE, Character .class );
        primitiveTypeMap.put( Double    .TYPE, Double    .class );
        primitiveTypeMap.put( Float     .TYPE, Float     .class );
        primitiveTypeMap.put( Integer   .TYPE, Integer   .class );
        primitiveTypeMap.put( Long      .TYPE, Long      .class );
        primitiveTypeMap.put( Short     .TYPE, Short     .class );
        primitiveTypeMap.put( Boolean   .TYPE, Boolean   .class );
    }

    private static void createAlias(String alias, Class klass) {
        createAlias(alias, klass.getName());
    }

    private static void createAlias(String alias, String klass) {
        aliasClassMap.put(alias, klass);
        classAliasMap.put(klass, alias);
    }

    private static Class resolveClass(String type) throws ClassNotFoundException {
        String className = (String)aliasClassMap.get(type);
        if (className != null) {
            return Class.forName(className);
        } else {
            // this is a class name rather than an alias
            Class cls = Utils.classForName(type);
            Class mappedClass = (Class)primitiveTypeMap.get(cls);
            return (mappedClass == null) ? cls : mappedClass;
        }
    }

    // All the fields are transient. Serialization is provided by the
    // readObject and writeObject methods
    private transient List elementNames;          // Maintains ordering; element=String
    private transient Map namedElements;          // key=String(name), value=Property|PropertySet
    private transient PropertyChangeSupport pcs;  // PropertyChangeListeners
    private transient PropertySetListener [] propertySetListeners;
    private transient BulkChangeListener [] bulkChangeListeners;
    private transient PropertySetChangeForwarder propertySetChangeForwarder;  // Forwarding listener
    private transient PropertyChangeListener propertyChangeForwarder;  // Forwarding listener
    private transient boolean archiveEnabled;     // Archive enabled for this instance?
    private transient Map archiveMap;             // maps properties and property sets to Boolean
    private transient boolean bulkChangePending = false;
    private transient int bulkChangeInProgress = 0;

}   // End of PropertySet
