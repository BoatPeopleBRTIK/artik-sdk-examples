
ARTIK_SDK_LIBS := \
	libartik-sdk-base \
	libartik-sdk-connectivity

TEST_EXE_NAME := network-test
TEST_OBJS := \
	artik_network_test.o

DHCP_CLIENT_EXE_NAME := dhcp-client
DHCP_CLIENT_OBJS := \
	artik_dhcp_client_test.o

DHCP_SERVER_EXE_NAME := dhcp-server
DHCP_SERVER_OBJS := \
	artik_dhcp_server_test.o

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
	${CC} -o $@ $^ $(LDFLAGS)

$(DHCP_CLIENT_EXE_NAME): $(DHCP_CLIENT_OBJS)
	${CC} -o $@ $^ $(LDFLAGS)

$(DHCP_SERVER_EXE_NAME): $(DHCP_SERVER_OBJS)
	${CC} -o $@ $^ $(LDFLAGS)

all: $(TEST_EXE_NAME) $(DHCP_CLIENT_EXE_NAME) $(DHCP_SERVER_EXE_NAME)

clean:
	rm -f *.o $(TEST_EXE_NAME) $(DHCP_CLIENT_EXE_NAME) $(DHCP_SERVER_EXE_NAME)
