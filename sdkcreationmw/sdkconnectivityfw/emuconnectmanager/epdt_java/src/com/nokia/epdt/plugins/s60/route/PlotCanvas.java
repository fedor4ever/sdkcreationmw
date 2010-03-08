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


package com.nokia.epdt.plugins.s60.route;

import java.awt.*;
import java.awt.event.*;
import java.awt.geom.*;
import javax.swing.*;

import java.util.*;

/**
 * Defines a canvas to which one can freely paint. Handles the drawing
 * (plotting) of the route (essentially as a polyline) and the
 * coordinate conversions necessary to transform screen coordinates to
 * geographical coordinates.
 *
 * @version 1.0
 */
public class PlotCanvas extends JComponent implements MouseListener, MouseMotionListener {
	
	final static BasicStroke stroke = new BasicStroke(2.0f);
	
	private Image bgImage;
	private double top, left, bottom, right;
	private int width, height;
	private int horzMiddle, vertMiddle;
	
	/** Holds the coordinates of the user's last mousePressed event. */
	int last_x, last_y;
	boolean firstTime = true;
	
	LinkedList prevPoints;
	PositionData posData;
	
	
	/**
	 * Creates a new <code>PlotCanvas</code> instance.
	 *
	 * @param w The preferred width of the area
	 * @param h The preferred height of the area
	 */
	public PlotCanvas(int w, int h) {
		super();
		this.setMinimumSize(new Dimension(w, h));
		this.setPreferredSize(new Dimension(w, h));
		this.setMaximumSize(new Dimension(w*3, h*3));
		
		Dimension dim = getSize();
		this.width = (int) dim.getWidth();
		this.height = (int) dim.getHeight();
		
		this.horzMiddle = this.vertMiddle = 0;
		setBackground(Color.white);
		addMouseMotionListener(this);
		addMouseListener(this);
		prevPoints = new LinkedList();
		posData = PositionData.getInstance();
		
		this.addComponentListener(new java.awt.event.ComponentListener() {
			public void componentHidden(java.awt.event.ComponentEvent evt) {
			}
			public void componentMoved(java.awt.event.ComponentEvent evt) {
			}
			public void componentResized(java.awt.event.ComponentEvent evt) {
				Dimension dim = getSize();
				width = (int) dim.getWidth();
				height = (int) dim.getHeight();
			}
			public void componentShown(java.awt.event.ComponentEvent evt) {
			}
		});
		
	}
	
	/**
	 * Calculates the longitude from a screen x coordinate.
	 *
	 * @param x Screen coordinate on the x-axis
	 * @return Longitude
	 */
	private double coordLon(int x) {
		return coordGenX(x, left, right, horzMiddle, width, 180.0);
	}
	
	/**
	 * Calculates the latitude from a screen y coordinate.
	 *
	 * @param y Screen coordinate on the y-axis
	 * @return Latitude
	 */
	private double coordLat(int y) {
		return coordGenY(y, top, bottom, vertMiddle, height, 90.0);
	}
	
	/**
	 * Generic screen coordinate to geographical coordinate
	 * -conversion routine.
	 *
	 * @param y Screen coordinate
	 * @param top Upper bound of the geographical area
	 * @param bottom Lower bound of the geographical area
	 * @param vertMiddle Geographical "wraparound" -point as a screen coordinate
	 * @param pySize Size of the screen area
	 * @param range Range of the geographical coordinate system (only one side, ie. -90...90 is "90.0")
	 * @return The geographical coordinate
	 */
	private double coordGenY(int y, double top, double bottom, int vertMiddle, int pySize, double range) {
		if (vertMiddle > 0) {
			if (y < vertMiddle)
				return (vertMiddle - y) * (top + range)/vertMiddle - range;
			else
				return (pySize - y) * (range - bottom)/(pySize - vertMiddle) + bottom;
		} else {
			return (pySize - y) * (top-bottom)/pySize + bottom;
		}
			
	}
	
	/**
	 * Generic screen coordinate to geographical coordinate
	 * calculates the longitude value 
	 * @param x
	 * @param left
	 * @param right
	 * @param pxMiddle
	 * @param pxSize
	 * @param range
	 * @return
	 */
	private double coordGenX(int x, double left, double right, int pxMiddle, int pxSize, double range) {
		if (pxMiddle > 0) {
			if (x < pxMiddle)
				return x * (left + range)/pxMiddle - range;
			else
				return x * (range - right)/(pxSize - pxMiddle) + right;
		} else {
			return x * (left-right)/pxSize + right;
		}
			
	}
	
	/**
	 * Calculates respective screen x-coordinate from the longitude
	 *
	 * @param lon Longitude
	 * @return Screen coordinate on the x-axis
	 */
	private int coordX(double lon) {
		return coordPxGen(lon, left, right, horzMiddle, width, 180.0);
	}
	
	/**
	 * Calculates respective screen y-coordinate from the latitude
	 *
	 * @param lat Latitude
	 * @return Screen coordinate on the y-axis
	 */
	private int coordY(double lat) {
		return coordPyGen(lat, top, bottom, vertMiddle, height, 90.0);
	}
	
	/**
	 * Generic method for calculating a screen coordinate from a
	 * geographic coordinate.
	 *
	 * @param lat Geographic location (coordinate)
	 * @param top Upper bound of the geographical area
	 * @param bottom Lower bound of the geographical area
	 * @param vertMiddle Geographical "wraparound" -point as a screen coordinate
	 * @param pySize Size of the screen area
	 * @param range Range of the geographical coordinate system (only one side, ie. -90...90 is "90.0")
	 * @return The screen coordinate
	 */
	private int coordPyGen(double lat, double top, double bottom, int vertMiddle, int pySize, double range) {
		if (vertMiddle > 0) {
			if (lat < bottom)
				return (int) Math.round((double) (vertMiddle * (1 - (lat + range)/(top + range))));
			else
				return (int) Math.round((double) ((pySize - vertMiddle) * (1 - (lat - bottom)/(range - bottom)))) + vertMiddle;
		} else
			return (int) Math.round((double) (pySize * (1 - (lat - bottom)/(top - bottom))));
			
	}
	
	/**
	 * 
	 * @param lc
	 * @param left
	 * @param right
	 * @param pxMiddle
	 * @param pxSize
	 * @param range
	 * @return
	 */
	private int coordPxGen(double lc, double left, double right, int pxMiddle, int pxSize, double range) {
		if (pxMiddle > 0) {
			if (lc < right)
				return (int) Math.round((double) (pxMiddle * (1 - (lc + range)/(left + range))));
			else
				return (int) Math.round((double) ((pxSize - pxMiddle) * (1 - (lc - right)/(range - right)))) + pxMiddle;
		} else
			return (int) Math.round((double) (pxSize * (1 - (lc - right)/(left - right))));
	}
	
	
	/**
	 * Undo's the last coordinate added
	 *
	 */
	public void undoLast() {
		try {
			prevPoints.removeLast();
			posData.rmCoordinate();
		} catch (NoSuchElementException e) {}
		repaint();
	}
	
	
	/**
	 * Removes all coordinates
	 *
	 */
	public void clearAll() {
		prevPoints.clear();
		posData.rmAll();
		repaint();
	}
	
	/**
	 * Saves the given screen point to PositionData as a map coordinate.
	 *
	 * @param points The first value is the x dimension, the second y.
	 */
	private void saveCoord(int[] points) {
		posData.addPoint(coordLon(points[0]), coordLat(points[1]));
	}
	
	/**
	 * Updates the last coordinate in the screen-coordinate list
	 * (prevPoints)
	 *
	 * @param coords The new (geographical) coordinate value to update
	 * the last coordinate to
	 */
	public void updateLast(double[] coords) {
		try {
			int[] point = (int[]) prevPoints.getLast();
			point[0] = coordX(coords[0]);
			point[1] = coordY(coords[1]);
			
		} catch (NoSuchElementException e) {}
		repaint();
	}
	
	
	/**
	 * Clear old coordinates and fetches new coordinates from PositionData.
	 */
	public void updateAll() {
		prevPoints.clear();
		
		LinkedList coords = posData.getCoords();
		Iterator i = coords.iterator();
		
		while (i.hasNext()) { 			
			Coordinate c = (Coordinate) i.next();
		
			addPoint(new double[] { c.getLongitude(), c.getLatitude() });
		}
		
		repaint();
	}
	
	/**
	 * Adds a point to the screen-coordinate list
	 * (prevPoints)
	 *
	 * @param coords The (geographical) coordinate to add
	 */
	public void addPoint(double[] coords) {
		int[] point = new int[2];
		
		point[0] = coordX(coords[0]);
		point[1] = coordY(coords[1]);
		
		prevPoints.add(point);
		repaint();
	}
	
	/**
	 * Sets the scale used, ie. the boundaries of the viewing window
	 *
	 * @param top Geographical top
	 * @param left Geographical left
	 * @param bottom Geographical bottom
	 * @param right Geographical right
	 */
	public void setScale(double top, double left, double bottom, double right) {
		if (left < right) // since west=+ here, this means that 180 deg is somewhere on the current map
			horzMiddle = (int) Math.round((double) (width * (left + 180.0)/(left + 360.0 - right)));
		else
			horzMiddle = 0;
		
		if (top < bottom)
			vertMiddle = (int) Math.round((double) (height * (top + 90.0)/(top + 180.0 - bottom)));
		else
			vertMiddle = 0;
		
		this.top = top;
		this.left = left;
		this.bottom = bottom;
		this.right = right;
		
		//clears map view and generated coordinates
		this.clearAll();
	}
	
	public void mousePressed(MouseEvent e){}
	public void mouseDragged(MouseEvent e){}
	public void mouseReleased(MouseEvent e){}
	public void mouseMoved(MouseEvent e){}
	
	/**
	 * Adds a new coordinate-point where the mouse was just clicked
	 * and draws a line to it.
	 *
	 * @param e a <code>MouseEvent</code> value
	 */
	public void mouseClicked(MouseEvent e) {
		int[] point = new int[2];
		point[0] = e.getX();
		point[1] = e.getY();
		prevPoints.add(point);
		saveCoord(point);
		repaint();
	}
	
	public void mouseExited(MouseEvent e){}
	public void mouseEntered(MouseEvent e){}
	
	/**
	 * Loads an image to display as the route background
	 *
	 * @param filename Filename of the image file
	 */
	public void loadImage(String filename) {
		this.bgImage = (new ImageIcon(filename)).getImage();
		repaint();
	}
	
	
	/**
	 * Clears the background image
	 *
	 */
	public void clearImage() {
		this.bgImage = null;
		repaint();
	}
	
	
	/**
	 * The paint method, calls <code>update</code>, called by calling
	 * <code>repaint()</code>
	 *
	 * @param g The graphics context
	 */
	public void paint(Graphics g) {
		update(g);
	}
	
	/**
	 * The paint method, called by calling
	 * <code>repaint()</code>. Clears the drawing area, draws a
	 * background image (if defined) and draws the route as a polyline.
	 *
	 * @param g The graphics context
	 */
	public void update(Graphics g) {
		Graphics2D g2 = (Graphics2D) g.create();
		g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
		
		// Clears the rectangle that was previously drawn.
		// Do this just in case, as the bg-image might not cover the entire canvas
		g2.setPaint(Color.white);
		g2.fillRect(0, 0, width, height);
		
		if (this.bgImage != null)
			//g2.drawImage(this.bgImage, new AffineTransform(), null);
			g2.drawImage(this.bgImage, 0, 0, this.width, this.height, null);
		
		// Draws and fills the newly positioned rectangle.
		g2.setStroke(stroke);
		g2.setPaint(Color.black);
		
		int[] c, p;
		Iterator iter = prevPoints.iterator();
		if (iter.hasNext()) {
			p = (int[]) iter.next();
			if (!iter.hasNext())
				g2.draw(new Line2D.Double(p[0], p[1], p[0], p[1]));
			while (iter.hasNext()) {
				c = (int[]) iter.next();
				g2.draw(new Line2D.Double(c[0], c[1], p[0], p[1]));
				p = c;
			}
			g2.setPaint(Color.red);
			g2.draw(new Rectangle2D.Double(p[0]-1, p[1]-1, 2, 2));
		}
	}
}
