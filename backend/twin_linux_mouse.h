#ifndef _TWIN_LINUX_MOUSE_H_
#define _TWIN_LINUX_MOUSE_H_

#include <twin.h>

typedef struct _twin_linux_mouse {
	twin_screen_t   *screen;

	/* acceleration settings */
	twin_fixed_t	acc_num;
	twin_fixed_t	acc_den;
	twin_fixed_t	acc_threshold;

	/* internals */
	int		fd;
	char		residual[2];
	int		res_cnt;
	int		btns;
	int		x,y;
} twin_linux_mouse_t;

/**
 * twin_linux_mouse_create - create the linux mouse driver
 * @file: device file to open, pass NULL for default
 */
twin_linux_mouse_t *twin_linux_mouse_create(const char *file,
					    twin_screen_t *screen);

/**
 * twin_linux_mouse_destroy - destroy the linux mouse driver
 */
void twin_linux_mouse_destroy(twin_linux_mouse_t *tm);

/**
 * twin_linux_mouse_set_bounds - set mouse boundaries
 */
void twin_linux_mouse_screen_changed(twin_linux_mouse_t *tm);

#endif /* _TWIN_LINUX_MOUSE_H_ */
