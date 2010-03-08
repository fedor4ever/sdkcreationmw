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

/**
 * Data organized in rows. This interface exists to support
 * {@link ExpandableTableModel} which maintains an "empty"
 * row in the end of the table.
 *
 * @see ExpandableTableModel
 */
public interface RowModel {

    /**
     * Returns the number of rows in the model
     * @return the number of rows in the model
     */
    public int getRowCount();

    /**
     * Returns the row at specified index
     * @param rowIndex the position of the requested row
     */
    public Row getRow(int rowIndex);

    /**
     * Adds a new row
     *
     * @param rowIndex the position of the new row
     * @param row the row to insert
     *
     * @return <code>true</code> is row has been added, <code>false</code>
     * if it has been rejected by the model (duplicate entry, etc.)
     */
    public boolean addRow(int rowIndex, Row row);

    /**
     * Removes row at the specified index
     * @param rowIndex the position of the row to remove
     */
    public void removeRow(int rowIndex);

    /**
     * Moves a row
     * @param fromIndex the position of the row to move
     * @param toIndex the new position of the row
     */
    public void moveRow(int fromIndex, int toIndex);

    /**
     * Creates a new empty row
     */
    public Row createEmptyRow();

    /**
     * Returns <code>true</code> if an "empty" cell at specified
     * position cal be edited, <code>false</code> if not
     */
    public boolean isEmptyCellEditable(int column);

    /**
     * Interface that must be implemented by the code that uses
     * <code>ExpandableTableModel</code>
     */
    public interface Row {

        /**
         * Determines if the data represent an empty row.
         * <code>ExpandableTableModel</code> wants to know that in order
         * to keep a single "empty" row at the end of the table
         */
        public boolean isEmpty();

        /**
         * Returns the value at given column of this row
         */
        public Object getValue(int columnIndex);

        /**
         * Sets the value at given column of this row
         * @return <code>true</code> if the new value has been applied,
         *         <code>false</code> if it wasn't, most likely becase
         *         it's invalid
         */
        public boolean setValue(int columnIndex, Object value);
    }
}
