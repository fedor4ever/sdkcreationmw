/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides all neccessary data for CEcmtGuiSettingList. 
*
*/


#ifndef ECMTGUISETTINGDATAHANDLER_H
#define ECMTGUISETTINGDATAHANDLER_H


#include <e32base.h>
#include <FeatDiscovery.h>


/**
 *  Data part of the MVC design pattern. Provides all neccessary data for 
 *  CEcmtGuiSettingList. 
 *
 *  @since S60 v3.2
 */
class CEcmtGuiSettingDataHandler : public CBase
    {

public:

    /**
     * Two-phased constructor.
     */
    static CEcmtGuiSettingDataHandler* NewL();

    /**
    * Destructor.
    */
    virtual ~CEcmtGuiSettingDataHandler();
          
     /**
     * Getter for data for CAknSettingItemList. Available datas are Bluetooth, 
     * USB, and WLAN. First this function checks are these connectivities 
     * supported, if so, it fills given arrays with these items. 
     *
     * @param aEnumeratedTextArray Array for the setting item.
     * @param aPoppedUpTextArray Array for the setting page.
     * @return KErrNotFound if WLAN, USB, and BT are not supported, otherwise 
     *         KErrNone.
     */
    TInt GetDataL(CArrayPtr<CAknEnumeratedText>& aEnumeratedTextArray, 
                   CArrayPtr<HBufC>& aPoppedUpTextArray );
               
    /**
     * Returns the number of supported connectivities. Possible ones are WLAN, 
     * USB, and Bluetooth. Support is defined with CFeatureDiscovery. If 
     * CFeatureDiscovery is given as a parameter, this function uses it, but 
     * does not delete it.
     *
     * @param aFeatTest A pointer to CFeatureDiscovery instance. By 
     *        default NULL.
     * @return The number of available connectivities. 
     */   
    TInt NumberOfSupportedFeaturesL( CFeatureDiscovery* aFeatTest = NULL );
    
    
    /**
     * Getter for WLAN internet accesspoints. Gets WLAN IAP titles and database
     * record IDs from Symbian comms database.
     *
     * @param aIAP Array for WLAN IAP titles.
     * @param aId Array for WLAN IAP record IDs.
     * @return KErrNone if successful.
     */   
    TInt WlanIAPIDsL( CDesCArray& aIAP, RArray<TUint32>& aId );
    
    /**
     * Setter for iSelectedCommsDbId. This commsDb recordID is stored to file 
     * in SaveSelectionL, if user has selected WLAN.
     *
     * @param aSelectedCommsDbId A new value for iSelectedCommsDbId.
     */ 
    void SetSelectedWLANIAPCommsDbId( TUint32 aSelectedCommsDbId );
    
    /**
     * Saves user selection to the file c:\Data\EcmtBearer.dat. It saves the 
     * title of connection and WLAN accespoint database record id, if WLAN is 
     * selected.
     *
     * @param aSelection New selected bearer from setting item.
     * @return System wide error codes. KErrNone if successfull.
     */ 
    TInt SaveSelectionL( const TDesC& aSelection );

private:

    /**
     * C++ default constructor.
     */
    CEcmtGuiSettingDataHandler();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Retrieves the default bearer from file c:\Data\EcmtBearer.dat. 
     * Allocates title from heap. 
     *
     * @return HBufC* to the default bearer title.
     */              
    HBufC* DefaultBearerL() const ;
     
    /**
     * Adds item to the setting item arrays.
     *
     * @param aEnumeratedTextArray Array for the setting item.
     * @param aPoppedUpTextArray Array for the setting page.
     */       
    void AddTextL( CArrayPtr <CAknEnumeratedText>& aEnumeratedTextArray, 
	                    CArrayPtr <HBufC>& aPoppedUpTextArray,
	                    const TDesC8& aText,
	                    TInt aIndex );

private: // data

    /**
     * Symbian commsDb recordId for selected WLAN accesspoint. This is saved 
     * to file in SaveSelectionL, if WLAN is selected by the user.
     */
    TUint32 iSelectedCommsDbId;
    
    };


#endif // ECMTGUISETTINGDATAHANDLER_H
