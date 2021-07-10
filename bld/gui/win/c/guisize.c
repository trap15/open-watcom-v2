/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "guiwind.h"
#include "guixutil.h"
#include "guixwind.h"
#include "guixhook.h"
#include "guiscale.h"
#include "guirect.h"
#include "guipaint.h"
#include "guimdi.h"


bool GUIResizeWindow( gui_window *wnd, gui_rect *rect )
{
    guix_coord  scr_pos;
    guix_coord  scr_size;
    gui_window  *parent;
    HWND        frame;
    HWND        phwnd;
    HWND        rphwnd;
    WPI_POINT   pt;

    GUIInvalidatePaintHandles( wnd );
    if( wnd->hwnd != NULLHANDLE ) {
        frame = GUIGetParentFrameHWND( wnd );
        parent = wnd->parent;
        phwnd = HWND_DESKTOP;
        if( parent ) {
            phwnd = parent->hwnd;
        }
        GUICalcLocation( rect, &scr_pos, &scr_size, phwnd );
        if( GUI_IS_DIALOG( wnd ) ) {
            // dialogs are owned by, but not children of, phwnd
            // so lets map pos to its real parent
            pt.x = scr_pos.x;
            pt.y = scr_pos.y;
            rphwnd = _wpi_getparent( frame );
            _wpi_mapwindowpoints( phwnd, rphwnd, &pt, 1 );
            _wpi_movewindow( frame, pt.x, pt.y, scr_size.x, scr_size.y, TRUE );

// The following is a bandaid 'till I find out why WM_SIZE's aren't
// generated for PM GUI dialogs by this fuction
#ifdef __OS2_PM__
            {
                gui_coord   size;

                _wpi_getclientrect( frame, &wnd->hwnd_client_rect );
                wnd->root_client_rect = wnd->hwnd_client_rect;
                GUISetRowCol( wnd, NULL );
                size.x = GUIScreenToScaleH( scr_size.x );
                size.y = GUIScreenToScaleV( scr_size.y );
                GUIEVENT( wnd, GUI_RESIZE, &size );
            }
#endif
        } else {
            _wpi_setwindowpos( frame, NULLHANDLE, scr_pos.x, scr_pos.y, scr_size.x, scr_size.y,
                               SWP_NOACTIVATE | SWP_NOZORDER | SWP_SIZE | SWP_MOVE );
                               //SWP_NOREDRAW | SWP_NOACTIVATE | SWP_NOZORDER | SWP_SIZE | SWP_MOVE );
        }
    }
    return( true );
}

void GUISetRestoredSize( gui_window *wnd, gui_rect *rect )
{
    guix_coord          scr_pos;
    guix_coord          scr_size;
    gui_window          *parent;
    HWND                frame;
    HWND                phwnd;
    WPI_RECT            new_rect;

    if( GUIIsMaximized( wnd ) || GUIIsMinimized( wnd ) ) {
        if( wnd->hwnd != NULLHANDLE ) {
            frame = GUIGetParentFrameHWND( wnd );
            parent = wnd->parent;
            phwnd = HWND_DESKTOP;
            if( parent ) {
                phwnd = parent->hwnd;
            }
            GUICalcLocation( rect, &scr_pos, &scr_size, phwnd );
            _wpi_setrectvalues( &new_rect, scr_pos.x, scr_pos.y,
                    scr_pos.x + scr_size.x, scr_pos.y + scr_size.y );
            if( GUIMDIMaximized( wnd ) ) {
                GUISetMDIRestoredSize( frame, &new_rect );
            } else {
                _wpi_setrestoredrect( frame, &new_rect );
            }
        }
    } else {
        GUIResizeWindow( wnd, rect );
    }
}

bool GUIGetRestoredSize( gui_window *wnd, gui_rect *rect )
{
    HWND                top_hwnd;
    HWND                parent;
    WPI_RECT            new_rect;

    if( wnd->hwnd != NULLHANDLE ) {
        top_hwnd = GUIGetParentFrameHWND( wnd );
        parent = _wpi_getparent( top_hwnd );
        _wpi_getrestoredrect( top_hwnd, &new_rect );
        _wpi_mapwindowpoints( parent, HWND_DESKTOP, (WPI_LPPOINT)&new_rect, 2 );
        DoGetRelRect( top_hwnd, &new_rect, rect, (wnd->style & GUI_POPUP) );
        return( true );
    }

    return( false );
}

void GUIMinimizeWindow( gui_window *wnd )
{
    if( wnd->style & GUI_MINIMIZE ) {
        _wpi_minimizewindow( GUIGetParentFrameHWND( wnd ) );
    }
}

void GUIMaximizeWindow( gui_window *wnd )
{
    if( wnd->style & GUI_MAXIMIZE ) {
        if( GUIIsMDIChildWindow( wnd ) ) {
            GUIMDIMaximize( true, GUIGetFront() );
        } else {
            _wpi_maximizewindow( GUIGetParentFrameHWND( wnd ) );
        }
    }
}

void GUIHideWindow( gui_window *wnd )
{
    _wpi_showwindow( GUIGetParentFrameHWND( wnd ), SW_HIDE );
}

bool GUIIsWindowVisible( gui_window *wnd )
{
    HWND        hwnd;

    hwnd = GUIGetParentFrameHWND( wnd );
    if( hwnd != NULLHANDLE ) {
        if( IsWindowVisible( hwnd ) ) {
            return( true );
        }
    }
    return( false );
}

void GUIRestoreWindow( gui_window *wnd )
{
    if( GUIIsMDIChildWindow( wnd ) && GUIMDIMaximized( wnd ) ) {
        GUIMDIMaximize( false, GUIGetFront() );
    } else {
        _wpi_restorewindow( GUIGetParentFrameHWND( wnd ) );
    }
}

bool GUIIsMaximized( gui_window *wnd )
{
    bool        maximized;

    if( GUIMDI ) {
        maximized = GUIMDIMaximized( wnd );
    } else {
        maximized = _wpi_iszoomed( GUIGetParentFrameHWND( wnd ) );
    }
    return( maximized );
}

bool GUIIsMinimized( gui_window *wnd )
{
    return( _wpi_isiconic( GUIGetParentFrameHWND( wnd ) ) );
}
