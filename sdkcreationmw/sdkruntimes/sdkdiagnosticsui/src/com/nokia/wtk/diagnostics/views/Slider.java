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


package com.nokia.wtk.diagnostics.views;

import java.awt.BorderLayout;
import java.awt.Cursor;
import java.awt.Dimension;
import java.awt.Point;
import java.awt.Window;
import java.awt.dnd.DnDConstants;
import java.awt.dnd.DragGestureEvent;
import java.awt.dnd.DragGestureListener;
import java.awt.dnd.DragGestureRecognizer;
import java.awt.dnd.DragSource;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.Icon;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.SwingUtilities;

import com.nokia.wtk.app.AppEnvironment;

/**
 * <p>Title: DraggableButton</p>
 * <p>Description: A class for buttons that, when dragged, will change the
 * shape of an associated component.</p>
 * <p>Company: Nokia Corporation</p>
 * @version 1.0
 */
class DraggableButton extends JButton implements Resource {

    DraggableButton(Icon icon) {

        this.setIcon(icon);

        this.setBorder(BorderFactory.createEmptyBorder());
        this.setBorderPainted(false);
        this.setFocusPainted(false);
        this.setOpaque(false);
        this.setCursor(new Cursor(Cursor.S_RESIZE_CURSOR));
    }
}

/**
 *
 *
 * <p>Title: A class for a slider. If no thumb-image is given, the whole
 * slider will be senstitive for being dragged when enabled.</p>
 * <p>Description: </p>
 * <p>Company: Nokia Corporation</p>
 * @version 1.0
 */
public class Slider extends TexturedArea implements Resource {
    static {
        AppEnvironment.addResources(
            "resources/Slider.properties",
            Slider.class);
    }

    /** The command name for the PropertyChange that a drag started or ended**/
    public final static String DRAGGING_PROPERTY = "draggingPropertyForSlider";

    private final static String ALIGN =
        AppEnvironment.getString(SLIDER_THUMB_ALIGN);
    private final static ImageIcon closedIcon__ =
        AppEnvironment.getString(SLIDER_LINE_IMAGE).length() != 0
            ? AppEnvironment.getIcon(SLIDER_LINE_IMAGE)
            : null;
    private final static int INDENT =
        AppEnvironment.getInt(SLIDER_THUMB_INDENT);
    private final static ImageIcon openIcon__ =
        AppEnvironment.getString(SLIDER_LINE_IMAGE_OPEN).length() != 0
            ? AppEnvironment.getIcon(SLIDER_LINE_IMAGE_OPEN)
            : null;

    private final JComponent componentToDrag_;
    private final SliderGestureListener sliderGesture_;


    Slider() {
        super();
        setLayout(new BorderLayout());
        setOpaque(false);
        add(Box.createHorizontalStrut(INDENT));

        String iconFile = AppEnvironment.getString(SLIDER_THUMB_IMAGE);
        if ((iconFile == null) || (iconFile.length() == 0)) {
            componentToDrag_ = this;
        } else {
            Icon thumb = AppEnvironment.getIcon(SLIDER_THUMB_IMAGE);
            componentToDrag_ = new DraggableButton(thumb);
        }
        sliderGesture_ = new SliderGestureListener(componentToDrag_) {
            public void setDragging(boolean dragging) {
                super.setDragging(dragging);
                Slider.this.firePropertyChange(
                    Slider.DRAGGING_PROPERTY, !dragging, dragging);
            }
        };
        setEnabled(false);
    }

    /**
     * Is this slider being dragged as we speak?
     * @return boolean thrue if the slider is being dragged by user, false
     * otherwise.
     */
    public boolean isDraggingState() {
        return sliderGesture_.isDragging();
    }


    /**
     * Ask whether this slider is enabled for use, or invisible to the user
     * @return the state of the slider
     */
    public boolean isEnabled() {
        return sliderGesture_.isEnabled();
    }

    /**
     * Set the component whose size this slider will change
     * @param comp the JComponent affected by the slider dragging
     */
    void setComponent(JComponent comp) {
        sliderGesture_.setComponentToResize(comp);
        this.setOpaque(comp.isOpaque());
        this.setBackground(comp.getBackground());
    }

    /**
     * Return the JComponent this slider is influencing.
     * @return JComponent the component when set, null otherwise
     */
    public JComponent getComponent() {
        return sliderGesture_.getComponentToResize();
    }

    /**
     * Set whether this slider is visible and enabled for dragging
     * @param enabled the state to set the slider in
     */
    public void setEnabled(boolean enabled) {
        sliderGesture_.setEnabled(enabled);
        if (componentToDrag_ instanceof DraggableButton) {
            if (enabled && (openIcon__ != null)) {
                setImage(openIcon__);
            } else if (closedIcon__ != null) {
                setImage(closedIcon__);
            }
            if (enabled) {
                Box b = Box.createHorizontalBox();
                if ("WEST".equals(ALIGN)) {
                    b.add(Box.createHorizontalStrut(INDENT));
                    b.add(componentToDrag_);

                    add(b, BorderLayout.WEST);
                } else if ("EAST".equals(ALIGN)) {

                    b.add(componentToDrag_);
                    b.add(Box.createHorizontalStrut(INDENT));
                    add(b, BorderLayout.EAST);
                } else {
                    add(componentToDrag_, BorderLayout.CENTER);
                }
            } else {
                removeAll();
            }
        }
    }
}

class SliderGestureListener
    implements DragGestureListener, MouseListener, MouseMotionListener {

    private JComponent comp_;
    private final JComponent drag_;
    private boolean dragging_ = false;

    private final DragGestureRecognizer dragRecognizer_;
    private int maxHeight = 10000;
    private int minHeight = 0;

    private Dimension startD_;
    private Point startP_;
    private Window win_;

    SliderGestureListener(JComponent drag) {
        drag_ = drag;
        DragSource ds = DragSource.getDefaultDragSource();
        dragRecognizer_ =
            ds.createDefaultDragGestureRecognizer(
                drag_,
                DnDConstants.ACTION_MOVE,
                this);
        drag_.addMouseMotionListener(this);
        drag_.addMouseListener(this);
    }

    public void dragGestureRecognized(DragGestureEvent dge) {
        if (comp_ == null) {
            return;
        }
        startD_ = comp_.getSize();
        startP_ = dge.getDragOrigin();
        SwingUtilities.convertPointToScreen(startP_, drag_);
        maxHeight = comp_.getMaximumSize().height;
        minHeight = comp_.getMinimumSize().height;
        setDragging(true);
        win_.setCursor(new Cursor(Cursor.S_RESIZE_CURSOR));
    }
    boolean isEnabled() {
        return (dragRecognizer_.getComponent() != null);
    }
    public void mouseClicked(MouseEvent e) {}
    public void mouseDragged(MouseEvent e) {

        if (!isDragging()) {
            return;
        }
        Point deltaP = e.getPoint();
        Dimension newD = (Dimension) startD_.clone();

        SwingUtilities.convertPointToScreen(deltaP, drag_);
        newD.height -= startP_.getY() - deltaP.getY();
        if ((newD.height >= minHeight) && (newD.height <= maxHeight)) {
            comp_.setPreferredSize(newD);
            comp_.invalidate();
            int w = win_.getWidth();
            int h = win_.getHeight();
            win_.pack();
            if (win_.getWidth() != w || win_.getHeight() != h) {
                win_.repaint(0);
            }
        }
    }
    public void mouseEntered(MouseEvent e) {}
    public void mouseExited(MouseEvent e) {}
    public void mouseMoved(MouseEvent e) {}
    public void mousePressed(MouseEvent e) {
        win_ = SwingUtilities.getWindowAncestor(comp_);
    }
    public void mouseReleased(MouseEvent e) {
        win_.setCursor(Cursor.getDefaultCursor());
        setDragging(false);
        win_.validate();
    }
    void setComponentToResize(JComponent comp) {
        comp_ = comp;
    }
    JComponent getComponentToResize() {
        return comp_;
    }
    void setEnabled(boolean enabled) {
        if (enabled) {
            dragRecognizer_.setComponent(drag_);
            drag_.setCursor(new Cursor(Cursor.S_RESIZE_CURSOR));
        } else {
            dragRecognizer_.setComponent(null);
            drag_.setCursor(Cursor.getDefaultCursor());
        }
    }

    void setDragging(boolean dragging) {
        this.dragging_ = dragging;
    }

    boolean isDragging() {
        return dragging_;
    }
}
