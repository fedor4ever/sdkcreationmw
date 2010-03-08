/*
* Copyright (c) 2003 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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

package com.symbian.utils;

import java.util.*;

import java.io.*;
import java.net.*;
import java.awt.*;

import com.symbian.io.StreamsPiper;

/**
 * Static utilites
 *
 * <p>Copyright: Copyright (c) 2003 By Symbian Ltd. All Rights Reserved.</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 1.1 (CLDC-HI)
 */
public class Utilities
{


  //
  // Constants
  //

  /**
   * Max number of port tries that is reasonbly big enough to indicate a problem
   */
  private static final int MAX_PORT_ATTEMPTS = 50;


  //
  // Validate class name according to Java Language Specification
  //


  /**
   * Array of Java keywords which cannot be a part of a valid class name
   */
  public static final String[] JAVA_KEYWORDS_STRINGS =
      new String[] {
      "abstract", "default", "if", "private", "this",
      "boolean", "do", "implements", "protected", "throw",
      "break", "double", "import", "public", "throws",
      "byte", "else", "instanceof", "return", "transient",
      "case", "extends", "int", "short", "try",
      "catch", "final", "interface", "static", "void",
      "char", "finally", "long", "strictfp", "volatile",
      "class", "float", "native", "super", "while",
      "const", "for", "new", "switch",
      "continue", "goto", "package", "synchronized"};

  /**
   * Set of Java keywords which cannot be a part of a valid class name
   */
  public static final Set JAVA_KEYWORDS =
      new HashSet(Arrays.asList(JAVA_KEYWORDS_STRINGS));


  /**
   * Attempt to validate a given string as a class name
   *
   * @param name possible class name
   * @return true if this name can be a valid class name
   */
  public static boolean isValidClassName(String aName)
  {
    char[] nameChars = aName.toCharArray();
    if (!Character.isJavaIdentifierStart(nameChars[0]))
    {
      return false;
    }
    boolean identifierStarted = true;
    boolean dotFound = false;
    int identifierStart = 0;
    char currentChar;
    for (int i = 1; i < nameChars.length; i++)
    {
      currentChar = nameChars[i];
      if (currentChar == '.')
      {
        if (dotFound)
        {
          //2 dots together
          return false;
        }
        else
        {
          dotFound = true;
          identifierStarted = false;
          if (i == nameChars.length - 1
              ||
              JAVA_KEYWORDS.contains(aName.substring(identifierStart, i)))
          {
            //last char is dot OR name is a keyword
            return false;
          }
        }
      }
      else
      {
        if (identifierStarted)
        {
          //inside word
          if (!Character.isJavaIdentifierPart(currentChar))
          {
            return false;
          }
        }
        else
        {
          //beginning of a new word
          if (!Character.isJavaIdentifierStart(currentChar))
          {
            return false;
          }
          identifierStart = i;
          identifierStarted = true;
          dotFound = false;
        }
      }
    }
    return (! JAVA_KEYWORDS.contains(aName.substring(identifierStart)));
  }




  //
  // Copy files
  //



  /**
   * Utility method to create a new copy of the file
   *
   * @param aSrc source file
   * @param aDest output file
   *
   * @throws IOException if some error occured
   */
  public static void copyFiles(String aSrc, String aDest) throws IOException
  {
    copyToFile(new FileInputStream(aSrc), aDest);
  }

  /**
   * Utility method to create a new file
   *
   * @param aIs input stream
   * @param aDest output file
   *
   * @throws IOException if some error occured
   */
  public static void copyToFile(InputStream aIs, String aDest) throws IOException
  {
    StreamsPiper piper = new StreamsPiper(aIs,  new FileOutputStream(aDest));
    piper.pipeAll(true);
  }

  //
  // Find free port
  //

  /**
   * Find an available free port on the given host.
   *
   * @param aAddress host address
   * @param aMin min port number
   * @param aMax max port number
   *
   * @return port number
   *
   * @exception ConnectException if all ports are taken
   * @exception IOException if some error occured
   */
  public static int getFreePort(InetAddress aAddress, int aMin, int aMax) throws IOException
  {
    Socket s = null;
    int port = aMin;
    boolean notFound = true;
    while(notFound && port <= aMax)
    {
      try
      {
        s = new Socket(aAddress, port);
        Debug.println(Utilities.class, "port " + port + " is occupied");
        try {
          s.close();
        }catch (Exception e) {}
        port++;
      }
      catch (ConnectException ce)
      {
        notFound = false;
      }
    }
    if(notFound){
      throw new ConnectException
          ("ports " + aMin + ".." + aMax + " are occupied on host " + aAddress.getHostName());
    }
    Debug.println(Utilities.class, "Found free port: " + port);
    return port;
  }


  /**
   * Find an available free port on the given host.
   *
   * @param aAddress host address
   * @param aMin min port number
   * @param aMax max port number
   *
   * @return port number
   *
   * @exception ConnectException if all ports are taken
   * @exception IOException if some error occured
   */
  public static int getFreePort(InetAddress aAddress, int aMin) throws IOException
  {
    return getFreePort(aAddress, aMin, aMin + MAX_PORT_ATTEMPTS);
  }

  /**
   * Get a free port on the local host
   *
   * @return port
   *
   * @throws IOException if some error occured
   */
  public static int getFreePort() throws IOException{
    ServerSocket s = null;
    try{
      s = new ServerSocket(0);
      return s.getLocalPort();
    }
    finally{
      s.close();
    }
  }

  /**
   * Locate the window in the center of the screen
   *
   * @param aWindow window
   */
  public static void centerWindow(Window aWindow){
    Dimension d = Toolkit.getDefaultToolkit().getScreenSize();
    int x = (int)(d.getWidth() - aWindow.getWidth() / 2);
    aWindow.setLocation((int)(d.getWidth() / 2 - aWindow.getWidth() / 2),
                        (int)(d.getHeight() / 2 - aWindow.getHeight() / 2));
  }

}
