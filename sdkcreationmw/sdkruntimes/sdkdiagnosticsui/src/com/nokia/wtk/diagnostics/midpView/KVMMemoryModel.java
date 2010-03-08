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




package com.nokia.wtk.diagnostics.midpView;

import javax.swing.BoundedRangeModel;
import javax.swing.DefaultBoundedRangeModel;
import javax.swing.table.AbstractTableModel;
import javax.swing.table.TableModel;

import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.device.DeviceJ2MEListener;
import com.nokia.wtk.device.DeviceMemory;
import com.nokia.wtk.diagnostics.browser.Browser;
import com.nokia.wtk.util.MessageWriters;

/**
 *
 * A class to model the J2ME memory in a device. Singleton class
 * since this
 * system will only model one device at a time, yet multiple views must
 * listen to this model.
 */
class KVMMemoryModel   {

    static {
        AppEnvironment.addResources(
            "resources/KVMMemoryModel.properties",
            KVMMemoryModel.class);
    }
    private static KVMMemoryModel kvmmModel__;

    private final AbstractTableModel atModel_ = new AbstractTableModel() {
        final private String[] rowNames =
            {
                AppEnvironment.getString(Resource.KVM_USED_LABEL),
                AppEnvironment.getString(Resource.KVM_FREE_LABEL),
                AppEnvironment.getString(Resource.KVM_TOTAL_LABEL)};

        public Class getColumnClass(int column) {
            if (column > 0) {
                return Integer.class;
            }
            return String.class;
        }

        public int getColumnCount() {
            return 2;
        }
        public int getRowCount() {
            return 3;
        }
        public Object getValueAt(int row, int col) {
            switch (col) {
                case 0 :
                    return rowNames[row];
                case 1 :
                    switch (row) {
                        case 0 :
                            return ""
                                + KVMMemoryModel.this.dbrModel_.getValue()
                                + " bytes";
                        case 1 :
                            return ""
                                + (KVMMemoryModel.this.dbrModel_.getMaximum()
                                    - KVMMemoryModel.this.dbrModel_.getValue())
                                + " bytes";
                        case 2 :
                            return ""
                                + KVMMemoryModel.this.dbrModel_.getMaximum()
                                + " bytes";
                        default :
                            return null;
                    }
                default :
                    return null;
            }
        }
        public boolean isCellEditable(int row, int col) {
            return false;
        }
    };
    protected final DefaultBoundedRangeModel dbrModel_ =
        new DefaultBoundedRangeModel() {
        {
            setExtent(0);
            setMinimum(0);
            setMaximum(0);
        }
        protected void fireStateChanged() {
            atModel_.fireTableCellUpdated(0, 1);
            atModel_.fireTableCellUpdated(1, 1);
            atModel_.fireTableCellUpdated(2, 1);
            super.fireStateChanged();
        }

    };
    private DeviceJ2MEListener j2meListener_;
    private final boolean supports_;

    private KVMMemoryModel(Browser b) {
        supports_ =
            b.registerHostService(DeviceJ2MEListener.class, getJ2MEListener());
    }

    private DeviceJ2MEListener getJ2MEListener() {
        if (j2meListener_ == null) {
            j2meListener_ = new DeviceJ2MEListener() {

                public void KVMMemoryChanged(DeviceMemory memory) {
                    KVMMemoryModel.this.dbrModel_.setMaximum(memory.getTotal());
                    KVMMemoryModel.this.dbrModel_.setValue(memory.getUsed());
                }
            };
        }
        return j2meListener_;
    }
    /**
     * Get the central model for the KVM Memory
     * @param Browser device to get the J2ME memory model from. This parameter
     * will only be used in the first invocation of this call, after that the
     * model has been created and hooked up to this Browser.
     * @return KVMMemoryModel the single model that keeps track of the KVM
     * memory.
     */
    static KVMMemoryModel getKVMMemoryModel(Browser b) {
        if (kvmmModel__ == null) {
            kvmmModel__ = new KVMMemoryModel(b);
        }
        return kvmmModel__;
    }

    /**
     * Returns whether the device actually supports the concept of J2ME
     * applets memory changes.
     * @return boolean true if the device manages J2ME memory, false otherwise
     */
    boolean isKVMMemorySupported() {
        return supports_;
    }

    /**
     * Return a table model that reflects the KVM Memory, ready for a JTable,
     * complete with column headers
     * @return TableModel the table model
     */
    TableModel getTableModel() {
        return atModel_;
    }
    /**
     * Return a range model that reflects the KVM Memory, ready for a progress
     * bar
     * @return BoundedRangeModel a range model fo the KVM Memory
     */
    BoundedRangeModel getBoundedRangeModel() {
        return dbrModel_;
    }

    /**
     * Get the size of the heap used as a percentage of toal size used
     * @return int the percentage of heap memory used
     */
    int getHeapSizePercentage() {
        int max = dbrModel_.getMaximum();
        if (max == 0) {
            return 0;
        }
        return (
            (int) ((((double) dbrModel_.getValue())

                / ((double) dbrModel_.getMaximum()))
                * 100.0));
    }

    /**
     * Get the total size in bytes of the heap used
     * @return int the amount of heap used
     */
    int getValue() {
        return dbrModel_.getValue();
    }

}
