###################目标文件###################################
TARGET = libRedisConn
VERSION = 0.0.1
LIB_NAME = libRedisConn.so

###################目录配置###################################
PATH_HOME= ../redisconn
PATH_PRJ = $(PATH_HOME)
PATH_OBJ = $(PATH_HOME)
PATH_BIN = $(PATH_HOME)
PATH_LIB = ../hiredis-vip


###################编译选项###################################
CC=gcc
CPP=g++
DEBUG=-g
CXXFLAGS=-m64 -O0 -fPIC
SHARE=-shared

##################头文件和依赖库##############################
#依赖boost和开源hiredis-vip，需要先安装修改依赖路径才能编译
INC_HIREDIS=../hiredis-vip
BOOST_ROOT=/usr/local/boost
BOOST_INC=$(BOOST_ROOT)/include
BOOST_LIB=$(BOOST_ROOT)/lib

INC = -I$(BOOST_INC) -I$(PATH_PRJ)/include -I$(INC_HIREDIS)
LIB = -L$(BOOST_LIB) -L$(PATH_LIB) -lhiredis_vip

##################编译中间.o文件##############################
VPATH = ./src:$(PATH_OBJ)
LIBOBJ = \
redisClient.o\
redispool.o

##################固定########################################
.SUFFIXES:
.SUFFIXES: .cpp .hpp .c .h .o .so

.c.o:
	$(CC) $(DEBUG) $(CXXFLAGS) $(INC) -c -o $(PATH_OBJ)/$@ $<
.cpp.o:
	$(CPP) $(DEBUG) $(CXXFLAGS) $(INC) -c -o $(PATH_OBJ)/$@ $<
################编译指令######################################
all: $(TARGET)
	echo "build succ"

clean:
	rm -f *.o *.a *.out core

libRedisConn: $(LIBOBJ)
	-rm  -rf $(LIB_NAME)
	$(CPP) $(DEBUG) $(CXXFLAGS) $(SHARE) -o $(LIB_NAME) $(LIBOBJ) $(LIB)

