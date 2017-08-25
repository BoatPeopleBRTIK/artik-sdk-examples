
ARTIK_SDK_LIBS := \
	libartik-sdk-base \
	libartik-sdk-connectivity

TEST_EXE_NAME := security-test
TEST_OBJS := \
	artik_security_test.o

PKCS7_SIG_VERIF_EXE_NAME := pkcs7-sig-verify
PKCS7_SIG_VERIF_OBJS := \
	pkcs7_sig_verify.o

CC := ${CROSS_COMPILE}gcc
ifdef SYSROOT
PKG_CONFIG := PKG_CONFIG_PATH=${SYSROOT}/usr/lib/pkgconfig/ PKG_CONFIG_SYSROOT_DIR=${SYSROOT} pkg-config
CFLAGS := ${CFLAGS}
LDFLAGS := ${LDFLAGS} --sysroot=${SYSROOT} -L${SYSROOT}/usr/lib
else
PKG_CONFIG := PKG_CONFIG_PATH=${SYSROOT}/usr/lib/pkgconfig/ pkg-config
endif

CFLAGS := ${CFLAGS} $(shell ${PKG_CONFIG} --cflags --libs ${ARTIK_SDK_LIBS})
LDFLAGS := ${LDFLAGS} $(shell ${PKG_CONFIG} --libs ${ARTIK_SDK_LIBS})

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(TEST_EXE_NAME): $(TEST_OBJS)
	${CC} -o $@ $^ $(LDFLAGS)

$(PKCS7_SIG_VERIF_EXE_NAME): $(PKCS7_SIG_VERIF_OBJS)
	${CC} -o $@ $^ $(LDFLAGS)

all: $(TEST_EXE_NAME) $(PKCS7_SIG_VERIF_EXE_NAME)

clean:
	rm -f *.o $(TEST_EXE_NAME) $(PKCS7_SIG_VERIF_EXE_NAME)
