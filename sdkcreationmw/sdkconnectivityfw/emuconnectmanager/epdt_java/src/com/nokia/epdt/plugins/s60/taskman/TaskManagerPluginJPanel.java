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
import java.util.Observer;
import java.util.Observable;

/* java.awt */
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.Component;
import java.awt.GridBagLayout;
import java.awt.GridBagConstraints;

/* java.text */
import java.text.DecimalFormat;

/* javax.swing */
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTable;
import javax.swing.JComponent;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.SwingConstants;
import javax.swing.UIManager;

/* javax.swing.table */
import javax.swing.table.TableColumn;
import javax.swing.table.JTableHeader;
import javax.swing.table.TableColumnModel;
import javax.swing.table.DefaultTableCellRenderer;

/* javax.swing.border */
import javax.swing.border.EmptyBorder;
import javax.swing.border.TitledBorder;
import javax.swing.border.BevelBorder;
import javax.swing.border.CompoundBorder;

/**
 * TaskManagerJPanel
 *
 * @version 1.0
 */
class TaskManagerPluginJPanel extends JPanel implements Observer {

    private TaskManagerModel taskManagerModel;
    private TaskManagerPlugin taskManagerPlugin;
    private DecimalFormat timeFormat = new DecimalFormat("#,###,###.00");
    private DecimalFormat decimalFormat = new DecimalFormat();

    private static final int GAP = 6;

    // gui components
    private JTable applicationsJTable;
    private JLabel cpuUptimeValueJLabel;
    private JLabel cpuUsageValueJLabel;
    private JScrollPane drivesTableJScrollPane;
    private JLabel memoryUsageValueJLabel;
    private JLabel freeMemoryValueJLabel;
    private JLabel ramMemoryTotalValueJLabel;
    private JLabel romMemoryTotalValueJLabel;
    private Bar cpuUsageBar;
    private Bar memoryUsageBar;
    private Graph cpuUsageHistoryGraph;
    private Graph memoryUsageHistoryGraph;

    /**
     * Creates new form TaskManagerJPanel
     */
    public TaskManagerPluginJPanel(TaskManagerPlugin plugin) {
        super(new BorderLayout());
        taskManagerPlugin = plugin;
        taskManagerModel = plugin.getTaskManagerModel();
        createPanels();
        taskManagerModel.addObserver(this);
    }

    /**
     * Reads a string from the resource bundle
     * @param key the resource key
     * @return the string from the resource bundle, null if none
     */
    private static String getResString(String key) {
        return TaskManagerResourceBundle.getInstance().getValue(key);
    }

    /**
     * TaskManager tabs creation
     */
    private void createPanels() {
        // init components
        init();
        // adding tabs to main panel
        JTabbedPane tabs = new JTabbedPane();
        // tabs.addTab(getResString(TaskManagerConstants.THREADS_TAB_TITLE), createThreadsPanel());
        tabs.addTab(getResString(TaskManagerConstants.APPLICATIONS_TAB_TITLE), createApplicationsPanel());
        tabs.addTab(getResString(TaskManagerConstants.DRIVES_TAB_TITLE), createDrivesPanel());
        tabs.addTab(getResString(TaskManagerConstants.CPU_TAB_TITLE), createCpuPanel());
        tabs.addTab(getResString(TaskManagerConstants.MEMORY_TAB_TITLE), createMemoryPanel());
        add(tabs, BorderLayout.CENTER);
    }

    /**
     * Gui component initialization
     */
    private void init() {
        cpuUsageBar = new Bar();
        memoryUsageBar = new Bar();
        cpuUsageHistoryGraph = new Graph();
        memoryUsageHistoryGraph = new Graph();

        applicationsJTable = new JTable(taskManagerModel.getApplicationsTableModel());
        applicationsJTable.getTableHeader().setReorderingAllowed(false);
        applicationsJTable.setAutoResizeMode(JTable.AUTO_RESIZE_SUBSEQUENT_COLUMNS);
        applicationsJTable.setRowSelectionAllowed(false);
        applicationsJTable.setColumnSelectionAllowed(false);

        DefaultTableCellRenderer firstColumnRender = new DefaultTableCellRenderer();
        DefaultTableCellRenderer otherColumnsRender = new DefaultTableCellRenderer();
        firstColumnRender.setHorizontalAlignment(SwingConstants.LEFT);
        otherColumnsRender.setHorizontalAlignment(SwingConstants.RIGHT);

        // Set up "Applications" table. First column left-aligned, other
        // columns right-aligned, like Windows task manager.
        TableColumnModel cm = applicationsJTable.getColumnModel();
        TableColumn firstColumn = cm.getColumn(0);
        firstColumn.setPreferredWidth(200);
        firstColumn.setHeaderRenderer(new AlignedHeaderRenderer(SwingConstants.LEFT));
        firstColumn.setCellRenderer(firstColumnRender);
        applicationsJTable.setDefaultRenderer(Object.class, otherColumnsRender);
        applicationsJTable.getTableHeader().setDefaultRenderer(
            new AlignedHeaderRenderer(SwingConstants.RIGHT));

        cpuUsageValueJLabel = new JLabel("0 %", JLabel.CENTER);
        cpuUptimeValueJLabel = new JLabel();
        drivesTableJScrollPane = new JScrollPane();
        ramMemoryTotalValueJLabel = new JLabel("0");
        romMemoryTotalValueJLabel = new JLabel("0");
        memoryUsageValueJLabel = new JLabel("0", JLabel.CENTER);
        freeMemoryValueJLabel = new JLabel("0");
        setPreferredSize(new Dimension(480, 360));
    }

    private static JPanel createBarPanel(Bar bar, JLabel label) {
        JPanel p1 = new JPanel(new GridBagLayout());
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.fill = GridBagConstraints.VERTICAL;
        gbc.weightx = 1;
        gbc.weighty = 1;
        p1.add(bar, gbc);

        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.gridheight = GridBagConstraints.REMAINDER;
        gbc.insets.top = GAP;
        gbc.weighty = 0;
        p1.add(label, gbc);

        // Final touch
        label.setBackground(bar.getBackground());
        label.setForeground(bar.getForeground());
        p1.setBackground(bar.getBackground());
        p1.setBorder(new EmptyBorder(GAP,GAP,GAP,GAP));

        JPanel p2 = new JPanel(new BorderLayout());
        p2.add(p1, BorderLayout.CENTER);
        p2.setBorder(new CompoundBorder(
            new EmptyBorder(0,GAP,GAP,GAP),
            new BevelBorder(BevelBorder.LOWERED)));

        return p2;
    }

    private static JComponent createTablePanel(JTable table) {
        JPanel p = new JPanel(new BorderLayout());
        p.add(new JScrollPane(table), BorderLayout.CENTER);
        p.setBorder(new EmptyBorder(GAP,GAP,GAP,GAP));
        return p;
    }

    /**
     * Memory panel creation
     */
    private JPanel createMemoryPanel() {
        JPanel memoryTabJPanel = new JPanel(new BorderLayout());
        JPanel memoryJPanel = new JPanel(new BorderLayout());
        JPanel totalMemoryJPanel = new JPanel(new GridLayout(1, 0));
        JPanel ramMemoryJPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));

        JPanel panel1 = new JPanel(new BorderLayout());
        JPanel panel2 = new JPanel(new BorderLayout());

        ramMemoryJPanel.setBorder(new TitledBorder(getResString(TaskManagerConstants.MEMORY_RAM_BORDER_TITLE)));
        JLabel ramMemoryTotalJLabel = new JLabel(getResString(TaskManagerConstants.MEMORY_RAM_TOTAL_LABEL));
        panel1.add(ramMemoryTotalJLabel, BorderLayout.NORTH);
        panel1.add(new JLabel(getResString(TaskManagerConstants.MEMORY_FREE_RAM_TOTAL_LABEL)), BorderLayout.WEST);

        panel2.add(ramMemoryTotalValueJLabel, BorderLayout.NORTH);
        panel2.add(freeMemoryValueJLabel, BorderLayout.WEST);

        ramMemoryJPanel.add(panel1);
        ramMemoryJPanel.add(panel2);
        totalMemoryJPanel.add(ramMemoryJPanel);

        JPanel romMemoryJPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));

        romMemoryJPanel.setBorder(new TitledBorder(getResString(TaskManagerConstants.MEMORY_ROM_BORDER_TITLE)));
        romMemoryJPanel.add(new JLabel(getResString(TaskManagerConstants.MEMORY_ROM_TOTAL_LABEL)));
        romMemoryJPanel.add(romMemoryTotalValueJLabel);

        totalMemoryJPanel.add(romMemoryJPanel);
        memoryJPanel.add(totalMemoryJPanel, BorderLayout.SOUTH);

        JPanel memoryUsageHistoryJPanel = new JPanel(new BorderLayout());
        memoryUsageHistoryJPanel.add(memoryUsageHistoryGraph, BorderLayout.CENTER);
        memoryUsageHistoryJPanel.setBorder(
            new CompoundBorder(
            new CompoundBorder(
            new TitledBorder(getResString(TaskManagerConstants.MEMORY_USAGE_HISTORY_BORDER_TITLE)),
            new EmptyBorder(0,GAP,GAP,GAP)),
            new BevelBorder(BevelBorder.LOWERED)));

        memoryJPanel.add(memoryUsageHistoryJPanel, BorderLayout.CENTER);

        JPanel memoryUsageJPanel = new JPanel(new BorderLayout());
        memoryUsageJPanel.setBorder(new TitledBorder(getResString(TaskManagerConstants.MEMORY_USAGE_BORDER_TITLE)));
        memoryUsageJPanel.add(createBarPanel(memoryUsageBar,memoryUsageValueJLabel), BorderLayout.CENTER);

        memoryJPanel.add(memoryUsageJPanel, BorderLayout.WEST);

        memoryTabJPanel.add(memoryJPanel, BorderLayout.CENTER);
        return memoryTabJPanel;
    }

    /**
     * Cpu panel creation
     */
    private JPanel createCpuPanel() {
        JPanel cpuTabJPanel = new JPanel(new BorderLayout());
        JPanel cpuJPanel = new JPanel(new BorderLayout());
        JPanel cpuUsageJPanel = new JPanel(new BorderLayout());

        cpuUsageJPanel.setBorder(new TitledBorder(getResString(TaskManagerConstants.CPU_USAGE_BORDER_TITLE)));
        cpuUsageJPanel.add(createBarPanel(cpuUsageBar,cpuUsageValueJLabel), BorderLayout.CENTER);

        cpuJPanel.add(cpuUsageJPanel, BorderLayout.WEST);

        JPanel cpuUptimeJPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));

        cpuUptimeJPanel.setBorder(new TitledBorder(getResString(TaskManagerConstants.CPU_UPTIME_BORDER_TITLE)));
        cpuUptimeJPanel.add(new JLabel(getResString(TaskManagerConstants.CPU_UPTIME_LABEL)));

        cpuUptimeValueJLabel.setText("0");
        cpuUptimeJPanel.add(cpuUptimeValueJLabel);

        cpuJPanel.add(cpuUptimeJPanel, BorderLayout.SOUTH);

        JPanel cpuUsageHistoryJPanel = new JPanel(new BorderLayout());
        cpuUsageHistoryJPanel.add(cpuUsageHistoryGraph, BorderLayout.CENTER);
        cpuUsageHistoryJPanel.setBorder(
            new CompoundBorder(
            new CompoundBorder(
            new TitledBorder(getResString(TaskManagerConstants.CPU_USAGE_HISTORY_BORDER_TITLE)),
            new EmptyBorder(0,GAP,GAP,GAP)),
            new BevelBorder(BevelBorder.LOWERED)));

        cpuJPanel.add(cpuUsageHistoryJPanel, BorderLayout.CENTER);

        cpuTabJPanel.add(cpuJPanel, BorderLayout.CENTER);
        return cpuTabJPanel;
    }

    /**
     * Applications panel creation
     */
    private JComponent createApplicationsPanel() {
        return createTablePanel(applicationsJTable);
    }

    /**
     * Threads panel creation
     */
    private JComponent createThreadsPanel() {
        JTable table = new JTable(taskManagerModel.getThreadsTableModel());
        return createTablePanel(table);
    }

    /**
     * Drives panel creation
     */
    private JComponent createDrivesPanel() {
        JTable table = new JTable(taskManagerModel.getDrivesTableModel());
        return createTablePanel(table);
    }

    /**
     * This method is called whenever the model is changed (observed object is changed).
     * An application calls an Observable object's notifyObservers method
     * to have all the object's observers notified of the change.
     * @param o the observable object.
     * @param arg an argument passed to the notifyObservers method.
     */
    public void update(Observable o, Object arg) {
        // Memory
        int freeRamMemory = taskManagerModel.getMemory().getFree();
        int totalRamMemory = taskManagerModel.getMemory().getTotalRam();
        int usedRamMemory = totalRamMemory - freeRamMemory;
        if ( totalRamMemory != 0 ) {
            int memory = (int) (((double) usedRamMemory / (double) totalRamMemory) * 100);
            this.memoryUsageBar.setValue(memory);
            this.memoryUsageHistoryGraph.addValue(memory);
        }
        this.memoryUsageValueJLabel.setText(decimalFormat.format(usedRamMemory));
        this.freeMemoryValueJLabel.setText(decimalFormat.format(freeRamMemory));
        this.ramMemoryTotalValueJLabel.setText(decimalFormat.format(taskManagerModel.getMemory().getTotalRam()));
        this.romMemoryTotalValueJLabel.setText(decimalFormat.format(taskManagerModel.getMemory().getTotalRom()));
        // Cpu
        int usage = taskManagerModel.getCpu().getUsage();
        if(usage == -2147483648) {
                this.cpuUsageBar.setValue(0);
                this.cpuUsageHistoryGraph.addValue(0);
                this.cpuUsageValueJLabel.setText("- %");
        } else {
                this.cpuUsageBar.setValue(taskManagerModel.getCpu().getUsage());
                this.cpuUsageHistoryGraph.addValue(taskManagerModel.getCpu().getUsage());
                this.cpuUsageValueJLabel.setText(taskManagerModel.getCpu().getUsage() + " %");
        }
        this.cpuUptimeValueJLabel.setText(timeFormat.format(taskManagerModel.getCpu().getUptime()/1000000));
    }

    /**
     * Table header renderer that aligns the text any way we want. The default
     * renderer centers the text.
     */
    private static class AlignedHeaderRenderer extends DefaultTableCellRenderer {
        /**
         * Creates AlignedHeaderRenderer with the specified horizontal
         * alignment.
         *
         * @param horizontalAlignment  One of the following constants
         *           defined in <code>SwingConstants</code>:
         *           <code>LEFT</code>,
         *           <code>CENTER</code>,
         *           <code>RIGHT</code>,
         *           <code>LEADING</code> or
         *           <code>TRAILING</code>.
         */
        AlignedHeaderRenderer(int horizontalAlignment) {
            setHorizontalAlignment(horizontalAlignment);
        }

        public Component getTableCellRendererComponent(JTable table,
            Object value, boolean isSelected, boolean hasFocus, int row,
            int column) {
            if (table != null) {
                JTableHeader header = table.getTableHeader();
                if (header != null) {
                    setForeground(header.getForeground());
                    setBackground(header.getBackground());
                    setFont(header.getFont());
                }
            }

            setText((value == null) ? " " : value.toString());
            setBorder(UIManager.getBorder("TableHeader.cellBorder"));
            return this;
        }
    }
}
