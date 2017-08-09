###################目标文件###################################
TARGET = libPubDB
VERSION = 0.0.1
LIB_NAME = libPubDB.so

###################目录配置###################################
PATH_HOME= ../publicDB
PATH_LIB = ../lib

PATH_PRJ = $(PATH_HOME)
PATH_OBJ = $(PATH_HOME)
PATH_BIN = $(PATH_HOME)

###################编译选项###################################
CC=gcc
CPP=g++
DEBUG=-g
CXXFLAGS=-m64 -O0 -fPIC 
SHARE=-shared

##################头文件和依赖库##############################
#该库编译依赖mysql的开源库头文件和libmysqlcppconn
INC_COMM = -I$(PATH_HOME)/connector

INC = -I$(PATH_PRJ)/include -I$(INC_COMM)/include
LIB = -L${PATH_LIB}/lib -lmysqlcppconn

##################编译中间.o文件##############################
VPATH = ./src:$(PATH_OBJ)
LIBOBJ = MySQLDBPool.o

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

libPubDB: $(LIBOBJ)
	-rm  -rf $(LIB_NAME)
	$(CPP) $(DEBUG) $(CXXFLAGS) $(SHARE) -o $(LIB_NAME) $(LIBOBJ) $(LIB)
