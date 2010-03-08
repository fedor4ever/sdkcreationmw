/*
* Copyright (c) 2000 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.epdt.plugins.s60.securityconf;

/* javax.swing.table */
import javax.swing.table.AbstractTableModel;

/**
 * Table model for the permissions table
 */
class PermTableModel extends AbstractTableModel {

    private SecurityConfPlugin iParentPlugin;
    private SecConfModel iModel;
    private String[] iFunctionGroups;

    private static final String[] COLUMN_NAME = new String[] {
        "Functions available",
        "Permissions"
    };

    PermTableModel(SecurityConfPlugin aParentPlugin) {
        iParentPlugin = aParentPlugin;
        iModel = aParentPlugin.getSecConfModel();
        iFunctionGroups = iModel.getFunctionGroups();
    }

    public int getColumnCount() {
        return COLUMN_NAME.length;
    }

    public String getColumnName(int aColumnIndex) {
        return COLUMN_NAME[aColumnIndex];
    }

    public int getRowCount() {
        return iFunctionGroups.length;
    }

    public boolean isCellEditable(int aRowIndex, int aColumnIndex)  {
        if (aColumnIndex == 1) {
           ProtectionDomain d = iModel.getSelectedDomain();
           if (d != null && d.isUserDefined()) {
               return true;
           }
        }
        return false;
    }

    public Class getColumnClass(int aColumnIndex) {
        switch(aColumnIndex) {
        case 0:  return String.class;
        case 1:  return Permission.class;
        default: return null;
        }
    }

    public Object getValueAt(int aRowIndex, int aColumnIndex) {
        switch(aColumnIndex) {
        case 0:
            return iFunctionGroups[aRowIndex];
        case 1: {
            ProtectionDomain d = iModel.getSelectedDomain();
            return (d != null) ?
                d.getPermission(iFunctionGroups[aRowIndex]):
                null;
            }
        }
        return null;
    }

    public void setValueAt(Object aValue, int aRowIndex, int aColumnIndex) {
        if (aColumnIndex == 1) {
           ProtectionDomain d = iModel.getSelectedDomain();
           if (d != null && d.isUserDefined()) {
               String functionGroup = iFunctionGroups[aRowIndex];
               Permission currentPermission = d.getPermission(functionGroup);
               if (!currentPermission.equals(aValue)) {
                   // Permission value has actually changed
                   // Update it and fire the notification
                   d.setPermission(functionGroup,(Permission)aValue);
                   iParentPlugin.pluginModelChangedAction();
               }
           }
        }
    }
}
