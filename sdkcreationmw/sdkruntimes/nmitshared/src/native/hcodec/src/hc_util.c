/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
/* File    : T:\wpsvob\wsp\work\codecs\headercodec\hc_util.c
   Version : \main\32
   Date    : 02-May-00.15:51:46
*/

/*
 * ========================================================================
 *  Name        : hc_util.c
 *  Part of     : 
 *  Description : 
 *  Version     : 
 *
 *  Copyright (c) 2000 - 2005 Nokia Corporation.
 *  This material, including documentation and any related
 *  computer programs, is protected by copyright controlled by
 *  Nokia Corporation. All rights are reserved. Copying,
 *  including reproducing, storing, adapting or translating, any
 *  or all of this material requires the prior written consent of
 *  Nokia Corporation. This material also contains confidential
 *  information which may not be disclosed to others without the
 *  prior written consent of Nokia Corporation.
 * ========================================================================
 */


/*
-----------------------------------------------------------------------------
    Includes
-----------------------------------------------------------------------------
*/

#include "hc_util.h"
#include "hc_pages.h"

#include <string.h>
#include <ctype.h>

/*
-----------------------------------------------------------------------------
    Defines
-----------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
    Exported constants
-----------------------------------------------------------------------------
*/

/*  Well-known Content-Disposition values. */
const NameBytePair g_aContentDispValues[2] =
{
    { "form-data",  128 }, 
    { "attachment", 129 },
};

/*  Well-known Content-Encoding values. */
const NameBytePair g_aContentEncodingValues[3] =
{
    { "gzip",       128 }, 
    { "compress",   129 }, 
    { "deflate",    130 }, 
};

/*  Well-known Accept-Ranges values. */
const NameBytePair g_aAcceptRangesValues[2] =
{
    { "none",   128 }, 
    { "bytes",  129 }, 
};

/*  Well-known Month values. */
const NameBytePair g_aMonths[12] =
{
    { "Jan",     0 }, 
    { "Feb",     1 }, 
    { "Mar",     2 }, 
    { "Apr",     3 }, 
    { "May",     4 }, 
    { "Jun",     5 }, 
    { "Jul",     6 }, 
    { "Aug",     7 }, 
    { "Sep",     8 }, 
    { "Oct",     9 }, 
    { "Nov",    10 }, 
    { "Dec",    11 }, 
};

/*
-----------------------------------------------------------------------------
    Module-level constants
-----------------------------------------------------------------------------
*/

/*  Character Set Assignments. */
static const NameWordPair g_aCharSets[] =
{
    { "US-ASCII",                                          3 }, /* IANA MIBEnum 3 */
    { "ASCII",                                             3 }, /* IANA MIBEnum 3 */
    { "ANSI_X3.4-1968",                                    3 }, /* IANA MIBEnum 3 */
    { "iso-ir-6",										   3 }, /* IANA MIBEnum 3 */
    { "ANSI_X3.4-1986",									   3 }, /* IANA MIBEnum 3 */
    { "ISO_646.irv:1991",								   3 }, /* IANA MIBEnum 3 */
    { "ISO646-US",										   3 }, /* IANA MIBEnum 3 */
    { "us",												   3 }, /* IANA MIBEnum 3 */
	{ "IBM367",											   3 }, /* IANA MIBEnum 3 */
    { "cp367",											   3 }, /* IANA MIBEnum 3 */
    { "csASCII",										   3 }, /* IANA MIBEnum 3 */
    { "ISO-8859-1",                                        4 }, /* IANA MIBEnum 4 */
    { "ISO_8859-1:1987",                                   4 }, /* IANA MIBEnum 4 */
	{ "iso-ir-100",										   4 }, /* IANA MIBEnum 4 */
	{ "ISO_8859-1",										   4 }, /* IANA MIBEnum 4 */
	{ "latin1",											   4 }, /* IANA MIBEnum 4 */
	{ "l1",												   4 }, /* IANA MIBEnum 4 */
	{ "IBM819",											   4 }, /* IANA MIBEnum 4 */
	{ "CP819",											   4 }, /* IANA MIBEnum 4 */
	{ "csISOLatin1",									   4 }, /* IANA MIBEnum 4 */
    { "ISO-8859-2",                                        5 }, /* IANA MIBEnum 5 */
    { "ISO_8859-2:1987",                                   5 }, /* IANA MIBEnum 5 */
	{ "iso-ir-101",										   5 }, /* IANA MIBEnum 5 */
	{ "ISO_8859-2",										   5 }, /* IANA MIBEnum 5 */
	{ "latin2",											   5 }, /* IANA MIBEnum 5 */
	{ "l2",												   5 }, /* IANA MIBEnum 5 */
	{ "csISOLatin2",									   5 }, /* IANA MIBEnum 5 */
    { "ISO-8859-3",                                        6 }, /* IANA MIBEnum 6 */
    { "ISO_8859-3:1988",                                   6 }, /* IANA MIBEnum 6 */
	{ "iso-ir-109",										   6 }, /* IANA MIBEnum 6 */
	{ "ISO_8859-3",										   6 }, /* IANA MIBEnum 6 */
	{ "latin3",											   6 }, /* IANA MIBEnum 6 */
	{ "l3",												   6 }, /* IANA MIBEnum 6 */
	{ "csISOLatin3",									   6 }, /* IANA MIBEnum 6 */
    { "ISO-8859-4",                                        7 }, /* IANA MIBEnum 7 */
    { "ISO_8859-4:1988",                                   7 }, /* IANA MIBEnum 7 */
	{ "iso-ir-110",										   7 }, /* IANA MIBEnum 7 */
	{ "ISO_8859-4",										   7 }, /* IANA MIBEnum 7 */
	{ "latin4",											   7 }, /* IANA MIBEnum 7 */
	{ "l4",												   7 }, /* IANA MIBEnum 7 */
	{ "csISOLatin4",									   7 }, /* IANA MIBEnum 7 */
    { "ISO-8859-5",                                        8 }, /* IANA MIBEnum 8 */
    { "ISO_8859-5:1988",                                   8 }, /* IANA MIBEnum 8 */
	{ "iso-ir-144",										   8 }, /* IANA MIBEnum 8 */
	{ "ISO_8859-5",										   8 }, /* IANA MIBEnum 8 */
	{ "cyrillic",										   8 }, /* IANA MIBEnum 8 */
	{ "csISOLatinCyrillic",								   8 }, /* IANA MIBEnum 8 */
    { "ISO-8859-6",                                        9 }, /* IANA MIBEnum 9 */
    { "ISO_8859-6:1987",                                   9 }, /* IANA MIBEnum 9 */
	{ "iso-ir-127",										   9 }, /* IANA MIBEnum 9 */
	{ "ISO_8859-6",										   9 }, /* IANA MIBEnum 9 */
	{ "ECMA-114",										   9 }, /* IANA MIBEnum 9 */
	{ "ASMO-708",										   9 }, /* IANA MIBEnum 9 */
	{ "arabic",											   9 }, /* IANA MIBEnum 9 */
	{ "csISOLatinArabic",								   9 }, /* IANA MIBEnum 9 */
    { "ISO-8859-7",                                       10 }, /* IANA MIBEnum 10 */
    { "ISO_8859-7:1987",                                  10 }, /* IANA MIBEnum 10 */
	{ "iso-ir-126",										  10 }, /* IANA MIBEnum 10 */
	{ "ISO_8859-7",										  10 }, /* IANA MIBEnum 10 */
	{ "ELOT_928",										  10 }, /* IANA MIBEnum 10 */
	{ "ECMA-118",										  10 }, /* IANA MIBEnum 10 */
	{ "greek",											  10 }, /* IANA MIBEnum 10 */
	{ "greek8",											  10 }, /* IANA MIBEnum 10 */
	{ "csISOLatinGreek",								  10 }, /* IANA MIBEnum 10 */
    { "ISO-8859-8",                                       11 }, /* IANA MIBEnum 11 */
    { "ISO_8859-8:1988",                                  11 }, /* IANA MIBEnum 11 */
	{ "iso-ir-138",										  11 }, /* IANA MIBEnum 11 */
	{ "ISO_8859-8",										  11 }, /* IANA MIBEnum 11 */
	{ "hebrew",											  11 }, /* IANA MIBEnum 11 */
	{ "csISOLatinHebrew",								  11 }, /* IANA MIBEnum 11 */
    { "ISO-8859-9",                                       12 }, /* IANA MIBEnum 12 */
    { "ISO_8859-9:1989",                                  12 }, /* IANA MIBEnum 12 */
	{ "iso-ir-148",										  12 }, /* IANA MIBEnum 12 */
	{ "ISO_8859-9",										  12 }, /* IANA MIBEnum 12 */
	{ "latin5",											  12 }, /* IANA MIBEnum 12 */
	{ "l5",												  12 }, /* IANA MIBEnum 12 */
	{ "csISOLatin5",									  12 }, /* IANA MIBEnum 12 */
    { "latin6",                                           13 }, /* IANA MIBEnum 13 */
	{ "iso-ir-157",										  13 }, /* IANA MIBEnum 13 */
	{ "l6",												  13 }, /* IANA MIBEnum 13 */
	{ "ISO_8859-10:1992",								  13 }, /* IANA MIBEnum 13 */
	{ "csISOLatin6",									  13 }, /* IANA MIBEnum 13 */
    { "ISO_6937-2-add",                                   14 }, /* IANA MIBEnum 14 */
	{ "iso-ir-142",										  14 }, /* IANA MIBEnum 14 */
	{ "csISOTextComm",									  14 }, /* IANA MIBEnum 14 */
    { "JIS_X0201",                                        15 }, /* IANA MIBEnum 15 */
	{ "X0201",											  15 }, /* IANA MIBEnum 15 */
	{ "csHalfWidthKatakana",							  15 }, /* IANA MIBEnum 15 */
    { "JIS_Encoding",                                     16 }, /* IANA MIBEnum 16 */
	{ "csJISEncoding",									  16 }, /* IANA MIBEnum 16 */
    { "Shift_JIS",                                        17 }, /* IANA MIBEnum 17 */
	{ "MS_Kanji",										  17 }, /* IANA MIBEnum 17 */
	{ "csShiftJIS",										  17 }, /* IANA MIBEnum 17 */
    { "EUC-JP",                                           18 }, /* IANA MIBEnum 18 */
    { "Extended_UNIX_Code_Packed_Format_for_Japanese",    18 }, /* IANA MIBEnum 18 */
	{ "csEUCPkdFmtJapanese",							  18 }, /* IANA MIBEnum 18 */
    { "Extended_UNIX_Code_Fixed_Width_for_Japanese",      19 }, /* IANA MIBEnum 19 */
	{ "csEUCFixWidJapanese",							  19 }, /* IANA MIBEnum 19 */
    { "BS_4730",                                          20 }, /* IANA MIBEnum 20 */
    { "iso-ir-4",                                         20 }, /* IANA MIBEnum 20 */
    { "ISO646-GB",                                        20 }, /* IANA MIBEnum 20 */
    { "gb",									              20 }, /* IANA MIBEnum 20 */
    { "uk",											      20 }, /* IANA MIBEnum 20 */
    { "csISO4UnitedKingdom",                              20 }, /* IANA MIBEnum 20 */
    { "SEN_850200_C",                                     21 }, /* IANA MIBEnum 21 */
	{ "iso-ir-11",										  21 }, /* IANA MIBEnum 21 */
	{ "ISO646-SE2",										  21 }, /* IANA MIBEnum 21 */
	{ "se2",											  21 }, /* IANA MIBEnum 21 */
	{ "csISO11SwedishForNames",							  21 }, /* IANA MIBEnum 21 */
    { "IT",                                               22 }, /* IANA MIBEnum 22 */
	{ "iso-ir-15",										  22 }, /* IANA MIBEnum 22 */
	{ "ISO646-IT",										  22 }, /* IANA MIBEnum 22 */
	{ "csISO15Italian",									  22 }, /* IANA MIBEnum 22 */
    { "ES",                                               23 }, /* IANA MIBEnum 23 */
	{ "iso-ir-17",										  23 }, /* IANA MIBEnum 23 */
	{ "ISO646-ES",										  23 }, /* IANA MIBEnum 23 */
	{ "csISO17Spanish",									  23 }, /* IANA MIBEnum 23 */
    { "DIN_66003",                                        24 }, /* IANA MIBEnum 24 */
	{ "iso-ir-21",										  24 }, /* IANA MIBEnum 24 */
	{ "de",												  24 }, /* IANA MIBEnum 24 */
	{ "ISO646-DE",										  24 }, /* IANA MIBEnum 24 */
	{ "csISO21German",									  24 }, /* IANA MIBEnum 24 */
    { "NS_4551-1",                                        25 }, /* IANA MIBEnum 25 */
	{ "iso-ir-60",										  25 }, /* IANA MIBEnum 25 */
	{ "ISO646-NO",										  25 }, /* IANA MIBEnum 25 */
	{ "no",												  25 }, /* IANA MIBEnum 25 */
	{ "csISO60DanishNorwegian",							  25 }, /* IANA MIBEnum 25 */
	{ "csISO60Norwegian1",								  25 }, /* IANA MIBEnum 25 */
    { "NF_Z_62-010",                                      26 }, /* IANA MIBEnum 26 */
	{ "iso-ir-69",										  26 }, /* IANA MIBEnum 26 */
	{ "ISO646-FR",										  26 }, /* IANA MIBEnum 26 */
	{ "fr",												  26 }, /* IANA MIBEnum 26 */
	{ "csISO69French",									  26 }, /* IANA MIBEnum 26 */
    { "ISO-10646-UTF-1",                                  27 }, /* IANA MIBEnum 27 */
    { "csISO10646UTF1",                                   27 }, /* IANA MIBEnum 27 */
    { "ISO_646.basic:1983",                               28 }, /* IANA MIBEnum 28 */
    { "ref",				                              28 }, /* IANA MIBEnum 28 */
    { "csISO646basic1983",                                28 }, /* IANA MIBEnum 28 */
    { "INVARIANT",                                        29 }, /* IANA MIBEnum 29 */
    { "csINVARIANT",                                      29 }, /* IANA MIBEnum 29 */
    { "ISO_646.irv:1983",                                 30 }, /* IANA MIBEnum 30 */
    { "iso-ir-2",									      30 }, /* IANA MIBEnum 30 */
    { "irv",								              30 }, /* IANA MIBEnum 30 */
    { "csISO2IntlRefVersion",                             30 }, /* IANA MIBEnum 30 */
    { "NATS-SEFI",                                        31 }, /* IANA MIBEnum 31 */
    { "iso-ir-8-1",                                       31 }, /* IANA MIBEnum 31 */
    { "csNATSSEFI",                                       31 }, /* IANA MIBEnum 31 */
    { "NATS-SEFI-ADD",                                    32 }, /* IANA MIBEnum 32 */
    { "iso-ir-8-2",                                       32 }, /* IANA MIBEnum 32 */
    { "csNATSSEFIADD",                                    32 }, /* IANA MIBEnum 32 */
    { "NATS-DANO",                                        33 }, /* IANA MIBEnum 33 */
    { "iso-ir-9-1",                                       33 }, /* IANA MIBEnum 33 */
    { "csNATSDANO",                                       33 }, /* IANA MIBEnum 33 */
    { "NATS-DANO-ADD",                                    34 }, /* IANA MIBEnum 34 */
    { "iso-ir-9-2",                                       34 }, /* IANA MIBEnum 34 */
    { "csNATSDANOADD",                                    34 }, /* IANA MIBEnum 34 */
    { "SEN_850200_B",                                     35 }, /* IANA MIBEnum 35 */
    { "iso-ir-10",					                      35 }, /* IANA MIBEnum 35 */
    { "FI",										          35 }, /* IANA MIBEnum 35 */
    { "ISO646-FI",										  35 }, /* IANA MIBEnum 35 */
    { "ISO646-SE",										  35 }, /* IANA MIBEnum 35 */
    { "se",												  35 }, /* IANA MIBEnum 35 */
    { "csISO10Swedish",                                   35 }, /* IANA MIBEnum 35 */
    { "KS_C_5601-1987",                                   36 }, /* IANA MIBEnum 36 */
	{ "iso-ir-149",										  36 }, /* IANA MIBEnum 36 */
	{ "KS_C_5601-1989",									  36 }, /* IANA MIBEnum 36 */
	{ "KSC_5601",										  36 }, /* IANA MIBEnum 36 */
	{ "korean",											  36 }, /* IANA MIBEnum 36 */
	{ "csKSC56011987",									  36 }, /* IANA MIBEnum 36 */
    { "ISO-2022-KR",                                      37 }, /* IANA MIBEnum 37 */
	{ "csISO2022KR",									  37 }, /* IANA MIBEnum 37 */
    { "EUC-KR",                                           38 }, /* IANA MIBEnum 38 */
	{ "csEUCKR",										  38 }, /* IANA MIBEnum 38 */
    { "ISO-2022-JP",                                      39 }, /* IANA MIBEnum 39 */
	{ "csISO2022JP",									  39 }, /* IANA MIBEnum 39 */
    { "ISO-2022-JP-2",                                    40 }, /* IANA MIBEnum 40 */
	{ "csISO2022JP2",									  40 }, /* IANA MIBEnum 40 */
    { "JIS_C6220-1969-jp",                                41 }, /* IANA MIBEnum 41 */
	{ "JIS_C6220-1969",									  41 }, /* IANA MIBEnum 41 */
	{ "iso-ir-13",										  41 }, /* IANA MIBEnum 41 */
	{ "katakana",										  41 }, /* IANA MIBEnum 41 */
	{ "x0201-7",										  41 }, /* IANA MIBEnum 41 */
	{ "csISO13JISC6220jp",								  41 }, /* IANA MIBEnum 41 */
    { "JIS_C6220-1969-ro",                                42 }, /* IANA MIBEnum 42 */
	{ "iso-ir-14",										  42 }, /* IANA MIBEnum 42 */
	{ "jp",												  42 }, /* IANA MIBEnum 42 */
	{ "ISO646-JP",										  42 }, /* IANA MIBEnum 42 */
	{ "csISO14JISC6220ro",								  42 }, /* IANA MIBEnum 42 */
    { "PT",                                               43 }, /* IANA MIBEnum 43 */
	{ "iso-ir-16",										  43 }, /* IANA MIBEnum 43 */
	{ "ISO646-PT",										  43 }, /* IANA MIBEnum 43 */
	{ "csISO16Portuguese",								  43 }, /* IANA MIBEnum 43 */
    { "greek7-old",                                       44 }, /* IANA MIBEnum 44 */
	{ "iso-ir-18",										  44 }, /* IANA MIBEnum 44 */
	{ "csISO18Greek7Old",								  44 }, /* IANA MIBEnum 44 */
    { "latin-greek",                                      45 }, /* IANA MIBEnum 45 */
	{ "iso-ir-19",										  45 }, /* IANA MIBEnum 45 */
	{ "csISO19LatinGreek",								  45 }, /* IANA MIBEnum 45 */
    { "NF_Z_62-010_(1973)",                               46 }, /* IANA MIBEnum 46 */
	{ "iso-ir-25",										  46 }, /* IANA MIBEnum 46 */
	{ "ISO646-FR1",										  46 }, /* IANA MIBEnum 46 */
	{ "csISO25French",									  46 }, /* IANA MIBEnum 46 */
    { "Latin-greek-1",                                    47 }, /* IANA MIBEnum 47 */
	{ "iso-ir-27",										  47 }, /* IANA MIBEnum 47 */
	{ "csISO27LatinGreek1",								  47 }, /* IANA MIBEnum 47 */
    { "ISO_5427",                                         48 }, /* IANA MIBEnum 48 */
	{ "iso-ir-37",										  48 }, /* IANA MIBEnum 48 */
	{ "csISO5427Cyrillic",								  48 }, /* IANA MIBEnum 48 */
    { "JIS_C6226-1978",                                   49 }, /* IANA MIBEnum 49 */
	{ "iso-ir-42",										  49 }, /* IANA MIBEnum 49 */
	{ "csISO42JISC62261978",							  49 }, /* IANA MIBEnum 49 */
    { "BS_viewdata",                                      50 }, /* IANA MIBEnum 50 */
	{ "iso-ir-47",										  50 }, /* IANA MIBEnum 50 */
	{ "csISO47BSViewdata",								  50 }, /* IANA MIBEnum 50 */
    { "INIS",                                             51 }, /* IANA MIBEnum 51 */
	{ "iso-ir-49",										  51 }, /* IANA MIBEnum 51 */
	{ "csISO49INIS",									  51 }, /* IANA MIBEnum 51 */
    { "INIS-8",                                           52 }, /* IANA MIBEnum 52 */
	{ "iso-ir-50",										  52 }, /* IANA MIBEnum 52 */
	{ "csISO50INIS8",									  52 }, /* IANA MIBEnum 52 */
    { "INIS-cyrillic",                                    53 }, /* IANA MIBEnum 53 */
	{ "iso-ir-51",										  53 }, /* IANA MIBEnum 53 */
	{ "csISO51INISCyrillic",							  53 }, /* IANA MIBEnum 53 */
    { "ISO_5427:1981",					                  54 }, /* IANA MIBEnum 54 */
	{ "iso-ir-54",										  54 }, /* IANA MIBEnum 54 */
	{ "ISO5427Cyrillic1981",							  54 }, /* IANA MIBEnum 54 */
    { "ISO_5428:1980",                                    55 }, /* IANA MIBEnum 55 */
	{ "iso-ir-55",										  55 }, /* IANA MIBEnum 55 */
	{ "csISO5428Greek",									  55 }, /* IANA MIBEnum 55 */
    { "GB_1988-80",                                       56 }, /* IANA MIBEnum 56 */
	{ "iso-ir-57",										  56 }, /* IANA MIBEnum 56 */
	{ "cn",												  56 }, /* IANA MIBEnum 56 */
	{ "ISO646-CN",										  56 }, /* IANA MIBEnum 56 */
	{ "csISO57GB1988",									  56 }, /* IANA MIBEnum 56 */
    { "GB_2312-80",                                       57 }, /* IANA MIBEnum 57 */
	{ "iso-ir-58",										  57 }, /* IANA MIBEnum 57 */
	{ "chinese",										  57 }, /* IANA MIBEnum 57 */
	{ "csISO58GB231280",								  57 }, /* IANA MIBEnum 57 */
    { "NS_4551-2",                                        58 }, /* IANA MIBEnum 58 */
	{ "ISO646-NO2",										  58 }, /* IANA MIBEnum 58 */
	{ "iso-ir-61",										  58 }, /* IANA MIBEnum 58 */
	{ "no2",											  58 }, /* IANA MIBEnum 58 */
	{ "csISO61Norwegian2",								  58 }, /* IANA MIBEnum 58 */
    { "videotex-suppl",                                   59 }, /* IANA MIBEnum 59 */
	{ "iso-ir-70",										  59 }, /* IANA MIBEnum 59 */
	{ "csISO70VideotexSupp1",							  59 }, /* IANA MIBEnum 59 */
    { "PT2",                                              60 }, /* IANA MIBEnum 60 */
	{ "iso-ir-84",										  60 }, /* IANA MIBEnum 60 */
	{ "ISO646-PT2",										  60 }, /* IANA MIBEnum 60 */
	{ "csISO84Portuguese2",								  60 }, /* IANA MIBEnum 60 */
    { "ES2",                                              61 }, /* IANA MIBEnum 61 */
	{ "iso-ir-85",										  61 }, /* IANA MIBEnum 61 */
	{ "ISO646-ES2",										  61 }, /* IANA MIBEnum 61 */
	{ "csISO85Spanish2",								  61 }, /* IANA MIBEnum 61 */
    { "MSZ_7795.3",                                       62 }, /* IANA MIBEnum 62 */
	{ "iso-ir-86",										  62 }, /* IANA MIBEnum 62 */
	{ "ISO646-HU",										  62 }, /* IANA MIBEnum 62 */
	{ "hu",												  62 }, /* IANA MIBEnum 62 */
	{ "csISO86Hungarian",								  62 }, /* IANA MIBEnum 62 */
    { "JIS_C6226-1983",                                   63 }, /* IANA MIBEnum 63 */
	{ "iso-ir-87",										  63 }, /* IANA MIBEnum 63 */
	{ "x0208",											  63 }, /* IANA MIBEnum 63 */
	{ "JIS_X0208-1983",									  63 }, /* IANA MIBEnum 63 */
	{ "csISO87JISX0208",								  63 }, /* IANA MIBEnum 63 */
    { "greek7",                                           64 }, /* IANA MIBEnum 64 */
	{ "iso-ir-88",										  64 }, /* IANA MIBEnum 64 */
	{ "csISO88Greek7",									  64 }, /* IANA MIBEnum 64 */
    { "ASMO_449",		                                  65 }, /* IANA MIBEnum 65 */
	{ "ISO_9036",										  65 }, /* IANA MIBEnum 65 */
	{ "arabic7",										  65 }, /* IANA MIBEnum 65 */
	{ "iso-ir-89",										  65 }, /* IANA MIBEnum 65 */
	{ "csISO89ASMO449",									  65 }, /* IANA MIBEnum 65 */
    { "iso-ir-90",                                        66 }, /* IANA MIBEnum 66 */
	{ "csISO90",										  66 }, /* IANA MIBEnum 66 */
    { "JIS_C6229-1984-a",                                 67 }, /* IANA MIBEnum 67 */
	{ "iso-ir-91",										  67 }, /* IANA MIBEnum 67 */
	{ "jp-ocr-a",										  67 }, /* IANA MIBEnum 67 */
	{ "csISO91JISC62291984a",							  67 }, /* IANA MIBEnum 67 */
    { "JIS_C6229-1984-b",                                 68 }, /* IANA MIBEnum 68 */
	{ "iso-ir-92",										  68 }, /* IANA MIBEnum 68 */
	{ "ISO646-JP-OCR-B",								  68 }, /* IANA MIBEnum 68 */
	{ "jp-ocr-b",										  68 }, /* IANA MIBEnum 68 */
	{ "csISO92JISC62991984b",							  68 }, /* IANA MIBEnum 68 */
    { "JIS_C6229-1984-b-add",                             69 }, /* IANA MIBEnum 69 */
	{ "iso-ir-93",										  69 }, /* IANA MIBEnum 69 */
	{ "jp-ocr-b-add",									  69 }, /* IANA MIBEnum 69 */
	{ "csISO93JIS62291984badd",							  69 }, /* IANA MIBEnum 69 */
    { "JIS_C6229-1984-hand",                              70 }, /* IANA MIBEnum 70 */
	{ "iso-ir-94",										  70 }, /* IANA MIBEnum 70 */
	{ "jp-ocr-hand",									  70 }, /* IANA MIBEnum 70 */
	{ "csISO94JIS62291984hand",							  70 }, /* IANA MIBEnum 70 */
    { "JIS_C6229-1984-hand-add",                          71 }, /* IANA MIBEnum 71 */
	{ "iso-ir-95",										  71 }, /* IANA MIBEnum 71 */
	{ "jp-ocr-hand-add",								  71 }, /* IANA MIBEnum 71 */
	{ "csISO95JIS62291984handadd",						  71 }, /* IANA MIBEnum 71 */
    { "JIS_C6229-1984-kana",                              72 }, /* IANA MIBEnum 72 */
	{ "iso-ir-96",										  72 }, /* IANA MIBEnum 72 */
	{ "csISO96JISC62291984kana",						  72 }, /* IANA MIBEnum 72 */
    { "ISO_2033-1983",                                    73 }, /* IANA MIBEnum 73 */
	{ "iso-ir-98",										  73 }, /* IANA MIBEnum 73 */
	{ "e13b",											  73 }, /* IANA MIBEnum 73 */
	{ "csISO2033",										  73 }, /* IANA MIBEnum 73 */
    { "ANSI_X3.110-1983",                                 74 }, /* IANA MIBEnum 74 */
	{ "iso-ir-99",										  74 }, /* IANA MIBEnum 74 */
	{ "CSA_T500-1983",									  74 }, /* IANA MIBEnum 74 */
	{ "NAPLPS",											  74 }, /* IANA MIBEnum 74 */
	{ "csISO99NAPLPS",									  74 }, /* IANA MIBEnum 74 */
    { "T.61-7bit",                                        75 }, /* IANA MIBEnum 75 */
	{ "iso-ir-102",										  75 }, /* IANA MIBEnum 75 */
	{ "csISO102T617bit",								  75 }, /* IANA MIBEnum 75 */
    { "T.61-8bit",                                        76 }, /* IANA MIBEnum 76 */
	{ "T.61",											  76 }, /* IANA MIBEnum 76 */
	{ "iso-ir-103",										  76 }, /* IANA MIBEnum 76 */
	{ "csISO103T618bit",								  76 }, /* IANA MIBEnum 76 */
    { "ECMA-cyrillic",                                    77 }, /* IANA MIBEnum 77 */
	{ "iso-ir-111",										  77 }, /* IANA MIBEnum 77 */
	{ "csISO111ECMACyrillic",							  77 }, /* IANA MIBEnum 77 */
    { "CSA_Z243.4-1985-1",                                78 }, /* IANA MIBEnum 78 */
	{ "iso-ir-121",										  78 }, /* IANA MIBEnum 78 */
	{ "ISO646-CA",										  78 }, /* IANA MIBEnum 78 */
	{ "csa7-1",											  78 }, /* IANA MIBEnum 78 */
	{ "ca",												  78 }, /* IANA MIBEnum 78 */
	{ "csISO121Canadian1",								  78 }, /* IANA MIBEnum 78 */
    { "CSA_Z243.4-1985-2",                                79 }, /* IANA MIBEnum 79 */
	{ "iso-ir-122",										  79 }, /* IANA MIBEnum 79 */
	{ "ISO646-CA2",										  79 }, /* IANA MIBEnum 79 */
	{ "csa7-2",											  79 }, /* IANA MIBEnum 79 */
	{ "csISO122Canadian2",								  79 }, /* IANA MIBEnum 79 */
    { "CSA_Z243.4-1985-gr",                               80 }, /* IANA MIBEnum 80 */
	{ "iso-ir-123",										  80 }, /* IANA MIBEnum 80 */
	{ "csISO123CSAZ24341985gr",							  80 }, /* IANA MIBEnum 80 */
    { "ISO_8859-6-E",                                     81 }, /* IANA MIBEnum 81 */
	{ "csISO88596E",									  81 }, /* IANA MIBEnum 81 */
    { "ISO_8859-6-I",                                     82 }, /* IANA MIBEnum 82 */
	{ "csISO88596I",									  82 }, /* IANA MIBEnum 82 */
    { "T.101-G2",                                         83 }, /* IANA MIBEnum 83 */
	{ "iso-ir-128",										  83 }, /* IANA MIBEnum 83 */
	{ "csISO128T101G2",									  83 }, /* IANA MIBEnum 83 */
    { "ISO_8859-8-E",                                     84 }, /* IANA MIBEnum 84 */
	{ "csISO88598E",									  84 }, /* IANA MIBEnum 84 */
    { "ISO_8859-8-I",                                     85 }, /* IANA MIBEnum 85 */
	{ "csISO88598I",									  85 }, /* IANA MIBEnum 85 */
    { "CSN_369103",                                       86 }, /* IANA MIBEnum 86 */
	{ "iso-ir-139",										  86 }, /* IANA MIBEnum 86 */
	{ "csISO139CSN369103",								  86 }, /* IANA MIBEnum 86 */
    { "JUS_I.B1.002",                                     87 }, /* IANA MIBEnum 87 */
	{ "iso-ir-141",										  87 }, /* IANA MIBEnum 87 */
	{ "ISO646-YU",										  87 }, /* IANA MIBEnum 87 */
	{ "js",												  87 }, /* IANA MIBEnum 87 */
	{ "yu",												  87 }, /* IANA MIBEnum 87 */
	{ "csISO141JUSIB1002",								  87 }, /* IANA MIBEnum 87 */
    { "IEC_P27-1",                                        88 }, /* IANA MIBEnum 88 */
	{ "iso-ir-143",										  88 }, /* IANA MIBEnum 88 */
	{ "csISO143IECP271",								  88 }, /* IANA MIBEnum 88 */
    { "JUS_I.B1.003-serb",                                89 }, /* IANA MIBEnum 89 */
	{ "iso-ir-146",										  89 }, /* IANA MIBEnum 89 */
	{ "serbian",										  89 }, /* IANA MIBEnum 89 */
	{ "csISO146Serbian",								  89 }, /* IANA MIBEnum 89 */
    { "JUS_I.B1.003-mac",                                 90 }, /* IANA MIBEnum 90 */
	{ "macedonian",										  90 }, /* IANA MIBEnum 90 */
	{ "iso-ir-147",										  90 }, /* IANA MIBEnum 90 */
	{ "csISO147Macedonian",								  90 }, /* IANA MIBEnum 90 */
    { "greek-ccitt",                                      91 }, /* IANA MIBEnum 91 */
	{ "iso-ir-150",										  91 }, /* IANA MIBEnum 91 */
	{ "csISO150",										  91 }, /* IANA MIBEnum 91 */
	{ "csISO150GreekCCITT",								  91 }, /* IANA MIBEnum 91 */
    { "NC_NC00-10:81",                                    92 }, /* IANA MIBEnum 92 */
	{ "cuba",											  92 }, /* IANA MIBEnum 92 */
	{ "iso-ir-151",										  92 }, /* IANA MIBEnum 92 */
	{ "ISO646-CU",										  92 }, /* IANA MIBEnum 92 */
	{ "csISO151Cuba",									  92 }, /* IANA MIBEnum 92 */
    { "ISO_6937-2-25",                                    93 }, /* IANA MIBEnum 93 */
	{ "iso-ir-152",										  93 }, /* IANA MIBEnum 93 */
	{ "csISO6937Add",									  93 }, /* IANA MIBEnum 93 */
    { "GOST_19768-74",                                    94 }, /* IANA MIBEnum 94 */
	{ "ST_SEV_358-88",									  94 }, /* IANA MIBEnum 94 */
	{ "iso-ir-153",										  94 }, /* IANA MIBEnum 94 */
	{ "csISO153GOST1976874",							  94 }, /* IANA MIBEnum 94 */
    { "ISO_8859-supp",                                    95 }, /* IANA MIBEnum 95 */
	{ "iso-ir-154",										  95 }, /* IANA MIBEnum 95 */
	{ "latin1-2-5",										  95 }, /* IANA MIBEnum 95 */
	{ "csISO8859Supp",									  95 }, /* IANA MIBEnum 95 */
    { "ISO_10367-box",                                    96 }, /* IANA MIBEnum 96 */
	{ "iso-ir-155",										  96 }, /* IANA MIBEnum 96 */
	{ "csISO10367Box",									  96 }, /* IANA MIBEnum 96 */
    { "latin-lap",                                        97 }, /* IANA MIBEnum 97 */
	{ "lap",											  97 }, /* IANA MIBEnum 97 */
	{ "iso-ir-158",										  97 }, /* IANA MIBEnum 97 */
	{ "csISO158Lap",									  97 }, /* IANA MIBEnum 97 */
    { "JIS_X0212-1990",                                   98 }, /* IANA MIBEnum 98 */
	{ "x0212",											  98 }, /* IANA MIBEnum 98 */
	{ "iso-ir-159",										  98 }, /* IANA MIBEnum 98 */
	{ "csISO159JISX02121990",							  98 }, /* IANA MIBEnum 98 */
    { "DS_2089",                                          99 }, /* IANA MIBEnum 99 */
	{ "DS2089",											  99 }, /* IANA MIBEnum 99 */
	{ "ISO646-DK",										  99 }, /* IANA MIBEnum 99 */
	{ "dk",												  99 }, /* IANA MIBEnum 99 */
	{ "csISO646Danish",									  99 }, /* IANA MIBEnum 99 */
    { "us-dk",                                           100 }, /* IANA MIBEnum 100 */
	{ "csUSDK",											 100 }, /* IANA MIBEnum 100 */
    { "dk-us",                                           101 }, /* IANA MIBEnum 101 */
	{ "csDKUS",											 101 }, /* IANA MIBEnum 101 */
    { "KSC5636",                                         102 }, /* IANA MIBEnum 102 */
	{ "ISO646-KR",										 102 }, /* IANA MIBEnum 102 */
	{ "csKSC5636",										 102 }, /* IANA MIBEnum 102 */
    { "UNICODE-1-1-UTF-7",                               103 }, /* IANA MIBEnum 103 */
	{ "csUnicode11UTF7",						  		 103 }, /* IANA MIBEnum 103 */
    { "ISO-2022-CN",                                     104 }, /* IANA MIBEnum 104 */
    { "UTF-7",                                           104 }, /* IANA MIBEnum 104 */
	{ "NONE",											 104 }, /* IANA MIBEnum 104 */
    { "ISO-2022-CN-EXT",                                 105 }, /* IANA MIBEnum 105 */
    { "UTF-8",                                           106 }, /* IANA MIBEnum 106 */
    { "ISO-10646-UCS-2",                                1000 }, /* IANA MIBEnum 1000 */
	{ "csUnicode",										1000 }, /* IANA MIBEnum 1000 */
    { "ISO-10646-UCS-4",                                1001 }, /* IANA MIBEnum 1001 */
	{ "csUCS4",											1001 }, /* IANA MIBEnum 1001 */
    { "ISO-10646-UCS-Basic",                            1002 }, /* IANA MIBEnum 1002 */
	{ "csUnicodeASCII",									1002 }, /* IANA MIBEnum 1002 */
    { "ISO-10646-Unicode-Latin1",                       1003 }, /* IANA MIBEnum 1003 */
	{ "csUnicodeLatin1",								1003 }, /* IANA MIBEnum 1003 */
	{ "ISO-10646",										1003 }, /* IANA MIBEnum 1003 */
    { "ISO-8859-1",                                     1004 }, /* IANA MIBEnum 1004 */
	{ "csUnicodeIBM2039",								1004 }, /* IANA MIBEnum 1004 */
    { "ISO-Unicode-IBM-1261",                           1005 }, /* IANA MIBEnum 1005 */
	{ "csUnicodeIBM1261",								1005 }, /* IANA MIBEnum 1005 */
    { "ISO-Unicode-IBM-1268",                           1006 }, /* IANA MIBEnum 1006 */
	{ "csUnidoceIBM1268",								1006 }, /* IANA MIBEnum 1006 */
    { "ISO-Unicode-IBM-1276",                           1007 }, /* IANA MIBEnum 1007 */
	{ "csUnicodeIBM1276",								1007 }, /* IANA MIBEnum 1007 */
    { "ISO-Unicode-IBM-1264",                           1008 }, /* IANA MIBEnum 1008 */
	{ "csUnicodeIBM1264",								1008 }, /* IANA MIBEnum 1008 */
    { "ISO-Unicode-IBM-1265",                           1009 }, /* IANA MIBEnum 1009 */
	{ "csUnicodeIBM1265",								1009 }, /* IANA MIBEnum 1009 */
    { "UNICODE-1-1",                                    1010 }, /* IANA MIBEnum 1010 */
	{ "csUnicode11",									1010 }, /* IANA MIBEnum 1010 */
    { "ISO-8859-1-Windows-3.0-Latin-1",                 2000 }, /* IANA MIBEnum 2000 */
	{ "csWindows30Latin1",								2000 }, /* IANA MIBEnum 2000 */
    { "ISO-8859-1-Windows-3.1-Latin-1",                 2001 }, /* IANA MIBEnum 2001 */
	{ "csWindows31Latin1",								2001 }, /* IANA MIBEnum 2001 */
    { "ISO-8859-2-Windows-Latin-2",                     2002 }, /* IANA MIBEnum 2002 */
	{ "csWindows31Latin2",								2002 }, /* IANA MIBEnum 2002 */
    { "ISO-8859-9-Windows-Latin-5",                     2003 }, /* IANA MIBEnum 2003 */
	{ "csWindows31Latin5",								2003 }, /* IANA MIBEnum 2003 */
    { "hp-roman8",                                      2004 }, /* IANA MIBEnum 2004 */
	{ "roman8",											2004 }, /* IANA MIBEnum 2004 */
	{ "r8",												2004 }, /* IANA MIBEnum 2004 */
	{ "csHPRoman8",										2004 }, /* IANA MIBEnum 2004 */
    { "Adobe-Standard-Encoding",                        2005 }, /* IANA MIBEnum 2005 */
	{ "csAdobeStandardEncoding",						2005 }, /* IANA MIBEnum 2005 */
    { "Ventura-US",                                     2006 }, /* IANA MIBEnum 2006 */
	{ "csVenturaUS",									2006 }, /* IANA MIBEnum 2006 */
    { "Ventura-International",                          2007 }, /* IANA MIBEnum 2007 */
	{ "csVenturaInternational",							2007 }, /* IANA MIBEnum 2007 */
    { "DEC-MCS",                                        2008 }, /* IANA MIBEnum 2008 */
	{ "dec",											2008 }, /* IANA MIBEnum 2008 */
	{ "csDECMCS",										2008 }, /* IANA MIBEnum 2008 */
    { "IBM850",                                         2009 }, /* IANA MIBEnum 2009 */
	{ "cp850",											2009 }, /* IANA MIBEnum 2009 */
	{ "850",											2009 }, /* IANA MIBEnum 2009 */
	{ "csPC850Multilingual",							2009 }, /* IANA MIBEnum 2009 */
    { "IBM852",                                         2010 }, /* IANA MIBEnum 2010 */
	{ "cp852",											2010 }, /* IANA MIBEnum 2010 */
	{ "852",											2010 }, /* IANA MIBEnum 2010 */
	{ "csPCp852",										2010 }, /* IANA MIBEnum 2010 */
    { "IBM437",                                         2011 }, /* IANA MIBEnum 2011 */
	{ "cp437",											2011 }, /* IANA MIBEnum 2011 */
	{ "437",											2011 }, /* IANA MIBEnum 2011 */
	{ "csPC8CodePage437",								2011 }, /* IANA MIBEnum 2011 */
    { "PC8-Danish-Norwegian",                           2012 }, /* IANA MIBEnum 2012 */
	{ "csPC8DanishNorwegian",							2012 }, /* IANA MIBEnum 2012 */
    { "IBM862",                                         2013 }, /* IANA MIBEnum 2013 */
	{ "cp862",											2013 }, /* IANA MIBEnum 2013 */
	{ "862",											2013 }, /* IANA MIBEnum 2013 */
	{ "csPC862LatinHebrew",								2013 }, /* IANA MIBEnum 2013 */
    { "PC8-Turkish",                                    2014 }, /* IANA MIBEnum 2014 */
	{ "csPC8Turkish",									2014 }, /* IANA MIBEnum 2014 */
    { "IBM-Symbols",                                    2015 }, /* IANA MIBEnum 2015 */
	{ "csIBMSymbols",									2015 }, /* IANA MIBEnum 2015 */
    { "IBM-Thai",                                       2016 }, /* IANA MIBEnum 2016 */
	{ "csIBMThai",										2016 }, /* IANA MIBEnum 2016 */
    { "HP-Legal",                                       2017 }, /* IANA MIBEnum 2017 */
	{ "csHPLegal",										2017 }, /* IANA MIBEnum 2017 */
    { "HP-Pi-font",                                     2018 }, /* IANA MIBEnum 2018 */
	{ "csHPPiFont",										2018 }, /* IANA MIBEnum 2018 */
    { "HP-Math8",                                       2019 }, /* IANA MIBEnum 2019 */
	{ "csHPMath8",										2019 }, /* IANA MIBEnum 2019 */
    { "Adobe-Symbol-Encoding",                          2020 }, /* IANA MIBEnum 2020 */
	{ "csHPPSMath",										2020 }, /* IANA MIBEnum 2020 */
    { "HP-DeskTop",                                     2021 }, /* IANA MIBEnum 2021 */
	{ "csHPDesktop",									2021 }, /* IANA MIBEnum 2021 */
    { "Ventura-Math",                                   2022 }, /* IANA MIBEnum 2022 */
	{ "csVenturaMath",									2022 }, /* IANA MIBEnum 2022 */
    { "Microsoft-Publishing",                           2023 }, /* IANA MIBEnum 2023 */
	{ "csMicrosoftPublishing",							2023 }, /* IANA MIBEnum 2023 */
    { "Windows-31J",                                    2024 }, /* IANA MIBEnum 2024 */
	{ "csWindows31J",									2024 }, /* IANA MIBEnum 2024 */
    { "GB2312",                                         2025 }, /* IANA MIBEnum 2025 */
	{ "csGB2312",										2025 }, /* IANA MIBEnum 2025 */
    { "Big5",                                           2026 }, /* IANA MIBEnum 2026 */
	{ "csBig5",											2026 }, /* IANA MIBEnum 2026 */
    { "macintosh",                                      2027 }, /* IANA MIBEnum 2027 */
	{ "mac",											2027 }, /* IANA MIBEnum 2027 */
	{ "csMacintosh",									2027 }, /* IANA MIBEnum 2027 */
    { "IBM037",                                         2028 }, /* IANA MIBEnum 2028 */
	{ "cp037",											2028 }, /* IANA MIBEnum 2028 */
	{ "ebcdic-cp-us",									2028 }, /* IANA MIBEnum 2028 */
	{ "ebcdic-cp-ca",									2028 }, /* IANA MIBEnum 2028 */
	{ "ebcdic-cp-wt",									2028 }, /* IANA MIBEnum 2028 */
	{ "ebcdic-cp-nl",									2028 }, /* IANA MIBEnum 2028 */
	{ "csIBM037",										2028 }, /* IANA MIBEnum 2028 */
    { "IBM038",                                         2029 }, /* IANA MIBEnum 2029 */
	{ "EBCDIC-INT",										2029 }, /* IANA MIBEnum 2029 */
	{ "cp038",											2029 }, /* IANA MIBEnum 2029 */
	{ "csIBM038",										2029 }, /* IANA MIBEnum 2029 */
    { "IBM273",                                         2030 }, /* IANA MIBEnum 2030 */
	{ "CP273",											2030 }, /* IANA MIBEnum 2030 */
	{ "csIBM273",										2030 }, /* IANA MIBEnum 2030 */
    { "IBM274",                                         2031 }, /* IANA MIBEnum 2031 */
	{ "EBCDIC-BE",										2031 }, /* IANA MIBEnum 2031 */
	{ "CP274",											2031 }, /* IANA MIBEnum 2031 */
	{ "csIBM274",										2031 }, /* IANA MIBEnum 2031 */
    { "IBM275",                                         2032 }, /* IANA MIBEnum 2032 */
	{ "EBCDIC-BR",										2032 }, /* IANA MIBEnum 2032 */
	{ "cp275",											2032 }, /* IANA MIBEnum 2032 */
	{ "csIBM275",										2032 }, /* IANA MIBEnum 2032 */
    { "IBM277",                                         2033 }, /* IANA MIBEnum 2033 */
	{ "EBCDIC-CP-DK",									2033 }, /* IANA MIBEnum 2033 */
	{ "EBCDIC-CP-NO",									2033 }, /* IANA MIBEnum 2033 */
	{ "csIBM277",										2033 }, /* IANA MIBEnum 2033 */
    { "IBM278",                                         2034 }, /* IANA MIBEnum 2034 */
	{ "CP278",											2034 }, /* IANA MIBEnum 2034 */
	{ "ebcdic-cp-fi",									2034 }, /* IANA MIBEnum 2034 */
	{ "ebcdic-cp-se",									2034 }, /* IANA MIBEnum 2034 */
	{ "csIBM278",										2034 }, /* IANA MIBEnum 2034 */
    { "IBM280",                                         2035 }, /* IANA MIBEnum 2035 */
	{ "CP280",											2035 }, /* IANA MIBEnum 2035 */
	{ "ebcdic-cp-it",									2035 }, /* IANA MIBEnum 2035 */
	{ "csIBM280",										2035 }, /* IANA MIBEnum 2035 */
    { "IBM281",                                         2036 }, /* IANA MIBEnum 2036 */
	{ "EBCDIC-JP-E",									2036 }, /* IANA MIBEnum 2036 */
	{ "cp281",											2036 }, /* IANA MIBEnum 2036 */
	{ "csIBM281",										2036 }, /* IANA MIBEnum 2036 */
    { "IBM284",                                         2037 }, /* IANA MIBEnum 2037 */
	{ "CP284",											2037 }, /* IANA MIBEnum 2037 */
	{ "ebcdic-cp-es",									2037 }, /* IANA MIBEnum 2037 */
	{ "csIBM284",										2037 }, /* IANA MIBEnum 2037 */
    { "IBM285",                                         2038 }, /* IANA MIBEnum 2038 */
	{ "CP285",											2038 }, /* IANA MIBEnum 2038 */
	{ "ebcdic-cp-gb",									2038 }, /* IANA MIBEnum 2038 */
	{ "csIBM285",										2038 }, /* IANA MIBEnum 2038 */
    { "IBM290",                                         2039 }, /* IANA MIBEnum 2039 */
	{ "cp290",											2039 }, /* IANA MIBEnum 2039 */
	{ "EBCDIC-JP-kana",									2039 }, /* IANA MIBEnum 2039 */
	{ "csIBM290",										2039 }, /* IANA MIBEnum 2039 */
    { "IBM297",                                         2040 }, /* IANA MIBEnum 2040 */
	{ "cp297",											2040 }, /* IANA MIBEnum 2040 */
	{ "ebcdic-cp-fr",									2040 }, /* IANA MIBEnum 2040 */
	{ "csIBM297",										2040 }, /* IANA MIBEnum 2040 */
    { "IBM420",                                         2041 }, /* IANA MIBEnum 2041 */
	{ "cp420",											2041 }, /* IANA MIBEnum 2041 */
	{ "ebcdic-cp-ar1",									2041 }, /* IANA MIBEnum 2041 */
	{ "csIBM420",										2041 }, /* IANA MIBEnum 2041 */
    { "IBM423",                                         2042 }, /* IANA MIBEnum 2042 */
	{ "cp423",											2042 }, /* IANA MIBEnum 2042 */
	{ "ebcdic-cp-gr",									2042 }, /* IANA MIBEnum 2042 */
	{ "csIBM423",										2042 }, /* IANA MIBEnum 2042 */
    { "IBM424",                                         2043 }, /* IANA MIBEnum 2043 */
	{ "cp424",											2043 }, /* IANA MIBEnum 2043 */
	{ "ebcdic-cp-he",									2043 }, /* IANA MIBEnum 2043 */
	{ "csIBM424",										2043 }, /* IANA MIBEnum 2043 */
    { "IBM500",                                         2044 }, /* IANA MIBEnum 2044 */
	{ "CP500",											2044 }, /* IANA MIBEnum 2044 */
	{ "ebcdic-cp-be",									2044 }, /* IANA MIBEnum 2044 */
	{ "ebcdic-cp-ch",									2044 }, /* IANA MIBEnum 2044 */
	{ "csIBM500",										2044 }, /* IANA MIBEnum 2044 */
    { "IBM851",                                         2045 }, /* IANA MIBEnum 2045 */
	{ "cp851",											2045 }, /* IANA MIBEnum 2045 */
	{ "851",											2045 }, /* IANA MIBEnum 2045 */
	{ "csIBM851",										2045 }, /* IANA MIBEnum 2045 */
    { "IBM855",                                         2046 }, /* IANA MIBEnum 2046 */
	{ "cp855",											2046 }, /* IANA MIBEnum 2046 */
	{ "855",											2046 }, /* IANA MIBEnum 2046 */
	{ "csIBM855",										2046 }, /* IANA MIBEnum 2046 */
    { "IBM857",                                         2047 }, /* IANA MIBEnum 2047 */
	{ "cp857",											2047 }, /* IANA MIBEnum 2047 */
	{ "857",											2047 }, /* IANA MIBEnum 2047 */
	{ "csIBM857",										2047 }, /* IANA MIBEnum 2047 */
    { "IBM860",                                         2048 }, /* IANA MIBEnum 2048 */
	{ "cp860",											2048 }, /* IANA MIBEnum 2048 */
	{ "860",											2048 }, /* IANA MIBEnum 2048 */
	{ "csIBM860",										2048 }, /* IANA MIBEnum 2048 */
    { "IBM861",                                         2049 }, /* IANA MIBEnum 2049 */
	{ "cp861",											2049 }, /* IANA MIBEnum 2049 */
	{ "861",											2049 }, /* IANA MIBEnum 2049 */
	{ "cp-is",											2049 }, /* IANA MIBEnum 2049 */
	{ "csIBM861",										2049 }, /* IANA MIBEnum 2049 */
    { "IBM863",                                         2050 }, /* IANA MIBEnum 2050 */
	{ "cp863",											2050 }, /* IANA MIBEnum 2050 */
	{ "863",											2050 }, /* IANA MIBEnum 2050 */
	{ "csIBM863",										2050 }, /* IANA MIBEnum 2050 */
    { "IBM864",                                         2051 }, /* IANA MIBEnum 2051 */
	{ "cp864",											2051 }, /* IANA MIBEnum 2051 */
	{ "csIBM864",										2051 }, /* IANA MIBEnum 2051 */
    { "IBM865",                                         2052 }, /* IANA MIBEnum 2052 */
	{ "cp865",											2052 }, /* IANA MIBEnum 2052 */
	{ "865",											2052 }, /* IANA MIBEnum 2052 */
	{ "csIBM865",										2052 }, /* IANA MIBEnum 2052 */
    { "IBM868",                                         2053 }, /* IANA MIBEnum 2053 */
	{ "CP868",											2053 }, /* IANA MIBEnum 2053 */
	{ "cp-ar",											2053 }, /* IANA MIBEnum 2053 */
	{ "csIBM868",										2053 }, /* IANA MIBEnum 2053 */
    { "IBM869",                                         2054 }, /* IANA MIBEnum 2054 */
	{ "cp869",											2054 }, /* IANA MIBEnum 2054 */
	{ "869",											2054 }, /* IANA MIBEnum 2054 */
	{ "cp-gr",											2054 }, /* IANA MIBEnum 2054 */
	{ "csIBM869",										2054 }, /* IANA MIBEnum 2054 */
    { "IBM870",                                         2055 }, /* IANA MIBEnum 2055 */
	{ "CP870",											2055 }, /* IANA MIBEnum 2055 */
	{ "ebcdic-cp-roece",								2055 }, /* IANA MIBEnum 2055 */
	{ "ebcdic-cp-yu",									2055 }, /* IANA MIBEnum 2055 */
	{ "csIBM870",										2055 }, /* IANA MIBEnum 2055 */
    { "IBM871",                                         2056 }, /* IANA MIBEnum 2056 */
	{ "CP871",											2056 }, /* IANA MIBEnum 2056 */
	{ "ebcdic-cp-is",									2056 }, /* IANA MIBEnum 2056 */
	{ "csIBM871",										2056 }, /* IANA MIBEnum 2056 */
    { "IBM880",                                         2057 }, /* IANA MIBEnum 2057 */
	{ "cp880",											2057 }, /* IANA MIBEnum 2057 */
	{ "EBCDIC-Cyrillic",								2057 }, /* IANA MIBEnum 2057 */
	{ "csIBM880",										2057 }, /* IANA MIBEnum 2057 */
    { "IBM891",                                         2058 }, /* IANA MIBEnum 2058 */
	{ "cp891",											2058 }, /* IANA MIBEnum 2058 */
	{ "csIBM891",										2058 }, /* IANA MIBEnum 2058 */
    { "IBM903",                                         2059 }, /* IANA MIBEnum 2059 */
	{ "cp903",											2059 }, /* IANA MIBEnum 2059 */
	{ "csIBM903",										2059 }, /* IANA MIBEnum 2059 */
    { "IBM904",                                         2060 }, /* IANA MIBEnum 2060 */
	{ "cp904",											2060 }, /* IANA MIBEnum 2060 */
	{ "904",											2060 }, /* IANA MIBEnum 2060 */
	{ "csIBBM904",										2060 }, /* IANA MIBEnum 2060 */
    { "IBM905",                                         2061 }, /* IANA MIBEnum 2061 */
	{ "CP905",											2061 }, /* IANA MIBEnum 2061 */
	{ "ebcdic-cp-tr",									2061 }, /* IANA MIBEnum 2061 */
	{ "csIBM905",										2061 }, /* IANA MIBEnum 2061 */
    { "IBM918",                                         2062 }, /* IANA MIBEnum 2062 */
	{ "CP918",											2062 }, /* IANA MIBEnum 2062 */
	{ "ebcdic-cp-ar2",									2062 }, /* IANA MIBEnum 2062 */
	{ "csIBM918",										2062 }, /* IANA MIBEnum 2062 */
    { "IBM1026",                                        2063 }, /* IANA MIBEnum 2063 */
	{ "CP1026",											2063 }, /* IANA MIBEnum 2063 */
	{ "csIBM1026",										2063 }, /* IANA MIBEnum 2063 */
    { "EBCDIC-AT-DE",                                   2064 }, /* IANA MIBEnum 2064 */
	{ "csIBMEBCDICATDE",								2064 }, /* IANA MIBEnum 2064 */
    { "EBCDIC-AT-DE-A",                                 2065 }, /* IANA MIBEnum 2065 */
	{ "csEBCDICATDEA",									2065 }, /* IANA MIBEnum 2065 */
    { "EBCDIC-CA-FR",                                   2066 }, /* IANA MIBEnum 2066 */
	{ "csEBCDICCAFR",									2066 }, /* IANA MIBEnum 2066 */
    { "EBCDIC-DK-NO",                                   2067 }, /* IANA MIBEnum 2067 */
	{ "csEBCDICDKNO",									2067 }, /* IANA MIBEnum 2067 */
    { "EBCDIC-DK-NO-A",                                 2068 }, /* IANA MIBEnum 2068 */
	{ "csEBCDICDKNOA",									2068 }, /* IANA MIBEnum 2068 */
    { "EBCDIC-FI-SE",                                   2069 }, /* IANA MIBEnum 2069 */
	{ "csEBCDICFISE",									2069 }, /* IANA MIBEnum 2069 */
    { "EBCDIC-FI-SE-A",                                 2070 }, /* IANA MIBEnum 2070 */
	{ "csEBCDICFISEA",									2070 }, /* IANA MIBEnum 2070 */
    { "EBCDIC-FR",                                      2071 }, /* IANA MIBEnum 2071 */
	{ "csEBCDICFR",										2071 }, /* IANA MIBEnum 2071 */
    { "EBCDIC-IT",                                      2072 }, /* IANA MIBEnum 2072 */
	{ "csEBCDICIT",										2072 }, /* IANA MIBEnum 2072 */
    { "EBCDIC-PT",                                      2073 }, /* IANA MIBEnum 2073 */
    { "csEBCDICPT",										2073 }, /* IANA MIBEnum 2073 */
    { "EBCDIC-ES",                                      2074 }, /* IANA MIBEnum 2074 */
	{ "csEBCDICES",										2074 }, /* IANA MIBEnum 2074 */
    { "EBCDIC-ES-A",                                    2075 }, /* IANA MIBEnum 2075 */
	{ "csEBCDICESA",									2075 }, /* IANA MIBEnum 2075 */
    { "EBCDIC-ES-S",		                            2076 }, /* IANA MIBEnum 2076 */
	{ "csEBCDICESS",									2076 }, /* IANA MIBEnum 2076 */
    { "EBCDIC-UK",                                      2077 }, /* IANA MIBEnum 2077 */
	{ "csEBCDICUK",										2077 }, /* IANA MIBEnum 2077 */
    { "EBCDIC-US",                                      2078 }, /* IANA MIBEnum 2078 */
	{ "csEBCDICUS",										2078 }, /* IANA MIBEnum 2078 */
    { "UNKNOWN-8BIT",                                   2079 }, /* IANA MIBEnum 2079 */
	{ "csUnknown8BiT",									2079 }, /* IANA MIBEnum 2079 */
    { "MNEMONIC",                                       2080 }, /* IANA MIBEnum 2080 */
	{ "csMnemonic",										2080 }, /* IANA MIBEnum 2080 */
    { "MNEM",                                           2081 }, /* IANA MIBEnum 2081 */
	{ "csMnem",											2081 }, /* IANA MIBEnum 2081 */
    { "VISCII",                                         2082 }, /* IANA MIBEnum 2082 */
	{ "csVISCII",										2082 }, /* IANA MIBEnum 2082 */
    { "VIQR",                                           2083 }, /* IANA MIBEnum 2083 */
	{ "csVIQR",											2083 }, /* IANA MIBEnum 2083 */
    { "KOI8-R",                                         2084 }, /* IANA MIBEnum 2084 */
	{ "csKOI8R",										2084 }, /* IANA MIBEnum 2084 */
    { "HZ-GB-2312",                                     2085 }, /* IANA MIBEnum 2085 */
    { "IBM866",                                         2086 }, /* IANA MIBEnum 2086 */
	{ "cp866",											2086 }, /* IANA MIBEnum 2086 */
	{ "866",											2086 }, /* IANA MIBEnum 2086 */
	{ "csIBM866",										2086 }, /* IANA MIBEnum 2086 */
    { "IBM775",                                         2087 }, /* IANA MIBEnum 2087 */
	{ "cp775",											2087 }, /* IANA MIBEnum 2087 */
	{ "csPC775Baltic",									2087 }, /* IANA MIBEnum 2087 */
    { "KOI8-U",                                         2088 }, /* IANA MIBEnum 2088 */
    { "windows-1250",                                   2250 }, /* IANA MIBEnum 2250 */
    { "windows-1251",                                   2251 }, /* IANA MIBEnum 2251 */
    { "windows-1253",                                   2253 }, /* IANA MIBEnum 2253 */
    { "windows-1254",                                   2254 }, /* IANA MIBEnum 2254 */
    { "windows-1255",                                   2255 }, /* IANA MIBEnum 2255 */
    { "windows-1256",                                   2256 }, /* IANA MIBEnum 2256 */
    { "windows-1257",                                   2257 }, /* IANA MIBEnum 2257 */
    { "windows-1258",                                   2258 }, /* IANA MIBEnum 2258 */
    { "TIS-620",                                        2259 }, /* IANA MIBEnum 2259 */
	/* ISO_8859-15 Charset missing. It hasn't got MIBEnum. */
};

/* Base64 character-byte assignments. */
static const char g_aBase64Assignments[] =
{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 
    'w', 'x', 'y', 'z', '0', '1', '2', '3', 
    '4', '5', '6', '7', '8', '9', '+', '/', 
};

/*
-----------------------------------------------------------------------------
    Helper functions
-----------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
    Looks for the given name in the given table. If it is found, the 
    corresponding word value is filled in <pwValue>, otherwise <pwValue>
    remains unchanged.
    The table must be ordered by names.
    Returns TRUE if the name is found, otherwise FALSE.
-----------------------------------------------------------------------------
*/
BOOL HC_Name2WordValue(
    const NameWordPair  aPairs[],       /* in:  Table of name-value pairs. */
    int                 nNumPairs,      /* in:  Number of elements in table. */
    BOOL                bIsLowerCase,   /* in:  Tells whether the table contains 
                                                only lowercase names or not. */
    const char*         pcName,         /* in:  Name to look for. */
    WORD*               pwValue)        /* out: The corresponding word value. */
{
    BOOL    bFound  = FALSE;
    int     nLow    = 0, 
            nHigh   = nNumPairs - 1, 
            nMid;

    while (!bFound && nLow <= nHigh)
    {
        int nCmp;

        nMid = (nLow + nHigh) / 2;
        if (bIsLowerCase)
        {
            nCmp = HC_stricmp2 (pcName, aPairs[nMid].i_pcName);
        }
        else
        {
            nCmp = HC_stricmp (pcName, aPairs[nMid].i_pcName);
        }
        if (nCmp == 0)
        {
            bFound = TRUE;
            *pwValue = aPairs[nMid].i_wValue;
        }
        else if (nCmp < 0)
        {
            nHigh = nMid - 1;
        }
        else
        {
            nLow = nMid + 1;
        }
    }
    return bFound;
} /* HC_Name2WordValue */

/*
-----------------------------------------------------------------------------
    Looks for the given word value in the given table.
    The table must be ordered by numeric values.
    Returns a pointer to the name if it is found, otherwise NULL.
-----------------------------------------------------------------------------
*/
const char* HC_WordValue2Name(
    const NameWordPair  aPairs[],   /* in:  Table of name-value pairs. */
    int                 nNumPairs,  /* in:  Number of elements in table. */
    WORD                wValue)     /* in:  Word value to look for. */
{
    int         nLow    = 0, 
                nHigh   = nNumPairs - 1, 
                nMid    = 0;
    BOOL        bFound  = FALSE;
    const char* pcRet   = NULL;

    while (!bFound && nLow <= nHigh)
    {
        nMid = (nLow + nHigh) / 2;
        if (wValue == aPairs[nMid].i_wValue)
        {
            bFound = TRUE;
            pcRet = aPairs[nMid].i_pcName;
        }
        else if (wValue < aPairs[nMid].i_wValue)
        {
            nHigh = nMid - 1;
        }
        else
        {
            nLow = nMid + 1;
        }
    }

	/* OK. Now search the first in the table (be deterministic). */
	while (nLow < nMid)
	{
		if (wValue == aPairs[nMid-1].i_wValue)
		{
			nMid--;
			pcRet = aPairs[nMid].i_pcName;
		}
		else
		{
			nMid = nLow;
		}
	}

    return pcRet;
} /* HC_WordValue2Name */


/*
-----------------------------------------------------------------------------
    Implementation
-----------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
*/
BOOL HC_ContentTypeName2ContentTypeCode(
    const char*   pcName,   /* in:  Content type name. */
    HC_WapVersion version,  /* in:  Version to comply with */
    WORD*         pwValue)  /* out: The corresponding content type code. */
{
    /*  
     * Content Type assignments/1: Content Type names to numbers. 
     * The table must be ordered by names.
     */
    static const NameWordPair aCntTypeName2Num1_1[] = {
        { "*/*",                                        0x00 },
        { "application/*",                              0x10 },
        { "application/java-vm",                        0x11 },
        { "application/vnd.wap.multipart.*",            0x22 },
        { "application/vnd.wap.multipart.alternative",  0x26 },
        { "application/vnd.wap.multipart.byteranges",   0x25 },
        { "application/vnd.wap.multipart.form-data",    0x24 },
        { "application/vnd.wap.multipart.mixed",        0x23 },
        { "application/vnd.wap.multipart.related",      0x33 },
        { "application/vnd.wap.sia",                    0x34 },
        { "application/vnd.wap.sic",                    0x2E },
        { "application/vnd.wap.slc",                    0x30 },
        { "application/vnd.wap.uaprof",                 0x17 },
        { "application/vnd.wap.wbxml",                  0x29 },
        { "application/vnd.wap.wmlc",                   0x14 },
        { "application/vnd.wap.wmlscriptc",             0x15 },
        { "application/vnd.wap.wta-eventc",             0x16 },
        { "application/vnd.wap.wtls-ca-certificate",    0x18 },
        { "application/vnd.wap.wtls-user-certificate",  0x19 },
        { "application/x-hdmlc",                        0x13 },
        { "application/x-www-form-urlencoded",          0x12 },
        { "application/x-x509-ca-cert",                 0x1A },
        { "application/x-x509-user-cert",               0x1B },
        { "application/x-x968-ca-cert",                 0x2B },
        { "application/x-x968-cross-cert",              0x2A },
        { "application/x-x968-user-cert",               0x2C },
        { "application/xml",                            0x27 },
        { "image/*",                                    0x1C },
        { "image/gif",                                  0x1D },
        { "image/jpeg",                                 0x1E },
        { "image/png",                                  0x20 },
        { "image/tiff",                                 0x1F },
        { "image/vnd.wap.wbmp",                         0x21 },
        { "multipart/*",                                0x0B },
        { "multipart/alternative",                      0x0F },
        { "multipart/byteranges",                       0x0E },
        { "multipart/form-data",                        0x0D },
        { "multipart/mixed",                            0x0C },
        { "text/*",                                     0x01 },
        { "text/html",                                  0x02 },
        { "text/plain",                                 0x03 },
        { "text/vnd.wap.wml",                           0x08 },
        { "text/vnd.wap.wmlscript",                     0x09 },
        { "text/vnd.wap.wta-event",                     0x0A },
        { "text/x-hdml",                                0x04 },
        { "text/x-ttml",                                0x05 },
        { "text/x-vCalendar",                           0x06 },
        { "text/x-vCard",                               0x07 },
        { "text/xml",                                   0x28 },
    };

    static const NameWordPair aCntTypeName2Num1_2[] = {
        { "*/*",                                        0x00 },
        { "application/*",                              0x10 },
        { "application/java-vm",                        0x11 },
        { "application/vnd.wap.coc",                    0x32 },
        { "application/vnd.wap.multipart.*",            0x22 },
        { "application/vnd.wap.multipart.alternative",  0x26 },
        { "application/vnd.wap.multipart.byteranges",   0x25 },
        { "application/vnd.wap.multipart.form-data",    0x24 },
        { "application/vnd.wap.multipart.mixed",        0x23 },
        { "application/vnd.wap.multipart.related",      0x33 },
        { "application/vnd.wap.sia",                    0x34 },
        { "application/vnd.wap.sic",                    0x2E },
        { "application/vnd.wap.slc",                    0x30 },
        { "application/vnd.wap.uaprof",                 0x17 },
        { "application/vnd.wap.wbxml",                  0x29 },
        { "application/vnd.wap.wmlc",                   0x14 },
        { "application/vnd.wap.wmlscriptc",             0x15 },
        { "application/vnd.wap.wta-eventc",             0x16 },
        { "application/vnd.wap.wtls-ca-certificate",    0x18 },
        { "application/vnd.wap.wtls-user-certificate",  0x19 },
        { "application/x-hdmlc",                        0x13 },
        { "application/x-www-form-urlencoded",          0x12 },
        { "application/x-x509-ca-cert",                 0x1A },
        { "application/x-x509-user-cert",               0x1B },
        { "application/x-x968-ca-cert",                 0x2B },
        { "application/x-x968-cross-cert",              0x2A },
        { "application/x-x968-user-cert",               0x2C },
        { "application/xml",                            0x27 },
        { "image/*",                                    0x1C },
        { "image/gif",                                  0x1D },
        { "image/jpeg",                                 0x1E },
        { "image/png",                                  0x20 },
        { "image/tiff",                                 0x1F },
        { "image/vnd.wap.wbmp",                         0x21 },
        { "multipart/*",                                0x0B },
        { "multipart/alternative",                      0x0F },
        { "multipart/byteranges",                       0x0E },
        { "multipart/form-data",                        0x0D },
        { "multipart/mixed",                            0x0C },
        { "text/*",                                     0x01 },
        { "text/html",                                  0x02 },
        { "text/plain",                                 0x03 },
        { "text/vnd.wap.co",                            0x31 },
        { "text/vnd.wap.si",                            0x2D },
        { "text/vnd.wap.sl",                            0x2F },
        { "text/vnd.wap.wml",                           0x08 },
        { "text/vnd.wap.wmlscript",                     0x09 },
        { "text/vnd.wap.wta-event",                     0x0A },
        { "text/x-hdml",                                0x04 },
        { "text/x-ttml",                                0x05 },
        { "text/x-vCalendar",                           0x06 },
        { "text/x-vCard",                               0x07 },
        { "text/xml",                                   0x28 },
    };

    static const NameWordPair aCntTypeName2Num1_3[] = {
        { "*/*",                                        0x00 },
        { "application/*",                              0x10 },
        { "application/java-vm",                        0x11 },
        { "application/vnd.wap.coc",                    0x32 },
        { "application/vnd.wap.connectivity-wbxml",     0x36 },
        { "application/vnd.wap.multipart.*",            0x22 },
        { "application/vnd.wap.multipart.alternative",  0x26 },
        { "application/vnd.wap.multipart.byteranges",   0x25 },
        { "application/vnd.wap.multipart.form-data",    0x24 },
        { "application/vnd.wap.multipart.mixed",        0x23 },
        { "application/vnd.wap.multipart.related",      0x33 },
        { "application/vnd.wap.sia",                    0x34 },
        { "application/vnd.wap.sic",                    0x2E },
        { "application/vnd.wap.slc",                    0x30 },
        { "application/vnd.wap.uaprof",                 0x17 },
        { "application/vnd.wap.wbxml",                  0x29 },
        { "application/vnd.wap.wmlc",                   0x14 },
        { "application/vnd.wap.wmlscriptc",             0x15 },
        { "application/vnd.wap.wta-eventc",             0x16 },
        { "application/vnd.wap.wtls-ca-certificate",    0x18 },
        { "application/vnd.wap.wtls-user-certificate",  0x19 },
        { "application/x-hdmlc",                        0x13 },
        { "application/x-www-form-urlencoded",          0x12 },
        { "application/x-x509-ca-cert",                 0x1A },
        { "application/x-x509-user-cert",               0x1B },
        { "application/x-x968-ca-cert",                 0x2B },
        { "application/x-x968-cross-cert",              0x2A },
        { "application/x-x968-user-cert",               0x2C },
        { "application/xml",                            0x27 },
        { "image/*",                                    0x1C },
        { "image/gif",                                  0x1D },
        { "image/jpeg",                                 0x1E },
        { "image/png",                                  0x20 },
        { "image/tiff",                                 0x1F },
        { "image/vnd.wap.wbmp",                         0x21 },
        { "multipart/*",                                0x0B },
        { "multipart/alternative",                      0x0F },
        { "multipart/byteranges",                       0x0E },
        { "multipart/form-data",                        0x0D },
        { "multipart/mixed",                            0x0C },
        { "text/*",                                     0x01 },
        { "text/html",                                  0x02 },
        { "text/plain",                                 0x03 },
        { "text/vnd.wap.co",                            0x31 },
        { "text/vnd.wap.connectivity-xml",              0x35 },
        { "text/vnd.wap.si",                            0x2D },
        { "text/vnd.wap.sl",                            0x2F },
        { "text/vnd.wap.wml",                           0x08 },
        { "text/vnd.wap.wmlscript",                     0x09 },
        { "text/vnd.wap.wta-event",                     0x0A },
        { "text/x-hdml",                                0x04 },
        { "text/x-ttml",                                0x05 },
        { "text/x-vCalendar",                           0x06 },
        { "text/x-vCard",                               0x07 },
        { "text/xml",                                   0x28 },
    };

    static const NameWordPair aCntTypeName2Num1_4[] = {
        { "*/*",                                        0x00 },
        { "application/*",                              0x10 },
        { "application/java-vm",                        0x11 },
        { "application/pkcs7-mime",                     0x37 },
        { "application/vnd.wap.cert-response",          0x3A },
        { "application/vnd.wap.coc",                    0x32 },
        { "application/vnd.wap.connectivity-wbxml",     0x36 },
        { "application/vnd.wap.hashed-certificate",     0x38 },
        { "application/vnd.wap.mms-message",            0x3E },
        { "application/vnd.wap.multipart.*",            0x22 },
        { "application/vnd.wap.multipart.alternative",  0x26 },
        { "application/vnd.wap.multipart.byteranges",   0x25 },
        { "application/vnd.wap.multipart.form-data",    0x24 },
        { "application/vnd.wap.multipart.mixed",        0x23 },
        { "application/vnd.wap.multipart.related",      0x33 },
        { "application/vnd.wap.rollover-certificate",   0x3F },
        { "application/vnd.wap.sia",                    0x34 },
        { "application/vnd.wap.sic",                    0x2E },
        { "application/vnd.wap.signed-certificate",     0x39 },
        { "application/vnd.wap.slc",                    0x30 },
        { "application/vnd.wap.uaprof",                 0x17 },
        { "application/vnd.wap.wbxml",                  0x29 },
        { "application/vnd.wap.wmlc",                   0x14 },
        { "application/vnd.wap.wmlscriptc",             0x15 },
        { "application/vnd.wap.wta-eventc",             0x16 },
        { "application/vnd.wap.wtls-ca-certificate",    0x18 },
        { "application/vnd.wap.wtls-user-certificate",  0x19 },
        { "application/wml+xml",                        0x3C },
        { "application/x-hdmlc",                        0x13 },
        { "application/x-www-form-urlencoded",          0x12 },
        { "application/x-x509-ca-cert",                 0x1A },
        { "application/x-x509-user-cert",               0x1B },
        { "application/x-x968-ca-cert",                 0x2B },
        { "application/x-x968-cross-cert",              0x2A },
        { "application/x-x968-user-cert",               0x2C },
        { "application/xhtml+xml",                      0x3B },
        { "application/xml",                            0x27 },
        { "image/*",                                    0x1C },
        { "image/gif",                                  0x1D },
        { "image/jpeg",                                 0x1E },
        { "image/png",                                  0x20 },
        { "image/tiff",                                 0x1F },
        { "image/vnd.wap.wbmp",                         0x21 },
        { "multipart/*",                                0x0B },
        { "multipart/alternative",                      0x0F },
        { "multipart/byteranges",                       0x0E },
        { "multipart/form-data",                        0x0D },
        { "multipart/mixed",                            0x0C },
        { "text/*",                                     0x01 },
        { "text/css",                                   0x3D },
        { "text/html",                                  0x02 },
        { "text/plain",                                 0x03 },
        { "text/vnd.wap.co",                            0x31 },
        { "text/vnd.wap.connectivity-xml",              0x35 },
        { "text/vnd.wap.si",                            0x2D },
        { "text/vnd.wap.sl",                            0x2F },
        { "text/vnd.wap.wml",                           0x08 },
        { "text/vnd.wap.wmlscript",                     0x09 },
        { "text/vnd.wap.wta-event",                     0x0A },
        { "text/x-hdml",                                0x04 },
        { "text/x-ttml",                                0x05 },
        { "text/x-vCalendar",                           0x06 },
        { "text/x-vCard",                               0x07 },
        { "text/xml",                                   0x28 },
    };

    static const NameWordPair aCntTypeName2Num1_5[] = {
        { "*/*",                                        0x00 },
        { "application/*",                              0x10 },
        { "application/java-vm",                        0x11 },
        { "application/pkcs7-mime",                     0x37 },
        { "application/vnd.syncml.dm+wbxml",            0x42 },
        { "application/vnd.syncml.dm+xml",              0x43 },
        { "application/vnd.syncml.notification",        0x44 },
        { "application/vnd.wap.cert-response",          0x3A },
        { "application/vnd.wap.coc",                    0x32 },
        { "application/vnd.wap.connectivity-wbxml",     0x36 },
        { "application/vnd.wap.hashed-certificate",     0x38 },
        { "application/vnd.wap.loc+xml",                0x41 },
        { "application/vnd.wap.locc+wbxml",             0x40 },
        { "application/vnd.wap.mms-message",            0x3E },
        { "application/vnd.wap.multipart.*",            0x22 },
        { "application/vnd.wap.multipart.alternative",  0x26 },
        { "application/vnd.wap.multipart.byteranges",   0x25 },
        { "application/vnd.wap.multipart.form-data",    0x24 },
        { "application/vnd.wap.multipart.mixed",        0x23 },
        { "application/vnd.wap.multipart.related",      0x33 },
        { "application/vnd.wap.rollover-certificate",   0x3F },
        { "application/vnd.wap.sia",                    0x34 },
        { "application/vnd.wap.sic",                    0x2E },
        { "application/vnd.wap.signed-certificate",     0x39 },
        { "application/vnd.wap.slc",                    0x30 },
        { "application/vnd.wap.uaprof",                 0x17 },
        { "application/vnd.wap.wbxml",                  0x29 },
        { "application/vnd.wap.wmlc",                   0x14 },
        { "application/vnd.wap.wmlscriptc",             0x15 },
        { "application/vnd.wap.wta-eventc",             0x16 },
        { "application/vnd.wap.wtls-ca-certificate",    0x18 },
        { "application/vnd.wap.wtls-user-certificate",  0x19 },
        { "application/vnd.wap.xhtml+xml",              0x45 },
        { "application/wml+xml",                        0x3C },
        { "application/x-hdmlc",                        0x13 },
        { "application/x-www-form-urlencoded",          0x12 },
        { "application/x-x509-ca-cert",                 0x1A },
        { "application/x-x509-user-cert",               0x1B },
        { "application/x-x968-ca-cert",                 0x2B },
        { "application/x-x968-cross-cert",              0x2A },
        { "application/x-x968-user-cert",               0x2C },
        { "application/xhtml+xml",                      0x3B },
        { "application/xml",                            0x27 },
        { "image/*",                                    0x1C },
        { "image/gif",                                  0x1D },
        { "image/jpeg",                                 0x1E },
        { "image/png",                                  0x20 },
        { "image/tiff",                                 0x1F },
        { "image/vnd.wap.wbmp",                         0x21 },
        { "multipart/*",                                0x0B },
        { "multipart/alternative",                      0x0F },
        { "multipart/byteranges",                       0x0E },
        { "multipart/form-data",                        0x0D },
        { "multipart/mixed",                            0x0C },
        { "text/*",                                     0x01 },
        { "text/css",                                   0x3D },
        { "text/html",                                  0x02 },
        { "text/plain",                                 0x03 },
        { "text/vnd.wap.co",                            0x31 },
        { "text/vnd.wap.connectivity-xml",              0x35 },
        { "text/vnd.wap.si",                            0x2D },
        { "text/vnd.wap.sl",                            0x2F },
        { "text/vnd.wap.wml",                           0x08 },
        { "text/vnd.wap.wmlscript",                     0x09 },
        { "text/vnd.wap.wta-event",                     0x0A },
        { "text/x-hdml",                                0x04 },
        { "text/x-ttml",                                0x05 },
        { "text/x-vCalendar",                           0x06 },
        { "text/x-vCard",                               0x07 },
        { "text/xml",                                   0x28 },
    };

    const NameWordPair * table;
    int size;

    switch (version) {
    default:
        CODEC_ASSERT(FALSE);
        /* fall through */
    case HC_WapVersion1_1:      /* WAP 1.1 */
        table = aCntTypeName2Num1_1;
        size = DIM(aCntTypeName2Num1_1);
        break;
    case HC_WapVersion1_2:      /* WAP 1.2 */
        table = aCntTypeName2Num1_2;
        size = DIM(aCntTypeName2Num1_2);
        break;
    case HC_WapVersion1_3:      /* WAP 1.3 */
        table = aCntTypeName2Num1_3;
        size = DIM(aCntTypeName2Num1_3);
        break;
    case HC_WapVersion1_4:      /* WAP 1.4 */
        table = aCntTypeName2Num1_4;
        size = DIM(aCntTypeName2Num1_4);
        break;
    case HC_WapVersion1_5:      /* WAP 1.5 */
        table = aCntTypeName2Num1_5;
        size = DIM(aCntTypeName2Num1_5);
        break;
    }

    return HC_Name2WordValue (table, size, TRUE, pcName, pwValue);
} /* HC_ContentTypeName2ContentTypeCode */

/*
-----------------------------------------------------------------------------
*/
const char* HC_ContentTypeCode2ContentTypeName(
    WORD    wValue,         /* in:  Content type code. */
    HC_WapVersion version)  /* in:  Version to comply with */
{
    /* 
     * Content Type assignments/2: Numbers to Content Type names.
     * The table must be ordered by numeric values.
     */
    static const NameWordPair aNum2CntTypeName1_1[] = {
        { "*/*",                                        0x00 },
        { "text/*",                                     0x01 },
        { "text/html",                                  0x02 },
        { "text/plain",                                 0x03 },
        { "text/x-hdml",                                0x04 },
        { "text/x-ttml",                                0x05 },
        { "text/x-vCalendar",                           0x06 },
        { "text/x-vCard",                               0x07 },
        { "text/vnd.wap.wml",                           0x08 },
        { "text/vnd.wap.wmlscript",                     0x09 },
        { "text/vnd.wap.wta-event",                     0x0A },
        { "multipart/*",                                0x0B },
        { "multipart/mixed",                            0x0C },
        { "multipart/form-data",                        0x0D },
        { "multipart/byteranges",                       0x0E },
        { "multipart/alternative",                      0x0F },
        { "application/*",                              0x10 },
        { "application/java-vm",                        0x11 },
        { "application/x-www-form-urlencoded",          0x12 },
        { "application/x-hdmlc",                        0x13 },
        { "application/vnd.wap.wmlc",                   0x14 },
        { "application/vnd.wap.wmlscriptc",             0x15 },
        { "application/vnd.wap.wta-eventc",             0x16 },
        { "application/vnd.wap.uaprof",                 0x17 },
        { "application/vnd.wap.wtls-ca-certificate",    0x18 },
        { "application/vnd.wap.wtls-user-certificate",  0x19 },
        { "application/x-x509-ca-cert",                 0x1A },
        { "application/x-x509-user-cert",               0x1B },
        { "image/*",                                    0x1C },
        { "image/gif",                                  0x1D },
        { "image/jpeg",                                 0x1E },
        { "image/tiff",                                 0x1F },
        { "image/png",                                  0x20 },
        { "image/vnd.wap.wbmp",                         0x21 },
        { "application/vnd.wap.multipart.*",            0x22 },
        { "application/vnd.wap.multipart.mixed",        0x23 },
        { "application/vnd.wap.multipart.form-data",    0x24 },
        { "application/vnd.wap.multipart.byteranges",   0x25 },
        { "application/vnd.wap.multipart.alternative",  0x26 },
        { "application/xml",                            0x27 },
        { "text/xml",                                   0x28 },
        { "application/vnd.wap.wbxml",                  0x29 },
        { "application/x-x968-cross-cert",              0x2A },
        { "application/x-x968-ca-cert",                 0x2B },
        { "application/x-x968-user-cert",               0x2C },
    };

    static const NameWordPair aNum2CntTypeName1_2[] = {
        { "*/*",                                        0x00 },
        { "text/*",                                     0x01 },
        { "text/html",                                  0x02 },
        { "text/plain",                                 0x03 },
        { "text/x-hdml",                                0x04 },
        { "text/x-ttml",                                0x05 },
        { "text/x-vCalendar",                           0x06 },
        { "text/x-vCard",                               0x07 },
        { "text/vnd.wap.wml",                           0x08 },
        { "text/vnd.wap.wmlscript",                     0x09 },
        { "text/vnd.wap.wta-event",                     0x0A },
        { "multipart/*",                                0x0B },
        { "multipart/mixed",                            0x0C },
        { "multipart/form-data",                        0x0D },
        { "multipart/byteranges",                       0x0E },
        { "multipart/alternative",                      0x0F },
        { "application/*",                              0x10 },
        { "application/java-vm",                        0x11 },
        { "application/x-www-form-urlencoded",          0x12 },
        { "application/x-hdmlc",                        0x13 },
        { "application/vnd.wap.wmlc",                   0x14 },
        { "application/vnd.wap.wmlscriptc",             0x15 },
        { "application/vnd.wap.wta-eventc",             0x16 },
        { "application/vnd.wap.uaprof",                 0x17 },
        { "application/vnd.wap.wtls-ca-certificate",    0x18 },
        { "application/vnd.wap.wtls-user-certificate",  0x19 },
        { "application/x-x509-ca-cert",                 0x1A },
        { "application/x-x509-user-cert",               0x1B },
        { "image/*",                                    0x1C },
        { "image/gif",                                  0x1D },
        { "image/jpeg",                                 0x1E },
        { "image/tiff",                                 0x1F },
        { "image/png",                                  0x20 },
        { "image/vnd.wap.wbmp",                         0x21 },
        { "application/vnd.wap.multipart.*",            0x22 },
        { "application/vnd.wap.multipart.mixed",        0x23 },
        { "application/vnd.wap.multipart.form-data",    0x24 },
        { "application/vnd.wap.multipart.byteranges",   0x25 },
        { "application/vnd.wap.multipart.alternative",  0x26 },
        { "application/xml",                            0x27 },
        { "text/xml",                                   0x28 },
        { "application/vnd.wap.wbxml",                  0x29 },
        { "application/x-x968-cross-cert",              0x2A },
        { "application/x-x968-ca-cert",                 0x2B },
        { "application/x-x968-user-cert",               0x2C },
        { "text/vnd.wap.si",                            0x2D },
        { "application/vnd.wap.sic",                    0x2E },
        { "text/vnd.wap.sl",                            0x2F },
        { "application/vnd.wap.slc",                    0x30 },
        { "text/vnd.wap.co",                            0x31 },
        { "application/vnd.wap.coc",                    0x32 },
        { "application/vnd.wap.multipart.related",      0x33 },
        { "application/vnd.wap.sia",                    0x34 },
    };

    static const NameWordPair aNum2CntTypeName1_3[] = {
        { "*/*",                                        0x00 },
        { "text/*",                                     0x01 },
        { "text/html",                                  0x02 },
        { "text/plain",                                 0x03 },
        { "text/x-hdml",                                0x04 },
        { "text/x-ttml",                                0x05 },
        { "text/x-vCalendar",                           0x06 },
        { "text/x-vCard",                               0x07 },
        { "text/vnd.wap.wml",                           0x08 },
        { "text/vnd.wap.wmlscript",                     0x09 },
        { "text/vnd.wap.wta-event",                     0x0A },
        { "multipart/*",                                0x0B },
        { "multipart/mixed",                            0x0C },
        { "multipart/form-data",                        0x0D },
        { "multipart/byteranges",                       0x0E },
        { "multipart/alternative",                      0x0F },
        { "application/*",                              0x10 },
        { "application/java-vm",                        0x11 },
        { "application/x-www-form-urlencoded",          0x12 },
        { "application/x-hdmlc",                        0x13 },
        { "application/vnd.wap.wmlc",                   0x14 },
        { "application/vnd.wap.wmlscriptc",             0x15 },
        { "application/vnd.wap.wta-eventc",             0x16 },
        { "application/vnd.wap.uaprof",                 0x17 },
        { "application/vnd.wap.wtls-ca-certificate",    0x18 },
        { "application/vnd.wap.wtls-user-certificate",  0x19 },
        { "application/x-x509-ca-cert",                 0x1A },
        { "application/x-x509-user-cert",               0x1B },
        { "image/*",                                    0x1C },
        { "image/gif",                                  0x1D },
        { "image/jpeg",                                 0x1E },
        { "image/tiff",                                 0x1F },
        { "image/png",                                  0x20 },
        { "image/vnd.wap.wbmp",                         0x21 },
        { "application/vnd.wap.multipart.*",            0x22 },
        { "application/vnd.wap.multipart.mixed",        0x23 },
        { "application/vnd.wap.multipart.form-data",    0x24 },
        { "application/vnd.wap.multipart.byteranges",   0x25 },
        { "application/vnd.wap.multipart.alternative",  0x26 },
        { "application/xml",                            0x27 },
        { "text/xml",                                   0x28 },
        { "application/vnd.wap.wbxml",                  0x29 },
        { "application/x-x968-cross-cert",              0x2A },
        { "application/x-x968-ca-cert",                 0x2B },
        { "application/x-x968-user-cert",               0x2C },
        { "text/vnd.wap.si",                            0x2D },
        { "application/vnd.wap.sic",                    0x2E },
        { "text/vnd.wap.sl",                            0x2F },
        { "application/vnd.wap.slc",                    0x30 },
        { "text/vnd.wap.co",                            0x31 },
        { "application/vnd.wap.coc",                    0x32 },
        { "application/vnd.wap.multipart.related",      0x33 },
        { "application/vnd.wap.sia",                    0x34 },
        { "text/vnd.wap.connectivity-xml",              0x35 },
        { "application/vnd.wap.connectivity-wbxml",     0x36 },
    };

    static const NameWordPair aNum2CntTypeName1_4[] = {
        { "*/*",                                        0x00 },
        { "text/*",                                     0x01 },
        { "text/html",                                  0x02 },
        { "text/plain",                                 0x03 },
        { "text/x-hdml",                                0x04 },
        { "text/x-ttml",                                0x05 },
        { "text/x-vCalendar",                           0x06 },
        { "text/x-vCard",                               0x07 },
        { "text/vnd.wap.wml",                           0x08 },
        { "text/vnd.wap.wmlscript",                     0x09 },
        { "text/vnd.wap.wta-event",                     0x0A },
        { "multipart/*",                                0x0B },
        { "multipart/mixed",                            0x0C },
        { "multipart/form-data",                        0x0D },
        { "multipart/byteranges",                       0x0E },
        { "multipart/alternative",                      0x0F },
        { "application/*",                              0x10 },
        { "application/java-vm",                        0x11 },
        { "application/x-www-form-urlencoded",          0x12 },
        { "application/x-hdmlc",                        0x13 },
        { "application/vnd.wap.wmlc",                   0x14 },
        { "application/vnd.wap.wmlscriptc",             0x15 },
        { "application/vnd.wap.wta-eventc",             0x16 },
        { "application/vnd.wap.uaprof",                 0x17 },
        { "application/vnd.wap.wtls-ca-certificate",    0x18 },
        { "application/vnd.wap.wtls-user-certificate",  0x19 },
        { "application/x-x509-ca-cert",                 0x1A },
        { "application/x-x509-user-cert",               0x1B },
        { "image/*",                                    0x1C },
        { "image/gif",                                  0x1D },
        { "image/jpeg",                                 0x1E },
        { "image/tiff",                                 0x1F },
        { "image/png",                                  0x20 },
        { "image/vnd.wap.wbmp",                         0x21 },
        { "application/vnd.wap.multipart.*",            0x22 },
        { "application/vnd.wap.multipart.mixed",        0x23 },
        { "application/vnd.wap.multipart.form-data",    0x24 },
        { "application/vnd.wap.multipart.byteranges",   0x25 },
        { "application/vnd.wap.multipart.alternative",  0x26 },
        { "application/xml",                            0x27 },
        { "text/xml",                                   0x28 },
        { "application/vnd.wap.wbxml",                  0x29 },
        { "application/x-x968-cross-cert",              0x2A },
        { "application/x-x968-ca-cert",                 0x2B },
        { "application/x-x968-user-cert",               0x2C },
        { "text/vnd.wap.si",                            0x2D },
        { "application/vnd.wap.sic",                    0x2E },
        { "text/vnd.wap.sl",                            0x2F },
        { "application/vnd.wap.slc",                    0x30 },
        { "text/vnd.wap.co",                            0x31 },
        { "application/vnd.wap.coc",                    0x32 },
        { "application/vnd.wap.multipart.related",      0x33 },
        { "application/vnd.wap.sia",                    0x34 },
        { "text/vnd.wap.connectivity-xml",              0x35 },
        { "application/vnd.wap.connectivity-wbxml",     0x36 },
        { "application/pkcs7-mime",                     0x37 },
        { "application/vnd.wap.hashed-certificate",     0x38 },
        { "application/vnd.wap.signed-certificate",     0x39 },
        { "application/vnd.wap.cert-response",          0x3A },
        { "application/xhtml+xml",                      0x3B },
        { "application/wml+xml",                        0x3C },
        { "text/css",                                   0x3D },
        { "application/vnd.wap.mms-message",            0x3E },
        { "application/vnd.wap.rollover-certificate",   0x3F },
    };

    static const NameWordPair aNum2CntTypeName1_5[] = {
        { "*/*",                                        0x00 },
        { "text/*",                                     0x01 },
        { "text/html",                                  0x02 },
        { "text/plain",                                 0x03 },
        { "text/x-hdml",                                0x04 },
        { "text/x-ttml",                                0x05 },
        { "text/x-vCalendar",                           0x06 },
        { "text/x-vCard",                               0x07 },
        { "text/vnd.wap.wml",                           0x08 },
        { "text/vnd.wap.wmlscript",                     0x09 },
        { "text/vnd.wap.wta-event",                     0x0A },
        { "multipart/*",                                0x0B },
        { "multipart/mixed",                            0x0C },
        { "multipart/form-data",                        0x0D },
        { "multipart/byteranges",                       0x0E },
        { "multipart/alternative",                      0x0F },
        { "application/*",                              0x10 },
        { "application/java-vm",                        0x11 },
        { "application/x-www-form-urlencoded",          0x12 },
        { "application/x-hdmlc",                        0x13 },
        { "application/vnd.wap.wmlc",                   0x14 },
        { "application/vnd.wap.wmlscriptc",             0x15 },
        { "application/vnd.wap.wta-eventc",             0x16 },
        { "application/vnd.wap.uaprof",                 0x17 },
        { "application/vnd.wap.wtls-ca-certificate",    0x18 },
        { "application/vnd.wap.wtls-user-certificate",  0x19 },
        { "application/x-x509-ca-cert",                 0x1A },
        { "application/x-x509-user-cert",               0x1B },
        { "image/*",                                    0x1C },
        { "image/gif",                                  0x1D },
        { "image/jpeg",                                 0x1E },
        { "image/tiff",                                 0x1F },
        { "image/png",                                  0x20 },
        { "image/vnd.wap.wbmp",                         0x21 },
        { "application/vnd.wap.multipart.*",            0x22 },
        { "application/vnd.wap.multipart.mixed",        0x23 },
        { "application/vnd.wap.multipart.form-data",    0x24 },
        { "application/vnd.wap.multipart.byteranges",   0x25 },
        { "application/vnd.wap.multipart.alternative",  0x26 },
        { "application/xml",                            0x27 },
        { "text/xml",                                   0x28 },
        { "application/vnd.wap.wbxml",                  0x29 },
        { "application/x-x968-cross-cert",              0x2A },
        { "application/x-x968-ca-cert",                 0x2B },
        { "application/x-x968-user-cert",               0x2C },
        { "text/vnd.wap.si",                            0x2D },
        { "application/vnd.wap.sic",                    0x2E },
        { "text/vnd.wap.sl",                            0x2F },
        { "application/vnd.wap.slc",                    0x30 },
        { "text/vnd.wap.co",                            0x31 },
        { "application/vnd.wap.coc",                    0x32 },
        { "application/vnd.wap.multipart.related",      0x33 },
        { "application/vnd.wap.sia",                    0x34 },
        { "text/vnd.wap.connectivity-xml",              0x35 },
        { "application/vnd.wap.connectivity-wbxml",     0x36 },
        { "application/pkcs7-mime",                     0x37 },
        { "application/vnd.wap.hashed-certificate",     0x38 },
        { "application/vnd.wap.signed-certificate",     0x39 },
        { "application/vnd.wap.cert-response",          0x3A },
        { "application/xhtml+xml",                      0x3B },
        { "application/wml+xml",                        0x3C },
        { "text/css",                                   0x3D },
        { "application/vnd.wap.mms-message",            0x3E },
        { "application/vnd.wap.rollover-certificate",   0x3F },
        { "application/vnd.wap.locc+wbxml",             0x40 },
        { "application/vnd.wap.loc+xml",                0x41 },
        { "application/vnd.syncml.dm+wbxml",            0x42 },
        { "application/vnd.syncml.dm+xml",              0x43 },
        { "application/vnd.syncml.notification",        0x44 },
        { "application/vnd.wap.xhtml+xml",              0x45 },
    };

    const NameWordPair * table;
    int size;

    switch (version) {
    case HC_WapVersion1_1:      /* WAP 1.1 */
        table = aNum2CntTypeName1_1;
        size = DIM(aNum2CntTypeName1_1);
        break;
    case HC_WapVersion1_2:      /* WAP 1.2 */
        table = aNum2CntTypeName1_2;
        size = DIM(aNum2CntTypeName1_2);
        break;
    case HC_WapVersion1_3:      /* WAP 1.3 */
        table = aNum2CntTypeName1_3;
        size = DIM(aNum2CntTypeName1_3);
        break;
    case HC_WapVersion1_4:      /* WAP 1.4 */
        table = aNum2CntTypeName1_4;
        size = DIM(aNum2CntTypeName1_5);
        break;
    default:
        CODEC_ASSERT(FALSE);
        /* fall through */
    case HC_WapVersion1_5:      /* WAP 1.5 */
        table = aNum2CntTypeName1_5;
        size = DIM(aNum2CntTypeName1_5);
        break;
    }

    return HC_WordValue2Name (table, size, wValue);
} /* HC_ContentTypeCode2ContentTypeName */

/*
-----------------------------------------------------------------------------
*/
BOOL HC_LangName2LangCode(
    const char* pcName, 
    HC_WapVersion version,
    WORD*       pwValue)
{
    /* ISO 639 Language Assignments/1: Short language names to numbers. */
    static const NameWordPair aShortLang2Num[] =
    {
        { "*",  0x00 }, 
        { "af", 0x03 }, 
        { "be", 0x0A }, 
        { "bg", 0x0B }, 
        { "ca", 0x11 }, 
        { "cs", 0x13 },
        { "da", 0x15 }, 
        { "de", 0x16 },
        { "el", 0x18 }, 
        { "en", 0x19 }, 
        { "es", 0x1B }, 
        { "eu", 0x1D }, 
        { "fi", 0x1F }, 
        { "fo", 0x82 }, 
        { "fr", 0x22 }, 
        { "ga", 0x24 }, 
        { "gd", 0x25 }, 
        { "gl", 0x26 }, 
        { "hr", 0x2C }, 
        { "hu", 0x2D }, 
        { "id", 0x30 }, 
        { "is", 0x33 }, 
        { "it", 0x34 }, 
        { "ja", 0x36 }, 
        { "ko", 0x3D }, 
        { "mk", 0x48 }, 
        { "nl", 0x52 }, 
        { "no", 0x53 }, 
        { "po", 0x58 }, 
        { "pt", 0x5A }, 
        { "ro", 0x5E }, 
        { "ru", 0x5F }, 
        { "sk", 0x66 }, 
        { "sl", 0x67 }, 
        { "sq", 0x6B }, 
        { "sr", 0x6C }, 
        { "sv", 0x70 },
        { "tr", 0x7B }, 
        { "uk", 0x50 }, 
        { "zh", 0x41 }, 
    };
    /* ISO 639 Language Assignments/2: Long language names to numbers. */
    static const NameWordPair aLongLang2Num[] =
    {
        { "abkhazian",      0x02 }, 
        { "afan oromo",     0x55 }, 
        { "afar",           0x01 },
        { "afrikaans",      0x03 }, 
        { "albanian",       0x6B }, 
        { "amharic",        0x04 }, 
        { "arabic",         0x05 }, 
        { "armenian",       0x2E }, 
        { "assamese",       0x06 }, 
        { "aymara",         0x07 }, 
        { "azerbaijani",    0x08 }, 
        { "bangla",         0x0E }, 
        { "bashkir",        0x09 }, 
        { "basque",         0x1D }, 
        { "bengali",        0x0E }, 
        { "bhutani",        0x17 }, 
        { "bihari",         0x0C }, 
        { "bislama",        0x0D }, 
        { "breton",         0x10 }, 
        { "bulgarian",      0x0B }, 
        { "burmese",        0x4F }, 
        { "byelorussian",   0x0A },
        { "cambodian",      0x3B }, 
        { "catalan",        0x11 }, 
        { "chinese",        0x41 }, 
        { "corsican",       0x12 }, 
        { "croatian",       0x2C }, 
        { "czech",          0x13 }, 
        { "danish",         0x15 }, 
        { "dutch",          0x52 }, 
        { "english",        0x19 }, 
        { "esperanto",      0x1A }, 
        { "estonian",       0x1C }, 
        { "faeroese",       0x82 }, 
        { "fiji",           0x20 }, 
        { "finnish",        0x1F }, 
        { "french",         0x22 }, 
        { "frisian",        0x83 }, 
        { "galician",       0x26 }, 
        { "georgian",       0x38 }, 
        { "german",         0x16 }, 
        { "greek",          0x18 }, 
        { "greenlandic",    0x8A }, 
        { "guarani",        0x27 },
        { "gujarati",       0x28 }, 
        { "hausa",          0x29 }, 
        { "hebrew",         0x2A }, 
        { "hindi",          0x2B }, 
        { "hungarian",      0x2D }, 
        { "icelandic",      0x33 }, 
        { "indonesian",     0x30 },
        { "interlingua",    0x84 }, 
        { "interlingue",    0x86 }, 
        { "inuktitut",      0x89 }, 
        { "inupiak",        0x87 }, 
        { "irish",          0x24 }, 
        { "italian",        0x34 }, 
        { "japanese",       0x36 }, 
        { "javanese",       0x37 }, 
        { "kannada",        0x3C }, 
        { "kashmiri",       0x3E }, 
        { "kazakh",         0x39 }, 
        { "kinyarwanda",    0x60 }, 
        { "kirghiz",        0x40 }, 
        { "kirundi",        0x5D }, 
        { "korean",         0x3D }, 
        { "kurdish",        0x3F }, 
        { "laothian",       0x43 }, 
        { "latin",          0x8B }, 
        { "latvian",        0x45 }, 
        { "lettish",        0x45 }, 
        { "lingala",        0x42 }, 
        { "lithuanian",     0x44 }, 
        { "macedonian",     0x48 }, 
        { "malagasy",       0x46 }, 
        { "malay",          0x4D }, 
        { "malayalam",      0x49 }, 
        { "maltese",        0x4E }, 
        { "maori",          0x47 }, 
        { "marathi",        0x4C },
        { "moldavian",      0x4B }, 
        { "mongolian",      0x4A }, 
        { "nauru",          0x81 }, 
        { "nepali",         0x51 }, 
        { "norwegian",      0x53 },
        { "occitan",        0x54 }, 
        { "oriya",          0x56 }, 
        { "oromo",          0x55 }, 
        { "pashto",         0x59 }, 
        { "persian",        0x1E }, 
        { "polish",         0x58 }, 
        { "portuguese",     0x5A }, 
        { "punjabi",        0x57 }, 
        { "pushto",         0x59 }, 
        { "quechua",        0x5B }, 
        { "rhaeto-romance", 0x8C }, 
        { "romanian",       0x5E }, 
        { "russian",        0x5F }, 
        { "samoan",         0x68 }, 
        { "sangho",         0x63 }, 
        { "sanskrit",       0x61 }, 
        { "scots gaelic",   0x25 }, 
        { "serbian",        0x6C }, 
        { "serbo-croatian", 0x64 }, 
        { "sesotho",        0x6E }, 
        { "setswana",       0x79 }, 
        { "shona",          0x69 }, 
        { "sindhi",         0x62 }, 
        { "sinhalese",      0x65 },
        { "siswati",        0x6D }, 
        { "slovak",         0x66 }, 
        { "slovenian",      0x67 }, 
        { "somali",         0x6A }, 
        { "spanish",        0x1B }, 
        { "sundanese",      0x6F }, 
        { "swahili",        0x71 }, 
        { "swedish",        0x70 }, 
        { "tagalog",        0x78 }, 
        { "tajik",          0x74 }, 
        { "tamil",          0x72 }, 
        { "tatar",          0x7D }, 
        { "telugu",         0x73 }, 
        { "thai",           0x75 }, 
        { "tibetan",        0x0F }, 
        { "tigrinya",       0x76 }, 
        { "tonga",          0x7A },
        { "tsonga",         0x7C }, 
        { "turkish",        0x7B }, 
        { "turkmen",        0x77 }, 
        { "twi",            0x7E }, 
        { "uighur",         0x7F }, 
        { "ukrainian",      0x50 }, 
        { "urdu",           0x21 }, 
        { "uzbek",          0x23 }, 
        { "vietnamese",     0x2F }, 
        { "volapuk",        0x85 }, 
        { "welsh",          0x14 }, 
        { "wolof",          0x31 },
        { "xhosa",          0x32 }, 
        { "yiddish",        0x88 }, 
        { "yoruba",         0x35 }, 
        { "zhuang",         0x3A }, 
        { "zulu",           0x5C }, 
    };

    BOOL bRet   = FALSE;

    if (strlen (pcName) <= 2)
    { /* Short language name */
        bRet = HC_Name2WordValue (aShortLang2Num, DIM (aShortLang2Num), 
                                  TRUE, pcName, pwValue);
    }
    else
    { /* Long language name */
        bRet = HC_Name2WordValue (aLongLang2Num, DIM (aLongLang2Num), 
                                  TRUE, pcName, pwValue);
    }
    return bRet;
} /* HC_LangName2LangCode */

/*
-----------------------------------------------------------------------------
*/
const char* HC_LangCode2LangName(
    WORD          wValue,   /* in:  Content type code. */
    HC_WapVersion version)  /* in:  Version to comply with */
{
    /* ISO 639 Language Assignments/3: Numbers to language names. */
    static const NameWordPair aNum2Lang[] =
    {
        { "*",              0x00 }, 
        { "Afar",           0x01 }, 
        { "Abkhazian",      0x02 }, 
        { "af",             0x03 }, 
        { "Amharic",        0x04 }, 
        { "Arabic",         0x05 }, 
        { "Assamese",       0x06 }, 
        { "Aymara",         0x07 }, 
        { "Azerbaijani",    0x08 }, 
        { "Bashkir",        0x09 }, 
        { "be",             0x0A }, 
        { "bg",             0x0B }, 
        { "Bihari",         0x0C }, 
        { "Bislama",        0x0D }, 
        { "Bengali",        0x0E }, 
        { "Tibetan",        0x0F }, 
        { "Breton",         0x10 }, 
        { "ca",             0x11 }, 
        { "Corsican",       0x12 }, 
        { "cs",             0x13 }, 
        { "Welsh",          0x14 }, 
        { "da",             0x15 }, 
        { "de",             0x16 }, 
        { "Bhutani",        0x17 }, 
        { "el",             0x18 }, 
        { "en",             0x19 }, 
        { "Esperanto",      0x1A },
        { "es",             0x1B }, 
        { "Estonian",       0x1C }, 
        { "eu",             0x1D }, 
        { "Persian",        0x1E }, 
        { "fi",             0x1F }, 
        { "Fiji",           0x20 }, 
        { "Urdu",           0x21 }, 
        { "fr",             0x22 }, 
        { "Uzbek",          0x23 }, 
        { "ga",             0x24 }, 
        { "gd",             0x25 }, 
        { "gl",             0x26 },
        { "Guarani",        0x27 }, 
        { "Gujarati",       0x28 }, 
        { "Hausa",          0x29 }, 
        { "Hebrew",         0x2A }, 
        { "Hindi",          0x2B }, 
        { "hr",             0x2C }, 
        { "hu",             0x2D }, 
        { "Armenian",       0x2E }, 
        { "Vietnamese",     0x2F }, 
        { "id",             0x30 }, 
        { "Wolof",          0x31 }, 
        { "Xhosa",          0x32 }, 
        { "is",             0x33 }, 
        { "it",             0x34 }, 
        { "Yoruba",         0x35 }, 
        { "ja",             0x36 }, 
        { "Javanese",       0x37 },
        { "Georgian",       0x38 }, 
        { "Kazakh",         0x39 }, 
        { "Zhuang",         0x3A }, 
        { "Cambodian",      0x3B }, 
        { "Kannada",        0x3C }, 
        { "ko",             0x3D }, 
        { "Kashmiri",       0x3E }, 
        { "Kurdish",        0x3F }, 
        { "Kirghiz",        0x40 }, 
        { "zh",             0x41 }, 
        { "Lingala",        0x42 }, 
        { "Laothian",       0x43 }, 
        { "Lithuanian",     0x44 }, 
        { "Latvian",        0x45 }, 
        { "Malagasy",       0x46 }, 
        { "Maori",          0x47 }, 
        { "mk",             0x48 }, 
        { "Malayalam",      0x49 }, 
        { "Mongolian",      0x4A }, 
        { "Moldavian",      0x4B }, 
        { "Marathi",        0x4C }, 
        { "Malay",          0x4D }, 
        { "Maltese",        0x4E }, 
        { "Burmese",        0x4F },
        { "uk",             0x50 }, 
        { "Nepali",         0x51 }, 
        { "nl",             0x52 }, 
        { "no",             0x53 }, 
        { "Occitan",        0x54 },
        { "Oromo",          0x55 }, 
        { "Oriya",          0x56 }, 
        { "Punjabi",        0x57 }, 
        { "po",             0x58 }, 
        { "Pashto",         0x59 }, 
        { "pt",             0x5A }, 
        { "Quechua",        0x5B }, 
        { "Zulu",           0x5C }, 
        { "Kirundi",        0x5D }, 
        { "ro",             0x5E }, 
        { "ru",             0x5F }, 
        { "Kinyarwanda",    0x60 }, 
        { "Sanskrit",       0x61 }, 
        { "Sindhi",         0x62 }, 
        { "Sangho",         0x63 }, 
        { "Serbo-Croatian", 0x64 }, 
        { "Sinhalese",      0x65 }, 
        { "sk",             0x66 }, 
        { "sl",             0x67 }, 
        { "Samoan",         0x68 }, 
        { "Shona",          0x69 }, 
        { "Somali",         0x6A }, 
        { "sq",             0x6B }, 
        { "sr",             0x6C }, 
        { "Siswati",        0x6D }, 
        { "Sesotho",        0x6E }, 
        { "Sundanese",      0x6F }, 
        { "sv",             0x70 }, 
        { "Swahili",        0x71 },
        { "Tamil",          0x72 }, 
        { "Telugu",         0x73 }, 
        { "Tajik",          0x74 }, 
        { "Thai",           0x75 }, 
        { "Tigrinya",       0x76 }, 
        { "Turkmen",        0x77 }, 
        { "Tagalog",        0x78 },
        { "Setswana",       0x79 }, 
        { "Tonga",          0x7A }, 
        { "tr",             0x7B }, 
        { "Tsonga",         0x7C }, 
        { "Tatar",          0x7D }, 
        { "Twi",            0x7E }, 
        { "Uighur",         0x7F }, 
        { "Nauru",          0x81 }, 
        { "fo",             0x82 }, 
        { "Frisian",        0x83 }, 
        { "Interlingua",    0x84 }, 
        { "Volapuk",        0x85 }, 
        { "Interlingue",    0x86 }, 
        { "Inupiak",        0x87 }, 
        { "Yiddish",        0x88 }, 
        { "Inuktitut",      0x89 }, 
        { "Greenlandic",    0x8A }, 
        { "Latin",          0x8B }, 
        { "Rhaeto-Romance", 0x8C }, 
    };

    return HC_WordValue2Name (aNum2Lang, DIM (aNum2Lang), wValue);
} /* HC_LangCode2LangName */

/*
-----------------------------------------------------------------------------
*/
BOOL HC_CharsetName2CharsetCode(
    const char*   pcName, 
    HC_WapVersion wapVersion,
    WORD*         pwValue)
{
    BOOL    bFound  = FALSE;
    int     nSize   = DIM (g_aCharSets), 
            i;

    /*  Note: Simple linear search is used here because I have no 
        idea about how frequently the particular character sets are 
        used. The function can be optimized if we obtain such information, 
        or if someone takes the effort to order the charset table by name. */
    for (i = 0; !bFound && i < nSize; i++)
    {
        bFound = (HC_stricmp (pcName, g_aCharSets[i].i_pcName) == 0);
        if (bFound)
        {
            *pwValue = g_aCharSets[i].i_wValue;
        }
    }
    return bFound;
} /* HC_CharsetName2CharsetCode */

/*
-----------------------------------------------------------------------------
*/
const char* HC_CharsetCode2CharsetName(
    WORD          wValue,   /* in:  Character set code. */
    HC_WapVersion version)  /* in:  WAP version to comply with */
{
    return HC_WordValue2Name (g_aCharSets, DIM (g_aCharSets), wValue);
} /* HC_CharsetCode2CharsetName */

/*
-----------------------------------------------------------------------------
*/
BOOL HC_ParamName2ParamCode(
    const char* pcName, 
    BYTE*       pbValue)
{
    /* Well-known parameter assignments/1: Parameter names to numbers. */
    static const NameBytePair aParamName2Num[] =
    {
        { "charset",        WPS_HFC_PARAM_CHARSET       }, 
        { "differences",    WPS_HFC_PARAM_DIFFERENCES   }, 
        { "filename",       WPS_HFC_PARAM_FILENAME      }, 
        { "level",          WPS_HFC_PARAM_LEVEL         }, 
        { "name",           WPS_HFC_PARAM_NAME          }, 
        { "padding",        WPS_HFC_PARAM_PADDING       }, 
        { "q",              WPS_HFC_PARAM_Q             }, 
        { "type",           WPS_HFC_PARAM_TYPE          }, 
    };

    return HC_Name2Byte_Ord (aParamName2Num, DIM (aParamName2Num),
                             TRUE, pcName, pbValue);
} /* HC_ParamName2ParamCode */

/*
-----------------------------------------------------------------------------
*/
const char* HC_ParamCode2ParamName(
    BYTE    bValue)
{
    /* Well-known parameter assignments/2: Numbers to parameter names. */
    static const NameBytePair aNum2ParamName[] =
    {
        { "Q",              WPS_HFC_PARAM_Q             }, 
        { "Charset",        WPS_HFC_PARAM_CHARSET       }, 
        { "Level",          WPS_HFC_PARAM_LEVEL         }, 
        { "Type",           WPS_HFC_PARAM_TYPE          }, 
        { "Name",           WPS_HFC_PARAM_NAME          }, 
        { "Filename",       WPS_HFC_PARAM_FILENAME      }, 
        { "Differences",    WPS_HFC_PARAM_DIFFERENCES   }, 
        { "Padding",        WPS_HFC_PARAM_PADDING       }, 
    };

    return HC_Byte2Name_Ord (aNum2ParamName, DIM (aNum2ParamName), bValue);
} /* HC_ParamCode2ParamName */

/*
-----------------------------------------------------------------------------
*/
BOOL HC_MethodName2MethodCode(
    const char* pcName, 
    BYTE*       pbValue)
{
    /* Well-known method name assignments/1: Method names to numbers. */
    static const NameBytePair aMethodName2Num[] =
    {
        { "delete",     0x43 }, 
        { "get",        0x40 }, 
        { "head",       0x42 }, 
        { "options",    0x41 }, 
        { "post",       0x60 }, 
        { "put",        0x61 }, 
        { "trace",      0x44 }, 
    };

    return HC_Name2Byte_Ord (aMethodName2Num, DIM (aMethodName2Num), 
                             TRUE, pcName, pbValue);
} /* HC_MethodName2MethodCode */

/*
-----------------------------------------------------------------------------
*/
const char* HC_MethodCode2MethodName(
    BYTE    bValue)
{
    /* Well-known method name assignments/2: Numbers to method names. */
    static const NameBytePair aNum2MethodName[] =
    {
        { "get",        0x40 },
        { "options",    0x41 }, 
        { "head",       0x42 }, 
        { "delete",     0x43 }, 
        { "trace",      0x44 }, 
        { "post",       0x60 }, 
        { "put",        0x61 }, 
    };

    return HC_Byte2Name_Ord (aNum2MethodName, DIM (aNum2MethodName), bValue);
} /* HC_MethodCode2MethodName */

/*
-----------------------------------------------------------------------------
*/
BOOL HC_CacheCtlName2CacheCtlCode(
    const char* pcName, 
    BYTE*       pbValue)
{
    /* Well-known Cache-Control name assignments/1: Cache-Control names to numbers. */
    static const NameBytePair aCacheCtlName2Num[] =
    {
        { "max-age",            WPS_HFC_CACHECTL_MAX_AGE            }, 
        { "max-stale",          WPS_HFC_CACHECTL_MAX_STALE          }, 
        { "min-fresh",          WPS_HFC_CACHECTL_MIN_FRESH          }, 
        { "must-revalidate",    WPS_HFC_CACHECTL_MUST_REVALIDATE    }, 
        { "no-cache",           WPS_HFC_CACHECTL_NO_CACHE           },
        { "no-store",           WPS_HFC_CACHECTL_NO_STORE           }, 
        { "no-transform",       WPS_HFC_CACHECTL_NO_TRANSFORM       }, 
        { "only-if-cached",     WPS_HFC_CACHECTL_ONLY_IF_CACHED     },
        { "private",            WPS_HFC_CACHECTL_PRIVATE            }, 
        { "proxy-revalidate",   WPS_HFC_CACHECTL_PROXY_REVALIDATE   }, 
        { "public",             WPS_HFC_CACHECTL_PUBLIC             }, 
    };

    return HC_Name2Byte_Ord (aCacheCtlName2Num, DIM (aCacheCtlName2Num), 
                             TRUE, pcName, pbValue);
} /* HC_CacheCtlName2CacheCtlCode */

/*
-----------------------------------------------------------------------------
*/
const char* HC_CacheCtlCode2CacheCtlName(
    BYTE    bValue)
{
    /* Well-known Cache-Control name assignments/2: Numbers to Cache-Control names. */
    static const NameBytePair aNum2CacheCtlName[] =
    {
        { "no-cache",           WPS_HFC_CACHECTL_NO_CACHE           }, 
        { "no-store",           WPS_HFC_CACHECTL_NO_STORE           }, 
        { "max-age",            WPS_HFC_CACHECTL_MAX_AGE            }, 
        { "max-stale",          WPS_HFC_CACHECTL_MAX_STALE          }, 
        { "min-fresh",          WPS_HFC_CACHECTL_MIN_FRESH          }, 
        { "only-if-cached",     WPS_HFC_CACHECTL_ONLY_IF_CACHED     }, 
        { "public",             WPS_HFC_CACHECTL_PUBLIC             }, 
        { "private",            WPS_HFC_CACHECTL_PRIVATE            }, 
        { "no-transform",       WPS_HFC_CACHECTL_NO_TRANSFORM       }, 
        { "must-revalidate",    WPS_HFC_CACHECTL_MUST_REVALIDATE    }, 
        { "proxy-revalidate",   WPS_HFC_CACHECTL_PROXY_REVALIDATE   },
    };

    return HC_Byte2Name_Ord (aNum2CacheCtlName, DIM (aNum2CacheCtlName), bValue);
} /* HC_CacheCtlCode2CacheCtlName */

#define BASE64_FILTER   0x3F    /* 00111111 */

/*
-----------------------------------------------------------------------------
*/
int HC_EncodeBase64(
    const char* pcOrigText, 
    char*       pcEncodedText,
	UINT32		nOrigTextLen)
{
    int nGroups;
	UINT32 nPos = 0;
    for (nGroups = 0; nPos < nOrigTextLen; nGroups++)
    {
        long    lBuf;
        int     nChars, 
                i;

        /* Get 24 bits from the original stream */
        for (lBuf = 0, nChars = 0; (nChars < 3) && (nPos < nOrigTextLen); nChars++, pcOrigText++, nPos++)
        {
            lBuf += (BYTE)*pcOrigText << (8 * (2 - nChars));
        }
        /* Convert the 24 bits into 4*6 bits */
        for (i = 3; i >= 3 - nChars; i--)
        {
            BYTE    bCode   = (BYTE)((lBuf >> (6 * i)) & BASE64_FILTER);
            char    cChar   = g_aBase64Assignments[bCode];

            *(pcEncodedText++) = cChar;
        }
        /* Add padding characters if there were not exactly 24 bits */
        for (; i >= 0; i--)
        {
            *(pcEncodedText++) = '=';
        }
    }
    return ((4 * nGroups));
} /* HC_EncodeBase64 */

/*
-----------------------------------------------------------------------------
*/
int HC_DecodeBase64(
    const char* pcOrigText,
    char*       pcDecodedText,
	UINT32		nOrigTextLen)
{
    int nGroups, 
        nChars  = 0;
	int iresult = 0;
	UINT32 nPos = 0;

    for (nGroups = 0; (nPos<nOrigTextLen) && *pcOrigText != '=' && iresult >= 0; nGroups++)
    {
        long    lBuf    = 0;
        int     i;

        /* Get 24 bits from the original stream */
        for (nChars = 3; 
             nChars >= 0 && (nPos<nOrigTextLen) && *pcOrigText != '='; 
             nChars--, pcOrigText++, nPos++)
        {
            char    cNext   = *pcOrigText;
            BYTE    bCode;
			int sizeofb64 = sizeof(g_aBase64Assignments);

            for (bCode = 0; (g_aBase64Assignments[bCode] != cNext && bCode < sizeofb64); bCode++);

			if (cNext!=g_aBase64Assignments[bCode])
			{
				iresult = -1;
				break;
			}

			lBuf += bCode << (6 * nChars);
        }
        /* Convert the 24 bits into 3*8 bits */
        for (i = 2; i >= 0 && iresult >= 0; i--)
        {
            *(pcDecodedText++) = (BYTE)((lBuf >> (8 * i)) & 0xFF);
        }
    }

	if (iresult<0)
	{
		return (0);
	}
	else
	{
		return ((3 * nGroups) - nChars - 1);
	}
} /* HC_DecodeBase64 */

/*
-----------------------------------------------------------------------------
*/
UINT32 HC_strnicmp (const char *string1, const char *string2, UINT32 count) {
    UINT32 i;

    for (i=0; (i<count && string1[i] !='\0' && string2[i] !='\0')  && ( 
             string1[i] - string2[i] ==0 || 
             string1[i] - string2[i] == 'a'-'A' || 
             string2[i] - string1[i] == 'a'-'A' ); i++);

    if (((string1[i] =='\0') || (string2[i] =='\0')) && (i!=count))
        return 1;

    return 0;

} /* HC_strnicmp */


/*
-----------------------------------------------------------------------------
    Determines whether the given header field may have multiple
    field values separated by comma, or comma is legal in its value.
    Returns TRUE if commas in the field value are to be interpreted
    as value separators, otherwise FALSE.
-----------------------------------------------------------------------------
*/
BOOL HC_MayHaveMultipleValues(
    BYTE    bCodePage,  /* in:  Header code page number for this field. */
    BYTE    bFieldCode) /* in:  Header field code. */
{
    /*  At the moment values of the below fields are known to
        possibly contain a comma */
    return (    bCodePage != WPS_HCP_DEFAULT ||
                (   bFieldCode != WPS_HFC_DEF_DATE &&
                    bFieldCode != WPS_HFC_DEF_EXPIRES &&
                    bFieldCode != WPS_HFC_DEF_IF_MODIFIED_SINCE &&
                    bFieldCode != WPS_HFC_DEF_IF_RANGE &&
                    bFieldCode != WPS_HFC_DEF_IF_UNMODIFIED_SINCE &&
                    bFieldCode != WPS_HFC_DEF_LAST_MODIFIED &&
/*                    bFieldCode != WPS_HFC_DEF_CACHE_CONTROL && */
                    bFieldCode != WPS_HFC_DEF_PROXY_AUTHENTICATE &&
                    bFieldCode != WPS_HFC_DEF_PROXY_AUTHORIZATION &&
                    bFieldCode != WPS_HFC_DEF_WWW_AUTHENTICATE &&
                    bFieldCode != WPS_HFC_DEF_AUTHORIZATION &&
                    bFieldCode != WPS_HFC_DEF_RETRY_AFTER )   );
} /* HC_MayHaveMultipleValues */


/* -------------------------------- Extern ------------------------- */

/*
-----------------------------------------------------------------------------
*/
BOOL HC_Name2Byte_Ord(
    const NameBytePair  aPairs[],
    int                 nNumPairs,
    BOOL                bIsLowerCase,
    const char*         pcName,
    BYTE*               pbByte)
{
    BOOL    bFound      = FALSE;
    int     nLow        = 0,
            nHigh       = nNumPairs - 1,
            nMid;

    while (!bFound && nLow <= nHigh)
    {
        int nCmp;

        nMid = (nLow + nHigh) / 2;
        if (bIsLowerCase)
        {
            nCmp = HC_stricmp2 (pcName, aPairs[nMid].i_pcName);
        }
        else
        {
            nCmp = HC_stricmp (pcName, aPairs[nMid].i_pcName);
        }
        if (nCmp == 0)
        {
            bFound = TRUE;
            *pbByte = aPairs[nMid].i_bValue;
        }
        else if (nCmp < 0)
        {
            nHigh = nMid - 1;
        }
        else
        {
            nLow = nMid + 1;
        }
    }
    return bFound;
} /* HC_Name2Byte_Ord */

/*
-----------------------------------------------------------------------------
*/
const char* HC_Byte2Name_Ord(
    const NameBytePair  aPairs[],
    int                 nNumPairs,
    BYTE                bByte)
{
    int         nLow    = 0,
                nHigh   = nNumPairs - 1,
                nMid;
    BOOL        bFound  = FALSE;
    const char* pcRet   = NULL;

    while (!bFound && nLow <= nHigh)
    {
        nMid = (nLow + nHigh) / 2;
        if (bByte == aPairs[nMid].i_bValue)
        {
            bFound = TRUE;
            pcRet = aPairs[nMid].i_pcName;
        }
        else if (bByte < aPairs[nMid].i_bValue)
        {
            nHigh = nMid - 1;
        }
        else
        {
            nLow = nMid + 1;
        }
    }

    return pcRet;
} /* HC_Byte2Name_Ord */

/*
-----------------------------------------------------------------------------
*/
BOOL HC_Name2Byte_Unord(
    const NameBytePair  aPairs[],
    int                 nNumPairs,
    BOOL                bIsLowerCase,
    const char*         pcName,
    BYTE*               pbByte)
{
    BOOL    bFound  = FALSE;
    int     i;

    for (i = 0; !bFound && i < nNumPairs; i++)
    {
        if (bIsLowerCase)
        {
            bFound = (HC_stricmp2 (pcName, aPairs[i].i_pcName) == 0);
        }
        else
        {
            bFound = (HC_stricmp (pcName, aPairs[i].i_pcName) == 0);
        }
        if (bFound)
        {
            *pbByte = aPairs[i].i_bValue;
        }
    }
    return bFound;
} /* HC_Name2Byte_Unord */

/*
-----------------------------------------------------------------------------
*/
const char* HC_Byte2Name_Unord(
    const NameBytePair  aPairs[],
    int                 nNumPairs,
    BYTE                bByte)
{
    int         i;
    BOOL        bFound  = FALSE;
    const char* pcRet   = NULL;

    for (i = 0; !bFound && i < nNumPairs; i++)
    {
        bFound = (bByte == aPairs[i].i_bValue);
        if (bFound)
        {
            pcRet = aPairs[i].i_pcName;
        }
    }
    return pcRet;
} /* HC_Byte2Name_Unord */

/*
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeUintvar(
    UINT32  nValue,
    BYTE*   pbDestBuf,
    UINT32* pnBufLen)
{
    HC_STATUS   nRet            = WPS_HC_STATUS_OK;
	BYTE        abEncodedVal    [5];
	int         nBytes,
                i;

	if (nValue == 0)
    {
		abEncodedVal[0] = 0;
		nBytes = 1;
	}
	else
    {
		for (nBytes = 0; nValue != 0; nBytes++, nValue = nValue / 128)
        {
			abEncodedVal[nBytes] = (UINT8)(nValue % 128);
        }
    }
    if (*pnBufLen + nBytes <= WPS_CODEC_BUF_SIZE)
    { /* Value fits into the buffer - copy it */
	    for (i = 0; i < nBytes; i++)
        {
		    pbDestBuf[*pnBufLen + i] = abEncodedVal[nBytes - i - 1];
		    if (i != nBytes - 1)
            {
                pbDestBuf[*pnBufLen + i] += 128;
            }
	    }
        *pnBufLen += nBytes;
    }
    else
    { /* Error - destination buffer is too small! */
        nRet = WPS_CODEC_ERROR_INT_BUFFER_TOO_SMALL;
    }
    return nRet;
} /* HC_EncodeUintvar */

/*
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeText(
    const char*     pcToken,
    BYTE*           pbDestBuf,
    UINT32*         pnBufLen,
    BOOL            bCheckQuotes)
{
    HC_STATUS  nRet    = WPS_HC_STATUS_OK;

    if (pcToken)
    {
        int nLen    = strlen (pcToken);

        if (bCheckQuotes && pcToken[0] == '\"' && pcToken[nLen - 1] == '\"')
        {   /* Quoted-string - leave the ending quotation */
            nLen--;
        }
        else if ((BYTE)pcToken[0] > 127 && *pnBufLen < WPS_CODEC_BUF_SIZE)
        {   /* String begins with a non-7bit character - precede it with a quote */
            pbDestBuf[(*pnBufLen)++] = WPS_CODEC_NON7BIT_QUOTE;
        }
        if (*pnBufLen + nLen < WPS_CODEC_BUF_SIZE)
        { /* Field value fits into the buffer - copy it */
            strncpy ((char*)(pbDestBuf + *pnBufLen), pcToken, nLen + 1);
            *pnBufLen += nLen + 1;
            pbDestBuf[*pnBufLen - 1] = '\0';
        }
        else
        { /* Error - destination buffer is too small! */
            nRet = WPS_CODEC_ERROR_INT_BUFFER_TOO_SMALL;
        }
    }
    return nRet;
} /* HC_EncodeText */

/*
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeOctet(
    BYTE    bValue,
    BYTE*   pbDestBuf,
    UINT32* pnBufLen)
{
    HC_STATUS  nRet    = WPS_HC_STATUS_OK;

    if (*pnBufLen < WPS_CODEC_BUF_SIZE)
    {
        pbDestBuf[(*pnBufLen)++] = bValue;
    }
    else
    { /* Error - destination buffer is too small! */
        nRet = WPS_CODEC_ERROR_INT_BUFFER_TOO_SMALL;
    }
    return nRet;
} /* HC_EncodeOctet */

/*
-----------------------------------------------------------------------------
*/
HC_STATUS N_HC_EncodeOctet (
    BYTE    bValue,
    BYTE*   pbDestBuf,
    UINT32* pnDestBufLen,
    UINT32  nMaxDestBufLen)
{
    pbDestBuf += *pnDestBufLen;
    *pnDestBufLen +=1;

    if (*pnDestBufLen <= nMaxDestBufLen) {
        *pbDestBuf = bValue;
        pbDestBuf++;
    }

    return WPS_HC_STATUS_OK;
} /* N_HC_EncodeOctet */

/*
-----------------------------------------------------------------------------
*/
HC_STATUS N_HC_EncodeText (
    const char*     pcToken,
    UINT32          nTokenLen,
    BYTE*           pbDestBuf,
    UINT32*         pnDestBufLen,
    UINT32          nMaxDestBufLen,
    BOOL            bCheckQuote)
{

    /* Sanity check */
    if (pcToken == NULL)
        return 0;

    pbDestBuf += *pnDestBufLen;

    if (bCheckQuote) {
        /* Check the quotes */
        if ((pcToken[0] != '\"') || (pcToken[nTokenLen-1] != '\"'))
            return 1;
    }

    if ((pcToken[0] == '\"') && (pcToken[nTokenLen-1] == '\"')) {
        /* Skip quotes */
        pcToken ++;
        nTokenLen -=2;
    }

    if ( (pcToken[0] & 0x80) ) {
        *pnDestBufLen +=1;

        if ( *pnDestBufLen <= nMaxDestBufLen ) {
            /* String begins with a non-7bit character - precede it with a quote */
             *pbDestBuf = WPS_CODEC_NON7BIT_QUOTE;
             pbDestBuf++;
        }
    }

    *pnDestBufLen += nTokenLen+1;

    if (*pnDestBufLen <= nMaxDestBufLen) {
        strncpy ((char*)pbDestBuf, pcToken, nTokenLen);
        pbDestBuf[nTokenLen] = '\0';
        pbDestBuf+=nTokenLen+1;
    }

    return WPS_HC_STATUS_OK;
} /* N_HC_EncodeText */

/*
-----------------------------------------------------------------------------
*/
int HC_stricmp(
    const char* pcString1, 
    const char* pcString2)
{
    int nChar1, 
        nChar2;

    do
    {
        nChar1 = tolower (*(pcString1++));
        nChar2 = tolower (*(pcString2++));
    }
    while (nChar1 && (nChar1 == nChar2));
    return (nChar1 - nChar2);
} /* HC_stricmp */

/*
-----------------------------------------------------------------------------
*/
int HC_stricmp2(
    const char* pcString1, 
    const char* pcString2)
{
    int nChar1, 
        nChar2;

    do
    {
        nChar1 = tolower (*(pcString1++));
        nChar2 = *(pcString2++);
    }
    while (nChar1 && (nChar1 == nChar2));
    return (nChar1 - nChar2);
} /* HC_stricmp2 */

/*
-----------------------------------------------------------------------------
*/
UINT32 HC_GetUintvarLength(
	UINT32	Uvalue)
{
	if (Uvalue < 0x80) return 1;
	if (Uvalue < 0x4000) return 2;
	if (Uvalue < 0x200000) return 3;
	if (Uvalue < 0x10000000) return 4;
	return 5;
} /* This functions is returns the length in bytes of the UINT32 encoded to Uintvar */

/*
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeUintvar(
    const BYTE* pbSrcBuf, 
    UINT32*     pnSrcPos, 
    UINT32*     pnValue,
	UINT32		nMaxLen)
{
	int nByte;
	UINT32 i = 0;
	HC_STATUS nRet = WPS_CODEC_STATUS_OK;

    *pnValue = 0;
	do
    {
        nByte = pbSrcBuf [(*pnSrcPos)++];
        *pnValue *= 128;
		*pnValue += nByte & 127;
		i++;
	}
    while ((nByte & 128) && (i < nMaxLen));
	
	if (nByte & 128)
	{
		nRet = WPS_CODEC_ERROR_INT_BUFFER_TOO_SMALL;
	}

    return (nRet);
} /* HC_DecodeUintvar */


/*
-----------------------------------------------------------------------------
*/
INT32 HC_Strnlen(
    const char* pbBuf, 
    INT32		nMaxLen)
{
const char *pcTmp = pbBuf;
int	i = 0;
INT32 nRet = 0;

	if (nMaxLen > 0)
	{
		while ( (*pcTmp!='\0') && (i < nMaxLen-1) )
		{
			i++;
			pcTmp++;
		}
	}
	else
	{
		while (*pcTmp!='\0')
		{
			i++;
			pcTmp++;
		}
	}
	if (*pcTmp=='\0')
	{
		nRet = i;
	}
	else
	{
		nRet = -1;
	}
	return (nRet);
} /* HC_Strnlen */
