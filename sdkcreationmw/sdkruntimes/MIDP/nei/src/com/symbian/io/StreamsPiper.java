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

package com.symbian.io;

import java.io.*;

/**
 * Utility for piping streams.
 *
 * Role: This is a utility class.
 *
 * <p>Copyright: Copyright (c) 2003 By Symbian Ltd. All Rights Reserved.</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 1.1 (CLDC-HI)
 */
public class StreamsPiper
{

  //
  // Constants
  //

  /**
   * Max buffer size
   */
  private static final int MAX_BUFFER_SIZE = 2048;

  /**
   * Medium buffer size
   */
  private static final int MED_BUFFER_SIZE = 1024;

  /**
   * Min buffer size
   */
  private static final int MIN_BUFFER_SIZE = 512;

  /**
   * Default number of output streams
   */
  private static final int DEF_OS_NUM = 2;



  //
  // Members
  //



  /**
   * Buffer
   */
  private byte[] iBuffer = new byte[MIN_BUFFER_SIZE];

  /**
   * Source input stream
   */
  private InputStream iInStream;

  /**
   * Destination output streams
   */
  private OutputStream[] iOutStreams = new OutputStream[DEF_OS_NUM];

  /**
   * Number of output streams
   */
  private int iOsNum = 0;



  //
  // Constructors
  //



  /**
   * Constructor
   *
   * @param aIn source InputStream
   * @param aOut destination OutputStream
   */
  public StreamsPiper(InputStream aIn, OutputStream aOut)
  {
    this(aIn);
    addOutputStream(aOut);
  }

  /**
   * Constructor
   *
   * @param aIn source InputStream
   */
  public StreamsPiper(InputStream aIn)
  {
    iInStream = aIn;
  }

  /**
   * Constructor
   */
  public StreamsPiper()
  {
  }



  //
  // Operations
  //




  /**
   * Set source input stream
   *
   * @param aIn InputStream
   */
  public void setInputStream(InputStream aIn)
  {
    iInStream = aIn;
  }

  /**
   * Set output stream and discards all previous output streams.
   *
   * @param aOS destination output stream
   */
  public void setOutputStream(OutputStream aOS)
  {
    iOsNum = 1;
    iOutStreams[0] = aOS;
  }

  /**
   * Add destination output stream
   *
   * @param os output stream
   */
  public void addOutputStream(OutputStream aOs)
  {
    if (iOsNum > iOutStreams.length)
    {
      OutputStream[] tmp = new OutputStream[iOsNum + DEF_OS_NUM];
      System.arraycopy(iOutStreams, 0, tmp, 0, iOsNum);
      iOutStreams = tmp;
    }
    iOutStreams[iOsNum++] = aOs;
  }

  /**
   * Pipe bytes from source to destination Output streams
   *
   * @param bytesNum number of bytes to pipe
   *
   * @exception IOException if some error occured
   */
  public void pipe(int aBytesNum) throws IOException
  {
    if (aBytesNum > iBuffer.length && iBuffer.length < MAX_BUFFER_SIZE)
    {
      iBuffer = new byte[aBytesNum > MED_BUFFER_SIZE ?
          MAX_BUFFER_SIZE : MED_BUFFER_SIZE];
    }
    int i;
    int read = 0;
    int totalRead = 0;
    while (totalRead < aBytesNum && read != -1)
    {
      read = iInStream.read(iBuffer,
                             0,
                             Math.min(iBuffer.length, aBytesNum - totalRead));
      totalRead += read;
      for (i = 0; read != -1 && i < iOsNum; i++)
      {
        iOutStreams[i].write(iBuffer, 0, read);
        iOutStreams[i].flush();
      }
    }
    if (totalRead < aBytesNum)
    {
      throw new IOException
          ("reached end of stream before reading " + aBytesNum + " bytes");
    }
  }

  /**
   * Pipe bytes to output streams
   *
   * @param bytes bytes array
   * @param offset start offset
   * @param length number of bytes to stream
   *
   * @exception IOException if some error occured
   */
  public void pipeBytes(byte[] aBytes, int aOffset, int aLength)
      throws IOException
  {
    for (int i = 0; i < iOsNum; i++)
    {
      iOutStreams[i].write(aBytes, aOffset, aLength);
      iOutStreams[i].flush();
    }
  }

  /**
   * Pipe bytes from input stream
   *
   * @param bytes bytes array
   * @param offset start offset
   * @param length number of bytes to stream
   *
   * @exception IOException if some error occured
   */
  public void readBytes(byte[] aBytes, int aOffset, int aLength)
      throws IOException
  {
    int read = 0;
    int totalRead = 0;
    while (totalRead < aLength && read != -1)
    {
      read = iInStream.read(aBytes, totalRead, aLength - totalRead);
      totalRead += read;
    }
    if (totalRead < aLength)
    {
      throw new IOException
          ("reached end of stream before reading " + aLength + " bytes");
    }
  }

  /**
   * Pipe all bytes till end of stream
   *
   * @exception IOException if some error occured
   */
  public void pipeAll() throws IOException
  {
    int i = 0;
    int read = 0;
    while (read != -1)
    {
      read = iInStream.read(iBuffer);
      for (i = 0; read != -1 && i < iOsNum; i++)
      {
        iOutStreams[i].write(iBuffer, 0, read);
        iOutStreams[i].flush();
      }
    }
  }

  /**
   * Pipe all bytes till end of stream
   *
   * @param aCloseStreams close streams on exit
   *
   * @exception IOException if some error occured
   */
  public void pipeAll(boolean aCloseStreams) throws IOException
  {
    try
    {
      pipeAll();
    }
    finally
    {
      if(aCloseStreams)
      {
        iInStream.close();
        for (int i = 0; i < iOsNum; i++)
        {
          iOutStreams[i].close();
        }
      }
    }

  }




}