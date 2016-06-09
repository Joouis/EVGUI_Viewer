/*
 * Twin - A Tiny Window System
 * Copyright © 2004 Keith Packard <keithp@keithp.com>
 * All rights reserved.
 */

#ifndef _TWIN_X11_H_
#define _TWIN_X11_H_

#include <twin.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

typedef struct _twin_x11 {
    twin_screen_t   *screen;
    Display	    *dpy;
    Window	    win;
    GC		    gc;
    Visual	    *visual;
    int		    depth;
    XImage	    *image;
    int		    image_y;
} twin_x11_t;

/*
 * twin_x11.c 
 */

twin_x11_t *
twin_x11_create_ext (Display *dpy, int width, int height, int handle_events);

static inline twin_x11_t *
twin_x11_create (Display *dpy, int width, int height)
	{return twin_x11_create_ext(dpy, width, height, 1);}

void
twin_x11_destroy (twin_x11_t *tx);

void
twin_x11_damage (twin_x11_t *tx, XExposeEvent *ev);

void
twin_x11_configure (twin_x11_t *tx, XConfigureEvent *ev);

void
twin_x11_update (twin_x11_t *tx);

twin_bool_t
twin_x11_process_events (twin_x11_t *tx);

#endif /* _TWIN_X11_H_ */
