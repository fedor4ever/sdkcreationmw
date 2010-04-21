/*
* Copyright (c) 2003 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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

package com.symbian.tools.j2me.sei.emulator.commands;


import java.util.*;

import com.symbian.vm.*;

import com.symbian.io.Dumpable;
import java.io.*;



/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2004 By Symbian Ltd. All Rights Reserved</p>
 * <p>Company: Symbian Ltd</p>
 * @author Roy Ben Hayun
 * @version 2.0
 */
public class VMLaunchArguments {

  //
  // Constant verbose types
  //

  /*
    These options are not supported by HotSpot.

    private static final String ALLOCATION_VERBOSE_TYPE = "-traceallocation";
    private static final String METHODS_VERBOSE_TYPE = "-tracemethods";
    private static final String DETAILED_METHODS_VERBOSE_TYPE = "-tracemethodsverbose";
       private static final String STACK_CHUNKS_VERBOSE_TYPE = "-tracestackchunks";
    private static final String STACK_FRAMES_VERBOSE_TYPE = "-traceframes";
    private static final String EVENTS_VERBOSE_TYPE = "-traceevents";
    private static final String MONITORS_VERBOSE_TYPE = "-tracemonitors";
    private static final String NETWORKING_VERBOSE_TYPE = "-tracenetworking";
   */

  private static final String ALL_VERBOSE_TYPE = "-traceall";
  private static final String THREADING_VERBOSE_TYPE = "+TraceThreadEvents";
  private static final String VERIFIER_VERBOSE_TYPE = "+TraceVerifier";
  private static final String STACKMAPS_VERBOSE_TYPE = "+TraceStackmaps";
  private static final String CLASS_VERBOSE_TYPE = "+TraceClassLoading";
  private static final String GC_VERBOSE_TYPE = "+TraceGC";
  private static final String DETAILED_GC_VERBOSE_TYPE = "+VerboseGC";
  private static final String DETAILED_CLASS_VERBOSE_TYPE = "+VerboseClassLoading";
  private static final String BYTE_CODE_VERBOSE_TYPE = "+TraceBytecodes";
  private static final String EXCEPTIONS_VERBOSE_TYPE = "+TraceExceptions";


  /**
   * Map of verbose types
   * SEI form -> KVM form
   */
  private static Map sVerboseArgsMap = new HashMap();



  //
  // Members
  //

  /**
   * VM arguments
   */
  private VMArguments iVMArguments = new VMArguments();

  /**
   * MIDlet information
   */
  private MIDletInfo iMIDletInfo;


  //
  // Life cycle
  //

  /**
   * Initializer
   */
  static
  {
    sVerboseArgsMap.put(SEICommand.ALL_VERBOSE_TYPE,
                        " " + ALL_VERBOSE_TYPE);
    sVerboseArgsMap.put(SEICommand.GC_VERBOSE_TYPE,
                         " " + GC_VERBOSE_TYPE);
    sVerboseArgsMap.put(SEICommand.DETAILED_GC_VERBOSE_TYPE,
                         " " + DETAILED_GC_VERBOSE_TYPE);
    sVerboseArgsMap.put(SEICommand.CLASS_VERBOSE_TYPE,
                         " " + CLASS_VERBOSE_TYPE);
    sVerboseArgsMap.put(SEICommand.DETAILED_CLASS_VERBOSE_TYPE,
                         " " + DETAILED_CLASS_VERBOSE_TYPE);
    sVerboseArgsMap.put(SEICommand.VERIFIER_VERBOSE_TYPE,
                         " " + VERIFIER_VERBOSE_TYPE);
    sVerboseArgsMap.put(SEICommand.BYTE_CODE_VERBOSE_TYPE,
                         " " + BYTE_CODE_VERBOSE_TYPE);
     sVerboseArgsMap.put(SEICommand.EXCEPTIONS_VERBOSE_TYPE,
                         " " + EXCEPTIONS_VERBOSE_TYPE);
     sVerboseArgsMap.put(SEICommand.THREADING_VERBOSE_TYPE,
                         " " + THREADING_VERBOSE_TYPE);

     /*
     These options are not supported by HotSpot.

    sVerboseArgsMap.put(SEICommand.STACKMAPS_VERBOSE_TYPE,
                         " " + STACKMAPS_VERBOSE_TYPE);
     sVerboseArgsMap.put(SEICommand.ALLOCATION_VERBOSE_TYPE,
                         " " + ALLOCATION_VERBOSE_TYPE);
     sVerboseArgsMap.put(SEICommand.METHODS_VERBOSE_TYPE,
                         " " + METHODS_VERBOSE_TYPE);
     sVerboseArgsMap.put(SEICommand.DETAILED_METHODS_VERBOSE_TYPE,
                         " " + DETAILED_METHODS_VERBOSE_TYPE);
     sVerboseArgsMap.put(SEICommand.STACK_FRAMES_VERBOSE_TYPE,
                         " " + STACK_FRAMES_VERBOSE_TYPE);
     sVerboseArgsMap.put(SEICommand.STACK_CHUNKS_VERBOSE_TYPE,
                         " " + STACK_CHUNKS_VERBOSE_TYPE);
     sVerboseArgsMap.put(SEICommand.EVENTS_VERBOSE_TYPE,
                         " " + EVENTS_VERBOSE_TYPE);
     sVerboseArgsMap.put(SEICommand.MONITORS_VERBOSE_TYPE,
                         " " + MONITORS_VERBOSE_TYPE);
     sVerboseArgsMap.put(SEICommand.NETWORKING_VERBOSE_TYPE,
                         " " + NETWORKING_VERBOSE_TYPE);
      */
  }


  /**
   * Constructor.
   * Initialize arguments using the SEICommand object
   *
   * @param aSEICommand SEI command
   */
  public VMLaunchArguments(SEICommand aSEICommand) {
    //verbose
    for(Iterator iter = aSEICommand.getVerboseTypes().iterator();iter.hasNext() ;)
    {
      iVMArguments.setVerboseType((String)sVerboseArgsMap.get(iter.next()));
    }

    //boostrap classes
    if(aSEICommand.getBootstrapClasses() != null){
      iVMArguments.setBootstrapClassPath(aSEICommand.getBootstrapClasses());
    }
  }



  //
  // Operations
  //


  /**
   * Get VM arguments
   *
   * @return VM arguments
   */
  public VMArguments getVMArguments(){
    return iVMArguments;
  }

  /**
   * Get MIDlet info
   *
   * @return MIDlet info
   */
  public MIDletInfo getMIDletInfo(){
    return iMIDletInfo;
  }

  /**
   * Set MIDlet info
   *
   * @param aInfo MIDlet info
   */
  public void setMIDletInfo(MIDletInfo aInfo){
    iMIDletInfo = aInfo;
  }

  //
  // Dumpable implementation
  //

  /**
   * Dump the object to the stream.
   *
   * @param aDos Data output stream
   *
   * @throws IOException if some error occured
   */
  public void dump(DataOutputStream aDos) throws IOException{
    iMIDletInfo.dump(aDos);
    //No need to write AMSArguments to the OutputStream 
    aDos.writeUTF(iVMArguments.getArguments());
  }

}
