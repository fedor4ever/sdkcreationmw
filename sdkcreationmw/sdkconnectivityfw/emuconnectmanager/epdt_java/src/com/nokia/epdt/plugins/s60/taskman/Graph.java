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
import java.awt.Paint;
import java.awt.Insets;
import java.awt.Graphics;

/* java.awt */
import java.awt.Graphics2D;
import java.awt.geom.Line2D;

/* javax.swing */
import javax.swing.JComponent;

/**
 * Graph
 *
 * @version 1.0
 */
class Graph extends JComponent {
    private static double MIN_VALUE = 0.0;
    private static double MAX_VALUE = 100.0;

    private double [] historyData = new double[4046];
    private int historySize;
    private int historyPos;

    private Color gridColor = new Color(0,130,66);
    private Color graphColor = Color.yellow;
    private int gridStep = 12;

    /** Creates a new instance of Graph */
    public Graph() {
        setBackground(Color.black);
    }

    public void setGridColor(Color c) {
        Color old = gridColor;
        gridColor = c;
        firePropertyChange("gridColor", old, c);
        repaint();
    }

    public void setGraphColor(Color c) {
        Color old = graphColor;
        graphColor = c;
        firePropertyChange("graphColor", old, c);
        repaint();
    }

    protected synchronized void paintComponent(Graphics g) {
        Insets insets = getInsets();
        int x0 = insets.left;
        int y0 = insets.top;
        int w = getWidth() - x0 - insets.right;
        int h = getHeight() - y0 - insets.bottom;
        if (w > 0 && h > 0) {
            // Save graphics parameters
            Graphics2D g2 = (Graphics2D)g;
            Paint p = g2.getPaint();

            // Fill the background
            g2.setColor(getBackground());
            g2.fillRect(x0, y0, w, h);

            // Draw the vertical grid lines
            g2.setColor(gridColor);
            int x = x0 + w - 1 - (historyPos % gridStep);
            if ((historyPos % gridStep) == 0) x -= gridStep;
            while (x > x0) {
                g2.drawLine(x,y0,x,y0+h-1);
                x -= gridStep;
            }

            // Draw the horizontal grid lines
            int y = y0 + h - 1 - gridStep;
            while (y > y0) {
                g2.drawLine(x0,y,x0+w-1,y);
                y -= gridStep;
            }

            // Draw the graph
            if (historySize > 1) {
                g2.setColor(graphColor);
                x = x0+w-2;

                int k = (historyPos - 1);
                if (k < 0) k = historyData.length-1;
                double v = historyData[k];
                Line2D.Double line = new Line2D.Double(x+1,
                    y0+(h-1)*(1.0-(v-MIN_VALUE)/(MAX_VALUE-MIN_VALUE)),
                    0, 0);

                if (--k < 0) k = historyData.length-1;
                for (int i=1; i<historySize && x>=x0; i++, x--) {
                    v = historyData[k];
                    line.x2 = x;
                    line.y2 = y0+(h-1)*(1.0-(v-MIN_VALUE)/(MAX_VALUE-MIN_VALUE));
                    g2.draw(line);
                    line.x1 = line.x2;
                    line.y1 = line.y2;
                    if (--k < 0) k = historyData.length-1;
                }
            }

            // Restore graphics parameters
            g2.setPaint(p);
        }
    }

    public synchronized void addValue(double value) {
        if (value < MIN_VALUE) {
            value = MIN_VALUE;
        } else if (value > MAX_VALUE) {
            value = MAX_VALUE;
        }
        historyData[historyPos] = value;
        historyPos = (historyPos+1)%historyData.length;
        if (historySize < historyData.length) historySize++;
        repaint();
    }
}
