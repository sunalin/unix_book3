#source file
#源文件，自动找所有.c和.cpp文件，并将目标定义为同名.o文件
SOURCE := $(wildcard *.c) $(wildcard *.cpp)
OBJS := $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCE)))
#$(warning $(SOURCE))
#$(warning $(OBJS))


#target you can change test to what you want
#目标文件名，输入任意你想要的执行文件名
#TARGET := $(patsubst %.c,%,$(patsubst %.cpp,%.o,$(SOURCE)))
TARGET := system prit_uids
#$(warning $(TARGET))


#compile and lib parameter
#编译参数
CC := gcc
#LIBS := -lpthread -lsqlite3
LIBS := 
DEFINES :=
LDFLAGS :=
INCLUDE := -I.
#CFLAGS := -g -Wall $(DEFINES) $(INCLUDE)
CFLAGS += -Wall -O2 $(DEFINES) $(INCLUDE)

#外部变量传入编译参数
ifdef thread
CFLAGS += -DTHREAD
endif

ifdef signal
CFLAGS += -DSIGNAL
endif

ifdef njournal
CFLAGS += -DNJOURNAL
endif

ifdef string
CFLAGS += -DSTRING
endif
CXXFLAGS := $(CFLAGS)
  
  
#下面的基本上不需要做任何改动了
.PHONY : everything objs clean veryclean rebuild
  
everything : $(TARGET)
  
all : $(TARGET)
  
objs : $(OBJS)
  
rebuild: veryclean everything
                
clean :
	@rm -fr *.so
	@rm -fr *.o
	@rm -fr $(TARGET)
    
veryclean : clean
	rm -fr $(TARGET)
  
$(TARGET) : $(OBJS)
	@$(CC) $(CXXFLAGS) $@.o $(LDFLAGS) $(LIBS) -o $@
	@echo "$@......^_^"
    
%.o : %.c
	@$(CC) $(CFLAGS) -c $< -o $@

