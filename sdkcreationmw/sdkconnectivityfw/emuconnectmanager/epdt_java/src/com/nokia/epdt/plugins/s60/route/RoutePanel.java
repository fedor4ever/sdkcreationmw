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
import javax.swing.*;
import javax.swing.filechooser.*;

import java.util.*;
import java.io.File;

/**
 * Container panel for all visual objects in the Route plugin.
 *
 * @version 1.0
 */
public class RoutePanel extends JPanel {
	
	public static final String CW_PATH = "../../winscw/c/documents/simpsydata";
	public static final String VC_PATH = "../../wins/c/documents/simpsydata";
	
	//private JPanel plotPanel, coordPanel, filePanel;
	private PlotPanel plotPanel;
	private CoordPanel coordPanel;
	private JPanel filePanel;
	
	//private JButton saveNmeaB, saveSpsB;
	private JButton saveB, loadB, clearB;
	
	private JFileChooser fc;
	private RoutePlugin route;
	
	private class LocFilter extends FileFilter {
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
			
			if (ext.equals("nme") || ext.equals("sps"))
				return true;
			else
				return false;
		}
		
		public String getDescription() {
			return "Location files (*.nme, *.sps)";
		}
	}
	
	
	public RoutePanel(RoutePlugin route) {
		this.route = route;
		
		coordPanel = new CoordPanel();
		plotPanel = new PlotPanel();
		
		filePanel = new JPanel();
		
		//Add various widgets to the sub panels.
		addWidgets();
		
		//Create the main panel to contain the two sub panels.
		//this = new JPanel();
		this.setLayout(new GridBagLayout());
		GridBagConstraints c = new GridBagConstraints();
		
		
		if ((new File(CW_PATH)).exists()) {
			fc = new JFileChooser(CW_PATH);
			fc.addChoosableFileFilter(new LocFilter());
		} else if ((new File(VC_PATH)).exists()) {
			fc = new JFileChooser(VC_PATH);
			fc.addChoosableFileFilter(new LocFilter());
		} else {
			fc = new JFileChooser(".");
			fc.addChoosableFileFilter(new LocFilter());
		}
		
		this.setBorder(BorderFactory.createEmptyBorder(5,5,5,5));
		
		c.fill = GridBagConstraints.BOTH;
		
		c.gridheight = GridBagConstraints.RELATIVE;;
		this.add(plotPanel, c);
		
		//c.gridheight = 2;
		//c.gridheight = GridBagConstraints.REMAINDER;
		c.gridwidth = GridBagConstraints.REMAINDER; //end row
		this.add(coordPanel, c);
		
		//c.gridwidth = 2;
		//c.gridheight = 1;
		c.gridwidth = GridBagConstraints.RELATIVE;;
		c.gridheight = GridBagConstraints.REMAINDER;
		this.add(filePanel, c);
		
		//this.setSize(300, 100);
		//this.setPreferredSize(new java.awt.Dimension(690, 600));
	}
	
	
	/**
	 * Adds the buttons to the panel
	 *
	 */
	private void addWidgets() {
		saveB = new JButton("Save");
		saveB.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				saveActionPerformed(evt);
			}
		});
		filePanel.add(saveB);
		
		clearB = new JButton("Clear");
		clearB.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				clearActionPerformed(evt);
			}
		});
		filePanel.add(clearB);
		
		
	}
	
	/**
	 * Action handler for the reset-button, removes all coordinates.
	 *
	 * @param evt a <code>java.awt.event.ActionEvent</code> value
	 */
	private void clearActionPerformed(java.awt.event.ActionEvent evt) {
		PositionData.getInstance().rmAll();
		clearRoutePanel(); // Clear Route Panel
	}
	
	// Clear Route panel once user click clear button
	private void clearRoutePanel(){
		// Remove plot panel components 
		plotPanel.mapView.clearAll();
		plotPanel.botTf.setText("");
		plotPanel.topTf.setText("");
		plotPanel.leftTf.setText("");
		plotPanel.rightTf.setText("");
		
		// Remove coord panel components
		((JTextField) coordPanel.getComponent(1)).setText("");
	    ((JTextField) coordPanel.getComponent(3)).setText("");
	    ((JTextField) coordPanel.getComponent(5)).setText("");
	    ((JTextField) coordPanel.getComponent(7)).setText("");
	    ((JTextField) coordPanel.getComponent(9)).setText("");
	    ((JTextField) coordPanel.getComponent(11)).setText("");
	    ((JTextField) coordPanel.getComponent(13)).setText("");
	    ((JTextField) coordPanel.getComponent(15)).setText("");
	    ((JTextField) coordPanel.getComponent(17)).setText("");
	    coordPanel.spinner.setValue((Object) new Integer("0"));
	    
	    coordPanel.detButton.setSelected(false);
	    coordPanel.randButton.setSelected(false);
	    
	}
	
	private void saveActionPerformed(java.awt.event.ActionEvent evt) {
		int returnVal = fc.showSaveDialog(this);
		if (returnVal == JFileChooser.APPROVE_OPTION) {
			File file = fc.getSelectedFile();
			String fname = file.getName();
			int idx = fname.lastIndexOf('.');
			if (idx == -1) {
				JOptionPane.showMessageDialog(this,
						"The file suffix must be either .nme or .sps (SimPSY searches files by suffix)",
						"Error",
						JOptionPane.ERROR_MESSAGE);
				return;
			}
			fname = fname.substring(idx);
			boolean nmea;
			if (".nme".equals(fname))
				nmea = true;
			else if (".sps".equals(fname))
				nmea = false;
			else {
				JOptionPane.showMessageDialog(this,
						"The file suffix must be either .nme or .sps (SimPSY searches files by suffix)",
						"Error",
						JOptionPane.ERROR_MESSAGE);
				return;
			}
			
			try {
				CoordWriter cw = new CoordWriter(file);
				cw.write(PositionData.getInstance().getCoords(), nmea);
				PositionData.getInstance().setFilename(file.getName());
				route.pluginModelChangedAction();
			} catch (Exception e) {
				JOptionPane.showMessageDialog(this,
						"File write failed, check that the path and permissions are correct\n"
						+ e.getMessage(),
						"Error",
						JOptionPane.ERROR_MESSAGE);
			}
		}
	}
}
