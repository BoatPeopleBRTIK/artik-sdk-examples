
ARTIK_SDK_LIBS := \
	libartik-sdk-base \
	libartik-sdk-bluetooth \
	gio-2.0

TEST_EXE_NAME := bluetooth-test
TEST_OBJS := \
	artik_bluetooth_test.o

ADV_EXE_NAME := bluetooth-test-advertiser
ADV_OBJS := \
	artik_bluetooth_test_commandline.o \
	artik_bluetooth_test_advertiser.o
	
AGENT_EXE_NAME := bluetooth-test-agent
AGENT_OBJS := \
	artik_bluetooth_test_commandline.o \
	artik_bluetooth_test_agent.o
	
AVRCP_EXE_NAME := bluetooth-test-avrcp
AVRCP_OBJS := \
	artik_bluetooth_test_commandline.o \
	artik_bluetooth_test_avrcp.o

FTP_EXE_NAME := bluetooth-test-ftp
FTP_OBJS := \
	artik_bluetooth_test_commandline.o \
	artik_bluetooth_test_ftp.o

GATT_CLIENT_RW_EXE_NAME := bluetooth-test-gatt_client_rw
GATT_CLIENT_RW_OBJS := \
	artik_bluetooth_test_commandline.o \
	artik_bluetooth_test_gatt_client_rw.o

GATT_CLIENT_EXE_NAME := bluetooth-test-gatt_client
GATT_CLIENT_OBJS := \
	artik_bluetooth_test_commandline.o \
	artik_bluetooth_test_gatt_client.o

GATT_SERVER_EXE_NAME := bluetooth-test-gatt_server
GATT_SERVER_OBJS := \
	artik_bluetooth_test_commandline.o \
	artik_bluetooth_test_gatt_server.o

NAP_EXE_NAME := bluetooth-test-nap
NAP_OBJS := \
	artik_bluetooth_test_commandline.o \
	artik_bluetooth_test_nap.o

OBSERVER_EXE_NAME := bluetooth-test-observer
OBSERVER_OBJS := \
	artik_bluetooth_test_commandline.o \
	artik_bluetooth_test_observer.o

PANU_EXE_NAME := bluetooth-test-panu
PANU_OBJS := \
	artik_bluetooth_test_commandline.o \
	artik_bluetooth_test_panu.o

SPP_CLIENT_EXE_NAME := bluetooth-test-spp_client
SPP_CLIENT_OBJS := \
	artik_bluetooth_test_commandline.o \
	artik_bluetooth_test_spp_client.o
	
SPP_EXE_NAME := bluetooth-test-spp
SPP_OBJS := \
	artik_bluetooth_test_commandline.o \
	artik_bluetooth_test_spp.o

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

$(ADV_EXE_NAME): $(ADV_OBJS)
	${CC} -o $@ $^ $(LDFLAGS)

$(AGENT_EXE_NAME): $(AGENT_OBJS)
	${CC} -o $@ $^ $(LDFLAGS)

$(AVRCP_EXE_NAME): $(AVRCP_OBJS)
	${CC} -o $@ $^ $(LDFLAGS)

$(FTP_EXE_NAME): $(FTP_OBJS)
	${CC} -o $@ $^ $(LDFLAGS)

$(GATT_CLIENT_RW_EXE_NAME): $(GATT_CLIENT_RW_OBJS)
	${CC} -o $@ $^ $(LDFLAGS)

$(GATT_CLIENT_EXE_NAME): $(GATT_CLIENT_OBJS)
	${CC} -o $@ $^ $(LDFLAGS)

$(GATT_SERVER_EXE_NAME): $(GATT_SERVER_OBJS)
	${CC} -o $@ $^ $(LDFLAGS)

$(NAP_EXE_NAME): $(NAP_OBJS)
	${CC} -o $@ $^ $(LDFLAGS)

$(OBSERVER_EXE_NAME): $(OBSERVER_OBJS)
	${CC} -o $@ $^ $(LDFLAGS)

$(PANU_EXE_NAME): $(PANU_OBJS)
	${CC} -o $@ $^ $(LDFLAGS)

$(SPP_CLIENT_EXE_NAME): $(SPP_CLIENT_OBJS)
	${CC} -o $@ $^ $(LDFLAGS)

$(SPP_EXE_NAME): $(SPP_OBJS)
	${CC} -o $@ $^ $(LDFLAGS)

all: $(TEST_EXE_NAME) $(ADV_EXE_NAME) $(AGENT_EXE_NAME) $(AVRCP_EXE_NAME) $(FTP_EXE_NAME) $(GATT_CLIENT_RW_EXE_NAME) $(GATT_CLIENT_EXE_NAME) $(GATT_SERVER_EXE_NAME) $(NAP_EXE_NAME) $(OBSERVER_EXE_NAME) $(PANU_EXE_NAME) $(SPP_CLIENT_EXE_NAME) $(SPP_EXE_NAME)

clean:
	rm -f *.o $(TEST_EXE_NAME) $(ADV_EXE_NAME) $(AGENT_EXE_NAME) $(AVRCP_EXE_NAME) $(FTP_EXE_NAME) $(GATT_CLIENT_RW_EXE_NAME) $(GATT_CLIENT_EXE_NAME) $(GATT_SERVER_EXE_NAME) $(NAP_EXE_NAME) $(OBSERVER_EXE_NAME) $(PANU_EXE_NAME) $(SPP_CLIENT_EXE_NAME) $(SPP_EXE_NAME)
