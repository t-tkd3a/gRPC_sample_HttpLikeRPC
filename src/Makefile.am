CPPFLAGS += `pkg-config --cflags protobuf grpc`
CXXFLAGS += -std=c++11

LDFLAGS +=\
 `pkg-config --libs protobuf grpc++ grpc`\
 -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed -ldl


# protoc を用いての *.proto から .grpc.pb.cc と .pb.cc の作成ルール
PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

.PRECIOUS: %.grpc.pb.cc
%.grpc.pb.cc: %.proto
	$(PROTOC) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

.PRECIOUS: %.pb.cc
%.pb.cc: %.proto
	$(PROTOC) --cpp_out=. $<

dist_noinst_DATA = http_like_rpc.proto

# protoc での作成物の make clean 時の削除
MOSTLYCLEANFILES = \ 
 http_like_rpc.pb.cc\
 http_like_rpc.pb.h \
 http_like_rpc.grpc.pb.cc\
 http_like_rpc.grpc.pb.h 


# 生成物毎のルール
## 
bin_PROGRAMS=
bin_PROGRAMS+= greeting_server
### .pb.cc → .grpc.pb.cc → grpcを用いるソース群 の順。 
### .protoc で .proto → pb.cc , .proto → .grpc.pb.cc が行われる
greeting_server_SOURCES= \
	http_like_rpc.pb.cc \
	http_like_rpc.grpc.pb.cc \
	greeting_server.cpp 
greeting_server_LDADD=

## 
bin_PROGRAMS+= greeting_client
### .pb.cc → .grpc.pb.cc → grpcを用いるソース群 の順。 
### .protoc で .proto → pb.cc , .proto → .grpc.pb.cc が行われる
greeting_client_SOURCES= \
	http_like_rpc.pb.cc \
	http_like_rpc.grpc.pb.cc \
	greeting_client.cpp 
greeting_client_LDADD=

AM_CPPFLAGS = 
AM_CPPFLAGS += -I$(builddir)


