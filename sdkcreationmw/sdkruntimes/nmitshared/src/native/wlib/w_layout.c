/*
 * $Id: w_layout.c,v 1.2 2002/06/13 03:58:39 slava Exp $
 *
 * Copyright (C) 2000-2002 by Slava Monich
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 *
 *   1.Redistributions of source code must retain the above copyright 
 *     notice, this list of conditions and the following disclaimer. 
 *   2.Redistributions in binary form must reproduce the above copyright 
 *     notice, this list of conditions and the following disclaimer 
 *     in the documentation and/or other materials provided with the 
 *     distribution. 
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) ARISING 
 * IN ANY WAY OUT OF THE USE OR INABILITY TO USE THIS SOFTWARE, EVEN 
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 *
 * The views and conclusions contained in the software and documentation 
 * are those of the authors and should not be interpreted as representing 
 * any official policies, either expressed or implied.
 */

#include "w_lib.h"

#define MAX_LOOPS 10000
#define POSITION_BASE 100
#define IS_RESIZABLE_WIDGET(_x,_y,_z) TRUE

#define _RECT_Width(_r) ((_r).right - (_r).left)
#define _RECT_Height(_r) ((_r).bottom - (_r).top)
#define RECT_Equal(_r1,_r2) \
    (((_r1).left == (_r2).left) && \
    ((_r1).right == (_r2).right) && \
    ((_r1).top == (_r2).top) && \
    ((_r1).bottom == (_r2).bottom))

#define WINDOW_HasStyle(_hwnd,_style) \
    ((GetWindowLong(_hwnd,GWL_STYLE) & (_style)) == 0)

/*==========================================================================*
 *          L A Y O U T
 *==========================================================================*/

STATIC int LAYOUT_ConvertOffset(HWND hwnd, int nOffset, ATTACH_TYPE nType)
{
    if (nOffset) {
        RECT rcOffset;
        rcOffset.left = rcOffset.right = 0;
        rcOffset.right = nOffset;
        rcOffset.bottom = nOffset;

        /* MapDialogRect fails for non-dialog windows */
        if (MapDialogRect(hwnd, &rcOffset)) {
            switch (nType) {
            case ATTACH_LEFT: case ATTACH_RIGHT:   return rcOffset.right;
            case ATTACH_TOP:  case ATTACH_BOTTOM:  return rcOffset.bottom;
            default: ASSERT(FALSE);
            }          
        }
    }      
    return nOffset;
}

/*
 * Calculates item rectangle
 */
STATIC
void 
LAYOUT_CalcItemRect(
    HWND hwnd, 
    const GEOMETRY_RULE * rules, 
    const GEOMETRY_RULE * pItem, 
    LPRECT rcClient, 
    LPRECT rcItem)
{
    int nItem;
    BOOL bFound;
    const GEOMETRY_RULE * pOtherItem; 

    RECT rcSave;
    int nOffset = LAYOUT_ConvertOffset(hwnd, pItem->nOffset, pItem->nType);
    rcSave = (*rcItem);
          
    /*  branch to appropriate attach method */
    switch (pItem->nMethod) {

    /*----------------------------------------------------------------------
     * the dialog item is attached to one the borders of the parent dialog.
     *---------------------------------------------------------------------*/
    case ATTACH_FORM:                                                  
    {                                     
      switch (pItem->nType)
      {
        case ATTACH_LEFT:  rcItem->left  = nOffset; break;
        case ATTACH_RIGHT: rcItem->right = RECT_Width(rcClient) - nOffset; break;
        case ATTACH_TOP:   rcItem->top   = nOffset; break;
        case ATTACH_BOTTOM:rcItem->bottom= RECT_Height(rcClient)- nOffset; break;
        default: ASSERT(FALSE);
      }          
    }
    break;

    /*----------------------------------------------------------------------
     * the dialog item is attached to the opposite border of the parent 
     *---------------------------------------------------------------------*/
    case ATTACH_OPPOSITE_FORM:
    {                                       
      switch (pItem->nType)
      {
        case ATTACH_LEFT:  rcItem->left  = RECT_Width(rcClient) - nOffset; break;
        case ATTACH_RIGHT: rcItem->right = nOffset; break;
        case ATTACH_TOP:   rcItem->top   = RECT_Height(rcClient)- nOffset; break;
        case ATTACH_BOTTOM:rcItem->bottom= nOffset; break;
        default: ASSERT(FALSE);
      }          
    }
    break;

    /*----------------------------------------------------------------------
     * the dialog item is attached to another dialog item
     *---------------------------------------------------------------------*/
    case ATTACH_WIDGET:                      
    {              
      /* get the rectangle of the other item we're attached to */
      RECT rcOtherItem;                                           
      int nOtherItem = pItem->nOtherID;
      HWND hwndOtherItem = GetDlgItem(hwnd, nOtherItem);
      if (!IsWindow(hwndOtherItem)) {
        ASSMSG("Invalid item ID");
        break;
      }                                                

      VERIFY(GetWindowRect(hwndOtherItem, &rcOtherItem));
      VERIFY(ScreenToClient(hwnd, (LPPOINT)&rcOtherItem));
      VERIFY(ScreenToClient(hwnd, ((LPPOINT)&rcOtherItem)+1));

      switch (pItem->nType) {
        case ATTACH_LEFT:  rcItem->left  = rcOtherItem.right + nOffset; break;
        case ATTACH_RIGHT: rcItem->right = rcOtherItem.left  - nOffset; break;
        case ATTACH_TOP:   rcItem->top   = rcOtherItem.bottom+ nOffset; break;
        case ATTACH_BOTTOM:rcItem->bottom= rcOtherItem.top   - nOffset; break;
        default: ASSERT(FALSE);
      }
    }
    break;

    /*----------------------------------------------------------------------
     * the dialog item is attached to the opposite side of another item
     *---------------------------------------------------------------------*/
    case ATTACH_OPPOSITE_WIDGET:
    {              
      /* get the rectangle of the other item we're attached to */
      RECT rcOtherItem;                                           
      HWND hwndOtherItem = GetDlgItem(hwnd, pItem->nOtherID);
      if (!IsWindow(hwndOtherItem)) {
        ASSMSG1("Invalid item ID %d",pItem->nOtherID);
        break;
      }

      VERIFY(GetWindowRect(hwndOtherItem, &rcOtherItem));
      VERIFY(ScreenToClient(hwnd, (LPPOINT)&rcOtherItem));
      VERIFY(ScreenToClient(hwnd, ((LPPOINT)&rcOtherItem)+1));
  
      switch (pItem->nType) {
        case ATTACH_LEFT:  rcItem->left  = rcOtherItem.left  + nOffset; break;
        case ATTACH_RIGHT: rcItem->right = rcOtherItem.right - nOffset; break;
        case ATTACH_TOP:   rcItem->top   = rcOtherItem.top   + nOffset; break;
        case ATTACH_BOTTOM:rcItem->bottom= rcOtherItem.bottom- nOffset; break;
        default: ASSERT(FALSE);
      }
    }
    break;

    /*----------------------------------------------------------------------
     * the dialog item is attached to a specified position inside the dialog
     *---------------------------------------------------------------------*/
    case ATTACH_POSITION:
    {
      long tmp;             /* have to do something about overflow... */
      switch (pItem->nType)
      {
        case ATTACH_LEFT:  
          tmp = RECT_Width(rcClient); 
          tmp *= pItem->nPos; 
          tmp /= POSITION_BASE; 
          rcItem->left  = nOffset + (int)tmp;
          break;
    
        case ATTACH_RIGHT: 
          tmp = RECT_Width(rcClient); 
          tmp *= pItem->nPos; 
          tmp /= POSITION_BASE; 
          rcItem->right = nOffset + (int)tmp;
          break;
    
        case ATTACH_TOP:   
          tmp = RECT_Height(rcClient); 
          tmp *= pItem->nPos; 
          tmp /= POSITION_BASE; 
          rcItem->top   = nOffset + (int)tmp;
          break;
    
        case ATTACH_BOTTOM:
          tmp = RECT_Height(rcClient); 
          tmp *= pItem->nPos; 
          tmp /= POSITION_BASE; 
          rcItem->bottom = nOffset + (int)tmp;
          break;
    
        default: ASSERT(FALSE);
      }
    }
    break;

    case ATTACH_SELF:    
    default: 
        ASSERT(FALSE);
        return;
    }    

    /*
     * If the opposite side of the item is not attached anywhere, we want 
     * to move the whole item without changing its size.
     */
    bFound = FALSE;
    pOtherItem = rules;
    
    for (nItem=0; pOtherItem->nMethod != ATTACH_NONE; (nItem++,pOtherItem++)) {
      if (pOtherItem->nID == pItem->nID && pOtherItem != pItem &&
        ((pOtherItem->nType == ATTACH_RIGHT  && pItem->nType == ATTACH_LEFT) ||
         (pOtherItem->nType == ATTACH_LEFT   && pItem->nType == ATTACH_RIGHT) ||
         (pOtherItem->nType == ATTACH_TOP    && pItem->nType == ATTACH_BOTTOM) ||
         (pOtherItem->nType == ATTACH_BOTTOM && pItem->nType == ATTACH_TOP))) 
      {          
        bFound = TRUE;
        break;
      }
    }

    /*
     * if there's no rule for the opposite side of the control, we need to 
     * fix the coordinates of the opposite side to keep the size of the 
     * window unchanged
     */
    if (!bFound)
      switch (pItem->nType)
      {
        case ATTACH_LEFT:  rcItem->right  += rcItem->left   - rcSave.left;   break;
        case ATTACH_RIGHT: rcItem->left   += rcItem->right  - rcSave.right;  break;
        case ATTACH_TOP:   rcItem->bottom += rcItem->top    - rcSave.top;    break;
        case ATTACH_BOTTOM:rcItem->top    += rcItem->bottom - rcSave.bottom; break;
        default: ASSERT(FALSE);
      }

    /* sanity check */
    if (rcItem->right < rcItem->left) rcItem->right = rcItem->left;
    if (rcItem->bottom < rcItem->top) rcItem->bottom = rcItem->top;

    /* Now check if control is outside the window */
    if (rcItem->right < rcClient->left) 
        OffsetRect(rcItem, rcClient->left - rcItem->left,0);
    if (rcItem->left > rcClient->right) 
        OffsetRect(rcItem, rcClient->right - rcItem->right,0);
    if (rcItem->bottom < rcClient->top ) 
        OffsetRect(rcItem, 0,rcClient->top - rcItem->top);
    if (rcItem->top > rcClient->bottom) 
        OffsetRect(rcItem, 0,rcClient->bottom - rcItem->bottom);
}

/**
 * Layout the container given a set of geometry rules
 */
void WIN32_Layout(HWND hwnd, const GEOMETRY_RULE * rules)
{
    RECT rcClient;
    memset(&rcClient, 0, sizeof(rcClient));
    GetClientRect(hwnd, &rcClient);

    if (rcClient.bottom > rcClient.top && rcClient.right > rcClient.left) {

        int i,j;
        int nMoves = 1; /* number of controls moved/resized in the last loop */

        /*
         * loop until the layout is settled (nMoves == 0) or maximum 
         * number of iterations is exceeded (usually caused by contradictory 
         * layout rules
         */
        for (i=0; i<MAX_LOOPS && nMoves; i++) {

            /*
             * we keep the last item's rectangle in attempt to optimize the 
             * layout. the point is - when we have several rules in a row 
             * for the same item (which is normally the case), we don't 
             * need to move the item after each step, it's well enough to 
             * move it after we have processed the last rule.
             */
            HWND hwndLast = NULL;                   /* last item */
            RECT rcItem;                            /* item rectangle */
            RECT rcItemNew;                         /* new item rectangle */    
            const GEOMETRY_RULE * pItem = rules;    /* first rule */

            /*
             *  allocate memory for a multiple-window position operations. 
             * The DeferWindowPos function increases the size of the 
             * structure, if necessary. 
             */
            HDWP hWinPosInfo = BeginDeferWindowPos(nMoves);

            /* reset number of moves */
            nMoves = 0; 

            /* process all rules */
            for (j=0; pItem->nMethod != ATTACH_NONE; (j++,pItem++)) {                                          
               
                /* 
                 * check that the item ID is valid and get its window 
                 * rectangle and the rectangle relative to the upper left 
                 * corner of the target window
                 */
                HWND hwndItem = GetDlgItem(hwnd, pItem->nID);
                if (!IsWindow( hwndItem)) {
                    ASSMSG2("Rule #%d, no such item: %d",j+1,pItem->nID);
                    continue;       /* try the next one */
                }

                /* ignore hidden controls */
                /*
                if (WINDOW_HasStyle(hwndItem,WS_VISIBLE)) {
                    continue;
                }
                */

                /*
                 * get the item's rectangle and convert it to client 
                 * coordinates of the form
                 */
                if (hwndLast != hwndItem) {

                    /* 
                     * if the item's rectangle is different from what we 
                     * already have, resize the item and increment nMoves 
                     * to indicate that we need another iteration
                     */
                    if (hwndLast && !RECT_Equal(rcItemNew,rcItem)) {
                        nMoves++;
                        hWinPosInfo = DeferWindowPos( 
                                      hWinPosInfo, hwndLast, NULL,
                                      rcItemNew.left, rcItemNew.top,
                                      _RECT_Width(rcItemNew), 
                                      _RECT_Height(rcItemNew),
                                      SWP_NOZORDER | SWP_NOACTIVATE);
                    }                      
                    /* get rectangle of the new item */
                    GetWindowRect(hwndItem, &rcItem);
                    ScreenToClient(hwnd, (LPPOINT)&rcItem);
                    ScreenToClient(hwnd, ((LPPOINT)&rcItem)+1);
                    rcItemNew = rcItem;
                    hwndLast = hwndItem;
                }

                /* now see how we want to fix this item's size and position */
                LAYOUT_CalcItemRect( hwnd, rules, pItem, &rcClient, &rcItemNew);
            }

            /* check the last item's rectangle */
            if (hwndLast && !RECT_Equal(rcItemNew,rcItem)) {                
                nMoves++;
                hWinPosInfo = DeferWindowPos( 
                              hWinPosInfo, hwndLast, NULL,
                              rcItemNew.left, rcItemNew.top,
                              _RECT_Width(rcItemNew), 
                              _RECT_Height(rcItemNew),
                              SWP_NOZORDER | SWP_NOACTIVATE );
            }

            VERIFY(EndDeferWindowPos(hWinPosInfo));
        }
  
        /*
         * if MAX_LOOPS wasn't enough to settle the layout, something
         * must be TERRIBLY WRONG. Typically it's caused by contradictory 
         * set of geometry rules. Yet another possible cause is that the
         * window size has become too small. 
         */
        ASSERT(!nMoves);
    }
}
