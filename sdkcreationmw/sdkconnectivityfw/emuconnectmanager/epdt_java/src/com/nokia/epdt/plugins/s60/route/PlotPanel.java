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
import javax.swing.*;
import java.util.Observer;

import javax.swing.filechooser.FileFilter;
import java.io.File;

/**
 * Implements a panel for containing a graphical plot of the route drawn, as well
 * as controls to manipulate the view.
 *
 * @version 1.0
 */
public class PlotPanel extends JPanel implements Observer {
	
	PlotCanvas mapView;
	JButton undoB, imageB, applyB, clearB;
	
	JLabel topL, leftL, botL, rightL;
	JTextField topTf, leftTf, botTf, rightTf;
	
	private String topLeft, botRight;
	private JLabel coordTL, coordBR;
	
	private final JFileChooser fc = new JFileChooser(".");
	
	
	private class MapFilter extends FileFilter {
		public boolean accept(File f) {
			if (f.isDirectory()) {
				return true;
			}
			String ext;
			String fname = f.getName();
			int i = fname.lastIndexOf('.');
			
			if (i > 0 &&  i < (fname.length() - 1))
				ext = fname.substring(i+1).toLowerCase();
			else
				return false;
			
			if (ext.equals("jpg") || ext.equals("jpeg")
					|| ext.equals("png") || ext.equals("gif"))
				return true;
			else
				return false;
		}
		
		public String getDescription() {
			return "Image files (jpeg, png, gif)";
		}
	}
	
	
	/**
	 * Creates a new <code>PlotPanel</code> instance. Lays out the
	 * components, loads the default values and adds the action
	 * listeners.
	 *
	 */
	public PlotPanel() {
		super();
		this.setMaximumSize(new Dimension(800,600));
		this.setPreferredSize(null);
		
		this.fc.addChoosableFileFilter(new MapFilter());
		
		this.setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
		
		this.setBorder(
				BorderFactory.createCompoundBorder(BorderFactory.createTitledBorder("Plot"), 
				BorderFactory.createEmptyBorder(5,5,5,5)));
		
		topLeft = "Top left: 60.10N 24.53E";
		botRight = "Bottom right: 6.09S 106.51E";
		coordTL = new JLabel(topLeft);
		coordTL.setAlignmentX(Component.CENTER_ALIGNMENT);
		coordBR = new JLabel(botRight);
		coordBR.setAlignmentX(Component.CENTER_ALIGNMENT);
		
		this.add(coordTL);
		this.add(coordBR);
		
		mapView = new PlotCanvas(300, 200);
		mapView.setScale(60.1,-24.53,-6.09,-106.51);
		this.add(mapView);

		JPanel buttonPanel = new JPanel();
		
		buttonPanel.setLayout(new GridLayout(1, 0, 5, 5));
		
		undoB = new JButton("Undo point");
		undoB.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				undoBActionPerformed(evt);
			}
		});
		buttonPanel.add(undoB);
		
		imageB = new JButton("Overlay map");
		imageB.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				imageBActionPerformed(evt);
			}
		});
		buttonPanel.add(imageB);
		
		clearB = new JButton("Clear map");
		clearB.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				clearBActionPerformed(evt);
			}
		});
		buttonPanel.add(clearB);

		JPanel tempPanel = new JPanel();
		
		tempPanel.add(Box.createHorizontalGlue(), BorderLayout.EAST);
		tempPanel.add(buttonPanel, BorderLayout.CENTER);
		tempPanel.add(Box.createHorizontalGlue(), BorderLayout.EAST);
		this.add(tempPanel);
		
		JPanel fieldPanel = new JPanel();
		
		fieldPanel.setLayout(new GridLayout(2, 0));
		
		topL = new JLabel("North(N) ", JLabel.TRAILING);
		fieldPanel.add(topL);
		
		topTf = new JTextField(7);
		topL.setLabelFor(topTf);
		fieldPanel.add(topTf);
		
		leftL = new JLabel("East(E) ", JLabel.RIGHT);
		fieldPanel.add(leftL);
		
		leftTf = new JTextField(7);
		leftL.setLabelFor(leftTf);
		fieldPanel.add(leftTf);
		
		botL = new JLabel("South(S) ", JLabel.TRAILING);
		fieldPanel.add(botL);
		
		botTf = new JTextField(7);
		botL.setLabelFor(botTf);
		fieldPanel.add(botTf);
		
		rightL = new JLabel("West(W) ", JLabel.RIGHT);
		fieldPanel.add(rightL);
		
		rightTf = new JTextField(7);
		rightL.setLabelFor(rightTf);
		fieldPanel.add(rightTf);
		
		applyB = new JButton("Load scale");
		applyB.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				applyBActionPerformed(evt);
			}
		});
		
		tempPanel = new JPanel();
		tempPanel.add(fieldPanel, BorderLayout.CENTER);
		tempPanel.add(applyB, BorderLayout.WEST);
		this.add(tempPanel);
		
		PositionData.getInstance().addObserver(this);
	}

	/**
	 * Action handler for the undo-button, undoes the last coordinate
	 * added.
	 *
	 * @param evt a <code>java.awt.event.ActionEvent</code> value
	 */
	private void undoBActionPerformed(java.awt.event.ActionEvent evt) {
		mapView.undoLast();
	}
	
	/**
	 * Action handler for overlay map-button, fetches the image name and asks
	 * the mapView-object to load it on the screen.
	 *
	 * @param evt a <code>java.awt.event.ActionEvent</code> value
	 */
	private void imageBActionPerformed(java.awt.event.ActionEvent evt) {
		int returnVal = fc.showOpenDialog(this);
		if (returnVal == JFileChooser.APPROVE_OPTION) {
			//System.out.println("Loading " + fc.getSelectedFile().getPath());			
			mapView.loadImage(fc.getSelectedFile().getPath());
		}
	}
	
	/**
	 * Action handler for clear map-button, clears the background image
	 *
	 * @param evt a <code>java.awt.event.ActionEvent</code> value
	 */
	private void clearBActionPerformed(java.awt.event.ActionEvent evt) {
		mapView.clearImage();
	}
	
	/**
	 * Action handler for the apply-button, takes the (geographical)
	 * coordinates entered and if they are valid loads them as the
	 * current viewport. On successful completion, the current
	 * coordinates are deleted (due to the ambiguity in how to handle
	 * them after the scale change.)
	 *
	 * @param evt a <code>java.awt.event.ActionEvent</code> value
	 */
	private void applyBActionPerformed(java.awt.event.ActionEvent evt) {
		String tops = topTf.getText().toUpperCase();
		String lefts = leftTf.getText().toUpperCase();
		String bots = botTf.getText().toUpperCase();
		String rights = rightTf.getText().toUpperCase();
		
		try {
			double topd, leftd, botd, rightd;
			if (tops.endsWith("N") || (tops.endsWith("S"))) {
				topd = Double.parseDouble(tops.substring(0, tops.length() - 1));
				if (tops.endsWith("S"))
					topd *= -1;
			} else {
				topd = Double.parseDouble(tops);
				tops = (tops.startsWith("-") ? tops.substring(1) + "S" : tops + "N");
			}
			
			
			if (lefts.endsWith("E") || (lefts.endsWith("W"))) {
				leftd = Double.parseDouble(lefts.substring(0, lefts.length() - 1));
				if (lefts.endsWith("W"))
					leftd *= -1;
			} else {
				leftd = Double.parseDouble(lefts);
				lefts = (lefts.startsWith("-") ? lefts.substring(1) + "W" : lefts + "E");
			}
			
			
			if (bots.endsWith("N") || (bots.endsWith("S"))) {
				botd = Double.parseDouble(bots.substring(0, bots.length() - 1));
				if (bots.endsWith("S"))
					botd *= -1;
			} else {
				botd = Double.parseDouble(bots);
				bots = (bots.startsWith("-") ? bots.substring(1) + "S" : bots + "N");
			}
			
			
			if (rights.endsWith("E") || (rights.endsWith("W"))) {
				rightd = Double.parseDouble(rights.substring(0, rights.length() - 1));
				if (rights.endsWith("W"))
					rightd *= -1;
			} else {
				rightd = Double.parseDouble(rights);
				rights = (rights.startsWith("-") ? rights.substring(1) + "W" : rights + "E");
			}
			
			topLeft = "Top left: " + tops + "; " + lefts;
			botRight = "Bottom right: " + bots + "; " + rights;
			
			coordTL.setText(topLeft);
			coordBR.setText(botRight);
			
			//now actually update the model of the view
			mapView.setScale(topd, leftd, botd, rightd);
			
		} catch (NumberFormatException e) {
			//notify user
			JOptionPane.showMessageDialog(this,
					"Bad coordinate format; use eg. 23.56N, 14.3E, -123.4",
					"Error",
					JOptionPane.ERROR_MESSAGE);
			return;
		}
	}
	
	/**
	 * Handler for updating the view when the model
	 * (<code>PositionData</code>) changes.
	 *
	 * @param o a <code>java.util.Observable</code> value
	 * @param arg an <code>Object</code> value
	 */
	public void update(java.util.Observable o, Object arg) {
		//update the canvas if we get some interesting coordinate
		
		if (arg instanceof double[]) { // Single coord updated
			double[] argf = (double[]) arg;
			double[] nCoords = new double[2];
			nCoords[0] = argf[1];
			nCoords[1] = argf[2];
			
			if (argf[0] < 0) { //update
				mapView.updateLast(nCoords);
			} else if (argf[0] > 0) { //add
				mapView.addPoint(nCoords);
			}
		} else { // All coords updated
			//mapView.updateAll();
		}
	}
}
