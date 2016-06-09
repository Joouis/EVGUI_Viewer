LIBSUF ?= .a

LIB_SRCS := \
	src/twin_box.c \
	src/twin_file.c \
	src/twin_poly.c \
	src/twin_toplevel.c \
	src/twin_button.c \
	src/twin_fixed.c \
	src/twin_label.c \
	src/twin_primitive.c \
	src/twin_trig.c \
	src/twin_convolve.c \
	src/twin_font.c \
	src/twin_matrix.c \
	src/twin_queue.c \
	src/twin_widget.c \
	src/twin_font_default.c \
	src/twin_path.c \
	src/twin_screen.c \
	src/twin_window.c \
	src/twin_dispatch.c \
	src/twin_geom.c \
	src/twin_pattern.c \
	src/twin_spline.c \
	src/twin_work.c \
	src/twin_draw.c \
	src/twin_hull.c \
	src/twin_pixmap.c \
	src/twin_timeout.c \
	libsvgtiny/src/svgColor2.c \
	libsvgtiny/src/svgtiny.c \
	libsvgtiny/src/svgtiny_gradient.c \
	libsvgtiny/src/svgtiny_list.c \
	libsvgtiny/src/xml2dom.c \
	libsvgtiny/src/ezxml.c

LIB_OBJS := $(patsubst %.c, %.o, $(LIB_SRCS))
PRODUCTS := libtwin$(LIBSUF)

DEMO_SRCS = \
	apps/twin_calc.c \
	apps/twin_demospline.c \
	apps/twin_clock.c \
	apps/twin_demoline.c \
	apps/twin_hello.c \
	apps/twin_text.c \
	apps/twin_demo.c \
	apps/twin_svg.c

DEMO_OBJS = $(patsubst %.c, %.o, $(DEMO_SRCS))

X_BACKEND_SRCS = \
	backend/twin_x11.c \
	apps/xtwin.c

X_BACKEND_OBJS := $(patsubst %.c, %.o, $(X_BACKEND_SRCS))

FB_BACKEND_SRCS = \
	backend/twin_fbdev.c \
	backend/twin_linux_mouse.c \
	apps/fbtwin.c

FB_BACKEND_OBJS := $(patsubst %.c, %.o, $(FB_BACKEND_SRCS))

.PHONY: all
all: $(PRODUCTS) xdemo

xdemo: $(PRODUCTS) $(DEMO_OBJS) $(X_BACKEND_OBJS)
	$(CC) $(CFLAGS) -o $@ \
		$(DEMO_OBJS) $(X_BACKEND_SRCS) $(PRODUCTS) \
		-lX11 -lm

fbdemo: $(PRODUCTS) $(DEMO_OBJS) $(FB_BACKEND_OBJS)
	$(CC) $(CFLAGS) -o $@ \
		$(DEMO_OBJS) $(FB_BACKEND_SRCS) $(PRODUCTS)

%$(LIBSUF): $(LIB_OBJS)
	$(AR) -r $@ $?

CFLAGS = \
	-Wall -pipe -Os -ffast-math \
	-I include \
	-I backend \
	-I apps \
	-I libsvgtiny/src \
	-I libsvgtiny/inc \
	-g -DEZXML_NOMMAP

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $< -lm

.PHONY: clean
clean:
	@rm -fv xdemo fbdemo \
		$(LIB_OBJS) $(DEMO_OBJS) \
		$(X_BACKEND_OBJS) $(FB_BACKEND_OBJS) \
		*$(LIBSUF) | xargs echo --
