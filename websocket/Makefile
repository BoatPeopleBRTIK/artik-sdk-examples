
ARTIK_SDK_LIBS := \
	libartik-sdk-base \
	libartik-sdk-connectivity

TEST_EXE_NAME := websocket-test
TEST_OBJS := \
	artik_websocket_test.o

CLOUD_EXE_NAME := websocket-cloud
CLOUD_OBJS := \
	artik_websocket_cloud_test.o

CLIENT_EXE_NAME := websocket-client
CLIENT_OBJS := \
	artik_websocket_client_test.o

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

$(CLOUD_EXE_NAME): $(CLOUD_OBJS)
	${CC} -o $@ $^ $(LDFLAGS)

$(CLIENT_EXE_NAME): $(CLIENT_OBJS)
	${CC} -o $@ $^ $(LDFLAGS)

all: $(TEST_EXE_NAME) $(CLOUD_EXE_NAME) $(CLIENT_EXE_NAME)

clean:
	rm -f *.o $(TEST_EXE_NAME) $(CLOUD_EXE_NAME) $(CLIENT_EXE_NAME)
