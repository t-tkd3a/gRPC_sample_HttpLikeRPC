#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([cpp_template],[0.0.0],[support@retrieva.jp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign subdir-objects])
LT_INIT

PKG_CHECK_MODULES(PROTOBUF, protobuf >= 2.4.0)
AC_SUBST(PROTOBUF_LIBS)
AC_SUBST(PROTOBUF_CFLAGS)
AC_SUBST(PROTOBUF_VERSION)

AC_CHECK_PROG([PROTOC], [protoc], [protoc])
AS_IF([test "x${PROTOC}" == "x"],
    [AC_MSG_ERROR([ProtoBuf compiler "protoc" not found.])])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h float.h limits.h netdb.h stddef.h stdlib.h string.h strings.h sys/socket.h sys/time.h sys/timeb.h unistd.h wchar.h wctype.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_STRERROR_R
AC_CHECK_FUNCS([dup2 fchdir getcwd getpagesize gettimeofday localtime_r memset mkdir munmap regcomp rmdir socket strcasecmp strchr strdup strerror strrchr strstr strtol strtoull])

AC_CONFIG_FILES([Makefile
                 gtest/Makefile
                 src/Makefile])

PKG_CHECK_MODULES([GRPC], [grpc >= 7.0.0],
                  [
                  AM_CONDITIONAL(HAVE_GRPC, true)
                  AC_SUBST(have_grpc, 1)
                  AC_SUBST(grpc_version, `pkg-config --modversion grpc`)
                  ],
                  [
                  AM_CONDITIONAL(HAVE_GRPC, false)
                  AC_SUBST(grpc_version, 0)
                  AC_MSG_ERROR([Can not find voice_recognizer. Please check whether grpc is installed. Maybe you want to set environment variable PKG_CONFIG_PATH.])
                  ])


PKG_CHECK_MODULES([GRPCPP], [grpc++ >= 1.20.0],
                  [
                  AM_CONDITIONAL(HAVE_GRPCPP, true)
                  AC_SUBST(have_grpcpp, 1)
                  AC_SUBST(grpcpp_version, `pkg-config --modversion grpc++`)
                  ],
                  [
                  AM_CONDITIONAL(HAVE_GRPCPP, false)
                  AC_SUBST(grpcpp_version, 0)
                  AC_MSG_ERROR([Can not find voice_recognizer. Please check whether grpc++ is installed. Maybe you want to set environment variable PKG_CONFIG_PATH.])
                  ])



AC_OUTPUT

AC_MSG_RESULT([
Configure Information:
  C Compiler        : $CC
    DEFS            :   $DEFS
    CPPFLAGS        :   $CPPFLAGS
    CFLAGS          :   $CFLAGS

  C++ Compiler      : $CXX
    DEFS            :   $DEFS
    CPPFLAGS        :   $CPPFLAGS
    CXXFLAGS        :   $CXXFLAGS

  Linker            : $LD
    LDFLAGS         :   $LDFLAGS
    LIBS            :   $LIBS

LIBRARY:

  gRPC              : $(if test "x$have_grpc" = "x1"; then echo yes; else echo no; fi)
     version        : $grpc_version

  gRPC++            : $(if test "x$have_grpcpp" = "x1"; then echo yes; else echo no; fi)
     version        : $grpcpp_version

])
