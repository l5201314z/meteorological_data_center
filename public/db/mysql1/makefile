# mysql头文件存放的目录。
MYSQLINCL = -I/usr/local/mysql/include

# mysql库文件存放的目录。
MYSQLLIB = -L/usr/local/mysql/lib

# 需要链接的mysql库。
MYSQLLIBS = -lmysqlclient

#CFLAGS = -O2
#CFLAGS = -O2 -Wall
CFLAGS = -g -Wno-write-strings 
#g++ $(CFLAGS) -o createtable createtable.cpp _mysql.cpp -lmysqlclient -lm -lc
all:createtable 

createtable: createtable.cpp _mysql.cpp
	g++ $(CFLAGS) -o createtable createtable.cpp _mysql.cpp $(MYSQLLIBS)  -lm -lc
	

clean:	
	rm -rf createtable 
