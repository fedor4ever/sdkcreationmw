/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of the License "Eclipse Public License v1.0"
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
package com.nokia.epdt.plugins.s60.pan;

import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.LineNumberReader;
import java.util.Enumeration;
import java.util.LinkedList;
import java.util.Properties;

/**
 * 
 * Class PropSets Contains a number of property sets - Sections, each Section is
 * titled: [title1] property11= value11 property12= value12
 * 
 * [titleN] propertyNK= valueNK
 * 
 */
public class PropSets {
	LinkedList sections = new LinkedList();

	private class Section {
		private String title = new String();
		private Properties props = new Properties();

		public Properties getProps() {
			return props;
		}

		public void setTitle(String title) {
			this.title = title;
		}

		public String getTitle() {
			return title;
		}

		public String getValue(String key) {
			return (String) props.get(key);
		}

		public void setValue(String key, String value) {
			props.put(key, value);
		}

		/**
		 * Function for saving a section into FileOutputStream
		 * 
		 * @param writer
		 * @throws IOException
		 */
		public void save(FileWriter writer) throws IOException {
			String newLine = System.getProperty("line.separator");
			writer.write(title + newLine);
			// props.store(writer, null);
			Enumeration keys = props.keys();
			while (keys.hasMoreElements()) {
				String key = (String) keys.nextElement();
				writer.write(key + "= " + props.getProperty(key) + newLine);
			}
			writer.write(newLine);
		}
	}

	/**
	 * Loading sections from file
	 */
	public void load(FileReader reader) throws IOException {
		LineNumberReader lineReader = new LineNumberReader(reader);
		Section currentSection = null;
		String currentLine;

		while ((currentLine = lineReader.readLine()) != null) {
			if (currentLine.startsWith("[") && currentLine.endsWith("]")) {
				// Found a title, i.e. new section set the new section
				// to be current, the old one will be garbage-collected
				currentSection = new Section();
				currentSection.setTitle(currentLine);
				sections.add(currentSection);
			} else// Found a property
			{
				if (currentSection != null) {
					// if there is no current section, we didn't find a
					// title first therefore we skip the property
					String[] propPair;// = new String[2];
					propPair = currentLine.toString().split("= ", 2);
					if (propPair.length == 2) {
						// If we split successfully, add key-value pair,
						// if not - skip the line
						currentSection.getProps().put(propPair[0], propPair[1]);
					}
				}
			}
		}
	}

	/**
	 * Saving Sections to a file
	 * 
	 */
	public void save(FileWriter writer) throws IOException {
		for (int i = 0; i < sections.size(); i++) {
			((Section) sections.get(i)).save(writer);
		}
		writer.flush();
	}

	/**
	 * Function returns a property value, specified by section name and key
	 * 
	 * @param sectionName
	 * @param key
	 * @return
	 */
	public String getValue(String sectionName, String key) {
		for (int i = 0; i < sections.size(); i++) {
			if (sectionName.equals(((Section) sections.get(i)).getTitle())) {
				// Found specified section
				return ((Section) sections.get(i)).getValue(key);
			}
		}
		return null;
	}

	/**
	 * Function sets a property value, specified by section name and key
	 * 
	 * @param sectionName
	 * @param key
	 * @return 0=success, -1 = error
	 */
	public int setValue(String sectionName, String key, String value) {
		for (int i = 0; i < sections.size(); i++) {
			if (sectionName.equals(((Section) sections.get(i)).getTitle())) {
				// Found specified section
				((Section) sections.get(i)).setValue(key, value);
				return 0;
			}
		}
		return -1;
	}

}