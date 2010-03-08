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

/**
 * An extension to JPanel which presents a GUI to manipulate the
 * coordinate values textually.
 *
 * @version 1.0
 */
public class CoordPanel extends JPanel implements Observer {

    PositionData posData;
    JButton addB, applyB;
    JRadioButton detButton, randButton;
    SpinnerNumberModel model;
    JSpinner spinner;


    /**
     * Creates the panel and lays out the components
     *
     */
    public CoordPanel() {
	super();

	this.setLayout(new SpringLayout());

	this.setBorder(BorderFactory.createCompoundBorder(
							  BorderFactory.createTitledBorder("Coordinate"), 
							  BorderFactory.createEmptyBorder(4,4,4,4)));

	String[] labels = {"Horizontal accuracy (m)", "Vertical accuracy (m)", "TimeTofix min. (s)", "TimeToFix max. (s)",
			   "Powerup (s)", "Longitude", "Latitude", "Speed (m/s)", "Course"};

	for (int i = 0; i < labels.length; i++) {
	    JLabel l = new JLabel(labels[i], JLabel.TRAILING);
	    this.add(l);
	    JTextField textField = new JTextField(10);
	    l.setLabelFor(textField);
	    this.add(textField);
	}

	JLabel l = new JLabel("Randomness", JLabel.TRAILING);
	this.add(l);
	model = new SpinnerNumberModel(0, 0, 1000, 1);
	spinner = new JSpinner(model);
	l.setLabelFor(spinner);
	this.add(spinner);

	detButton = new JRadioButton("Deterministic");
	randButton = new JRadioButton("Random");

	ButtonGroup group = new ButtonGroup();
	group.add(detButton);
	group.add(randButton);
                
	this.add(detButton);
	this.add(randButton);


	applyB = new JButton("Apply");
	applyB.addActionListener(new java.awt.event.ActionListener() {
		public void actionPerformed(java.awt.event.ActionEvent evt) {
		    applyBActionPerformed(evt);
		}
	    });
	this.add(applyB);

	addB = new JButton("Add");
	addB.addActionListener(new java.awt.event.ActionListener() {
		public void actionPerformed(java.awt.event.ActionEvent evt) {
		    addBActionPerformed(evt);
		}
	    });
	this.add(addB);

	// put the components on a grid
	makeCompactGrid(this,
			labels.length+3, 2, // rows, cols
			6, 6,               // initial x, initial y
			6, 6);              // x padding, y padding

	posData = PositionData.getInstance();
	posData.addObserver(this);
    }


    /**
     * Reads the textfields and creates a <code>Coordinate</code>
     * -object based on them. It is important to note that the
     * values are read in the same order as the fields are laid
     * out (see the labels-variable in the constructor.)
     *
     * @return the <code>Coordinate</code> value that the
     * textfields currently contain
     */
    private Coordinate readCoordinate() {
	try {
	    boolean det = this.detButton.getSelectedObjects() == null ? false : true;
	    Coordinate c = new Coordinate(Float.parseFloat(((JTextField) this.getComponent(1)).getText()),
					  Float.parseFloat(((JTextField) this.getComponent(3)).getText()),
					  Integer.parseInt(((JTextField) this.getComponent(5)).getText()),
					  Integer.parseInt(((JTextField) this.getComponent(7)).getText()),
					  Integer.parseInt(((JTextField) this.getComponent(9)).getText()),
					  Double.parseDouble(((JTextField) this.getComponent(11)).getText()),
					  Double.parseDouble(((JTextField) this.getComponent(13)).getText()),
					  Float.parseFloat(((JTextField) this.getComponent(15)).getText()),
					  Float.parseFloat(((JTextField) this.getComponent(17)).getText()),
					  model.getNumber().intValue(),
					  det);
	    return c;
	} catch (NumberFormatException e) {
	    return null;
	}
    }


    /**
     * Event handler for the Apply-button, changes the last
     * coordinate added to the data model
     *
     * @param evt a <code>java.awt.event.ActionEvent</code> value
     */
    private void applyBActionPerformed(java.awt.event.ActionEvent evt) {
	Coordinate c = readCoordinate();
	if (c != null)
	    posData.replaceCoordinate(c);
    }

    /**
     * Event handler for the Add-button, adds a coordinate to the
     * data model
     *
     * @param evt a <code>java.awt.event.ActionEvent</code> value
     */
    private void addBActionPerformed(java.awt.event.ActionEvent evt) {
	Coordinate c = readCoordinate();
	if (c != null)
	    posData.addCoordinate(c);
    }
 

    /**
     * Places the components in the given SpringLayout on a grid
     *
     * @param parent The parent Container
     * @param rows Rows in the grid
     * @param cols Columns in the grid
     * @param initialX Placement of the left edge of the grid
     * @param initialY Placement of the upper edge of the grid
     * @param xPad Padding in the x-direction
     * @param yPad Padding in the y-direction
     */
    private static void makeCompactGrid(Container parent,
					int rows, int cols,
					int initialX, int initialY,
					int xPad, int yPad) {
	SpringLayout layout = (SpringLayout)parent.getLayout();

	//Align all cells in each column and make them the same width.
	Spring x = Spring.constant(initialX);

	for (int c = 0; c < cols; c++) {
	    Spring width = Spring.constant(0);

	    // get max width for this column
	    for (int r = 0; r < rows; r++) {
		width = Spring.max(width, layout.getConstraints(parent.getComponent(r * cols + c)).getWidth());
	    }
	    for (int r = 0; r < rows; r++) {
		SpringLayout.Constraints constraints = layout.getConstraints(parent.getComponent(r * cols + c));
		constraints.setX(x);
		constraints.setWidth(width);
	    }
	    x = Spring.sum(x, Spring.sum(width, Spring.constant(xPad)));
	}

	//Align all cells in each row and make them the same height.
	Spring y = Spring.constant(initialY);
	for (int r = 0; r < rows; r++) {
	    Spring height = Spring.constant(0);
	    for (int c = 0; c < cols; c++) {
		height = Spring.max(height, layout.getConstraints(parent.getComponent(r * cols + c)).getHeight());
	    }
	    for (int c = 0; c < cols; c++) {
		SpringLayout.Constraints constraints = layout.getConstraints(parent.getComponent(r * cols + c));
		constraints.setY(y);
		constraints.setHeight(height);
	    }
	    y = Spring.sum(y, Spring.sum(height, Spring.constant(yPad)));
	}

	//Set the parent's size.
	SpringLayout.Constraints pCons = layout.getConstraints(parent);
	pCons.setConstraint(SpringLayout.SOUTH, y);
	pCons.setConstraint(SpringLayout.EAST, x);
    }


    /**
     * Updates the view to reflect the changes in the data model (never called directly,
     * only by notifying oberservers of the observable object)
     *
     * @param o a <code>java.util.Observable</code> value
     * @param arg an <code>Object</code> value
     */
    public void update(java.util.Observable o, Object arg) {
	if (arg instanceof Coordinate) {
	    ((JTextField) this.getComponent(1)).setText(Float.toString(((Coordinate) arg).getHorz()));
	    ((JTextField) this.getComponent(3)).setText(Float.toString(((Coordinate) arg).getVert()));
	    ((JTextField) this.getComponent(5)).setText(Integer.toString(((Coordinate) arg).getFixmin()));
	    ((JTextField) this.getComponent(7)).setText(Integer.toString(((Coordinate) arg).getFixmax()));
	    ((JTextField) this.getComponent(9)).setText(Integer.toString(((Coordinate) arg).getPowerup()));

	    // The string representation of the coordinate might be so long that it doesn't fit the JTextField without
	    // scrolling. This kind of precision serves no practical purpose, so the string is shortened to a maximum
	    // length of 13 chars
	    String lons = Double.toString(((Coordinate) arg).getLongitude());
	    String lats = Double.toString(((Coordinate) arg).getLatitude());
	    ((JTextField) this.getComponent(11)).setText(lons.substring(0,(13 > lons.length()? lons.length()-1:13)));
	    ((JTextField) this.getComponent(13)).setText(lats.substring(0,(13 > lats.length()? lats.length()-1:13)));

	    ((JTextField) this.getComponent(15)).setText(Float.toString(((Coordinate) arg).getSpeed()));
	    ((JTextField) this.getComponent(17)).setText(Float.toString(((Coordinate) arg).getCourse()));
	    spinner.setValue((Object) new Integer(((Coordinate) arg).getRandom()));
	    if (((Coordinate) arg).isDeterministic()) {
		this.detButton.setSelected(true);
		this.randButton.setSelected(false);
	    } else {
		this.detButton.setSelected(false);
		this.randButton.setSelected(true);
	    }
	} else {
	    // most likely arg is null, so clearing the values could be one option
	    //System.out.println("coordpanel: observed arg wasn't a Coordinate, " + arg);
	}
    }
}
