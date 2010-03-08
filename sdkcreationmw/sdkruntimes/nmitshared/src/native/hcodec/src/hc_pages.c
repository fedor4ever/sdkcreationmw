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

/* File    : T:\wpsvob\wsp\work\codecs\headercodec\hc_pages.c
   Version : \main\9
   Date    : 02-May-00.16:14:43
*/

/*
 * ========================================================================
 *  Name        : hc_pages.c
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

#include "hc_pages.h"

/*
-----------------------------------------------------------------------------
    Defines
-----------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
    Global constants
-----------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
    Local helper functions
-----------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
    Implementation
-----------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
*/
BOOL HC_FieldName2FieldCode(
    int           nPage, 
    const char*   pcPage, 
    const char*   pcName, 
    HC_WapVersion wapVer,
    BYTE*         pbValue)
{
    /* Header field name assignments/1: Field names to numbers. */
    /* Must be in alfanumeric order */
    static const NameBytePair aFieldName2Num1_1[] =
    {
        { "accept",                 WPS_HFC_DEF_ACCEPT               }, 
        { "accept-charset",         WPS_HFC_DEF_ACCEPT_CHARSET       }, 
        { "accept-encoding",        WPS_HFC_DEF_ACCEPT_ENCODING      }, 
        { "accept-language",        WPS_HFC_DEF_ACCEPT_LANGUAGE      }, 
        { "accept-ranges",          WPS_HFC_DEF_ACCEPT_RANGES        }, 
        { "age",                    WPS_HFC_DEF_AGE                  }, 
        { "allow",                  WPS_HFC_DEF_ALLOW                }, 
        { "authorization",          WPS_HFC_DEF_AUTHORIZATION        }, 
        { "cache-control",          WPS_HFC_DEF_CACHE_CONTROL        }, 
        { "connection",             WPS_HFC_DEF_CONNECTION           }, 
        { "content-base",           WPS_HFC_DEF_CONTENT_BASE         }, 
        { "content-disposition",    WPS_HFC_DEF_CONTENT_DISPOSITION  }, 
        { "content-encoding",       WPS_HFC_DEF_CONTENT_ENCODING     }, 
        { "content-language",       WPS_HFC_DEF_CONTENT_LANGUAGE     }, 
        { "content-length",         WPS_HFC_DEF_CONTENT_LENGTH       }, 
        { "content-location",       WPS_HFC_DEF_CONTENT_LOCATION     }, 
        { "content-md5",            WPS_HFC_DEF_CONTENT_MD5          }, 
        { "content-range",          WPS_HFC_DEF_CONTENT_RANGE        }, 
        { "content-type",           WPS_HFC_DEF_CONTENT_TYPE         }, 
        { "date",                   WPS_HFC_DEF_DATE                 }, 
        { "etag",                   WPS_HFC_DEF_ETAG                 }, 
        { "expires",                WPS_HFC_DEF_EXPIRES              }, 
        { "from",                   WPS_HFC_DEF_FROM                 }, 
        { "host",                   WPS_HFC_DEF_HOST                 }, 
        { "if-match",               WPS_HFC_DEF_IF_MATCH             }, 
        { "if-modified-since",      WPS_HFC_DEF_IF_MODIFIED_SINCE    }, 
        { "if-none-match",          WPS_HFC_DEF_IF_NONE_MATCH        }, 
        { "if-range",               WPS_HFC_DEF_IF_RANGE             }, 
        { "if-unmodified-since",    WPS_HFC_DEF_IF_UNMODIFIED_SINCE  }, 
        { "last-modified",          WPS_HFC_DEF_LAST_MODIFIED        }, 
        { "location",               WPS_HFC_DEF_LOCATION             }, 
        { "max-forwards",           WPS_HFC_DEF_MAX_FORWARDS         }, 
        { "pragma",                 WPS_HFC_DEF_PRAGMA               }, 
        { "proxy-authenticate",     WPS_HFC_DEF_PROXY_AUTHENTICATE   }, 
        { "proxy-authorization",    WPS_HFC_DEF_PROXY_AUTHORIZATION  }, 
        { "public",                 WPS_HFC_DEF_PUBLIC               }, 
        { "range",                  WPS_HFC_DEF_RANGE                }, 
        { "referer",                WPS_HFC_DEF_REFERER              }, 
        { "retry-after",            WPS_HFC_DEF_RETRY_AFTER          }, 
        { "server",                 WPS_HFC_DEF_SERVER               }, 
        { "transfer-encoding",      WPS_HFC_DEF_TRANSFER_ENCODING    }, 
        { "upgrade",                WPS_HFC_DEF_UPGRADE              }, 
        { "user-agent",             WPS_HFC_DEF_USER_AGENT           }, 
        { "vary",                   WPS_HFC_DEF_VARY                 }, 
        { "via",                    WPS_HFC_DEF_VIA                  }, 
        { "warning",                WPS_HFC_DEF_WARNING              }, 
        { "www-authenticate",       WPS_HFC_DEF_WWW_AUTHENTICATE     }, 
    };

    static const NameBytePair aFieldName2Num1_2[] =
    {
        { "accept",                 WPS_HFC_DEF_ACCEPT               }, 
        { "accept-application",     WPS_HFC_DEF_ACCEPT_APPLICATION   }, 
        { "accept-charset",         WPS_HFC_DEF_ACCEPT_CHARSET       }, 
        { "accept-encoding",        WPS_HFC_DEF_ACCEPT_ENCODING      }, 
        { "accept-language",        WPS_HFC_DEF_ACCEPT_LANGUAGE      }, 
        { "accept-ranges",          WPS_HFC_DEF_ACCEPT_RANGES        }, 
        { "age",                    WPS_HFC_DEF_AGE                  }, 
        { "allow",                  WPS_HFC_DEF_ALLOW                }, 
        { "authorization",          WPS_HFC_DEF_AUTHORIZATION        }, 
        { "bearer-indication",      WPS_HFC_DEF_BEARER_INDICATION    }, 
        { "cache-control",          WPS_HFC_DEF_CACHE_CONTROL        }, 
        { "connection",             WPS_HFC_DEF_CONNECTION           }, 
        { "content-base",           WPS_HFC_DEF_CONTENT_BASE         }, 
        { "content-disposition",    WPS_HFC_DEF_CONTENT_DISPOSITION  }, 
        { "content-encoding",       WPS_HFC_DEF_CONTENT_ENCODING     }, 
        { "content-language",       WPS_HFC_DEF_CONTENT_LANGUAGE     }, 
        { "content-length",         WPS_HFC_DEF_CONTENT_LENGTH       }, 
        { "content-location",       WPS_HFC_DEF_CONTENT_LOCATION     }, 
        { "content-md5",            WPS_HFC_DEF_CONTENT_MD5          }, 
        { "content-range",          WPS_HFC_DEF_CONTENT_RANGE        }, 
        { "content-type",           WPS_HFC_DEF_CONTENT_TYPE         }, 
        { "date",                   WPS_HFC_DEF_DATE                 }, 
        { "etag",                   WPS_HFC_DEF_ETAG                 }, 
        { "expires",                WPS_HFC_DEF_EXPIRES              }, 
        { "from",                   WPS_HFC_DEF_FROM                 }, 
        { "host",                   WPS_HFC_DEF_HOST                 }, 
        { "if-match",               WPS_HFC_DEF_IF_MATCH             }, 
        { "if-modified-since",      WPS_HFC_DEF_IF_MODIFIED_SINCE    }, 
        { "if-none-match",          WPS_HFC_DEF_IF_NONE_MATCH        }, 
        { "if-range",               WPS_HFC_DEF_IF_RANGE             }, 
        { "if-unmodified-since",    WPS_HFC_DEF_IF_UNMODIFIED_SINCE  }, 
        { "last-modified",          WPS_HFC_DEF_LAST_MODIFIED        }, 
        { "location",               WPS_HFC_DEF_LOCATION             }, 
        { "max-forwards",           WPS_HFC_DEF_MAX_FORWARDS         }, 
        { "pragma",                 WPS_HFC_DEF_PRAGMA               }, 
        { "profile",                WPS_HFC_DEF_PROFILE              }, 
        { "profile-diff",           WPS_HFC_DEF_PROFILE_DIFF         }, 
        { "profile-warning",        WPS_HFC_DEF_PROFILE_WARNING      }, 
        { "proxy-authenticate",     WPS_HFC_DEF_PROXY_AUTHENTICATE   }, 
        { "proxy-authorization",    WPS_HFC_DEF_PROXY_AUTHORIZATION  }, 
        { "public",                 WPS_HFC_DEF_PUBLIC               }, 
        { "push-flag",              WPS_HFC_DEF_PUSH_FLAG            }, 
        { "range",                  WPS_HFC_DEF_RANGE                }, 
        { "referer",                WPS_HFC_DEF_REFERER              }, 
        { "retry-after",            WPS_HFC_DEF_RETRY_AFTER          }, 
        { "server",                 WPS_HFC_DEF_SERVER               }, 
        { "transfer-encoding",      WPS_HFC_DEF_TRANSFER_ENCODING    }, 
        { "upgrade",                WPS_HFC_DEF_UPGRADE              }, 
        { "user-agent",             WPS_HFC_DEF_USER_AGENT           }, 
        { "vary",                   WPS_HFC_DEF_VARY                 }, 
        { "via",                    WPS_HFC_DEF_VIA                  }, 
        { "warning",                WPS_HFC_DEF_WARNING              }, 
        { "www-authenticate",       WPS_HFC_DEF_WWW_AUTHENTICATE     }, 
        { "x-wap-application-id",   WPS_HFC_DEF_X_WAP_APPLICATION_ID }, 
        { "x-wap-content-uri",      WPS_HFC_DEF_X_WAP_CONTENT_URI    }, 
        { "x-wap-initiator-uri",    WPS_HFC_DEF_X_WAP_INITIATOR_URI  }, 
    };

    static const NameBytePair aFieldName2Num1_3[] =
    {
        { "accept",                 WPS_HFC_DEF_ACCEPT               }, 
        { "accept-application",     WPS_HFC_DEF_ACCEPT_APPLICATION   }, 
        { "accept-charset",         WPS_HFC_DEF_ACCEPT_CHARSET       }, 
        { "accept-encoding",        WPS_HFC_DEF_ACCEPT_ENCODING      }, 
        { "accept-language",        WPS_HFC_DEF_ACCEPT_LANGUAGE      }, 
        { "accept-ranges",          WPS_HFC_DEF_ACCEPT_RANGES        }, 
        { "age",                    WPS_HFC_DEF_AGE                  }, 
        { "allow",                  WPS_HFC_DEF_ALLOW                }, 
        { "authorization",          WPS_HFC_DEF_AUTHORIZATION        }, 
        { "bearer-indication",      WPS_HFC_DEF_BEARER_INDICATION    }, 
        { "cache-control",          WPS_HFC_DEF_CACHE_CONTROL        }, 
        { "connection",             WPS_HFC_DEF_CONNECTION           }, 
        { "content-base",           WPS_HFC_DEF_CONTENT_BASE         }, 
        { "content-disposition",    WPS_HFC_DEF_CONTENT_DISPOSITION  }, 
        { "content-encoding",       WPS_HFC_DEF_CONTENT_ENCODING     }, 
        { "content-id",             WPS_HFC_DEF_CONTENT_ID           }, 
        { "content-language",       WPS_HFC_DEF_CONTENT_LANGUAGE     }, 
        { "content-length",         WPS_HFC_DEF_CONTENT_LENGTH       }, 
        { "content-location",       WPS_HFC_DEF_CONTENT_LOCATION     }, 
        { "content-md5",            WPS_HFC_DEF_CONTENT_MD5          }, 
        { "content-range",          WPS_HFC_DEF_CONTENT_RANGE        }, 
        { "content-type",           WPS_HFC_DEF_CONTENT_TYPE         }, 
        { "cookie",                 WPS_HFC_DEF_COOKIE               }, 
        { "date",                   WPS_HFC_DEF_DATE                 }, 
        { "encoding-version",       WPS_HFC_ENCODING_VERSION         }, 
        { "etag",                   WPS_HFC_DEF_ETAG                 }, 
        { "expect",                 WPS_HFC_DEF_EXPECT               }, 
        { "expires",                WPS_HFC_DEF_EXPIRES              }, 
        { "from",                   WPS_HFC_DEF_FROM                 }, 
        { "host",                   WPS_HFC_DEF_HOST                 }, 
        { "if-match",               WPS_HFC_DEF_IF_MATCH             }, 
        { "if-modified-since",      WPS_HFC_DEF_IF_MODIFIED_SINCE    }, 
        { "if-none-match",          WPS_HFC_DEF_IF_NONE_MATCH        }, 
        { "if-range",               WPS_HFC_DEF_IF_RANGE             }, 
        { "if-unmodified-since",    WPS_HFC_DEF_IF_UNMODIFIED_SINCE  }, 
        { "last-modified",          WPS_HFC_DEF_LAST_MODIFIED        }, 
        { "location",               WPS_HFC_DEF_LOCATION             }, 
        { "max-forwards",           WPS_HFC_DEF_MAX_FORWARDS         }, 
        { "pragma",                 WPS_HFC_DEF_PRAGMA               }, 
        { "profile",                WPS_HFC_DEF_PROFILE              }, 
        { "profile-diff",           WPS_HFC_DEF_PROFILE_DIFF         }, 
        { "profile-warning",        WPS_HFC_DEF_PROFILE_WARNING      }, 
        { "proxy-authenticate",     WPS_HFC_DEF_PROXY_AUTHENTICATE   }, 
        { "proxy-authorization",    WPS_HFC_DEF_PROXY_AUTHORIZATION  }, 
        { "public",                 WPS_HFC_DEF_PUBLIC               }, 
        { "push-flag",              WPS_HFC_DEF_PUSH_FLAG            }, 
        { "range",                  WPS_HFC_DEF_RANGE                }, 
        { "referer",                WPS_HFC_DEF_REFERER              }, 
        { "retry-after",            WPS_HFC_DEF_RETRY_AFTER          }, 
        { "server",                 WPS_HFC_DEF_SERVER               }, 
        { "set-cookie",             WPS_HFC_DEF_SET_COOKIE           }, 
        { "trailer",                WPS_HFC_DEF_TRAILER              }, 
        { "transfer-encoding",      WPS_HFC_DEF_TRANSFER_ENCODING    }, 
        { "upgrade",                WPS_HFC_DEF_UPGRADE              }, 
        { "user-agent",             WPS_HFC_DEF_USER_AGENT           }, 
        { "vary",                   WPS_HFC_DEF_VARY                 }, 
        { "via",                    WPS_HFC_DEF_VIA                  }, 
        { "warning",                WPS_HFC_DEF_WARNING              }, 
        { "www-authenticate",       WPS_HFC_DEF_WWW_AUTHENTICATE     }, 
        { "x-wap-application-id",   WPS_HFC_DEF_X_WAP_APPLICATION_ID }, 
        { "x-wap-content-uri",      WPS_HFC_DEF_X_WAP_CONTENT_URI    }, 
        { "x-wap-initiator-uri",    WPS_HFC_DEF_X_WAP_INITIATOR_URI  }, 
        { "x-wap-tod",              WPS_HFC_DEF_X_WAP_TOD            }, 
    };

    BOOL    bRet    = FALSE;

	/* Avoiding Warning */ if (pcPage);

    if (nPage == 1)
    { /* This is the default header code page - the only one we know */
        const NameBytePair * table;
        int size;

        switch (wapVer) {
        default:
             CODEC_ASSERT(FALSE);
            /* fall through */
        case HC_WapVersion1_1:      /* WAP 1.1 */
            table = aFieldName2Num1_1;
            size = DIM(aFieldName2Num1_1);
            break;
        case HC_WapVersion1_2:      /* WAP 1.2 */
            table = aFieldName2Num1_2;
            size = DIM(aFieldName2Num1_2);
            break;
        case HC_WapVersion1_3:      /* WAP 1.3 */
        case HC_WapVersion1_4:      /* WAP 1.4 */
        case HC_WapVersion1_5:      /* WAP 1.5 */
            table = aFieldName2Num1_3;
            size = DIM(aFieldName2Num1_3);
            break;
        }

        bRet = HC_Name2Byte_Ord (table, size, TRUE, pcName, pbValue);
    }
    return bRet;
} /* HC_FieldName2FieldCode */

/*
-----------------------------------------------------------------------------
*/
const char* HC_FieldCode2FieldName(
    BYTE        bPage, 
    const char* pcPage, 
    BYTE        bValue,
    HC_WapVersion wapVer)
{
    /* Header field name assignments/2: Numbers to field names. */
    static const NameBytePair aNum2FieldName1_1[] =
    /* Must be in numeric order */
    {
        { "Accept",                 WPS_HFC_DEF_ACCEPT               }, 
        { "Accept-Charset",         WPS_HFC_DEF_ACCEPT_CHARSET       }, 
        { "Accept-Encoding",        WPS_HFC_DEF_ACCEPT_ENCODING      }, 
        { "Accept-Language",        WPS_HFC_DEF_ACCEPT_LANGUAGE      }, 
        { "Accept-Ranges",          WPS_HFC_DEF_ACCEPT_RANGES        }, 
        { "Age",                    WPS_HFC_DEF_AGE                  }, 
        { "Allow",                  WPS_HFC_DEF_ALLOW                }, 
        { "Authorization",          WPS_HFC_DEF_AUTHORIZATION        }, 
        { "Cache-Control",          WPS_HFC_DEF_CACHE_CONTROL        }, 
        { "Connection",             WPS_HFC_DEF_CONNECTION           }, 
        { "Content-Base",           WPS_HFC_DEF_CONTENT_BASE         }, 
        { "Content-Encoding",       WPS_HFC_DEF_CONTENT_ENCODING     }, 
        { "Content-Language",       WPS_HFC_DEF_CONTENT_LANGUAGE     }, 
        { "Content-Length",         WPS_HFC_DEF_CONTENT_LENGTH       }, 
        { "Content-Location",       WPS_HFC_DEF_CONTENT_LOCATION     }, 
        { "Content-MD5",            WPS_HFC_DEF_CONTENT_MD5          }, 
        { "Content-Range",          WPS_HFC_DEF_CONTENT_RANGE        }, 
        { "Content-Type",           WPS_HFC_DEF_CONTENT_TYPE         }, 
        { "Date",                   WPS_HFC_DEF_DATE                 }, 
        { "Etag",                   WPS_HFC_DEF_ETAG                 }, 
        { "Expires",                WPS_HFC_DEF_EXPIRES              }, 
        { "From",                   WPS_HFC_DEF_FROM                 }, 
        { "Host",                   WPS_HFC_DEF_HOST                 }, 
        { "If-Modified-Since",      WPS_HFC_DEF_IF_MODIFIED_SINCE    }, 
        { "If-Match",               WPS_HFC_DEF_IF_MATCH             }, 
        { "If-None-Match",          WPS_HFC_DEF_IF_NONE_MATCH        }, 
        { "If-Range",               WPS_HFC_DEF_IF_RANGE             }, 
        { "If-Unmodified-Since",    WPS_HFC_DEF_IF_UNMODIFIED_SINCE  }, 
        { "Location",               WPS_HFC_DEF_LOCATION             }, 
        { "Last-Modified",          WPS_HFC_DEF_LAST_MODIFIED        }, 
        { "Max-Forwards",           WPS_HFC_DEF_MAX_FORWARDS         }, 
        { "Pragma",                 WPS_HFC_DEF_PRAGMA               }, 
        { "Proxy-Authenticate",     WPS_HFC_DEF_PROXY_AUTHENTICATE   }, 
        { "Proxy-Authorization",    WPS_HFC_DEF_PROXY_AUTHORIZATION  }, 
        { "Public",                 WPS_HFC_DEF_PUBLIC               }, 
        { "Range",                  WPS_HFC_DEF_RANGE                }, 
        { "Referer",                WPS_HFC_DEF_REFERER              }, 
        { "Retry-After",            WPS_HFC_DEF_RETRY_AFTER          }, 
        { "Server",                 WPS_HFC_DEF_SERVER               }, 
        { "Transfer-Encoding",      WPS_HFC_DEF_TRANSFER_ENCODING    }, 
        { "Upgrade",                WPS_HFC_DEF_UPGRADE              }, 
        { "User-Agent",             WPS_HFC_DEF_USER_AGENT           }, 
        { "Vary",                   WPS_HFC_DEF_VARY                 }, 
        { "Via",                    WPS_HFC_DEF_VIA                  }, 
        { "Warning",                WPS_HFC_DEF_WARNING              }, 
        { "WWW-Authenticate",       WPS_HFC_DEF_WWW_AUTHENTICATE     }, 
        { "Content-Disposition",    WPS_HFC_DEF_CONTENT_DISPOSITION  }, 
    };

    static const NameBytePair aNum2FieldName1_2[] =
    /* Must be in numeric order */
    {
        { "Accept",                 WPS_HFC_DEF_ACCEPT               }, 
        { "Accept-Charset",         WPS_HFC_DEF_ACCEPT_CHARSET       }, 
        { "Accept-Encoding",        WPS_HFC_DEF_ACCEPT_ENCODING      }, 
        { "Accept-Language",        WPS_HFC_DEF_ACCEPT_LANGUAGE      }, 
        { "Accept-Ranges",          WPS_HFC_DEF_ACCEPT_RANGES        }, 
        { "Age",                    WPS_HFC_DEF_AGE                  }, 
        { "Allow",                  WPS_HFC_DEF_ALLOW                }, 
        { "Authorization",          WPS_HFC_DEF_AUTHORIZATION        }, 
        { "Cache-Control",          WPS_HFC_DEF_CACHE_CONTROL        }, 
        { "Connection",             WPS_HFC_DEF_CONNECTION           }, 
        { "Content-Base",           WPS_HFC_DEF_CONTENT_BASE         }, 
        { "Content-Encoding",       WPS_HFC_DEF_CONTENT_ENCODING     }, 
        { "Content-Language",       WPS_HFC_DEF_CONTENT_LANGUAGE     }, 
        { "Content-Length",         WPS_HFC_DEF_CONTENT_LENGTH       }, 
        { "Content-Location",       WPS_HFC_DEF_CONTENT_LOCATION     }, 
        { "Content-MD5",            WPS_HFC_DEF_CONTENT_MD5          }, 
        { "Content-Range",          WPS_HFC_DEF_CONTENT_RANGE        }, 
        { "Content-Type",           WPS_HFC_DEF_CONTENT_TYPE         }, 
        { "Date",                   WPS_HFC_DEF_DATE                 }, 
        { "Etag",                   WPS_HFC_DEF_ETAG                 }, 
        { "Expires",                WPS_HFC_DEF_EXPIRES              }, 
        { "From",                   WPS_HFC_DEF_FROM                 }, 
        { "Host",                   WPS_HFC_DEF_HOST                 }, 
        { "If-Modified-Since",      WPS_HFC_DEF_IF_MODIFIED_SINCE    }, 
        { "If-Match",               WPS_HFC_DEF_IF_MATCH             }, 
        { "If-None-Match",          WPS_HFC_DEF_IF_NONE_MATCH        }, 
        { "If-Range",               WPS_HFC_DEF_IF_RANGE             }, 
        { "If-Unmodified-Since",    WPS_HFC_DEF_IF_UNMODIFIED_SINCE  }, 
        { "Location",               WPS_HFC_DEF_LOCATION             }, 
        { "Last-Modified",          WPS_HFC_DEF_LAST_MODIFIED        }, 
        { "Max-Forwards",           WPS_HFC_DEF_MAX_FORWARDS         }, 
        { "Pragma",                 WPS_HFC_DEF_PRAGMA               }, 
        { "Proxy-Authenticate",     WPS_HFC_DEF_PROXY_AUTHENTICATE   }, 
        { "Proxy-Authorization",    WPS_HFC_DEF_PROXY_AUTHORIZATION  }, 
        { "Public",                 WPS_HFC_DEF_PUBLIC               }, 
        { "Range",                  WPS_HFC_DEF_RANGE                }, 
        { "Referer",                WPS_HFC_DEF_REFERER              }, 
        { "Retry-After",            WPS_HFC_DEF_RETRY_AFTER          }, 
        { "Server",                 WPS_HFC_DEF_SERVER               }, 
        { "Transfer-Encoding",      WPS_HFC_DEF_TRANSFER_ENCODING    }, 
        { "Upgrade",                WPS_HFC_DEF_UPGRADE              }, 
        { "User-Agent",             WPS_HFC_DEF_USER_AGENT           }, 
        { "Vary",                   WPS_HFC_DEF_VARY                 }, 
        { "Via",                    WPS_HFC_DEF_VIA                  }, 
        { "Warning",                WPS_HFC_DEF_WARNING              }, 
        { "WWW-Authenticate",       WPS_HFC_DEF_WWW_AUTHENTICATE     }, 
        { "Content-Disposition",    WPS_HFC_DEF_CONTENT_DISPOSITION  }, 
        { "X-Wap-Application-Id",   WPS_HFC_DEF_X_WAP_APPLICATION_ID }, 
        { "X-Wap-Content-URI",      WPS_HFC_DEF_X_WAP_CONTENT_URI    }, 
        { "X-Wap-Initiator-URI",    WPS_HFC_DEF_X_WAP_INITIATOR_URI  }, 
        { "Accept-Application",     WPS_HFC_DEF_ACCEPT_APPLICATION   }, 
        { "Bearer-Indication",      WPS_HFC_DEF_BEARER_INDICATION    }, 
        { "Push-Flag",              WPS_HFC_DEF_PUSH_FLAG            }, 
        { "Profile",                WPS_HFC_DEF_PROFILE              }, 
        { "Profile-Diff",           WPS_HFC_DEF_PROFILE_DIFF         }, 
        { "Profile-Warning",        WPS_HFC_DEF_PROFILE_WARNING      }, 
    };

    static const NameBytePair aNum2FieldName1_3[] =
    /* Must be in numeric order */
    {
        { "Accept",                 WPS_HFC_DEF_ACCEPT               }, 
        { "Accept-Charset",         WPS_HFC_DEF_ACCEPT_CHARSET       }, 
        { "Accept-Encoding",        WPS_HFC_DEF_ACCEPT_ENCODING      }, 
        { "Accept-Language",        WPS_HFC_DEF_ACCEPT_LANGUAGE      }, 
        { "Accept-Ranges",          WPS_HFC_DEF_ACCEPT_RANGES        }, 
        { "Age",                    WPS_HFC_DEF_AGE                  }, 
        { "Allow",                  WPS_HFC_DEF_ALLOW                }, 
        { "Authorization",          WPS_HFC_DEF_AUTHORIZATION        }, 
        { "Cache-Control",          WPS_HFC_DEF_CACHE_CONTROL        }, 
        { "Connection",             WPS_HFC_DEF_CONNECTION           }, 
        { "Content-Base",           WPS_HFC_DEF_CONTENT_BASE         }, 
        { "Content-Encoding",       WPS_HFC_DEF_CONTENT_ENCODING     }, 
        { "Content-Language",       WPS_HFC_DEF_CONTENT_LANGUAGE     }, 
        { "Content-Length",         WPS_HFC_DEF_CONTENT_LENGTH       }, 
        { "Content-Location",       WPS_HFC_DEF_CONTENT_LOCATION     }, 
        { "Content-MD5",            WPS_HFC_DEF_CONTENT_MD5          }, 
        { "Content-Range",          WPS_HFC_DEF_CONTENT_RANGE        }, 
        { "Content-Type",           WPS_HFC_DEF_CONTENT_TYPE         }, 
        { "Date",                   WPS_HFC_DEF_DATE                 }, 
        { "Etag",                   WPS_HFC_DEF_ETAG                 }, 
        { "Expires",                WPS_HFC_DEF_EXPIRES              }, 
        { "From",                   WPS_HFC_DEF_FROM                 }, 
        { "Host",                   WPS_HFC_DEF_HOST                 }, 
        { "If-Modified-Since",      WPS_HFC_DEF_IF_MODIFIED_SINCE    }, 
        { "If-Match",               WPS_HFC_DEF_IF_MATCH             }, 
        { "If-None-Match",          WPS_HFC_DEF_IF_NONE_MATCH        }, 
        { "If-Range",               WPS_HFC_DEF_IF_RANGE             }, 
        { "If-Unmodified-Since",    WPS_HFC_DEF_IF_UNMODIFIED_SINCE  }, 
        { "Location",               WPS_HFC_DEF_LOCATION             }, 
        { "Last-Modified",          WPS_HFC_DEF_LAST_MODIFIED        }, 
        { "Max-Forwards",           WPS_HFC_DEF_MAX_FORWARDS         }, 
        { "Pragma",                 WPS_HFC_DEF_PRAGMA               }, 
        { "Proxy-Authenticate",     WPS_HFC_DEF_PROXY_AUTHENTICATE   }, 
        { "Proxy-Authorization",    WPS_HFC_DEF_PROXY_AUTHORIZATION  }, 
        { "Public",                 WPS_HFC_DEF_PUBLIC               }, 
        { "Range",                  WPS_HFC_DEF_RANGE                }, 
        { "Referer",                WPS_HFC_DEF_REFERER              }, 
        { "Retry-After",            WPS_HFC_DEF_RETRY_AFTER          }, 
        { "Server",                 WPS_HFC_DEF_SERVER               }, 
        { "Transfer-Encoding",      WPS_HFC_DEF_TRANSFER_ENCODING    }, 
        { "Upgrade",                WPS_HFC_DEF_UPGRADE              }, 
        { "User-Agent",             WPS_HFC_DEF_USER_AGENT           }, 
        { "Vary",                   WPS_HFC_DEF_VARY                 }, 
        { "Via",                    WPS_HFC_DEF_VIA                  }, 
        { "Warning",                WPS_HFC_DEF_WARNING              }, 
        { "WWW-Authenticate",       WPS_HFC_DEF_WWW_AUTHENTICATE     }, 
        { "Content-Disposition",    WPS_HFC_DEF_CONTENT_DISPOSITION  }, 
        { "X-Wap-Application-Id",   WPS_HFC_DEF_X_WAP_APPLICATION_ID }, 
        { "X-Wap-Content-URI",      WPS_HFC_DEF_X_WAP_CONTENT_URI    }, 
        { "X-Wap-Initiator-URI",    WPS_HFC_DEF_X_WAP_INITIATOR_URI  }, 
        { "Accept-Application",     WPS_HFC_DEF_ACCEPT_APPLICATION   }, 
        { "Bearer-Indication",      WPS_HFC_DEF_BEARER_INDICATION    }, 
        { "Push-Flag",              WPS_HFC_DEF_PUSH_FLAG            }, 
        { "Profile",                WPS_HFC_DEF_PROFILE              }, 
        { "Profile-Diff",           WPS_HFC_DEF_PROFILE_DIFF         }, 
        { "Profile-Warning",        WPS_HFC_DEF_PROFILE_WARNING      }, 
        { "Expect",                 WPS_HFC_DEF_EXPECT               }, 
        { "Trailer",                WPS_HFC_DEF_TRAILER              }, 
        { "X-Wap-Tod",              WPS_HFC_DEF_X_WAP_TOD            }, 
        { "Content-Id",             WPS_HFC_DEF_CONTENT_ID           }, 
        { "Set-Cookie",             WPS_HFC_DEF_SET_COOKIE           }, 
        { "Cookie",                 WPS_HFC_DEF_COOKIE               }, 
        { "Encoding-Version",       WPS_HFC_ENCODING_VERSION         }, 
    };

    const char* pcName  = NULL;

	/* Avoiding Warning */ if (pcPage);

    if (bPage == 1)
    { /* This is the default header code page - the only one we know */
        const NameBytePair * table;
        int size;

        switch (wapVer) {
        case HC_WapVersion1_1:      /* WAP 1.1 */
            table = aNum2FieldName1_1;
            size = DIM(aNum2FieldName1_1);
            break;
        case HC_WapVersion1_2:      /* WAP 1.2 */
            table = aNum2FieldName1_2;
            size = DIM(aNum2FieldName1_2);
            break;
        default:
             CODEC_ASSERT(FALSE);
            /* fall through */
        case HC_WapVersion1_3:      /* WAP 1.3 */
        case HC_WapVersion1_4:      /* WAP 1.4 */
        case HC_WapVersion1_5:      /* WAP 1.5 */
            table = aNum2FieldName1_3;
            size = DIM(aNum2FieldName1_3);
            break;
        }
        pcName = HC_Byte2Name_Ord(table, size, bValue);
    }
    return pcName;
} /* HC_FieldCode2FieldName */
