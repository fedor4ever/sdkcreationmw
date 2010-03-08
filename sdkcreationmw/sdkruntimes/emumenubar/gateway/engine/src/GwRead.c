/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include "EcmtGwP.h"

/**
 * Reads the specified number of bytes from the connection to the buffer.
 * Returns False on any error, including I/O error and out of memory.
 */
STATIC Bool GWENG_Read(EcmtConnection* con, Buffer* buf, int nbytes)
{
    int prevSize = BUFFER_Size(buf);
    if (BUFFER_EnsureCapacity(buf, BUFFER_Size(buf) + nbytes, False)) {
        I8u* ptr;
        BUFFER_Access(buf);
        if (buf->start > 0) {
            memmove(buf->data, buf->data + buf->start, BUFFER_Size(buf));
            buf->end -= buf->start;
            buf->start = 0;
        }
        ptr = buf->data;
        ptr += BUFFER_Size(buf);
        while (nbytes > 0) {
            int n = GWENG_IoRead(con, ptr, nbytes);
            if (n > 0) {
                buf->end += n;
                nbytes -= n;
                ptr += n;
            } else {
                BUFFER_Unput(buf, BUFFER_Size(buf) - prevSize);
                return False;
            }
        }
        return True;
    } else {
        return False;
    }
}

/**
 * Reads Ecmt messages from one connection and sends it to another.
 * Invoked on its own thread.
 */
void GWENG_Reader(void* ctx)
{
    EcmtReader* reader = ctx;
    Bool readingHeader = True;
    Bool protErr = False;
    Buffer buf;
    int uid;
    int chunkSize = ECMT_MSG_HEADER_SIZE;
    
    BUFFER_Init(&buf);
    while (GWENG_Read(reader->in, &buf, chunkSize)) {
        if (readingHeader) {
            /* we have received complete header */
            int len;
            ASSERT(BUFFER_Size(&buf) == ECMT_MSG_HEADER_SIZE);
            if (GWENG_ParseEcmtHeader(BUFFER_Access(&buf), &uid, &len)) {
                chunkSize = len - ECMT_MSG_HEADER_SIZE;
                GWENG_IoHeaderReceived(reader->in, uid, chunkSize);
                readingHeader = False;
            } else {
                TRACE("GWENG: protocol error!\n");
                protErr = True;
                break;
            }
        } else {
            /* we have received complete packet */
            const char* msg = BUFFER_Access(&buf);
            const char* data = msg + ECMT_MSG_HEADER_SIZE;
            ASSERT(BUFFER_Size(&buf) == (ECMT_MSG_HEADER_SIZE + chunkSize));
            GWENG_IoPacketReceived(reader->in, uid, data, chunkSize);

            /* Filter the packet */
            if (!reader->filter || 
                reader->filter(reader,uid,data,chunkSize)) {

                /* forward the packet */
                GWENG_IoPacketStart(reader->out, uid, data, chunkSize);
                if (GWENG_IoWrite(reader->out, msg, BUFFER_Size(&buf))) {
                    GWENG_IoPacketSent(reader->out);
                } else {
                    break;
                }
            }

            /* prepare for the new packet */
            chunkSize = ECMT_MSG_HEADER_SIZE;
            readingHeader = True;
            BUFFER_Clear(&buf);
        }
    }

    /* cleanup */
    BUFFER_Destroy(&buf);
    reader->done(reader, protErr); 
    GWENG_IoReleaseRef(reader->in);
    GWENG_IoReleaseRef(reader->out);
    MEM_Free(reader);
}
