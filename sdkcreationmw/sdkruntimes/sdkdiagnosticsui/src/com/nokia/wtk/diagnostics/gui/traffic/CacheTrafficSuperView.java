/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
/*
 * Created on Feb 24, 2003
 */

// ========================================================================
//  Name        : CacheTrafficSuperView.java
//  Part of     : 
//  Description : 
//  Version     : 
//
//  Copyright (c) 2000 - 2005 Nokia Corporation.
//  This material, including documentation and any related
//  computer programs, is protected by copyright controlled by
//  Nokia Corporation. All rights are reserved. Copying,
//  including reproducing, storing, adapting or translating, any
//  or all of this material requires the prior written consent of
//  Nokia Corporation. This material also contains confidential
//  information which may not be disclosed to others without the
//  prior written consent of Nokia Corporation.
// ========================================================================



package com.nokia.wtk.diagnostics.gui.traffic;

import java.awt.Dimension;
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.MouseEvent;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

import javax.swing.AbstractAction;
import javax.swing.Action;
import javax.swing.BoundedRangeModel;
import javax.swing.JComponent;
import javax.swing.JScrollBar;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTree;
import javax.swing.JViewport;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.TreeSelectionModel;

import com.nokia.wtk.app.ActionsUtilities;
import com.nokia.wtk.app.EditMenuSupport;
import com.nokia.wtk.app.RightClickMenuSupport;
import com.nokia.wtk.app.Saveable;
import com.nokia.wtk.diagnostics.browser.Browser;
import com.nokia.wtk.diagnostics.gui.SaveableMenuListener;
import com.nokia.wtk.diagnostics.views.ViewAndControlsInterface;
import com.nokia.wtk.diagnostics.views.support.ContentNode;

/**
 *
 * A class to encapsulate the similiraties between the TRaffic and Chache
 * view, and thus cut down on bugs in shared code.
 */
public abstract class CacheTrafficSuperView
    extends JScrollPane
    implements
        Resource,
        Saveable,
        EditMenuSupport,
        RightClickMenuSupport,
        ViewAndControlsInterface {

    static class ContentTree extends JTree {
        ContentTree() {
            super();
            // Create the tree widget to display session/cache information:
            setCellRenderer(new TrafficCellRenderer());

            setRowHeight(0); // Each cell determines its own row height
            setEditable(false); // Not editable
            getSelectionModel().setSelectionMode(
                TreeSelectionModel.SINGLE_TREE_SELECTION);
            setScrollsOnExpand(true);
            setRootVisible(false); // Just show child nodes (looks like a list)
            setShowsRootHandles(true); // Allows one-click expansion
        }
        TrafficCellRenderer getTrafficCellRenderer() {
            return (TrafficCellRenderer) this.getCellRenderer();
        }
    }

    final protected Browser browser_;
    final protected ContentTree treeView_;

    private TrafficTreeNodes.ContentDataNode dataNode_ = null;
    private ContentNode selectedNode_;
    private File f_;

    final private Action copyAction_ = new AbstractAction() {
        {
            ActionsUtilities.copyActionStyler(this);
        }
        public void actionPerformed(ActionEvent e) {

            if (selectedNode_ != null) {
                JTextArea textA = new JTextArea(selectedNode_.getAsText(false));
                textA.selectAll();
                textA.copy();
            }
        }
    };

    final private Action saveAsAction_;

    final private TreeSelectionListener treeSelectionListener_ =
        new TreeSelectionListener() {
        public void valueChanged(TreeSelectionEvent e) {
            JTree tree = (JTree) e.getSource();
            selectedNode_ = (ContentNode) tree.getLastSelectedPathComponent();
            dataNode_ = null;

            if ((selectedNode_ != null)
                && (selectedNode_.getUserObject()
                    instanceof TrafficTreeNodes.ContentDataNode)) {
                dataNode_ =
                    (TrafficTreeNodes.ContentDataNode) selectedNode_
                        .getUserObject();
            }
            firePropertyChange(Saveable.DIRTY_PROPERTY, !isDirty(), isDirty());
            copyAction_.setEnabled(selectedNode_ != null);

        }
    };

    private ChangeListener autoScrollHandler = new ChangeListener() {
        private boolean autoScroll = true;
        private int value = -1;

        public void stateChanged(ChangeEvent e) {
            JScrollBar scrollB =
                CacheTrafficSuperView.this.getVerticalScrollBar();
            BoundedRangeModel model = scrollB.getModel();
            int max = model.getMaximum();

            if (scrollB.getValueIsAdjusting()) {
                autoScroll =
                    (max
                        == (model.getExtent() + model.getValue()));
            } else if ((autoScroll) && (max != value)) {
                value = max;
                JViewport viewP = CacheTrafficSuperView.this.getViewport();
                Dimension size = viewP.getViewSize();
                viewP.setViewPosition(new Point(0, size.height - 1));
            }
        }
    };
    private ChangeListener waitForScrollbar = new ChangeListener() {
        public void stateChanged(ChangeEvent e) {
            JScrollBar scrollB =
                CacheTrafficSuperView.this.getVerticalScrollBar();
            if ((scrollB != null) && scrollB.isShowing()) {
                getViewport().removeChangeListener(this);
                scrollB.getModel().addChangeListener(autoScrollHandler);
            }
        }
    };

    CacheTrafficSuperView(Browser b) {
        browser_ = b;
        getViewport().setScrollMode(JViewport.BLIT_SCROLL_MODE);
        treeView_ = new ContentTree();
        treeView_.addTreeSelectionListener(treeSelectionListener_);
        setViewportView(treeView_);
        getViewport().addChangeListener(waitForScrollbar);
        SaveableMenuListener.SaveableMenuSupport sms =
            SaveableMenuListener.register(this, this);
        saveAsAction_ = sms.getSaveAsAction();
        ActionsUtilities.manageEMS(this, treeView_);
        copyAction_.setEnabled(false);
    }

    public Action getCopyAction() {
        return copyAction_;
    }

    public Action getCutAction() {
        return null;
    }

    public Action getFindAction() {
        return null;
    }

    public Action getPasteAction() {
        return null;
    }

    public JComponent getPodContent() {
        return this;
    }

    public Action getRedoAction() {
        return null;
    }

    /**
     * Gets the tree used in this view
     * @return JTree the tree
     */
    protected ContentTree getTree() {
        return treeView_;
    }

    /**
     * Required implementation for RightClickMenuSystem.RightClickMenuSUpport.
     */
    public Object[] getRightClickActions(MouseEvent e) {
        // select what was clicked on, if anything. Right-click will not
        // have selected an item, eventhough user expects it.
        // selection is cleared because none of the menu-items work for
        // multiple nodes anyway.
        treeView_.clearSelection();

        // select path
        treeView_.addSelectionPath(
            treeView_.getPathForLocation(e.getX(), e.getY()));

        return new Object[] { saveAsAction_, copyAction_ };
    }

    public Action getUndoAction() {
        return null;
    }

    public void setCharEncoding(String encoding) {}
    public String getCharEncoding() {
        return dataNode_ == null ? null : dataNode_.getEncoding();
    }
    public String getContentType() {
        return dataNode_ == null ? null : dataNode_.getType();
    }
    public boolean isDirty() {
        return dataNode_ != null;
    }
    public void save() throws IOException {
        FileOutputStream fos = new FileOutputStream(f_);
        fos.write(dataNode_.getData());
        fos.flush();
        fos.close();
    }

    public void saveAsTemplate(File file) throws IOException {
    }

    public void setSourceFile(File f) {
        File oldf = f;
        f_ = f;
        firePropertyChange(SOURCE_FILE_PROPERTY, oldf, f_);
    }
    public File getSourceFile() {
        return f_;
    }
}
