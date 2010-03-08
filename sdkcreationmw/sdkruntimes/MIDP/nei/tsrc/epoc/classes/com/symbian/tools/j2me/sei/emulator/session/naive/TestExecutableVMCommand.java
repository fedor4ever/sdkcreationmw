/*
* Copyright (c) 1999 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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

package com.symbian.tools.j2me.sei.emulator.session.naive;

import java.util.*;
import junit.framework.*;

import com.symbian.vm.*;
import com.symbian.io.TestJADFile;
import com.symbian.tools.j2me.sei.emulator.commands.*;

import java.io.*;


public class TestExecutableVMCommand extends TestCase
{
  private static final String VERBOSE_TYPES = new String("-Xverbose:all,allocation,gc,gcverbose,class,classverbose,verifier,stackmaps,bytecodes,methods,methodsverbose,frames,stckchunks,exceptions,events,threading,monitors,networking");
  private static final String SYS_PARAMS[] = {
      "-Dparam1=value1", "-Dparam2=value2"
  };
  public static final String DEBUG_PARAMS[] = {
      "-Xdebug", "-Xrunjdwp:transport=dt_socket,address=5500,server=y"
  };

  private File iJad;

    public TestExecutableVMCommand(String name) throws IOException
    {
        super(name);
        iJad = TestJADFile.createTempJad();
    }

    public void testParseVerbose()
    {
      throw new UnsupportedOperationException("test is not implemented yet");
//        String args = new String("-Xverbose:all");
//        Set expected = new HashSet();
//        expected.add("-traceall");
//        verifyExpected(expected, new KVMCommand("", new SEICommand(new String[] {
//            args
//        })));
//        args = String.valueOf(String.valueOf(args)).concat(",allocation");
//        expected.add("-traceallocation");
//        verifyExpected(expected, new KVMCommand("", new SEICommand(new String[] {
//            args
//        })));
//        args = String.valueOf(String.valueOf(args)).concat(",gc");
//        expected.add("-tracegc");
//        verifyExpected(expected, new KVMCommand("", new SEICommand(new String[] {
//            args
//        })));
//        args = String.valueOf(String.valueOf(args)).concat(",gcverbose");
//        expected.add("-tracegcverbose");
//        verifyExpected(expected, new KVMCommand("", new SEICommand(new String[] {
//            args
//        })));
//        args = String.valueOf(String.valueOf(args)).concat(",class");
//        expected.add("-traceclassloading");
//        verifyExpected(expected, new KVMCommand("", new SEICommand(new String[] {
//            args
//        })));
//        args = String.valueOf(String.valueOf(args)).concat(",classverbose");
//        expected.add("-traceclassloadingverbose");
//        verifyExpected(expected, new KVMCommand("", new SEICommand(new String[] {
//            args
//        })));
//        args = String.valueOf(String.valueOf(args)).concat(",verifier");
//        expected.add("-traceverifier");
//        verifyExpected(expected, new KVMCommand("", new SEICommand(new String[] {
//            args
//        })));
//        args = String.valueOf(String.valueOf(args)).concat(",stackmaps");
//        expected.add("-tracestackmaps");
//        verifyExpected(expected, new KVMCommand("", new SEICommand(new String[] {
//            args
//        })));
//        args = String.valueOf(String.valueOf(args)).concat(",bytecodes");
//        expected.add("-tracebytecodes");
//        verifyExpected(expected, new KVMCommand("", new SEICommand(new String[] {
//            args
//        })));
//        args = String.valueOf(String.valueOf(args)).concat(",methods");
//        expected.add("-tracemethods");
//        verifyExpected(expected, new KVMCommand("", new SEICommand(new String[] {
//            args
//        })));
//        args = String.valueOf(String.valueOf(args)).concat(",methodsverbose");
//        expected.add("-tracemethodsverbose");
//        verifyExpected(expected, new KVMCommand("", new SEICommand(new String[] {
//            args
//        })));
//        args = String.valueOf(String.valueOf(args)).concat(",frames");
//        expected.add("-traceframes");
//        verifyExpected(expected, new KVMCommand("", new SEICommand(new String[] {
//            args
//        })));
//        args = String.valueOf(String.valueOf(args)).concat(",stackchunks");
//        expected.add("-tracestackchunks");
//        verifyExpected(expected, new KVMCommand("", new SEICommand(new String[] {
//            args
//        })));
//        args = String.valueOf(String.valueOf(args)).concat(",exceptions");
//        expected.add("-traceexceptions");
//        verifyExpected(expected, new KVMCommand("", new SEICommand(new String[] {
//            args
//        })));
//        args = String.valueOf(String.valueOf(args)).concat(",events");
//        expected.add("-traceevents");
//        verifyExpected(expected, new KVMCommand("", new SEICommand(new String[] {
//            args
//        })));
//        args = String.valueOf(String.valueOf(args)).concat(",threading");
//        expected.add("-tracethreading");
//        verifyExpected(expected, new KVMCommand("", new SEICommand(new String[] {
//            args
//        })));
//        args = String.valueOf(String.valueOf(args)).concat(",monitors");
//        expected.add("-tracemonitors");
//        verifyExpected(expected, new KVMCommand("", new SEICommand(new String[] {
//            args
//        })));
//        args = String.valueOf(String.valueOf(args)).concat(",networking");
//        expected.add("-tracenetworking");
//        verifyExpected(expected, new KVMCommand("", new SEICommand(new String[] {
//            args
//        })));
    }

    public void testParseSysParams() throws IOException
    {
      VMLaunchArguments args = new VMLaunchArguments(new SEICommand(SYS_PARAMS));
      Set expected = new HashSet();
      expected.add("-Dparam1=value1");
      expected.add("-Dparam2=value2");
      verifyExpected(expected, args);
    }

    public void testSetBootstrapClasses() throws IOException
    {
      VMLaunchArguments args = new VMLaunchArguments(new SEICommand(DEBUG_PARAMS));
      args.getVMArguments().setBootstrapClassPath("BOOTSTRAPCLASSPATH");
      Set expected = new HashSet();
      expected.add("-bcp");
      expected.add("BOOTSTRAPCLASSPATH");
      verifyExpected(expected, args);
    }


    public void testSetUnsuspended() throws IOException
    {
      VMLaunchArguments args = new VMLaunchArguments(new SEICommand(DEBUG_PARAMS));
      args.getVMArguments().setUnsuspendedMode();
      Set expected = new HashSet();
      expected.add("-nosuspend");
      verifyExpected(expected, args);
    }


    public void testParseDebugParams() throws IOException
    {
      VMLaunchArguments args = new VMLaunchArguments(new SEICommand(DEBUG_PARAMS));
      args.getVMArguments().setVMDebugPort(5500);
      Set expected = new HashSet();
      expected.add("-debugger");
      expected.add("-port");
      expected.add("5500");
      verifyExpected(expected, args);
    }


    private void verifyExpected(Set expected, VMLaunchArguments args) throws IOException
    {
      String jadName = iJad.getName();
      String jarName = iJad.getName() + ".jar";
      String dir = iJad.getParentFile().getAbsolutePath();
      String virtualDir = "";
      String type;
      MIDletSuiteInfo info = new MIDletSuiteInfo(jadName, jarName, dir + File.separator, virtualDir);
      args.setMIDletInfo(info.getMIDletInfo("MIDlet1Name"));
      ExecutableVMCommand cmd = new ExecutableVMCommand("", args);
      String cmdLine = cmd.getCommandLine();
      expected.add("-jar");
      expected.add("-jad");
      expected.add(jadName);
      expected.add(iJad.getName() + ".jar");
      expected.add("MIDlet1Name");

      for (StringTokenizer tokenzier = new StringTokenizer(cmdLine);
           tokenzier.hasMoreTokens(); ) {
        type = tokenzier.nextToken();
        Assert.assertTrue("unexpected type: " + type, expected.contains(type));
      }

      Assert.assertEquals("number of types doesnt match: " + cmdLine,
                          expected.size(),
                          (new StringTokenizer(cmd.getCommandLine())).countTokens());
    }


}

