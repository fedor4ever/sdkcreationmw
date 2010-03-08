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





package com.nokia.wtk.diagnostics.gui.traffic;

import java.awt.BorderLayout;

import javax.swing.event.TreeModelEvent;
import javax.swing.event.TreeModelListener;
import javax.swing.tree.TreeModel;
import javax.swing.JPanel;

import com.nokia.wtk.app.AppEnvironment;
import com.nokia.wtk.diagnostics.browser.Browser;
import com.nokia.wtk.diagnostics.views.support.PopLabel;

/**
 * A class for objects that show a lone-line summary of the
 * current state of the loader traffic.
 *
 */
class TrafficSummaryView extends JPanel implements Resource {

    static {
        AppEnvironment.addResources(
            "resources/TrafficSummaryView.properties",
            TrafficSummaryView.class);
    }
    private final static String bytes =
        AppEnvironment.getString(TRAFFIC_SUMMARY_BYTES_TXT);
    private final Browser browser_;
    private final TrafficModel traf_;

    private PopLabel lastNodeLabel_;
    private PopLabel summaryLabel_;

    TrafficSummaryView(CentralTrafficSynchronizer sync) {
        traf_ = sync.getTrafficModel();
        browser_ = sync.getBrowser();

        setLayout(new BorderLayout());
        lastNodeLabel_ = new PopLabel();
        summaryLabel_ = new PopLabel();
        add(summaryLabel_, BorderLayout.NORTH);
        add(lastNodeLabel_, BorderLayout.SOUTH);
        setNeutralState();
        setOpaque(false);

        traf_.addTrafficCountListener (new TrafficCountListener() {
            public void trafficCountUpdated() {
                showTrafficSummary();
            }
        });


        traf_.getTreeModel().addTreeModelListener(new TreeModelListener() {
            private void setNewNode(TreeModelEvent arg0) {
                TreeModel tmodel = (TreeModel) arg0.getSource();
                int latest = tmodel.getChildCount(tmodel.getRoot());
                if (latest == 0) {
                    setNeutralState();
                    return;
                } else {
                    latest--; // prepare count to be index of last node
                }
                Object z = tmodel.getChild(tmodel.getRoot(), latest);
                showNodeObject(z);
            }

            public void treeNodesChanged(TreeModelEvent arg0) {
                setNewNode(arg0);
            }
            public void treeNodesInserted(TreeModelEvent arg0) {
                setNewNode(arg0);
            }
            public void treeNodesRemoved(TreeModelEvent arg0) {
                setNewNode(arg0);
            }
            public void treeStructureChanged(TreeModelEvent arg0) {
                setNewNode(arg0);
            }

        });

        setOpaque(false);

    }
    protected void setNeutralState() {
        lastNodeLabel_.setText(
            AppEnvironment.getString(TRAFFIC_SUMMARY_SIZE_TXT)
                + traf_.getSize());
        lastNodeLabel_.setIcon(null);

        summaryLabel_.setText ("");
    }


    protected void showTrafficSummary() {

      StringBuffer buf = new StringBuffer();
      int count1 = 0;
      int count2 = 0;
      count1 = traf_.getHttpRequestCount();
      count2 = traf_.getHttpResponseCount();
      if (count1 > 0 || count2 > 0) {
        buf.append ("HTTP: " + count1 + "/" + count2 + "  ");
      }
      count1 = traf_.getTcpSentCount();
      count2 = traf_.getTcpReceivedCount();
      if (count1 > 0 || count2 > 0) {
        buf.append ("TCP: " + count1 + "/" + count2 + "  ");
      }
      count1 = traf_.getUdpSentCount();
      count2 = traf_.getUdpReceivedCount();
      if (count1 > 0 || count2 > 0) {
        buf.append ("UDP: " + count1 + "/" + count2 + "  ");
      }
      count1 = traf_.getSmsSentCount();
      count2 = traf_.getSmsReceivedCount();
      if (count1 > 0 || count2 > 0) {
        buf.append ("SMS: " + count1 + "/" + count2);
      }
      // pushed messages (includes: si, sl, co, multipart, mms received, dr)
      count1 = traf_.getPushedCount();
      if (count1 > 0) {
        buf.append ("Push: " + count1);
      }
      count1 = traf_.getBluetoothSentCount();
      count2 = traf_.getBluetoothReceivedCount();
      if (count1 > 0 || count2 > 0) {
        buf.append ("Bluetooth: " + count1 + "/" + count2);
      }
      summaryLabel_.setText(buf.toString());
    }

    protected void showNodeObject(Object elem) {

        if (elem instanceof TopLevelContentNode) {
            TopLevelContentNode n = (TopLevelContentNode) elem;
            lastNodeLabel_.setIcon(n.getIcon());
            lastNodeLabel_.setText(n.getSummary());
        } else {
            setNeutralState();
        }
    }
}
