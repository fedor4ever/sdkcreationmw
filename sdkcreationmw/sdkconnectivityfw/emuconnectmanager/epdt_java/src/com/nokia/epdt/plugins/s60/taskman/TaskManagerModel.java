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


package com.nokia.epdt.plugins.s60.taskman;

/* java.util */
import java.util.List;
import java.util.ArrayList;
import java.util.Observable;
import java.util.Comparator;
import java.util.Collections;

/* javax.swing.table */
import javax.swing.table.TableModel;
import javax.swing.table.AbstractTableModel;

/* com.nokia.wtk.util */
import com.nokia.wtk.util.Property;
import com.nokia.wtk.util.PropertySet;

/**
 * TaskManagerModel
 *
 * @version 1.0
 */
class TaskManagerModel extends Observable implements TaskManagerConstants {

    // Property for the update speed (type = Integer)
    static final String UPDATE_SPEED_PROP = "update-speed";
    private static final int DEFAULT_UPDATE_SPEED = 3;
    private static final int MIN_UPDATE_SPEED = 0;
    private static final int MAX_UPDATE_SPEED = 5;
    private static final String [] UPDATE_SPEED_LABEL_KEYS = {
        UPDATE_SPEED_LABEL_PAUSE,
        UPDATE_SPEED_LABEL_1,
        UPDATE_SPEED_LABEL_2,
        UPDATE_SPEED_LABEL_3,
        UPDATE_SPEED_LABEL_4,
        UPDATE_SPEED_LABEL_5
    };

    // Model properties
    private PropertySet properties;
    private int defaultUpdateSpeed = DEFAULT_UPDATE_SPEED;

    // Cpu
    private Cpu cpu = Cpu.create(0, 0);
    // Memory
    private Memory memory = Memory.create(0, 0, 0);
    // Applications
    private List appList = new ArrayList(0);
    private ApplicationsTableModel applicationsTableModel = new ApplicationsTableModel();
    // Threads
    private List threadList = new ArrayList(0);
    private ThreadsTableModel threadsTableModel = new ThreadsTableModel();
    // Drives
    private List driveList = new ArrayList(0);
    private DrivesTableModel drivesTableModel = new DrivesTableModel();

    private class ApplicationsTableModel extends AbstractTableModel {
        private String [] columnNames;

        /**
         * Creates a new instance of ApplicationsTableModel
         */
        ApplicationsTableModel() {
            columnNames = new String [] {
                getResString(APPLICATIONS_TABLE_NAME_COLUMN_NAME),
                getResString(APPLICATIONS_TABLE_ID_COLUMN_NAME),
                //getResString(APPLICATIONS_TABLE_MEMORY_USED_COLUMN_NAME),
                //getResString(APPLICATIONS_TABLE_MEMORY_RESERVED_COLUMN_NAME),
                //getResString(APPLICATIONS_TABLE_STACK_SIZE_COLUMN_NAME),
                getResString(APPLICATIONS_TABLE_CPU_USAGE_COLUMN_NAME),
                getResString(APPLICATIONS_TABLE_CPU_TIME_COLUMN_NAME)
            };
        }

        /**
         * Returns the number of columns in the model.
         * A JTable uses this method to determine how many
         * columns it should create and display by default.
         * @return the number of columns in the model
         */
        public int getColumnCount() {
            return columnNames.length;
        }

        /**
         * Returns a name for the column.
         * @param column the column being queried
         * @return a string containing the name of column
         */
        public String getColumnName(int column) {
            return columnNames[column];
        }

        /**
         * Returns the number of rows in the model.
         * A JTable uses this method to determine how many
         * rows it should display. This method should be quick,
         * as it is called frequently during rendering.
         * @return the number of rows in the model
         */
        public int getRowCount() {
            return appList.size();
        }

        /**
         * Returns the value for the cell at columnIndex and rowIndex.
         * @param rowIndex the row whose value is to be queried
         * @param columnIndex the column whose value is to be queried
         * @return the value Object at the specified cell
         */
        public Object getValueAt(int rowIndex, int columnIndex) {
            Application application = (Application) appList.get(rowIndex);
            switch (columnIndex) {
                case 0 : return application.getName();
                case 1 : return String.valueOf(application.getId());
                //case x : return String.valueOf(application.getMemoryUsed());
                //case x : return String.valueOf(application.getMemoryReserved());
                //case x : return String.valueOf(application.getStack());
                case 2 : return formatCpuUsage(application);
                case 3 : return formatCpuTime(application);
                default : return "";
            }
        }

        private String formatCpuTime(Application app) {
            long micros = app.getCpuTime();
            StringBuffer buf = new StringBuffer(10);
            int sec = (int)(micros/1000000);
            int hours = sec/3600; sec -= hours*3600;
            int mins = sec/60; sec -= mins*60;
            buf.append(hours).append(':');
            if (mins < 10) buf.append('0');
            buf.append(mins).append(':');
            if (sec < 10) buf.append('0');
            buf.append(sec);
            return buf.toString();
        }

        private String formatCpuUsage(Application app) {
            int usage = app.getCpuUsage();
            return (usage <= 0) ? " " : Integer.toString(usage);
        }
    }

    private class ThreadsTableModel extends AbstractTableModel {
        private String [] columnNames;

        /**
         * Creates a new instance of ThreadsTableModel
         */
        public ThreadsTableModel() {
            columnNames = new String [] {
                getResString(THREADS_TABLE_NAME_COLUMN_NAME),
                getResString(THREADS_TABLE_ID_COLUMN_NAME),
                getResString(THREADS_TABLE_MEMORY_USED_COLUMN_NAME),
                getResString(THREADS_TABLE_MEMORY_RESERVED_COLUMN_NAME),
                getResString(THREADS_TABLE_STACK_SIZE_COLUMN_NAME),
                getResString(THREADS_TABLE_CPU_USAGE_COLUMN_NAME)
            };
        }

        /**
         * Returns the number of columns in the model.
         * A JTable uses this method to determine how many
         * columns it should create and display by default.
         * @return the number of columns in the model
         */
        public int getColumnCount() {
            return columnNames.length;
        }

        /**
         * Returns a name for the column.
         * @param column the column being queried
         * @return a string containing the name of column
         */
        public String getColumnName(int column) {
            return columnNames[column];
        }

        /**
         * Returns the number of rows in the model.
         * A JTable uses this method to determine how many
         * rows it should display. This method should be quick,
         * as it is called frequently during rendering.
         * @return the number of rows in the model
         */
        public int getRowCount() {
            return threadList.size();
        }
        /**
         * Returns the value for the cell at columnIndex and rowIndex.
         * @param rowIndex the row whose value is to be queried
         * @param columnIndex the column whose value is to be queried
         * @return the value Object at the specified cell
         */
        public Object getValueAt(int rowIndex, int columnIndex) {
            Thread thread = (Thread) threadList.get(rowIndex);
            switch (columnIndex) {
                case 0 : return thread.getName();
                case 1 : return String.valueOf(thread.getId());
                case 2 : return String.valueOf(thread.getMemoryUsed());
                case 3 : return String.valueOf(thread.getMemoryReserved());
                case 4 : return String.valueOf(thread.getStack());
                case 5 : return String.valueOf(thread.getCpuUsage());
                default : return "";
            }
        }
    }
    private class DrivesTableModel extends AbstractTableModel {
        private String [] columnNames;

        /**
         * Creates a new instance of DrivesTableModel
         */
        public DrivesTableModel() {
            columnNames = new String [] {
                getResString(DRIVES_TABLE_NAME_COLUMN_NAME),
                getResString(DRIVES_TABLE_SIZE_COLUMN_NAME),
                getResString(DRIVES_TABLE_FREE_COLUMN_NAME)
            };
        }

        /**
         * Returns the number of columns in the model.
         * A JTable uses this method to determine how many
         * columns it should create and display by default.
         * @return the number of columns in the model
         */
        public int getColumnCount() {
            return columnNames.length;
        }

        /**
         * Returns a name for the column.
         * @param column the column being queried
         * @return a string containing the name of column
         */
        public String getColumnName(int column) {
            return columnNames[column];
        }

        /**
         * Returns the number of rows in the model.
         * A JTable uses this method to determine how many
         * rows it should display. This method should be quick,
         * as it is called frequently during rendering.
         * @return the number of rows in the model
         */
        public int getRowCount() {
            return driveList.size();
        }

        /**
         * Returns the value for the cell at columnIndex and rowIndex.
         * @param rowIndex the row whose value is to be queried
         * @param columnIndex the column whose value is to be queried
         * @return the value Object at the specified cell
         */
        public Object getValueAt(int rowIndex, int columnIndex) {
            Drive drive = (Drive) driveList.get(rowIndex);
            switch (columnIndex) {
                case 0 : return drive.getName();
                case 1 : return String.valueOf(drive.getSize());
                case 2 : return String.valueOf(drive.getFree());
                default : return "";
            }
        }
    }

    /**
     * Creates a new instance of TaskManagerModel
     */
    TaskManagerModel() {
        try {
            int val = Integer.parseInt(getResString(UPDATE_SPEED_DEFAULT_VALUE));
            if (val >= MIN_UPDATE_SPEED && val <= MAX_UPDATE_SPEED) {
                defaultUpdateSpeed = val;
            }
        } catch (NumberFormatException nfe) {
        }

        properties = new PropertySet();
        properties.addIntProperty(UPDATE_SPEED_PROP,defaultUpdateSpeed,true);
    }

    /**
     * Returns model properties
     * @return model properties
     */
    PropertySet getProperties() {
        return properties;
    }

    /**
     * Reads a string from the resource bundle
     * @param key the resource key
     * @return the string from the resource bundle, null if none
     */
    private static String getResString(String key) {
        return TaskManagerResourceBundle.getInstance().getValue(key);
    }

    /** Getter for property applicationsTableModel.
     * @return Value of property applicationsTableModel.
     *
     */
    public TableModel getApplicationsTableModel() {
        return applicationsTableModel;
    }

    /** Getter for property threadsTableModel.
     * @return Value of property threadsTableModel.
     *
     */
    public TableModel getThreadsTableModel() {
        return threadsTableModel;
    }

    /** Getter for property updateSpeedValue.
     * @return Value of property updateSpeedValue.
     */
    Property getUpdateSpeedProperty() {
        return properties.getProperty(UPDATE_SPEED_PROP);
    }

    /** Getter for property updateSpeedValue.
     * @return Value of property updateSpeedValue.
     */
    int getUpdateSpeedValue() {
        return properties.getIntValue(UPDATE_SPEED_PROP,defaultUpdateSpeed);
    }

    /**
     * Returns set of valid values for the update speed property.
     * @return valid values for the update speed property.
     */
    static Integer[] getUpdateSpeedValues() {
        Integer[] values = new Integer[MAX_UPDATE_SPEED-MIN_UPDATE_SPEED+1];
        for (int i=0; i<values.length; i++) {
            values[i] = new Integer(i+MIN_UPDATE_SPEED);
        }
        return values;
    }

    /**
     * Returns labels for the update speed values.
     * @return labels for the update speed values.
     */
    static String[] getUpdateSpeedLabels() {
        String[] labels = new String[MAX_UPDATE_SPEED-MIN_UPDATE_SPEED+1];
        for (int i=0; i<labels.length; i++) {
            labels[i] = getResString(UPDATE_SPEED_LABEL_KEYS[i]);
        }
        return labels;
    }

    /** Getter for property applicationList.
     * @return Value of property applicationList.
     *
     */
    public List getApplicationList() {
        return appList;
    }

    /** Setter for property applicationList.
     * @param list New value of property applicationList.
     *
     */
    public void setApplicationList(List list) {
        int n = list.size();
        long [] delta = new long[n];
        long deltaTotal = 0;

        Collections.sort(appList, APP_ID_COMPARATOR);
        for (int i=0; i<n; i++) {
            Application app = (Application)list.get(i);
            int k = Collections.binarySearch(appList, app, APP_ID_COMPARATOR);
            if (k >= 0) {
                long t1 = ((Application)appList.get(k)).getCpuTime();
                long t2 = app.getCpuTime();
                long dt = (t2 - t1);
                if (dt > 0) {
                    deltaTotal += dt;
                    delta[i] = dt;
                }
            }
        }

        if (deltaTotal  > 0) {
            int usageTotal = cpu.getUsage();
            int usageLeft = usageTotal;
            for (int i=0; i<n; i++) {
                if (delta[i] > 0) {
                    Application app = (Application) list.get(i);
                    int usage = (int) ((delta[i] * usageTotal) / deltaTotal);
                    usageLeft -= usage;
                    app.setCpuUsage(usage);
                }
            }

            // Distribute leftovers
            for (int i=0; i<n && usageLeft>0; i++) {
                if (delta[i] > 0) {
                    Application app = (Application) list.get(i);
                    app.setCpuUsage(app.getCpuUsage() + 1);
                    usageLeft--;
                }
            }
        }

        appList = list;
        applicationsTableModel.fireTableDataChanged();
    }

    private static class AppIdComparator implements Comparator {
        public int compare(Object o1, Object o2) {
            int id1 = ((Application)o1).getId();
            int id2 = ((Application)o2).getId();
            return (id2 - id1);
        }
    }

    private static final Comparator APP_ID_COMPARATOR = new AppIdComparator();

    /** Getter for property cpu.
     * @return Value of property cpu.
     *
     */
    public Cpu getCpu() {
        return cpu;
    }

    /** Setter for property cpu.
     * @param cpu New value of property cpu.
     *
     */
    public void setCpu(Cpu cpu) {
        this.cpu = cpu;
    }

    /** Getter for property driveList.
     * @return Value of property driveList.
     *
     */
    public List getDriveList() {
        return driveList;
    }

    /**
     * Setter for property driveList.
     * @param list New value of property driveList.
     */
    public void setDriveList(List list) {
        driveList = list;
        drivesTableModel.fireTableDataChanged();
    }

    /** Getter for property memory.
     * @return Value of property memory.
     *
     */
    public Memory getMemory() {
        return memory;
    }

    /** Setter for property memory.
     * @param memory New value of property memory.
     *
     */
    public void setMemory(Memory memory) {
        this.memory = memory;
    }

    /** Getter for property threadList.
     * @return Value of property threadList.
     *
     */
    public List getThreadList() {
        return threadList;
    }

    /**
     * Setter for property threadList.
     * @param list New value of property threadList.
     */
    public void setThreadList(List list) {
        threadList = list;
        threadsTableModel.fireTableDataChanged();
    }

    /** Getter for property drivesTableModel.
     * @return Value of property drivesTableModel.
     *
     */
    public TableModel getDrivesTableModel() {
        return drivesTableModel;
    }

    /**
     * Marks this Observable object as having been changed; the hasChanged method will now return true.
     * If this object has changed, as indicated by the hasChanged method, then notify all of its observers
     * and then call the clearChanged method to indicate that this object has no longer changed.
     */
    public void setChangedAndNotifyObservers() {
        this.setChanged();
        this.notifyObservers();
    }
}
