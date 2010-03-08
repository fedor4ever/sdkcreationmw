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

import java.awt.Cursor;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.Point;
import java.awt.Polygon;
import java.awt.Rectangle;
import java.awt.Toolkit;
import java.awt.event.MouseEvent;
import java.awt.image.BufferedImage;
import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import java.beans.PropertyVetoException;
import java.beans.VetoableChangeListener;
import java.beans.VetoableChangeSupport;

import javax.swing.Icon;
import javax.swing.ImageIcon;
import javax.swing.JTree;
import javax.swing.event.MouseInputAdapter;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.tree.DefaultTreeCellRenderer;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.MutableTreeNode;
import javax.swing.tree.TreeModel;
import javax.swing.tree.TreeNode;
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeSelectionModel;

public class DraggableTree extends JTree {
    // All the rules for deciding whether certain input ends in a drop somewhere
    // selRow: row on which the cursor is dragged, x, y: coordinates
    // bound_x, bound_width: the boundaries of selRow
    final private int considerDropping(int selRow, int x, int y,
                        int bound_x, int bound_width) {
        if (selRow == -1) return NO_DROP;
        TreePath tree_p = getPathForRow(selRow);
        TreeNode node = (TreeNode) tree_p.getLastPathComponent();
        TreeNode parent = node.getParent();
        int triple = bound_width / 3;
        // if left of icon _and_ last visible node of this branch
        if ((x < bound_x + triple) && (parent != null) &&
                                (parent.getParent() != null)) {
            int num = parent.getChildCount();
            if (num - 1 == parent.getIndex(node)) return DROP_BELOW_PARENT;
        }
        // if can have children
        if (node.getAllowsChildren()) {
            if (!isExpanded(selRow) && (x < bound_x + + bound_width - triple))
                return DROP_BELOW;
            return DROP_INTO;
        }
        if (isRowSelected(selRow)) return NO_DROP;
        return DROP_BELOW;
    }

    final private static int[] x_int = {0, 10, 10};
    final private static int[] y_int = {10, 0, 10};
    final private Polygon triangle = new Polygon(x_int, y_int, 3);
    final private static Point p = new Point(0, 0);
    private void switchCursor() {
        Icon icon;
        Object node = getLastSelectedPathComponent();
        if ((node != null) && (node instanceof TreeNode) &&
            (cellRenderer instanceof DefaultTreeCellRenderer)) {
            if (((TreeNode) node).isLeaf()) {
                icon = ((DefaultTreeCellRenderer) cellRenderer).getLeafIcon();
            } else if (isExpanded(getSelectionPath())) {
                icon = ((DefaultTreeCellRenderer) cellRenderer).getOpenIcon();
            } else {
                icon = ((DefaultTreeCellRenderer) cellRenderer).getClosedIcon();
            }
        } else icon = null;
        if ((icon != null) && (icon instanceof ImageIcon)) {
            try {
                int h, w;
                Image i = ((ImageIcon) icon).getImage();
                h =  i.getHeight(null); w = i.getWidth(null);
                Dimension d = Toolkit.getDefaultToolkit().getBestCursorSize(w, h);
                if ((d.height > h) || (d.width > w)) {
                    BufferedImage img = new BufferedImage(
                        d.width, d.height, BufferedImage.TYPE_4BYTE_ABGR);
                    Graphics2D g = img.createGraphics();
                    g.drawImage(i,0,0,this);
                    g.dispose();
                    i = img;
                }
                Cursor c =  Toolkit.getDefaultToolkit().createCustomCursor(i, p, "drop");
                setCursor(c);
                i.flush();
                return;
            } catch (IndexOutOfBoundsException i) {
            }
        }
        setCursor(Cursor.getPredefinedCursor(Cursor.CROSSHAIR_CURSOR));
    }

    final private class DraggableTreeMouseAdapter extends MouseInputAdapter {
        private int startX_ = -1, startY_ = -1;
        boolean threshholdPassed_ = false;

        public void mouseReleased(MouseEvent e) {
            if (draggedTo_ != -1) {
                setVetoableDrop(draggedTo_);
                setCursor(null);
            }
            setEditable(formerEditable_);
            draggedTo_ = -1;
            startX_ = -1;
            repaint();
        }

        public void mousePressed(MouseEvent e) {
            formerEditable_ = isEditable();
        }

        int distX, distY, x, y;
        Rectangle r, r2;
        Graphics g;

        public void mouseDragged(MouseEvent e) {
            x = e.getX(); y = e.getY();
            if (startX_ == -1) {
                startX_ = x; startY_ = y;
                threshholdPassed_ = false;
                return;
            } else if (threshholdPassed_ == false) {
                distX = startX_ - x;
                distY = startY_ - y;
                if ((distX < 10) && (distX > -10) && (distY < 10) &&
                    (distY > -10)) return;
                else {  // start drag
                    threshholdPassed_ = true;
                    setEditable(false);
                    switchCursor();
                }
            }
            int selRow = getClosestRowForLocation(x, y);
            if (selRow != -1) {
                r = getRowBounds(selRow);
                int dropMode = considerDropping(selRow, x, y, r.x, r.width);
                if ((selRow != draggedTo_) || (dropMode != dropMode_)) {
                    if (draggedTo_ != -1) {
                        r2 = getRowBounds(draggedTo_);
                        paintImmediately(r2.x - 30, r2.y,
                                    r2.x + r2.width + 30, r2.height + 7);
                    }
                    draggedTo_ = selRow;
                    if (dropMode == NO_DROP) return;
                    dropMode_ = dropMode;
                    g = getGraphics();
                    if (dropMode == DROP_BELOW_PARENT)
                        g.fillRect(r.x - 30, r.y + r.height + 1, r.width, 2);
                    else if (dropMode == DROP_INTO) {
                        x = r.x + r.width - 11;
                        y = r.y + r.height - 11;
                        triangle.translate(x, y);
                        ((Graphics2D) g).fill(triangle);
                        triangle.translate(-x, -y);
                    } else
                        g.fillRect(r.x, r.y + r.height + 1, r.width, 2);
                }
            }
        }
    }

    public DraggableTree() {
        getSelectionModel().setSelectionMode
                    (TreeSelectionModel.SINGLE_TREE_SELECTION);
        formerEditable_ = editable;
        final DraggableTree tr = this;

        DraggableTreeMouseAdapter drag = new DraggableTreeMouseAdapter();
        addMouseListener(drag);
        addMouseMotionListener(drag);
    }

    // Since this tree uses selection as the mechanism to do feedback for dragging,
    // this call needs to be re-implemented so as not to fire during dragging
    protected void fireValueChanged(TreeSelectionEvent e) {
        if (draggedTo_ == -1) { super.fireValueChanged(e); }
    }

    /** Will only allow models derived from DefaultTreeModel - if dragging is
     * going to mean anything, it must be a mutable TreeModel. Else the tree
     * will be empty with a DefaultTreeModel made from scratch.
     **/
    public DraggableTree(TreeModel model) {
        this();
        super.setModel(model);
    }

    public void addDropPropertyChangeListener(PropertyChangeListener dl) {
        p_support_.addPropertyChangeListener(dl);
    }
    public void removeDropPropertyChangeListener(PropertyChangeListener dl) {
        p_support_.removePropertyChangeListener(dl);
    }
    public void addDropVetoableChangeListener(VetoableChangeListener dl) {
        v_support_.addVetoableChangeListener(dl);
    }
    public void removeDropVetoableChangeListener(VetoableChangeListener dl) {
        v_support_.removeVetoableChangeListener(dl);
    }
    protected synchronized void setVetoableDrop(int draggedTo) {
        try {
            TreePath[] selPaths = getSelectionPaths();
            if ((selPaths != null) && (selPaths.length > 0) && (draggedTo != -1) &&
                                (treeModel instanceof DefaultTreeModel)) {
                MutableTreeNode draggedNode, draggedToNode, parentNode;
                TreePath draggedPath, draggedToPath, addPath;
                DefaultTreeModel model = (DefaultTreeModel) treeModel;

                draggedPath = getSelectionPath();
                draggedToPath = getPathForRow(draggedTo);
                if ((draggedPath.isDescendant(draggedToPath)) &&
                    (dropMode_ != DROP_BELOW_PARENT))
                    return;

                // construct the final draggedToPath depending on the dropMode_
                if (dropMode_ == NO_DROP) return;
                else if (dropMode_ == DROP_BELOW) {     // adding to parent
                    addPath = draggedToPath.getParentPath();
                } else if (dropMode_ == DROP_BELOW_PARENT) {    // go up the chain
                    addPath = draggedToPath.getParentPath().getParentPath();
                } else {                                // don't change draggedToPath
                    addPath = draggedToPath;
                }
                if (addPath == null) return;
                draggedNode =  (MutableTreeNode) draggedPath.getLastPathComponent();
                draggedToNode = (MutableTreeNode) draggedToPath.getLastPathComponent();
                if ((draggedNode == draggedToNode) && (dropMode_ != DROP_BELOW_PARENT))
                    return;

                // if someone doesn't like it, the exception will be thrown here
                v_support_.fireVetoableChange(DROP_CHANGE, addPath, draggedPath);
                // and the statements below won't happen.
                // if no-one cares, we'll do the actual change

                if (draggedToNode.getAllowsChildren() && (dropMode_ == DROP_INTO)) {   // easy
                    model.removeNodeFromParent(draggedNode);
                    model.insertNodeInto(draggedNode, draggedToNode, 0);
                } else if (dropMode_ == DROP_BELOW_PARENT) {
                    parentNode = (MutableTreeNode) addPath.getLastPathComponent();
                    if (parentNode == null) parentNode = (MutableTreeNode)
                                                        model.getRoot();
                    draggedToNode = (MutableTreeNode) draggedToNode.getParent();
                    model.removeNodeFromParent(draggedNode);
                    int pos = parentNode.getIndex(draggedToNode);
                    model.insertNodeInto(draggedNode, parentNode, pos +1);
                } else {    // standard drop
                    parentNode = (MutableTreeNode) draggedToNode.getParent();
                    if (parentNode == null) parentNode = (MutableTreeNode)
                                                        model.getRoot();
                    model.removeNodeFromParent(draggedNode);
                    int pos = parentNode.getIndex(draggedToNode);
                    model.insertNodeInto(draggedNode, parentNode, pos +1);
                }
                // inform everyone the change really happened
                expandPath(draggedToPath);
                p_support_.firePropertyChange(DROP_CHANGE, addPath, draggedPath);

            }
        } catch (PropertyVetoException pv) {
        }
    }



    private int draggedTo_ = -1, dropMode_ = 0;
    private VetoableChangeSupport v_support_ = new VetoableChangeSupport(this);
    private PropertyChangeSupport p_support_ = new PropertyChangeSupport(this);
    private boolean formerEditable_;

    // selected_row is not a target
    final private static int NO_DROP = 0;
    // standard: selected_row(parent).add(dragged, position(selected_row) + 1)
    final private static int DROP_BELOW = 1;
    // selected_row.allowsChildren(), selected_row.add(dragged, position 0)
    final private static int DROP_INTO = 2;
    // parent(parent(selected_row)).add(dragged, position(parent(selected_row)) + 1)
    final private static int DROP_BELOW_PARENT = 3;

    final public static String DROP_CHANGE = "Drop";
}
