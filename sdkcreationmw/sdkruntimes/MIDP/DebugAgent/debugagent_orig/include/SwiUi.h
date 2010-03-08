/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef HEADER_SWIENGINE_SWIUI
#define HEADER_SWIENGINE_SWIUI


#include <MJavaInstallerUI.h>

class CSwiTask;

class CSwiUi : public MJavaInstallerUI 
	{
public:

	CSwiUi(CSwiTask* aTask) { iTask = aTask ; }
	virtual ~CSwiUi(){};

public:
	/**
	 * Informs UI that installation is about to start. The UI should display
	 * to the user the MIDlet suite name, vendor, version, description, 
	 * as well as the MIDlets contained in the suite.
	 * @param aMIDlet Attributes of the MIDlet to install
	 * @return TBool indicating whether or not to continue
	 */
	TBool StartInstallL(const CMIDletSuiteAttributes& aMIDlet);

	/**
	 * Asks the UI for a drive to select
	 * @param aMIDlet Attributes of the MIDlet to install
	 * @param aSpaceRequired The amount of space required in bytes
	 * @param aDrive The returned selected drive
	 * @param aSufficientSpace Set to EFalse if not enough space on device
	 * @return TBool indicating whether or not to continue
	 */
	TBool SelectDriveL(const CMIDletSuiteAttributes& aMIDlet, 
		TInt aSpaceRequired, TChar& aDrive, TBool& aSufficientSpace);

	/**
	 * Verifies user wants to replace an existing midlet
	 * @param aMIDlet Attributes of the midlet
	 * @param aOldVersion The version of the midlet to be replaced
	 * @return TBool indicating whether or not to continue
	 */
	TBool ReplaceExistingMIDletL(const CMIDletSuiteAttributes& aMIDlet,
		const TAppVersion& aOldVersion);

	/**
	 * Verifies user wants to update RMS from one version to the next.
	 * Similar to ReplaceExistingMIDlet but only displayed in cases
	 * of an upgrade
	 * @param aMIDlet Attributes of the midlet
	 * @param aOldVersion The version of the midlet to be upgraded
	 * @return ETrue if the RMS is to be upgraded/retained, EFalse otherwise
	 */
	TBool UpgradeRMSL(const CMIDletSuiteAttributes& aMIDlet,
		const TAppVersion& aOldVersion);

	/**
	 * Inform user that MIDlet to be installed is untrusted
	 * @param aMIDlet Attributes of the untrusted MIDlet
	 * @return TBool indicating whether or not to continue
	 */
	TBool MIDletUntrustedL(const CMIDletSuiteAttributes& aMIDlet);

	/**
	 * Inform user that the certificate attached to the MIDlet
	 * does not resolve to a root certificate on the device. This
	 * is a fatal error and installation will be aborted after
	 * this
	 * @param aMIDlet Attributes of the MIDlet to be installed
	 * @param aCertChain The certificate chain
	 * @param aValidationResult The result of the validation
	 */
	void CertificateHasNoRootL(const CMIDletSuiteAttributes& aMIDlet,
		const CPKIXCertChain& aCertChain,
		const CPKIXValidationResult& aValidationResult);

	/**
	 * Inform user that the signature could not be validated
	 * against the certificate.
	 * @param aMIDlet Attributes of the MIDlet to be installed
	 * @param aCertChain The certificate chain
	 * @param aValidationResult The result of the validation
	 */
	void BadSignatureL(const CMIDletSuiteAttributes& aMIDlet,
		const CPKIXCertChain& aCertChain, 
		const CPKIXValidationResult& aValidationResult);

	/**
	 * Ask the UI if revocation should be performed. Note that this may or
	 * may not pop up a UI dialog depending on the control panel settings
	 * @param aDoCheck Returned from UI whether check should be performed
	 * @return ETrue if installation should continue
	 */
	TBool PerformRevocationCheckL(TBool& aDoCheck);

	/** Inform the UI that the revocation check has started */
	void StartRevocationCheckL();

	/** Inform the UI that the revocation check has finished */
	void FinishedRevocationCheckL();

	/** Inform the UI that icon conversion has started */
	void StartIconConversionL();

	/** Inform the UI that icon conversion has finished */
	void FinishedIconConversionL();

	/**
	 * Inform the UI of an OCSP warning - user is still allowed to proceed
	 * @param aMIDlet Attributes of the MIDlet to be installed
	 * @param aRevocationMsg The message to be displayed
	 * @param aOCSPoutcome The OCSP outcome
	 * @return TBool indicating whether or not to continue
	 */
	TBool OCSPWarningL(const CMIDletSuiteAttributes& aMIDlet,
		TRevocationMsg aRevocationMsg, 
		const TOCSPOutcome* aOCSPOutcome);

	/**
	 * Inform the UI of an OCSP error - installation is aborted
	 * @param aMIDlet Attributes of the MIDlet to be installed
	 * @param aRevocationMsg The message to be displayed
	 * @param aOCSPoutcome The OCSP outcome
	 */
	void OCSPErrorL(const CMIDletSuiteAttributes& aMIDlet,
		TRevocationMsg aRevocationMsg, 
		const TOCSPOutcome* aOCSPOutcome);

	TBool MIDletInUseL(const class CMIDletSuiteAttributes&);

	void OTAReportResponseL(TInt, const TDesC &);

private:
	CSwiTask* iTask;
	};


#endif