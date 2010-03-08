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

/* java.awt */
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Insets;
import java.awt.Paint;
import java.awt.Rectangle;
import java.awt.TexturePaint;
import java.awt.AlphaComposite;

/* java.awt.geom */
import java.awt.geom.AffineTransform;
import java.awt.geom.Rectangle2D;

/* java.awt.image */
import java.awt.image.BufferedImage;

/* javax.swing */
import javax.swing.JComponent;

/**
 * LED-like indicator.
 *
 * @version 1.0
 */
class Bar extends JComponent {
    private static final int MIN_WIDTH = 33;
    private static final int MIN_HEIGHT = 50;
    private static final int PREF_WIDTH = MIN_WIDTH;
    private static final int PREF_HEIGHT = 2*MIN_HEIGHT;
    private static final int LED_SEGMENT_HEIGHT = 2;
    private static final int LED_VGAP = 1;
    private static final int LED_HGAP = 1;

    public static final String INACTIVE_COLOR_PROPERTY = "inactiveColor";
    public static final String VALUE_PROPERTY = "value";

    private static double MIN_VALUE = 0.0;
    private static double MAX_VALUE = 100.0;
    private double value = 0; //  0 >= value <= 100

    private Color inactiveColor;
    private boolean autoInactiveColor = false;

    private Paint onPaint;
    private Paint offPaint;

    /** Creates a new instance of Bar */
    public Bar() {
        setForeground(Color.green);
        setBackground(Color.black);
        setPreferredSize(new Dimension(PREF_WIDTH,PREF_HEIGHT));
        setMinimumSize(new Dimension(MIN_WIDTH,MIN_HEIGHT));
    }

    private BufferedImage createTextureImage() {

        // Create the image
        BufferedImage buf =
            new BufferedImage(LED_SEGMENT_HEIGHT, LED_SEGMENT_HEIGHT+LED_VGAP,
                BufferedImage.TYPE_INT_ARGB);

        // Clear image with transparent alpha
        Graphics2D g = buf.createGraphics();
        g.setComposite(AlphaComposite.getInstance(AlphaComposite.CLEAR,0));
        g.fillRect(0, 0, buf.getWidth(), buf.getHeight());
        g.dispose();

        // Return clear image LED_SEGMENT x (LED_SEGMENT+LED_GAP)
        return buf;
    }

    private Paint getOffPaint() {
        if (offPaint == null) {
            BufferedImage b = createTextureImage();
            int rgb = getInactiveColor().getRGB();
            for (int y=0; y<LED_SEGMENT_HEIGHT; y++) {
                for(int x = y%2; x<LED_SEGMENT_HEIGHT; x+=2) {
                    b.setRGB(x, y+LED_VGAP, rgb);
                }
            }

            // Create the texture
            Rectangle2D anchor = new Rectangle(b.getWidth(), b.getHeight());
            offPaint = new TexturePaint(b, anchor);
        }
        return offPaint;
    }

    private Paint getOnPaint() {
        if (onPaint == null) {
            BufferedImage b = createTextureImage();
            Graphics2D g = b.createGraphics();
            g.setColor(getForeground());
            g.fillRect(0,LED_VGAP,LED_SEGMENT_HEIGHT,LED_SEGMENT_HEIGHT);
            g.dispose();

            // Create the texture
            Rectangle2D anchor = new Rectangle(b.getWidth(), b.getHeight());
            onPaint = new TexturePaint(b, anchor);
        }
        return onPaint;
    }

    private Color getInactiveColor() {
        if (inactiveColor == null) {
            inactiveColor = getForeground().darker();
            autoInactiveColor = true;
        }
        return inactiveColor;
    }

    public void setInactiveColor(Color c) {
        Color oldValue = getInactiveColor();
        autoInactiveColor = false;
        if (!oldValue.equals(c)) {
            inactiveColor = c;
            offPaint = null;
            firePropertyChange(INACTIVE_COLOR_PROPERTY, oldValue, c);
            repaint();
        }
    }

    public void setForeground(Color c) {
        Color fg = getForeground();
        if ((fg == null && c != null) || (fg != null && !fg.equals(c))) {
            onPaint = null;
            if (autoInactiveColor) {
                inactiveColor = null;
                offPaint = null;
            }
        }
        super.setForeground(c);
    }

    /**
     * Paints the component
     * @param g the graphics to paint to
     */
    protected void paintComponent(Graphics g) {
        Insets insets = getInsets();
        int x0 = insets.left;
        int y0 = insets.top;
        int w = getWidth() - x0 - insets.right;
        int h = getHeight() - y0 - insets.bottom;
        if (w > 0 && h > 0) {

            // Save graphics parameters
            Graphics2D g2 = (Graphics2D)g;
            Paint p = g2.getPaint();
            AffineTransform tx = g2.getTransform();

            // Fill the background
            g2.setColor(getBackground());
            g2.fillRect(x0, y0, w, h);

            // Select the location of the vertical divider
            int x1, x2;
            if(w%2==0) {
                x1 = w/2-1;
                x2 = w-2;
            } else {
                x1 = w/2;
                x2 = w-1;
            }

            // Aligh the bottom segment of the LED
            int dy = h%(LED_SEGMENT_HEIGHT+LED_VGAP) + insets.bottom;
            g2.translate(0, dy);

            if (value > MIN_VALUE) {
                // Paint the "off" part
                double y1 = h*(1.0-(value-MIN_VALUE)/(MAX_VALUE-MIN_VALUE));
                g2.setPaint(getOffPaint());
                Rectangle2D.Double r = new Rectangle2D.Double(x0,y0-dy,x1,y1);
                g2.fill(r);
                r.x += x1+LED_HGAP;
                g2.fill(r);

                // Paint the "on" part
                g2.setPaint(getOnPaint());
                r.setRect(x0, y0-dy+y1, x1, h-y1);
                g2.fill(r);
                r.x += x1+LED_HGAP;
                g2.fill(r);
            } else {
                // Optimized path for the "nothing is going on" state
                g2.setPaint(getOffPaint());
                Rectangle r = new Rectangle(x0, y0-dy, x1, h);
                g2.fill(r);
                r.x += x1+LED_HGAP;
                g2.fill(r);
            }

            // Restore graphics parameters
            g2.setPaint(p);
            g2.setTransform(tx);
        }
    }

    /**
     * Getter for property value.
     * @return Value of property value.
     */
    public double getValue() {
        return value;
    }

    /**
     * Setter for property value.
     * @param v New value of property value.
     */
    public void setValue(double v) {
        if (v < MIN_VALUE) {
            v = MIN_VALUE;
        } else if (v > MAX_VALUE) {
            v = MAX_VALUE;
        }
        if (value != v) {
            double oldValue = value;
            value = v;
            firePropertyChange(VALUE_PROPERTY, oldValue, v);
            repaint();
        }
    }
}
