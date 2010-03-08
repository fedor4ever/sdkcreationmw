@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description: 
@rem
@rem     ---------------------------------------------------------------------
@rem     This batch-file executes certain hardcoded tweaks to S60 example
@rem     applications (moving, renaming and deleting) and also executes
@rem     example application documentation generation.
@rem     ---------------------------------------------------------------------

rem Finalizing S60 examples...
cd \Series60SDK_dev\ExamplesCpp

rem Move BrowserSamples to parent directory...

move S60Ex\BrowserSamples\BrCtlSampleApp S60Ex\
move S60Ex\BrowserSamples\NPBitmap S60Ex\
rmdir /s /q S60Ex\BrowserSamples

rem Delete RGA-examples...
@rem     ---------------------------------------------------------------------
@rem     Currently RGA APIs are not published either in 3.2 or 5.0.
@rem     Therefore the RGA-examples are removed.
@rem     ---------------------------------------------------------------------
if exist S60Ex\rga_ex ( rmdir /s /q S60Ex\rga_ex )

@rem     ---------------------------------------------------------------------
@rem     Just to maintain look-and-feel as it was in 3.2 SDK
@rem     ---------------------------------------------------------------------

rename S60Ex\s60opencex OpenC_Ex
rename S60Ex\s60opencppex OpenCpp_Ex


rem Generate Doxygen documentation
@rem     ---------------------------------------------------------------------
@rem     Location examples have Doc-dir instead of doc-dir. doxy.pl-script
@rem     apparently does case-sensitive checks and does not generate documents
@rem     for Doc-directories. Hence we rename those 4 directories first.
@rem     ---------------------------------------------------------------------

rename S60Ex\LocationLandmarksRefAppForS60\Doc doc
rename S60Ex\LocationLandmarksUIRefApp\Doc doc
rename S60Ex\LocationRefAppForS60\Doc doc
rename S60Ex\LocationSatViewRefApp\Doc doc

cd \Series60SDK_dev\ExamplesCpp\Doxyfiles\
perl doxy.pl doxy.conf

rem S60Ex finalizing done.
