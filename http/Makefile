
ARTIK_SDK_LIBS := \
	libartik-sdk-base \
	libartik-sdk-connectivity

TEST_EXE_NAME := http-test
TEST_OBJS := \
	artik_http_test.o
TEST_LIBS := \
	-lcrypto \
	-lssl

OPENSSL_EXE_NAME := http-openssl-test
OPENSSL_OBJS := \
	artik_http_openssl_test.o

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

$(TEST_EXE_NAME): $(TEST_OBJS)
	${CC} -o $@ $^ $(LDFLAGS) $(TEST_LIBS)

$(OPENSSL_EXE_NAME): $(OPENSSL_OBJS)
	${CC} -o $@ $^ $(LDFLAGS)

all: $(TEST_EXE_NAME) $(OPENSSL_EXE_NAME)

clean:
	rm -f *.o $(TEST_EXE_NAME) $(OPENSSL_EXE_NAME)
