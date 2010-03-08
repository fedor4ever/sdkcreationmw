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
import java.util.Arrays;
import java.util.ArrayList;

/* javax.swing.table */
import javax.swing.table.AbstractTableModel;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.Utils;

/**
 * A table model that maintains a single "empty" row at the end of the table.
 *
 */
public class ExpandableTableModel extends AbstractTableModel {

    private RowModel rowModel;
    private RowModel.Row emptyRow;
    private boolean [] columnEditable;
    private Class [] columnClasses;
    private String [] columnNames;

    /**
     * Creates <code>ExpandableTableModel</code>
     */
    public ExpandableTableModel(String [] names) {
        this(names, new DefaultRowModel(names.length));
    }

    /**
     * Creates <code>ExpandableTableModel</code>
     */
    public ExpandableTableModel(String [] names, RowModel model) {
        rowModel = model;

        // Copy column names:
        columnNames = new String[names.length];
        System.arraycopy(names,0,columnNames,0,names.length);

        // By default all columns are editable
        columnEditable = new boolean[names.length];
        Arrays.fill(columnEditable,true);

        // create the initial row
        emptyRow = rowModel.createEmptyRow();
    }

    //=======================================================================
    //      I N T E R F A C E
    //=======================================================================

    /**
     * Returns the name of the column at <code>columnIndex</code>.  This is used
     * to initialize the table's column header name.  Note: this name does
     * not need to be unique; two columns in a table can have the same name.
     *
     * @param	column  the index of the column
     * @return  the name of the column
     */
    public String getColumnName(int column) {
        return columnNames[column];
    }

    /**
     * Returns the most specific superclass for all the cell values
     * in the column.  This is used by the <code>JTable</code> to set up a
     * default renderer and editor for the column.
     *
     * @param columnIndex  the index of the column
     * @return the common ancestor class of the object values in the model.
     */
    public Class getColumnClass(int columnIndex) {
        Object obj = emptyRow.getValue(columnIndex);
        if (obj != null) return obj.getClass();
        return Object.class;
    }

    /**
     * Returns the number of columns in the model. A <code>JTable</code>
     * uses this method to determine how many columns it should create
     * and display by default.
     *
     * @return the number of columns in the model
     */
    public int getColumnCount() {
        return columnNames.length;
    }

    /**
     * Returns the number of rows in the model. A <code>JTable</code> uses
     * this method to determine how many rows it should display.
     *
     * @return the number of rows in the model
     */
    public int getRowCount() {
        return rowModel.getRowCount() + 1;
    }

    /**
     * Returns true if the cell at <code>rowIndex</code> and
     * <code>columnIndex</code> is editable.  Otherwise,
     * <code>setValueAt</code> on the cell will not
     * change the value of that cell.
     *
     * @param	rowIndex	the row whose value to be queried
     * @param	columnIndex	the column whose value to be queried
     * @return	true if the cell is editable
     * @see #setValueAt
     */
    public boolean isCellEditable(int rowIndex, int columnIndex) {
        if (rowIndex < rowModel.getRowCount()) {
            return columnEditable[columnIndex];
        } else {
            return rowModel.isEmptyCellEditable(columnIndex);
        }
    }

    /**
     * Returns the value for the cell at <code>columnIndex</code> and
     * <code>rowIndex</code>.
     *
     * @param	rowIndex	the row whose value is to be queried
     * @param	columnIndex 	the column whose value is to be queried
     * @return	the value Object at the specified cell
     */
    public Object getValueAt(int rowIndex, int columnIndex) {
        RowModel.Row row;
        if (rowIndex < rowModel.getRowCount()) {
            row = rowModel.getRow(rowIndex);
        } else {
            row = emptyRow;
        }
        return row.getValue(columnIndex);
    }

    /**
     * Sets the value in the cell at <code>columnIndex</code> and
     * <code>rowIndex</code> to <code>aValue</code>.
     *
     * @param   value           the new value
     * @param   rowIndex        the row whose value is to be changed
     * @param   columnIndex     the column whose value is to be changed
     * @see #getValueAt
     * @see #isCellEditable
     */
    public void setValueAt(Object value, int rowIndex, int columnIndex) {
        int rowCount = rowModel.getRowCount();
        if (rowIndex < rowCount) {
            RowModel.Row row = rowModel.getRow(rowIndex);
            if (!Utils.equals(row.getValue(columnIndex),value)) {
                if (row.setValue(columnIndex, value)) {
                    fireTableCellUpdated(rowIndex, columnIndex);
                    // if the row has become empty, remove it
                    if (row.isEmpty()) {
                        removeRow(rowIndex);
                    }
                }
            }
        } else {
            if (!Utils.equals(emptyRow.getValue(columnIndex),value)) {
                RowModel.Row newRow = rowModel.createEmptyRow();
                if (newRow.setValue(columnIndex,value)) {
                    if (rowModel.addRow(rowIndex,newRow)) {
                        fireTableRowsInserted(rowIndex,rowIndex);
                    }
                }
            }
        }
    }

    //=======================================================================
    //      I M P L E M E N T A T I O N
    //=======================================================================

    /**
     * Enables or disables editing data in the specified column
     */
    public void setColumnEditable(int columnIndex, boolean b) {
        columnEditable[columnIndex] = b;
    }

    /**
     *  Inserts a row at <code>row</code> in the model.  The new row
     *  will contain default values unless <code>rowData</code> is
     *  specified.  Notification of the row being added will be generated.
     *
     * @param   rowIndex        the row index of the row to be inserted
     * @param   rowData         optional data of the row being added
     * @exception  ArrayIndexOutOfBoundsException  if the row was invalid
     */
    public void insertRow(int rowIndex, Object [] rowData) {
        rowIndex  = Math.max(rowIndex,rowModel.getRowCount());
        RowModel.Row row = rowModel.createEmptyRow();
        for (int i=0; i<getColumnCount(); i++) {
            row.setValue(i,rowData[i]);
        }
        if (!row.isEmpty()) {
            if (rowModel.addRow(rowIndex, row)) {
                fireTableRowsInserted(rowIndex,rowIndex);
            }
        }
    }

    /**
     *  Inserts a row at <code>row</code> in the model.  The new row
     *  will contain default values unless <code>rowData</code> is
     *  specified.  Notification of the row being added will be generated.
     *
     * @param   rowIndex        the row index of the row to be inserted
     * @param   rowData         optional data of the row being added
     * @exception  ArrayIndexOutOfBoundsException  if the row was invalid
     */
    public void moveRow(int fromRow, int toRow) {
        int lastRow = rowModel.getRowCount();
        if (fromRow != toRow && fromRow != lastRow && toRow != lastRow) {
            rowModel.moveRow(fromRow, toRow);
            int min = Math.min(fromRow, toRow);
            int max = Math.max(fromRow, toRow);
            fireTableRowsUpdated(min,max);
        }
    }

    /**
     *  Removes the row at <code>row</code> from the model.  Notification
     *  of the row being removed will be sent to all the listeners.
     *
     * @param   rowIndex the row index of the row to be removed
     * @exception  ArrayIndexOutOfBoundsException  if the row was invalid
     */
    public void removeRow(int rowIndex) {
        // ignore requests to remove the last row
        if (rowIndex != rowModel.getRowCount()) {
            rowModel.removeRow(rowIndex);
            fireTableRowsDeleted(rowIndex, rowIndex);
        }
    }

    /**
     * Notifies the model that the underlying row model has changed.
     * This will cause a repaint of <code>JTable</code>
     */
    public void updateModel() {
        // clean up the model
        for (int i=rowModel.getRowCount()-1; i>=0; i--) {
            if (rowModel.getRow(i).isEmpty()) {
                rowModel.removeRow(i);
            }
        }
        fireTableDataChanged();
    }

    /**
     * Returns the {@link RowModel} being used by this class
     */
    public RowModel getRowModel() {
        return rowModel;
    }

    /**
     * Sets a new {@link RowModel}
     */
    public void setRowModel(RowModel model) {
        rowModel = model;
        updateModel();
    }

    //=======================================================================
    //      D E F A U L T    R O W    M O D E L
    //=======================================================================

    /**
     * A default implementation of {@link RowModel} interface.
     * It assumes that all values are strings
     */
    private static class DefaultRowModel implements RowModel {
        private int ncolumns;
        private List rows = new ArrayList();

        DefaultRowModel(int ncolumns) {
            this.ncolumns = ncolumns;
        }

        public int getRowCount() {
            return rows.size();
        }
        public Row getRow(int rowIndex) {
            return (Row)rows.get(rowIndex);
        }
        public boolean addRow(int rowIndex, Row row) {
            rows.add(rowIndex, row);
            return true;
        }
        public void removeRow(int rowIndex) {
            rows.remove(rowIndex);
        }
        public void moveRow(int fromIndex, int toIndex) {
            rows.add(toIndex, rows.remove(fromIndex));
        }
        public Row createEmptyRow() {
            return new DefaultRow(ncolumns);
        }
        public boolean isEmptyCellEditable(int column) {
            return true;
        }

        /**
         * A default implementation of {@link Row} interface for use by
         * the {@link DefaultRowModel} class
         */
        private static class DefaultRow implements Row {
            private String [] values;
            DefaultRow(int ncolumns) {
                values = new String[ncolumns];
            }
            public boolean isEmpty() {
                for (int i=0; i<values.length; i++) {
                    String value = values[i];
                    if (value != null && value.length() > 0) {
                        return false;
                    }
                }
                return true;
            }
            public Object getValue(int columnIndex) {
                String value = values[columnIndex];
                if (value == null) values[columnIndex] = value = "";
                return value;
            }
            public boolean setValue(int columnIndex, Object value) {
                values[columnIndex] = (String)value;
                return true;
            }
        }
    }
}
