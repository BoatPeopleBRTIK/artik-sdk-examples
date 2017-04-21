
ARTIK_SDK_LIBS := \
	libartik-sdk-base \
	libartik-sdk-zigbee

TEST_EXE_NAME := zigbee-test
TEST_OBJS := \
	artik_zigbee_test_common.o \
	artik_zigbee_test.o

CLI_EXE_NAME := zigbee-cli	
CLI_OBJS := \
	artik_zigbee_cli.o

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

$(CLI_EXE_NAME): $(CLI_OBJS)
	${CC} -o $@ $^ $(LDFLAGS)
	
all: $(TEST_EXE_NAME) $(CLI_EXE_NAME)

clean:
	rm -f *.o $(TEST_EXE_NAME) $(CLI_EXE_NAME)
