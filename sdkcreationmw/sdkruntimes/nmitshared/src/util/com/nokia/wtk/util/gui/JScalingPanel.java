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

/* java.awt */
import java.awt.Rectangle;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;

/* java.awt.event */
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.event.MouseEvent;

/* javax.swing */
import javax.swing.JPanel;
import javax.swing.JComboBox;
import javax.swing.SwingUtilities;

/**
 * A JPanel that will allow the user to set a scale. It will transform
 * all input to match the scale, and will paint the scale properly.
 * Be careful with replacing the defined calls in derived instances of
 * this panel.
 **/
public class JScalingPanel extends JPanel {
    private double scaleX_ = 1, scaleY_ = 1;

    /**
     * Set the scaling factor for this panel. The panel will be rescaled immediatly.
     * @param x The scaling factor in the x direction. Setting 1.0 means no scaling
     * whatsoever, 2. means to double the size.
     * @param y Same as x in in the y direction.
     */
    public void setScale(double x, double y) {
        Rectangle r = new Rectangle();

        scaleX_ = x; scaleY_ = y;
        SwingUtilities.updateComponentTreeUI(this); // force a change through the whole tree
        this.computeVisibleRect(r);
        repaint(r);
    }

    /**
     * Set the scaling factor for this panel in the X direction.
     * @param x the factor. 1.0 means no scaling, 2. means scaling twice as big, etc.
     * @see setScale(double x, double y), setScaleY(double y);
     **/
    public void setScaleX(double x) { setScale(x, scaleY_);  }

    /**
     * Set the scaling factor for this panel in the Y direction.
     * @param x the factor. 1.0 means no scaling, 2. means scaling twice as big, etc.
     * @see setScale(double x, double y), setScaleX(double x);
     **/
    public void setScaleY(double y) { setScale(scaleX_, y); }

    public double getScaleX() { return scaleX_; }
    public double getScaleY() { return scaleY_; }


    /**
     * All paint(g) calls in all derived panels should call this super call
     * first to set the Graphics.
     **/
    public void paint(Graphics g) {
        ((Graphics2D) g).scale(scaleX_, scaleY_);
        super.paint(g);
    }
    protected void processMouseMotionEvent(MouseEvent e) {
        if ((scaleX_ != 1) || (scaleY_ != 1))
            e.translatePoint(
                -e.getX() + (int) (((double) e.getX()) / scaleX_),
                -e.getY() + (int) (((double) e.getY()) / scaleY_));
        super.processMouseMotionEvent(e);
    }
    protected void processMouseEvent(MouseEvent e) {
        if ((scaleX_ != 1) || (scaleY_ != 1))
            e.translatePoint(
                -e.getX() + (int) (((double) e.getX()) / scaleX_),
                -e.getY() + (int) (((double) e.getY()) / scaleY_));
        super.processMouseEvent(e);
    }
    public void repaint(Rectangle r)    {
        repaint(0, r.x, r.y, r.width, r.height);
    }
    public void repaint(long tm, int x, int y, int width, int height) {
        super.repaint(tm,
            (int) (scaleX_ * (double) x),
            (int) (scaleY_ * (double) y),
            (int) (scaleX_ * (double) width),
            (int) (scaleY_ * (double) height));
    }

    public Dimension getPreferredSize() {
        Dimension d = new Dimension(super.getPreferredSize());
        d.height = (int) (scaleY_ * (double) d.height);
        d.width = (int) (scaleX_ * (double) d.width);
        return d;
    }
    public Dimension getMinimumSize() {
        Dimension d = new Dimension(super.getMinimumSize());
        d.height = (int) (scaleY_ * (double) d.height);
        d.width = (int) (scaleX_ * (double) d.width);
        return d;
    }
    public Dimension getMaximumSize() {
        Dimension d = new Dimension(super.getMaximumSize());
        d.height = (int) (scaleY_ * (double) d.height);
        d.width = (int) (scaleX_ * (double) d.width);
        return d;
    }

    public Rectangle getBounds() { return this.getBounds(null); }
    public Rectangle getBounds(Rectangle r) {
        Rectangle ret = super.getBounds(r);
        ret.width = (int) ((double)ret.width * scaleX_);
        ret.height = (int) ((double) ret.height * scaleY_);
        return ret;
    }
    public Dimension getSize(Dimension sz) {
        Dimension d = super.getSize(sz);
        d.height = (int) ((double) d.height * scaleY_);
        d.width = (int) ((double) d.width * scaleX_);
        return d;
    }
    public int getHeight() { return (int) (((double) super.getHeight()) * scaleX_); }
    public int getWidth() { return (int) (((double) super.getWidth()) * scaleY_); }

    /**
     * A convenience function.
     * @return  a new scaling ComboBox for this Panel. It has some pre-selected
     * zoom sizes in percentages, and the user can input a zoomsize as well.
     **/
    public JComboBox getScalingComboBox() {
	    JComboBox zoomMenu = new JComboBox();
	    zoomMenu.setToolTipText("Select or type a scaling factor in %");
        zoomMenu.setEditable(true);
		zoomMenu.addItem("100%");
		zoomMenu.addItem("200%");
		zoomMenu.addItem("400%");
		zoomMenu.addItem("1000%");
		zoomMenu.addItem("2000%");
		zoomMenu.addItem("5000%");

	    JScalingPanelPercentageMenuAdapter zoomAdapter =
	                    new JScalingPanelPercentageMenuAdapter(this);
		zoomMenu.addActionListener(zoomAdapter);
		return zoomMenu;
    }
}

class JScalingPanelPercentageMenuAdapter implements ActionListener {
    private JScalingPanel zoom_ = null;

    public JScalingPanelPercentageMenuAdapter(JScalingPanel zoom) { zoom_ = zoom; }
	public void actionPerformed(ActionEvent event) {
		String s = ((String)
		            ((JComboBox)
		                event.getSource()).getSelectedItem()).trim();
		int i = -1;
		try {
		    i = Integer.parseInt(s);
		} catch (NumberFormatException nf) {
		    if (s.length() > 1) {
		        try {
		            i = Integer.parseInt(
		                s.substring(0, s.length() - 1).trim());
		        } catch (NumberFormatException nn) {}
		    }
		} finally {
		    if (i < 100) {
		        i = -1;
		    }
		    if (i != -1) {
		        zoom_.setScale(((double) i) / 100., ((double) i) / 100.);
		    }
		}
		((JComboBox) event.getSource()).setSelectedItem(
		                                ((int) (zoom_.getScaleX()* 100)) + "%");
	}

}

