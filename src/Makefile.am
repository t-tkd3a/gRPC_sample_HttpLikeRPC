CPPFLAGS += `pkg-config --cflags protobuf grpc`
CXXFLAGS += -std=c++11
LDFLAGS += -L/usr/local/lib `pkg-config --libs protobuf grpc++ grpc`\
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
           -ldl

# protobuf と grpc に付いてもここで設定したいが、方法が分からず...

bin_PROGRAMS=
bin_PROGRAMS+= greeting_server
greeting_server_SOURCES= \
	greeting_server.cpp \
	http_like_rpc.pb.cc \
	http_like_rpc.grpc.pb.cc
greeting_server_LDADD=

bin_PROGRAMS+= greeting_client
greeting_client_SOURCES= \
	greeting_client.cpp \
	http_like_rpc.pb.cc \
	http_like_rpc.grpc.pb.cc
greeting_client_LDADD=

AM_CPPFLAGS = 
AM_CPPFLAGS += -I$(builddir)


