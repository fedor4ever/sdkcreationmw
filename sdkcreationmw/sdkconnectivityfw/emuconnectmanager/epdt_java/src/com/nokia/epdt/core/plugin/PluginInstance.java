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


package com.nokia.epdt.core.plugin;

/* java.io */
import java.io.File;
import java.io.FilenameFilter;
import java.io.IOException;
import java.net.JarURLConnection;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.ArrayList;
import java.util.List;
import java.util.ResourceBundle;
import java.util.jar.Manifest;

import org.apache.log4j.Logger;

/* com.nokia.epdt.core */
import com.nokia.epdt.core.Constants;
import com.nokia.epdt.core.plugin.Plugin;

/**
 * This object represents a Ecmt plugin loaded from a plugin jar.
 *
 */
public class PluginInstance implements Constants {
    private static Logger log = Logger.getLogger(PluginInstance.class);

    //private PluginJar pluginJar;
    private Plugin plugin;

    public PluginInstance(File file, ResourceBundle bundle) throws InstantiationException,
        IllegalAccessException, ClassCastException, IOException {
        jarFile = file;

        this.bundle = bundle;
        fileUrl = jarFile.toURL();
        Class pluginClass = getPluginClass();
        if (pluginClass == null) {
            throw new InstantiationException(getAbsolutePath());
        }
        plugin = (Plugin)pluginClass.newInstance();
    }

    private PluginInstance(/*PluginJar jar*/) throws InstantiationException,
        IllegalAccessException, ClassCastException {
        Class pluginClass = getPluginClass();
        if (pluginClass == null) {
            throw new InstantiationException(getAbsolutePath());
        }
        plugin = (Plugin)pluginClass.newInstance();
    }

    /*public PluginJar getPluginJar() {
        return pluginJar;
    }*/

    public Plugin getPlugin() {
        return plugin;
    }
    
    /**
     * Creates a plug-ins and set they in to the model.
     * @return all currently available plug-ins
     */
    public static PluginInstance [] createPlugins(String dir, String ext, ResourceBundle bundle) {
        List plugins = new ArrayList();
        File[] pluginFiles = listFiles(System.getProperty(Constants.PLUGIN_DIR, dir), ext);

        int l = (pluginFiles != null) ? pluginFiles.length : 0;
        for ( int i = 0; i < l; i++ ) {
            try {
                log.debug("Plug-in file path: " + pluginFiles[i]);
                plugins.add(new PluginInstance(pluginFiles[i], bundle));
            } catch (Throwable t) {
                log.error(t, t);
            }
        }

        // Log info
        log.info(plugins.size() + " plug-ins successfully created.");
        return (PluginInstance[])plugins.toArray(new PluginInstance[plugins.size()]);
    }
    
    /**
     * Returns an array of abstract pathnames denoting the plug-in files and directories
     * in the directory denoted by an abstract pathname.
     * @param pathname A pathname string.
     * @param name the name of the file.
     * @return An array of abstract pathnames denoting the plug-in files and directories
     *         in the directory denoted by this abstract pathname.
     *         The array will be empty if the directory is empty.
     *         Returns null if this abstract pathname does not denote a directory, or if an I/O error occurs.
     */
    public static File[] listFiles(String pathname, String n) {
        File file = null;
        final String name = n;
        try {
            file = new File(pathname);
            log.info("Plug-ins directory:" + file.getAbsolutePath());
        } catch (Exception e ) {
            log.error(e);
        }
        File[] files = file.listFiles(new FilenameFilter() {     	
        	public boolean accept(File dir, String n) {
                return n.endsWith(name);
        	}
        });

        if ( files != null ) {
            log.info(files.length + " plug-in files found.");
        } else {
            log.error("Pathname '" + pathname + "' does not denote a plugin directory, or an I/O error occurs.");
        }
        return files;
    }
    
	private ResourceBundle bundle;
    private String name;
    private URL fileUrl;
    private File jarFile;
    private Class pluginClass;
    private URLClassLoader classLoader;
    private Manifest manifest;

    /**
     * Creates PluginJar for the specified file
     * @param file the jar file
     * @throws IOException if anything goes wrong
     */
    /*public PluginJar(File file, ResourceBundle bundle) throws IOException {
        jarFile = file;
        this.bundle = bundle;
        fileUrl = jarFile.toURL();
    }*/

    /**
     * Returns the jar file name without extension
     * @return the jar file name without extension
     */
    public String getPluginName() {
        if (name == null) {
            String fullName = jarFile.getName();
            int pos = fullName.lastIndexOf('.');
            if (pos > 0) {
                name = fullName.substring(0,pos);
            } else {
                name = fullName;
            }
        }
        return name;
    }

    /**
     * Returns the jar file object
     * @return the jar file object
     */
    public File getJarFile() {
        return jarFile;
    }

    /**
     * Returns the file:// URL to the jat file
     * @return URL to the jar file
     */
    public URL getFileUrl() {
        return fileUrl;
    }

    /**
     * Retrurns the path to the jar file
     * @return the path to the jar file
     */
    public String getPath() {
        return jarFile.getPath();
    }

    /**
     * Retrurns the absolute path to the jar file
     * @return the absolute path to the jar file
     */
    public String getAbsolutePath() {
        return jarFile.getAbsolutePath();
    }

    
    
    /**
     * Returns the value of the specified main attribute from the manifest
     * file.
     *
     * @param name the attribute name
     * @return the attribute value, <code>null</code> if none
     */
    public String getAttribute(String name) {
        return readValue(name, fileUrl);
    }

    /**
     * Returns the value of the main attribute from the manifest file.
     * The name of the attribute is the resource value referenced by the
     * specified key.
     *
     * @param key the key for the attribute name
     * @return the attribute value, <code>null</code> if none
     */
    public String getAttributeByKey(String key) {
        return getAttribute(bundle.getString(key));
    }

    /**
     * Returns the class loader that loads classes from this jar
     * @return the class loader for this jar
     */
    public URLClassLoader getClassLoader() {
        if (classLoader == null) {
            classLoader = new URLClassLoader(new URL[] { fileUrl });
        }
        return classLoader;
    }

    /**
     * Returns the plugin class.
     * @return the plugin class, null if the jar file isn't a plugin.
     */
    public Class getPluginClass() {
        if (pluginClass == null) {
            String className = getPluginClassName();
            if (className != null) {
                try {
                    pluginClass = getClassLoader().loadClass(className);
                } catch (Throwable x) {
                    x.printStackTrace();
                }
            }
        }
        return pluginClass;
    }

    /**
     * Returns the plugin class name. Does not actually load the class.
     * @return the plugin class, null if the jar file isn't a plugin.
     */
    public String getPluginClassName() {
        String attr = bundle.getString(PLUGIN_MODULE_KEY);
        return getAttribute(attr);
    }

    /**
     * Loads a class from the plugin jar and creates a new instance of that
     * class. The class name is the value of the main manifest attribute
     * table, the name of the attribute is the resource value referenced
     * by the key. Ugh! I didn't invent this.
     *
     * @param key the class attrubute key
     * @return a new instance of the class (hopefully) from the jar,
     *   <code>null</code> on any error
     */
    public Object instantiateClassByKey(String key) {
        String attr = bundle.getString(key);
        String className = getAttribute(attr);
        if (className != null) {
            try {
                Class klass = getClassLoader().loadClass(className);
                return klass.newInstance();
            } catch (Throwable x) {
                x.printStackTrace();
            }
        }
        return null;
    }

    /**
     * Returns the value of the specified manifest attribute name,
     * specified as a string, or null if the attribute was not found.
     * The attribute name of is case-insensitive.
     * @param name - the attribute name as a string
     * @return the String value of the specified attribute name, or null if not found.
     */
    public String readValue(String name, URL url) {
        Manifest m = getManifest(url);
        if (m != null) {
            try {
                return manifest.getMainAttributes().getValue(name);
            } catch (Throwable t) {
                log.error(t);
            }
        }
        return null;
    }

    /**
     * Loads manifest from the jar.
     * @return the jar manifest, <code>null</code> on failure
     */
    private Manifest getManifest(URL url) {
        if (manifest == null) {
            try {
                URL jarUrl = new URL("jar", "", url + "!/");
                JarURLConnection c = (JarURLConnection)jarUrl.openConnection();
                manifest = c.getManifest();
            } catch (Throwable t) {
                log.error(t);
            }
        }
        return manifest;
    }

    
    /**
     * Returns a string representation of this object. This method
     * is intended to be used only for debugging purposes, and the
     * content and format of the returned string may vary between
     * implementations. The returned string may be empty but may not
     * be <code>null</code>.
     *
     * @return  a string representation of this object.
     */
    public String toString() {
        return getAbsolutePath();
    }
}
