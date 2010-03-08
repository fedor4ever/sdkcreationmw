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

/* java.util */
import java.util.List;
import java.util.Iterator;
import java.util.ArrayList;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.Utils;
import com.nokia.wtk.util.Property;
import com.nokia.wtk.util.PropUtils;
import com.nokia.wtk.util.HTTPHeader;
import com.nokia.wtk.util.ArrayUtils;
import com.nokia.wtk.util.PropertySet;
import com.nokia.wtk.util.StringUtils;

/**
 * A {@link RowModel} implementation containing HTTP headers that can
 * be individually enabled or disabled. The model saves itself into a
 * {@link PropertySet}.
 *
 *
 * @see ExpandableTableModel
 * @see RowModel
 */
public class HeaderRowModel implements RowModel {

    public static final String ORDER_PROPERTY = "order";
    public static final String ENABLED_PROPERTY = "enabled";
    public static final String NAME_PROPERTY = "name";
    public static final String VALUE_PROPERTY = "value";

    private List rows;
    private Policy policy;

    /** the property set we are editing */
    private PropertySet props;
    private Property orderProp;

    /**
     * Creates empty <code>HeaderRowModel</code>
     */
    public HeaderRowModel() {
        rows = new ArrayList();
    }

    /**
     * Creates <code>HeaderRowModel</code>
     * @param ps the property set containing the header data
     */
    public HeaderRowModel(PropertySet ps) {
        this();
        setProperties(ps);
    }

    /**
     * Sets the policy for adding and changing headers. This does not
     * affect the current state of the model. The new policy will be
     * applied to the future modifications of the model.
     *
     * @param p the new policy
     */
    public void setPolicy(Policy p) {
        policy = p;
    }

    /**
     * Associates this model with a property set. From this point on all
     * changes in the list of headers will be reflected in the property
     * set. The state of the model is updated to reflect the contents of
     * the property set.
     *
     * @param ps the property set to associate with, or <code<null</code>
     */
    public void setProperties(PropertySet ps) {
        setProperties(ps, true);
    }

    /**
     * Associates this model with a property set. From this point on
     * all changes in the list of headers will be reflected in the
     * property set
     *
     * @param ps the property set to associate with, or <code<null</code>
     * @param apply <code>true</code> to initialize the state of the model
     *        from the contents of the property set; otherwise update property
     *        set to reflect current state of the model. This parameter is
     *        ignored if property set is <code<null</code>
     */
    public void setProperties(PropertySet ps, boolean apply) {
        props = ps;
        if (ps == null) {
            orderProp = null;
        } else {
            orderProp = ps.getProperty(ORDER_PROPERTY);
            if (orderProp == null || orderProp.getType() != String.class) {
                // create "order" property if it does not exist or has
                // a wrong type
                orderProp = ps.addProperty(ORDER_PROPERTY,String.class,"",true);
            }
            if (apply) {
                updateModel();
            } else {
                updateProperties();
            }
        }
    }

    /**
     * Updates the model from the associated property set.
     * As a side effect, removes garbage from the property set
     */
    public void updateModel() {
        if (props != null) {
            rows.clear();
            String s = (String)orderProp.getValue();
            String [] order = StringUtils.tokenize(s," ");
            if (order.length == 0) {
                props.removeAll();
                orderProp.setValue("");
                props.addProperty(orderProp);
            } else {
                // remove stale entries
                List names = null;
                PropertySet.PropertySetIterator itr = props.getPropertySets(false);
                while (itr.hasNext()) {
                    Object next = itr.next();
                    String name = itr.getCurrentName();
                    if (!ArrayUtils.contains(order,name)) {
                        if (names == null) names = new ArrayList();
                        names.add(name);
                    }
                }
                if (names != null) {
                    int n = names.size();
                    for(int i=0; i<n; i++) {
                        props.removePropertySet((String)names.get(i));
                    }
                }

                // add the rows
                for(int i=0; i<order.length; i++) {
                    String key = order[i];
                    PropertySet subset = props.getPropertySet(key);
                    if (subset != null &&
                        subset.containsProperty(ENABLED_PROPERTY) &&
                        subset.containsProperty(NAME_PROPERTY) &&
                        subset.containsProperty(VALUE_PROPERTY)) {
                        String name = (String)subset.getValue(NAME_PROPERTY);
                        Header header = new Header(key,subset);
                        if (policy == null ||
                            policy.canAddHeader(header,false)) {
                            rows.add(header);
                            continue;
                        }
                    }

                    // duplicate or invalid entry
                    props.remove(key);
                }
                saveOrder();
            }
        }
    }

    /**
     * Updates associated property set to match the current state of the model
     */
    public void updateProperties() {
        if (props != null) {
            props.removeAll();
            orderProp.setValue("");
            props.addProperty(orderProp);
            if (!rows.isEmpty()) {
                int k = 1;
                Iterator i = rows.iterator();
                while (i.hasNext()) {
                    String name = "h" + (k++);
                    Header h = (Header)i.next();
                    props.addProperties(name,h.getProperties());
                    h.setKey(name);
                }
                saveOrder();
            }
        }
    }

    /**
     * Updates the "order" property
     */
    private void saveOrder() {
        if (orderProp != null) {
            if (rows.isEmpty()) {
                orderProp.setValue("");
            } else {
                int n = rows.size();
                StringBuffer sb = new StringBuffer();
                for(int i=0; i<n; i++) {
                    Header h = (Header)rows.get(i);
                    if (sb.length() > 0) sb.append(" ");
                    sb.append(h.getKey());
                }
                orderProp.setValue(sb.toString());
            }
        }
    }

    /**
     * Converts this model into an array of headers.
     * Only enabled headers are returned
     *
     * @return the array of currently enabled headers, empty array if no such
     */
    public HTTPHeader [] getHeaders() {
        List headers = null;
        int n = rows.size();
        for (int i=0; i<n; i++) {
            Header header = (Header)rows.get(i);
            if (header.isEnabled()) {
                if (headers == null) headers = new ArrayList(n-i);
                headers.add(header);
            }
        }
        if (headers == null) {
            return new HTTPHeader[0];
        } else {
            return (HTTPHeader[])headers.toArray(new HTTPHeader[headers.size()]);
        }
    }

    /**
     * Return HTTP header at the specified row. The header may be enabled
     * or disabled.
     *
     * @param rowIndex the row index
     * @return the requested HTTP header
     */
    public HTTPHeader getHeader(int rowIndex) {
        return (Header)rows.get(rowIndex);
    }

    /**
     * Return <code>true</code> if the HTTP header at the specified row
     * is enabled.
     *
     * @param rowIndex the row index
     * @return <code>true</code> if the HTTP header at the specified row
     *          is enabled
     */
    public boolean isEnabled(int rowIndex) {
        return ((Header)rows.get(rowIndex)).isEnabled();
    }

    /**
     * Adds a header, given its name and value
     *
     * @param name the header name
     * @param value the header value
     */
    public void addHeader(String name, String value) {
        addHeader(getRowCount(), name, value);
    }

    /**
     * Adds a header, given its name and value, at the specified index.
     *
     * @param rowIndex the index of the new header
     * @param name the header name
     * @param value the header value
     */
    public void addHeader(int rowIndex, String name, String value) {
        addHeader(rowIndex, new Header(name, value));
    }

    /**
     * Adds a header at the specified index
     *
     * @param rowIndex the index of the new header
     * @param header the header to add
     */
    private void addHeader(int rowIndex, Header header) {
        rows.add(rowIndex, header);
        if (props != null) {
            int i = 1;
            String key;
            while (props.containsPropertySet(key = "h" + i)) i++;
            props.addProperties(key, header.getProperties());
            header.setKey(key);
            saveOrder();
        }
    }

    /**
     * Finds a header with the specified name
     *
     * @param headerName the name of the header to find
     * @return  the index of the found header, <code>-1</code> if
     *          no such header
     */
    public int findHeader(String headerName) {
        int n = rows.size();
        for(int i=0; i<n; i++) {
            Header h = (Header)rows.get(i);
            if (headerName.equalsIgnoreCase(h.getName())) {
                return i;
            }
        }
        return -1;
    }

    /**
     * Returns the index of the specified header, <code>-1</code> if it's
     * not a part of the model
     *
     * @param header the header to find
     * @return  the index of the header in this model, <code>-1</code> if
     *          no such header
     */
    public int indexOf(HTTPHeader header) {
        return rows.indexOf(header);
    }

    /**
     * Clears the model
     */
    public void clear() {
        rows.clear();
        if (props != null) {
            props.removeAll();
        }
    }

    /**
     * Returns a string representation of this object. This method
     * is intended to be used only for debugging purposes, and the
     * content and format of the returned string may vary between
     * implementations. The returned string may be empty but may not
     * be <code>null</code>.
     *
     * @return  a string representation of this object.
     */
    public String toString() {
        StringBuffer sb = new StringBuffer();
        sb.append("HeaderRowModel[");
        int n = getRowCount();
        for (int i=0; i<n; i++) {
            if (i > 0) sb.append(',');
            sb.append(getRow(i));
        }
        sb.append(']');
        return sb.toString();
    }

    //=======================================================================
    //      P O L I C Y
    //=======================================================================

    /**
     * An object that helps the model to make certain kinds of decisions
     */
    public interface Policy {

        /**
         * Determines whether or not a new header can be added.
         *
         * @param header the header being added
         * @param interactive <code>true</code> if this call is being made
         *        in an interactive context, i.e. the policy object is
         *        allowed to interact with the user, for example pop up
         *        an error message or request a confirmation. Otherwise,
         *        interaction with the user is not allowed.
         *
         * @return  <code>true</code> to allow adding this header,
         *          <code>false</code> ro disallow
         */
        public boolean canAddHeader(HTTPHeader header, boolean interactive);

        /**
         * Determines whether or not a header name can be changed
         *
         * @param header the header being changed
         * @param name the proposed new header name
         *
         * @return  <code>true</code> to allow changing the header name
         *          <code>false</code> ro disallow
         */
        public boolean canChangeName(HTTPHeader header, String name);

        /**
         * Determines whether or not a header value can be changed.
         *
         * @param header the header being changed
         * @param value the proposed new value
         *
         * @return  <code>true</code> to allow changing the header name
         *          <code>false</code> ro disallow
         */
        public boolean canChangeValue(HTTPHeader header, String value);
    }

    //=======================================================================
    //      R O W    M O D E L
    //=======================================================================

    public int getRowCount() {
        return rows.size();
    }
    public RowModel.Row getRow(int rowIndex) {
        return (RowModel.Row)rows.get(rowIndex);
    }
    public boolean addRow(int rowIndex, RowModel.Row row) {
        Header header = (Header)row;
        if (policy == null || policy.canAddHeader(header,true)) {
            addHeader(rowIndex, header);
            return true;
        }
        return false;
    }
    public void removeRow(int rowIndex) {
        Header header = (Header)rows.remove(rowIndex);
        if (props != null) {
            props.removePropertySet(header.getKey());
            saveOrder();
        }
    }
    public void moveRow(int fromIndex, int toIndex) {
        if (fromIndex != toIndex) {
            rows.add(toIndex, rows.remove(fromIndex));
            saveOrder();
        }
    }
    public RowModel.Row createEmptyRow() {
        return new Header();
    }
    public boolean isEmptyCellEditable(int column) {
        return (column == 1);
    }

    //=======================================================================
    //      S I N G L E    R O W
    //=======================================================================

    /**
     * Implementation of {@link RowModel.Row} interface for use by
     * the {@link HeaderRowModel} class. Contains information about
     * a single HTTP header
     */
    private class Header implements RowModel.Row, HTTPHeader {

        private static final long serialVersionUID = 2;

        private static final int ENABLED_COLUMN = 0;
        private static final int NAME_COLUMN = 1;
        private static final int VALUE_COLUMN = 2;

        private String key;
        private PropertySet propSet;
        private Property enabledProp;
        private Property nameProp;
        private Property valueProp;

        /**
         * Creates new empty header
         */
        Header() {
            this(null,"","");
        }

        /**
         * Creates a header associated with the specified property set
         *
         * @param k the unique key for this header
         * @param ps the property set associated with this header
         */
        Header(String k, PropertySet ps) {
            this(ps,"","");
            key = k;
        }

        /**
         * Creates header with the specified name and value
         *
         * @param name the header name
         * @param value the header value
         */
        Header(String name, String value) {
            this(null,name,value);
        }

        /**
         * Creates a header.
         *
         * @param ps the property set associated with this header
         * @param name  the default header name. The header name found in
         *              the property set overrides this parameter
         * @param value the default header value. The header value found in
         *              the property set overrides this parameter
         */
        Header(PropertySet ps, String name, String value) {
            propSet = ((ps == null) ? (new PropertySet()) : ps);
            enabledProp = getProp(ENABLED_PROPERTY,Boolean.class,Boolean.TRUE);
            nameProp = getProp(NAME_PROPERTY,String.class,name);
            valueProp = getProp(VALUE_PROPERTY,String.class,value);
        }

        /**
         * Extracts a property of the specified type from a proprty set
         * or creates a new one if it does not exist, or type does not
         * match
         *
         * @param name   property name
         * @param type   property type
         * @param defVal default value of the property
         * @return       the found or newly created property
         */
        private Property getProp(String name, Class type, Object defVal) {
            Property p = propSet.getProperty(name);
            if (p != null && p.getType() == type) return p;
            return propSet.addProperty(name,type,defVal,true);
        }

        /**
         * Returns the name of this header's node.
         * @return unique key for this header in the headers property set
         */
        String getKey() {
            return key;
        }

        /**
         * Sets the name of this header's node.
         * @param k unique key for this header in the headers property set
         */
        void setKey(String k) {
            key = k;
        }

        /**
         * Returns the property set for this header
         * @return the property set for this header
         */
        PropertySet getProperties() {
            return propSet;
        }

        /**
         * Returns <code>true</code> is this header is enabled,
         * <code>false</code> if it's disabled
         * @return <code>true</code> is this header is enabled
         */
        boolean isEnabled() {
            return PropUtils.getValue(enabledProp,false);
        }

        /**
         * Returns a string representation of this object. This method
         * is intended to be used only for debugging purposes, and the
         * content and format of the returned string may vary between
         * implementations. The returned string may be empty but may not
         * be <code>null</code>.
         *
         * @return  a string representation of this object.
         */
        public String toString() {
            return (getName() + ": " + getValue());
        }

        /**
         * Returns property for the given column
         * @param columnIndex the column index
         * @return the property at the specified column
         * @throws IllegalArgumentException of the column index is invalid
         */
        private Property getProperty(int columnIndex) {
            switch (columnIndex) {
            case ENABLED_COLUMN:   return enabledProp;
            case NAME_COLUMN:      return nameProp;
            case VALUE_COLUMN:     return valueProp;
            default:
            throw new IllegalArgumentException(Integer.toString(columnIndex));
            }
        }

        // RowModel.Row interface
        public boolean isEmpty() {
            String name = getName();
            return (name == null || name.length() == 0);
        }
        public Object getValue(int columnIndex) {
            return getProperty(columnIndex).getValue();
        }
        public boolean setValue(int columnIndex, Object value) {
            if (policy != null) {
                switch (columnIndex) {
                case NAME_COLUMN:
                    value = ((String)value).trim();
                    if (!policy.canChangeName(this,(String)value)) {
                        return false;
                    }
                    break;
                case VALUE_COLUMN:
                    if (!policy.canChangeValue(this,(String)value)) {
                        return false;
                    }
                    break;
                }
            }
            getProperty(columnIndex).setValue(value);
            return true;
        }

        // HTTPHeader interface
        public String getName() {
            return PropUtils.getValue(nameProp,null);
        }
        public String getValue() {
            return PropUtils.getValue(valueProp,null);
        }

        /**
         * Compares the specified object with this header for equality.
         * Returns <code>true</code> if the given object also implements
         * <code>HTTPHeader</code> interface and the two objects represent
         * the same header.  More formally, two headers <code>h1</code> and
         * <code>h2</code> represent the same header iff<pre>
         *     (h1.getName()==null ?
         *      h2.getName()==null : h1.getName().equals(h2.getName())) &&
         *     (h1.getValue()==null ?
         *      h2.getValue()==null : h1.getValue().equals(h2.getValue()))
         * </pre>
         * This ensures that the <code>equals</code> method works properly
         * across different implementations of the <code>HTTPHeader</code>
         * interface.
         *
         * @param obj   object to be compared for equality with this header.
         * @return      <code>true</code> if the specified object is equal
         *              to this header.
         */
        public boolean equals(Object obj) {
            if (obj == this) {
                return true;
            } else if (obj instanceof HTTPHeader) {
                HTTPHeader that = (HTTPHeader)obj;
                return Utils.equals(this.getName(),that.getName()) &&
                       Utils.equals(this.getValue(),that.getValue());
            } else {
                return false;
            }
        }

        /**
         * Returns the hash code value for this header.  The hash code
         * of an HTTP header <code>h</code> is defined to be: <pre>
         *     (h.getName()==null ? 0 : h.getName().hashCode()) ^
         *     (h.getValue()==null ? 0 : h.getValue().hashCode())
         * </pre>
         * This ensures that <code>h1.equals(h2)</code> implies that
         * <code>h1.hashCode()==h2.hashCode()</code> for any two headers
         * <code>h1</code> and <code>h2</code>, as required by the general
         * contract of <code>Object.hashCode</code>.
         *
         * @return the hash code value for this header.
         */
        public int hashCode() {
            return (Utils.hashCode(getName()) ^ Utils.hashCode(getValue()));
        }

    }
}
