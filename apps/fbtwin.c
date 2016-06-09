#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <unistd.h>
#include <syscall.h>
#include <twin_clock.h>
#include <twin_text.h>
#include <twin_demo.h>
#include <twin_hello.h>
#include <twin_calc.h>
#include <twin_demoline.h>
#include <twin_demospline.h>

#include "twin_fbdev.h"
#include "twin_linux_mouse.h"

#define maybe_unused __attribute__((unused))

twin_fbdev_t *tf;

static void exitfunc(void)
{
	if (tf)
		twin_fbdev_destroy(tf);
	tf = NULL;
}

static void sigint(int maybe_unused sig)
{
	exitfunc();
	syscall(__NR_exit);
}

static unsigned int twin_def_cursor_image[] = {
        0x00000000, 0x88ffffff, 0x88ffffff, 0x00000000,
        0x88ffffff, 0xff000000, 0xff000000, 0x88ffffff,
        0x88ffffff, 0xff000000, 0xff000000, 0x88ffffff,
        0x00000000, 0x88ffffff, 0x88ffffff, 0x00000000,

        0x00000000, 0x88ffffff, 0x88ffffff, 0x00000000,
        0x88ffffff, 0xff000000, 0xff000000, 0x88ffffff,
        0x88ffffff, 0xff000000, 0xff000000, 0x88ffffff,
        0x00000000, 0x88ffffff, 0x88ffffff, 0x00000000,

        0x00000000, 0x88ffffff, 0x88ffffff, 0x00000000,
        0x88ffffff, 0xff000000, 0xff000000, 0x88ffffff,
        0x88ffffff, 0xff000000, 0xff000000, 0x88ffffff,
        0x00000000, 0x88ffffff, 0x88ffffff, 0x00000000,

        0x00000000, 0x88ffffff, 0x88ffffff, 0x00000000,
        0x88ffffff, 0xff000000, 0xff000000, 0x88ffffff,
        0x88ffffff, 0xff000000, 0xff000000, 0x88ffffff,
        0x00000000, 0x88ffffff, 0x88ffffff, 0x00000000,
};

static twin_pixmap_t *twin_get_default_cursor(int *hx, int *hy)
{
    twin_pixmap_t *cur;
    twin_pointer_t data;

    data.argb32 = twin_def_cursor_image;
    cur = twin_pixmap_create_const(TWIN_ARGB32, 4+4, 4+4, 16+16, data);
    if (cur == NULL)
        return NULL;
    *hx = *hy = 4;
    return cur;
}

int main (void)
{
	int hx, hy;
	twin_pixmap_t *cur;

	tf = twin_fbdev_create(-1, SIGUSR1);
	if (tf == NULL)
		return 1;

	atexit(exitfunc);
	signal(SIGINT, sigint);

	twin_linux_mouse_create(NULL, tf->screen);

	cur = twin_get_default_cursor(&hx, &hy);
	if (cur != NULL)
		twin_screen_set_cursor(tf->screen, cur, hx, hy);
	twin_screen_set_background (tf->screen, twin_make_pattern ());

#if 1
    twin_demo_start (tf->screen, "Demo", 100, 100, 400, 400);
#endif
#if 1
    twin_text_start (tf->screen,  "Gettysburg Address", 400, 200, 300, 300);
#endif
#if 1
    twin_hello_start (tf->screen, "Hello, World", 200, 30, 200, 200);
#endif
#if 1
    twin_clock_start (tf->screen, "Clock", 10, 10, 200, 200);
#endif
#if 1
    twin_calc_start (tf->screen, "Calculator", 350, 450, 200, 200);
#endif
#if 1
    twin_demoline_start (tf->screen, "Demo Line", 200, 200, 200, 200);
#endif
#if 1
    twin_demospline_start (tf->screen, "Demo Spline", 520, 320, 400, 400);
#endif

	twin_fbdev_activate(tf);

	twin_dispatch ();

	return 0;
}
