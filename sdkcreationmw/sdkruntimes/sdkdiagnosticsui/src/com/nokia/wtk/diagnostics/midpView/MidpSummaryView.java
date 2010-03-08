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

import java.awt.BorderLayout;

import javax.swing.BoundedRangeModel;
import javax.swing.BoxLayout;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JProgressBar;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.TreeModelEvent;
import javax.swing.event.TreeModelListener;

import com.nokia.wtk.app.LookFactory;
import com.nokia.wtk.diagnostics.browser.Browser;
import com.nokia.wtk.diagnostics.views.support.PopLabel;

/**
 *
 * Show the summary of the state of the Midp machine
 */
public class MidpSummaryView extends JPanel {
    private final JLabel numbersPopLabel_;
    private final KVMMemoryModel guageModel_;

    public MidpSummaryView(Browser b) {
        super();

        setLayout(new BorderLayout());
        guageModel_ = KVMMemoryModel.getKVMMemoryModel(b);

        numbersPopLabel_ = new JLabel();
        add(numbersPopLabel_, BorderLayout.WEST);

        guageModel_
            .getBoundedRangeModel()
            .addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                BoundedRangeModel model = (BoundedRangeModel) arg0.getSource();
                updateKVMSummary(model.getValue(), model.getMaximum());

            }
        });
        if (guageModel_.isKVMMemorySupported()) {
            updateKVMSummary(
                guageModel_.getBoundedRangeModel().getValue(),
                guageModel_.getBoundedRangeModel().getMaximum());
        } else {
            numbersPopLabel_.setText("This device does not support J2ME.");
        }
    }

    private void updateKVMSummary(int val, int max) {
        numbersPopLabel_.setText(
                "RAM Used: "
                    + val
                    + " bytes ("
                    + guageModel_.getHeapSizePercentage()
                    + "%)  ");

        if (getComponentCount() == 2) { // add guage later
            // to avoid bad foreground color being set
            if (val > 0) {
                JProgressBar guage =
                    new JProgressBar(guageModel_.getBoundedRangeModel());

                add(guage, BorderLayout.CENTER);
                guage.setForeground(
                    LookFactory.getCodeColorSet().selectionColor);
                //guage.setOpaque(false);
            }
        }
    }
}
