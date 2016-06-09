#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

#include "twinint.h"
#include "twin_linux_mouse.h"

#define DEFAULT_ACC_NUMERATOR   	(2*TWIN_FIXED_ONE)
#define DEFAULT_ACC_DENOMINATOR 	TWIN_FIXED_ONE
#define DEFAULT_ACC_THRESHOLD   	(4*TWIN_FIXED_ONE)

static void twin_linux_mouse_check_bounds(twin_linux_mouse_t *tm)
{
	if (tm->x < 0)
		tm->x = 0;
	if (tm->x > tm->screen->width)
		tm->x = tm->screen->width;
	if (tm->y < 0)
		tm->y = 0;
	if (tm->y > tm->screen->height)
		tm->y = tm->screen->height;
}

/* This is directly copied from kdrive */
static void twin_linux_mouse_accel(twin_linux_mouse_t *tm, int *dx, int *dy)
{
	int square = *dx * *dx + *dy * *dy;
	twin_fixed_t speed = square>65535 ? twin_int_to_fixed(256) : 
			     twin_fixed_sqrt(twin_int_to_fixed(square));
	twin_fixed_t accel;
	accel = TWIN_FIXED_ONE;

	if (speed > tm->acc_threshold)
		accel = twin_fixed_div(tm->acc_num, tm->acc_den);
	*dx = twin_fixed_to_int(twin_fixed_mul(accel, twin_int_to_fixed(*dx)));
	*dy = twin_fixed_to_int(twin_fixed_mul(accel, twin_int_to_fixed(*dy)));
}

static twin_bool_t twin_linux_mouse_events(int file,
					   twin_file_op_t maybe_unused ops,
					   void *closure)
{
	twin_linux_mouse_t *tm = closure;
	char evts[34];
	char *ep;
	int n = tm->res_cnt;
	twin_event_t tev;

	if (n)
		memcpy(evts, tm->residual, n);
	n += read(file, evts + n, 32);

	for(ep = evts; n >= 3; n -= 3, ep += 3) {
		int dx, dy, btn;
		dx = ep[1];
		if (ep[0] & 0x10)
			dx -= 256;
		dy = ep[2];
		if (ep[0] & 0x20)
			dy -= 256;
		dy = -dy;
		/* we handle only one btn for now */
		btn = ep[0] & 0x1;
		if (dx || dy) {
			twin_linux_mouse_accel(tm, &dx, &dy);
			tm->x += dx;
			tm->y += dy;
			twin_linux_mouse_check_bounds(tm);
			tev.kind = TwinEventMotion;
			tev.u.pointer.screen_x = tm->x;
			tev.u.pointer.screen_y = tm->y;
			tev.u.pointer.button = tm->btns;
			twin_screen_dispatch (tm->screen, &tev);
		}
		if (btn != tm->btns) {
			tm->btns = btn;
			tev.kind = (btn & 0x1) ?
				TwinEventButtonDown : TwinEventButtonUp;
			tev.u.pointer.screen_x = tm->x;
			tev.u.pointer.screen_y = tm->y;
			tev.u.pointer.button = tm->btns;
			twin_screen_dispatch(tm->screen, &tev);
		}
	}
	tm->res_cnt = n;
	if (n)
		memcpy(tm->residual, ep, n);

	return 1;
}

twin_linux_mouse_t *twin_linux_mouse_create(const char *file,
					    twin_screen_t *screen)
{
	twin_linux_mouse_t *tm;

	tm = calloc(1, sizeof(twin_linux_mouse_t));
	if (tm == NULL)
		return NULL;

	if (file == NULL)
		file = "/dev/input/mice";

	tm->screen = screen;
	tm->acc_num = DEFAULT_ACC_NUMERATOR;
	tm->acc_den = DEFAULT_ACC_DENOMINATOR;
	tm->acc_threshold = DEFAULT_ACC_THRESHOLD;
	tm->x = screen->width / 2; 
	tm->y = screen->height / 2; 
	tm->fd = open(file, O_RDONLY);
	if (tm->fd < 0) {
		free(tm);
		return NULL;
	}

	twin_set_file(twin_linux_mouse_events, tm->fd, TWIN_READ, tm);

	return tm;
}

void twin_linux_mouse_destroy(twin_linux_mouse_t *tm)
{
	close(tm->fd);
	free(tm);
}

void twin_linux_mouse_screen_changed(twin_linux_mouse_t *tm)
{
	int oldx, oldy;

	oldx = tm->x;
	oldy = tm->y;
	twin_linux_mouse_check_bounds(tm);
	if (tm->x != oldx || tm->y != oldy) {
		twin_event_t tev;

		tev.kind = TwinEventMotion;
		tev.u.pointer.screen_x = tm->x;
		tev.u.pointer.screen_y = tm->y;
		tev.u.pointer.button = tm->btns;
		twin_screen_dispatch (tm->screen, &tev);
	}
}
