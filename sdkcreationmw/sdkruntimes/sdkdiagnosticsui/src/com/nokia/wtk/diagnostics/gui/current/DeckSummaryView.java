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





package com.nokia.wtk.diagnostics.gui.current;

import javax.swing.BoxLayout;
import javax.swing.JLabel;
import javax.swing.JPanel;

import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.diagnostics.browser.AttributedContent;
import com.nokia.wtk.diagnostics.browser.Browser;
import com.nokia.wtk.diagnostics.browser.BrowserHistory;
import com.nokia.wtk.diagnostics.browser.BrowserVariables;
//import com.nokia.wtk.diagnostics.gui.MainWindow;
import com.nokia.wtk.diagnostics.views.support.InvokeLaterQueueable;
import com.nokia.wtk.diagnostics.views.support.PopLabel;

/**
 * <p>Title: </p>
 * <p>Description: Show a summary for current loaded / variables</p>
 * <p>Company: Nokia Corporation</p>
 */

public class DeckSummaryView extends JPanel implements Resource {
    private final Browser browser_;
    private final JLabel deck_ = new PopLabel(" ");
    private final JLabel stats_ = new PopLabel(" ");
    private final JLabel variables_ = new PopLabel(" ");

    static {
        AppEnvironment.addResources(
            "resources/DeckSummaryView.properties",
            DeckSummaryView.class);
    }

    public DeckSummaryView(Browser b) {
        super();
        setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
        setDeckText(AppEnvironment.getString(DECKSUMM_NODECK_TEXT));
        add(deck_);
        browser_ = b;
        BrowserEventListener listener = new BrowserEventListener();
        browser_.getBrowserHistory().addHistoryListListener(listener);
        browser_.getBrowserVariables().addVariableListListener(listener);
    }

    private void setDeckText(String text) {
        deck_.setText(text);
    }

    private void setStatsText(String text) {
        stats_.setText(text);
    }

    private boolean isStatsVisible() {
        return stats_.getParent() != null;
    }

    private void setStatsVisible(boolean visible) {
        if (visible == isStatsVisible()) {
            return;
        }

        if (visible) {
            add(stats_);
        } else {
            remove(stats_);
        }
        invalidate();
        AppEnvironment
                    .getAppEnvironment()
                    .getMainWindow()
                    .getFrame()
                    .validate();
    }

    private void setVariablesText(String text) {
        variables_.setText(text);
    }

    private boolean isVarsVisible() {
        return variables_.getParent() != null;
    }

    private void setVarsVisible(boolean visible) {
        if (visible == isVarsVisible()) {
            return;
        }
        if (visible) {
            add(variables_);
        } else {
            remove(variables_);
        }
        invalidate();
        AppEnvironment
            .getAppEnvironment()
            .getMainWindow()
            .getFrame()
            .validate();
    }

    /**
     * Inner Class: implementation of {@link BrowserHistory.HistoryListListener}
     * interface which receives notifications from the current device
     */
    private class BrowserEventListener
        implements
            BrowserHistory.HistoryListListener,
            BrowserVariables.VariableListListener {

        private InvokeLaterQueueable newContent = new InvokeLaterQueueable() {
            protected void doRun() {
                AttributedContent entry =
                    DeckSummaryView
                        .this
                        .browser_
                        .getBrowserHistory()
                        .getCurrentDocument();
                if (entry != null) {
                    DeckSummaryView.this.setDeckText(entry.getLocation());
                } else {
                    DeckSummaryView.this.setDeckText(
                        AppEnvironment.getString(DECKSUMM_NODECK_TEXT));
                }

                if (entry != null) {
                    StringBuffer buf = new StringBuffer();
                    buf.append(entry.getType());
                    if (entry.getData() != null) {
                        buf.append(", ");
                        buf.append(entry.getData().length);
                        buf.append(" ");
                        buf.append(
                            AppEnvironment.getString(DECKSUMM_STATS_BYTES));
                    }
                    DeckSummaryView.this.setStatsText(buf.toString());
                    DeckSummaryView.this.setStatsVisible(true);
                } else {
                    DeckSummaryView.this.setStatsVisible(false);
                }

                String varEntry = "";
                BrowserVariables vars =
                    DeckSummaryView.this.browser_.getBrowserVariables();
                int varNums = vars.getVariableCount();
                if (entry == null || varNums == 0) {
                    DeckSummaryView.this.setVarsVisible(false);
                    return;
                }

                for (int i = 0; i < varNums; i++) {
                    if (i > 0) {
                        varEntry += "; ";
                    }
                    varEntry += vars.getVariableNameAt(i)
                        + " = "
                        + vars.getVariableValueAt(i);
                }
                DeckSummaryView.this.setVariablesText(
                    ""
                        + varNums
                        + " "
                        + AppEnvironment.getString(DECKSUMM_VAR_TEXT)
                        + varEntry);
                DeckSummaryView.this.setVarsVisible(true);
            }
        };
        public void currentPositionChanged() {
            newContent.queue();
        }
        public void historyRemoved(int index) {
            newContent.queue();
        }
        public void allHistoryRemoved() {
            newContent.queue();
        }
        public void historyAdded(int index) {
            newContent.queue();
        }
        public void historyUpdated(int index) {
            newContent.queue();
        };
        public void variableAdded(int i) {
            newContent.queue();
        }
        public void variableRemoved(int i) {
            newContent.queue();
        }
        public void variableChanged(int i) {
            newContent.queue();
        }
        public void allVariablesRemoved() {
            newContent.queue();
        }
    }
}
