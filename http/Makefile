# Specify here the name to give to the resulting executable
EXE_NAME := http-test

# Add here the SDK libraries you want to link against
ARTIK_SDK_LIBS := \
	libartik-sdk-base \
	libartik-sdk-connectivity

# Add here the source files (.c) you want to compile and link
# Use .o extension to target same name .c file.
OBJS := \
	artik_http_test.o

CC := ${CROSS_COMPILE}gcc
ifdef SYSROOT
PKG_CONFIG := PKG_CONFIG_PATH=${SYSROOT}/usr/lib/pkgconfig/ PKG_CONFIG_SYSROOT_DIR=${SYSROOT} pkg-config
CFLAGS := ${CFLAGS}# --sysroot=${SYSROOT}
LDFLAGS := ${LDFLAGS} --sysroot=${SYSROOT} -L${SYSROOT}/usr/lib
else
PKG_CONFIG := PKG_CONFIG_PATH=${SYSROOT}/usr/lib/pkgconfig/ pkg-config
endif

CFLAGS := ${CFLAGS} $(shell ${PKG_CONFIG} --cflags --libs ${ARTIK_SDK_LIBS})
LDFLAGS := ${LDFLAGS} $(shell ${PKG_CONFIG} --libs ${ARTIK_SDK_LIBS})

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXE_NAME): $(OBJS)
	${CC} -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o $(EXE_NAME)
