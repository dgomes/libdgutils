-include Makefile.options

SOCKET_OBJS = Socket.o SocketServer.o SocketClient.o SocketMulticast.o
ADDRESS_OBJS = ipv4address.o ipv6address.o lladdress.o
CONFIG_OBJS = Configuration.o NetDevice.o
CONF_TEST = conf_test test_netdevice test_ipv6address test_ipv4address 
ADDRESS_TEST = test_address
SOCKET_TEST = test_mc test_socket test_socket_server
ALL_TEST = ${CONF_TEST} ${SOCKET_TEST} ${ADDRESS_TEST} 

CXX_FLAGS += -Wall -g 

.cc.o:
	$(CXX) $(CXX_FLAGS) -c $< 

TARGET = libdgutils.a

all: ${TARGET}

tests: ${ALL_TEST}

conf_test: conf_test.o libconfiguration.a
	$(CXX) -o $@ $^

test_netdevice: test_netdevice.o libconfiguration.a libaddress.a
	$(CXX) $(CXX_FLAGS) -o $@ $^ $(NETLINK) -lutil 

test_address: test_address.o libaddress.a
	$(CXX) -o $@ $^

test_ipv4address: test_ipv4address.o libaddress.a
	$(CXX) -o $@ $^

test_ipv6address: test_ipv6address.o libaddress.a
	$(CXX) -o $@ $^

test_raw_socket: test_raw_socket.o libsocket.a libaddress.a
	$(CXX) -o $@ $^ $(BOOST_THREAD)

test_mc: test_mc.o libsocket.a libconfiguration.a libaddress.a
	$(CXX) -s -o $@ $^ $(BOOST_THREAD) $(NETLINK)

ara_mc: ara_mc.o libsocket.a libconfiguration.a libaddress.a 
	$(CXX) -s -o $@ $^ -lnetlink 

test_socket: test_socket.o libsocket.a
	$(CXX) $(CXX_FLAGS) -s -o $@ $^ $(BOOST_THREAD) 

test_socket_server: test_socket_server.o libsocket.a
	$(CXX) $(CXX_FLAGS) -s -o $@ $^ $(BOOST_THREAD) 

libconfiguration.a: ${CONFIG_OBJS}
	ar cru $@ $^ 
	ranlib $@

libsocket.a:	${SOCKET_OBJS}	
	ar cru $@ $^
	ranlib $@

libaddress.a:	${ADDRESS_OBJS}
	ar cru $@ $^
	ranlib $@

libdgutils.a: ${SOCKET_OBJS} ${ADDRESS_OBJS} ${CONFIG_OBJS} 
	ar cru $@ $^
	ranlib $@

clean:
	rm -f *.a *.o ${ALL_TEST} 

install: all 
	cp ${TARGET} /usr/local/lib
	cp *.h /usr/local/include/libdgutils
	ldconfig
