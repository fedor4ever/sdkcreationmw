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



package com.nokia.wtk.util;

/* java.io */
import java.io.File;
import java.io.Serializable;

/* java.util */
import java.util.List;
import java.util.Vector;
import java.util.Iterator;

/**
 * A convenience implementation of FileFilter that filters out all files
 * except for those type extensions that it knows about.
 *
 * Extensions are of the type ".ext" or "ext". Case is ignored.
 *
 * <p>
 * Example - create a new filter that filters out all files but GIF and JPEG
 * image files:
 *
 * <pre>
 *   JFileChooser chooser = new JFileChooser();
 *   FileFilter filter = new FileFilter(new String[] {"gif","jpg"},"Images")
 *   chooser.addChoosableFileFilter(filter);
 *   chooser.showOpenDialog(this);
 * </pre>
 *
 *
 */
public
class       FileFilter
extends     javax.swing.filechooser.FileFilter
implements  java.io.FileFilter, Serializable {

    private static final int DEFAULT_DESCRIPTION_LIMIT = 30;

    private List extensions;
    private String description;
    private String fullDescription;
    private boolean useExtensionsInDescription = true;
    private int descriptionLimit = DEFAULT_DESCRIPTION_LIMIT;

    /**
     * Creates a file filter. If no filters are added, only directories
     * are accepted.
     *
     * @see #addExtension addExtension
     */
    public FileFilter() {
        this((String) null, (String) null);
    }

    /**
     * Creates a file filter that accepts files with the given extension.
     * Example:
     * <pre>
     *   new FileFilter("jpg");
     * </pre>
     *
     * @param extension the file extension
     * @see #addExtension addExtension
     */
    public FileFilter(String extension) {
        this(extension,null);
    }

    /**
     * Creates a file filter that accepts the given file type.
     * Example:
     * <pre>
     *   new FileFilter("jpg", "JPEG Image Images");
     * </pre>
     * Note that the "." before the extension is not needed. If
     * provided, it will be ignored.
     *
     * @param extension the file extension
     * @param description the file description
     * @see #addExtension addExtension
     */
    public FileFilter(String extension, String description) {
        this(new String[] {extension}, description);
    }

    /**
     * Creates a file filter from the given string array.
     * Example:
     * <pre>
     *   new FileFilter(new String[] {"gif", "jpg"});
     * </pre>
     * Note that the "." before the extension is not needed and
     * will be ignored.
     *
     * @param extensions the file extensions
     * @see #addExtension addExtension
     */
    public FileFilter(String[] extensions) {
        this(extensions, null);
    }

    /**
     * Creates a file filter from the given string array and description.
     * Example:
     * <pre>
     *   new FileFilter(new String[] {"gif", "jpg"}, "GIF and JPEG Images");
     * </pre>
     * Note that the "." before the extension is not needed and will be
     * ignored.
     *
     * @param extensions the file extensions
     * @param description the file description
     * @see #addExtension addExtension
     */
    public FileFilter(String[] extensions, String description) {
        for (int i = 0; i < extensions.length; i++) {
            // add filters one by one
            addExtension(extensions[i]);
        }
        setDescription(description);
    }

    /**
     * Accepts all directories and only files with supported extensions.
     * Files that begin with "." are ignored.
     *
     * @return <code>true</code> if this file should be shown in the directory
     * pane, <code>false</code> if it shouldn't.
     *
     * @param f the file to test
     * @see #isSupportedExtension isSupportedExtension
     * @see java.io.FileFilter#accept
     */
    public boolean accept(File f) {
        if (f != null) {
            if (f.isDirectory()) {
                return true;
            }
            return isSupportedExtension(getFileExtension(f));
        }
        return false;
    }

    /**
     * Checks whether given extension is one of the extensions supported
     * by this file filter.
     *
     * @param ext the file extension to test
     * @return <code>true</code> if this extension is supported,
     *         <code>false</code> if it's not
     */
    public boolean isSupportedExtension(String ext) {
        if (ext != null && ext.length() > 0 && extensions != null) {
            String s = ext;
            if (s.charAt(0) == '.') s = s.substring(1);
            return extensions.contains(s.toLowerCase());
        }
        return false;
    }

    /**
     * Returns the extension portion of the file's name.
     *
     * @param f the file to get the extension for
     * @return the file extension without leading "." converted to lower case
     */
    public static String getFileExtension(File f) {
        if (f != null) {
            return getFileExtension(f.getName());
        }
        return null;
    }

    /**
     * Returns the extension portion of the file's name.
     *
     * @param filename the filename to get the extension for
     * @return the file extension without leading "." converted to lower case
     */
    public static String getFileExtension(String filename) {
        if (filename != null) {
            int i = filename.lastIndexOf('.');
            if (i >= 0) {
                String s = filename.substring(i+1).trim();
                if (s.length() > 0) {
                    return s.toLowerCase();
                }
            }
        }
        return null;
    }

    /**
     * Returns the default extension associated with this filter
     * @return the default extension associated with this filter
     */
    public String getExtension() {
        if (extensions != null) {
            return (String)extensions.get(0);
        }
        return null;
    }

    /**
     * Returns all extensions associated with this filter
     * @return all extensions associated with this filter
     */
    public String [] getExtensions() {
        if (extensions != null) {
            return (String[])extensions.toArray(new String[extensions.size()]);
        }
        return new String[0];
    }

    /**
     * Adds a filetype "dot" extension to filter against.
     * For example: the following code will create a filter that filters
     * out all files except those that end in ".jpg" and ".tif":
     * <pre>
     *   FileFilter filter = new FileFilter();
     *   filter.addExtension("jpg");
     *   filter.addExtension("tif");
     * </pre>
     * Note that the "." before the extension is not needed and will be
     * ignored.
     *
     * @param extension the extension to add
     */
    public void addExtension(String extension) {
        if (extensions == null) {
            extensions = new Vector();
        }
        if (extension.charAt(0) == '.') {
            extension = extension.substring(1);
        }
        extensions.add(extension.toLowerCase());
        fullDescription = null;
    }

    /**
     * Gets the raw description as was given to the setDescription call.
     * @return the raw file description
     */
    public String getRawDescription() {
        return description;
    }

    /**
     * Returns the human readable description of this filter.
     * For example: "JPEG and GIF Image Files (*.jpg, *.gif)"
     *
     * Note: Extension description is being truncated because FileChooser goes
     * haywire otherwise.
     *
     * @return the human readable description of this filter.
     * @see #setDescription setDescription
     * @see #setExtensionListInDescription setExtensionListInDescription
     * @see #isExtensionListInDescription isExtensionListInDescription
     * @see javax.swing.filechooser.FileFilter#getDescription
     */
    public String getDescription() {
        if (fullDescription == null) {
            String rawDescription = getRawDescription();
            if (rawDescription == null || isExtensionListInDescription()) {
                int n = Math.max(DEFAULT_DESCRIPTION_LIMIT, descriptionLimit);
                StringBuffer sb = new StringBuffer(n);
                if (rawDescription != null) sb.append(rawDescription);
                sb.append(" (");
                // build the description from the extension list
                if (extensions != null) {
                    // if it's not null, then we know it's not empty
                    boolean truncated = false;
                    Iterator i = extensions.iterator();
                    sb.append("*.").append(i.next());
                    while (i.hasNext() && !truncated) {
                        String ext = (String)i.next();
                        if (descriptionLimit <= 0 ||
                            descriptionLimit > (sb.length()+ext.length()+3)) {
                            sb.append(", *.").append(ext);
                        } else {
                            truncated = true;
                        }
                    }
                    if (truncated) sb.append(", ... ");
                }
                sb.append(")");
                fullDescription = sb.toString();
            } else {
                fullDescription = rawDescription;
            }
        }
        return fullDescription;
    }

    /**
     * Sets the human readable description of this filter. For example:
     * <pre>
     *   filter.setDescription("Gif and JPG Images");
     * </pre>
     *
     * @param description the file description
     * @see #setExtensionListInDescription setExtensionListInDescription
     * @see #isExtensionListInDescription isExtensionListInDescription
     */
    public void setDescription(String description) {
        this.description = description;
        fullDescription = null;
    }

    /**
     * Determines whether the extension list (.jpg, .gif, etc) should
     * show up in the human readable description.
     *
     * Only relevent if a description was provided in the constructor
     * or using the {@link #setDescription(String) setDescription} method.
     *
     * @param b <code>true</code> to prevent {@link #getDescription} from
     * appending extension list to the raw description of the file,
     * <code>false</code> otherwise.
     *
     * @see #getDescription getDescription
     * @see #setDescription setDescription
     * @see #isExtensionListInDescription isExtensionListInDescription
     */
    public void setExtensionListInDescription(boolean b) {
        useExtensionsInDescription = b;
        fullDescription = null;
    }

    /**
     * Returns whether the extension list (.jpg, .gif, etc) should
     * show up in the human readable description.
     *
     * Only relevent if a description was provided in the constructor
     * or using the {@link #setDescription(String) setDescription} method.
     *
     * @return <code>true</code> if {@link #getDescription} appends
     * extension list to the raw description of the file,  <code>false</code>
     * otherwise.
     *
     * @see #getDescription getDescription
     * @see #setDescription setDescription
     * @see #setExtensionListInDescription setExtensionListInDescription
     */
    public boolean isExtensionListInDescription() {
        return useExtensionsInDescription;
    }

    /**
     * Sets the maximum length of the description string. If the description
     * string is longer that the limit, the list of file extensions (but not
     * the description string itself) gets truncated. At least on file
     * extension is always shown.
     *
     * @param limit the description string limit, <code>&lt;=0</code> if
     *   there's no limit.
     */
    public void setDescriptionLimit(int limit) {
        descriptionLimit = Math.max(limit, 0);
    }

    /**
     * Gets the description string limit. If this parameter is greater than
     * zero, then the list of file extensions (but not the description string
     * itself) gets truncated if it gets longer than this limit. If this
     * parameter is zero or negative, the description string is never
     * truncated.
     *
     * @return the description string limit, <code>0</code> if
     *   there's no limit.
     */
    public int getDescriptionLimit() {
        return descriptionLimit;
    }
}
