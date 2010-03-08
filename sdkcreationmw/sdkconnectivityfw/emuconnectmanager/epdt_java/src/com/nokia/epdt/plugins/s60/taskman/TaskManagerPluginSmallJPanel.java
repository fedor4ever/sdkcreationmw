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

/* java.text */
import java.text.DecimalFormat;

/* javax.swing */
import javax.swing.JLabel;

/**
 * TaskManagerPluginSmallJPanel
 *
 * @version 1.0
 */
class TaskManagerPluginSmallJPanel extends JLabel implements Observer {
    private DecimalFormat decimalFormat = new DecimalFormat();

    /**
     * Creates a new instance of TaskManagerPluginSmallJPanel
     */
    public TaskManagerPluginSmallJPanel(TaskManagerModel model) {
        super(" "); // empty string shrinks preferred size to zero
        model.addObserver(this);
    }

    /**
     * Receives the data model change notifications.
     * @param o the data model
     * @param arg an argument
     */
    public void update(Observable o, Object arg) {
        TaskManagerModel model = (TaskManagerModel)o;
        Memory memory = model.getMemory();
        int freeRamMemory = memory.getFree();
        int totalRamMemory = memory.getTotalRam();
        int usedRamMemory = totalRamMemory - freeRamMemory;
        setText("Applications: " + model.getApplicationList().size() +
                "   CPU Usage: " + model.getCpu().getUsage() +
                "%   Memory Usage: " + decimalFormat.format(usedRamMemory) +
                " Bytes");
    }
}
