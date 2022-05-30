# 开发框架头文件路径。
PUBINCL = -I/project/public

# 开发框架cpp文件名，这里直接包含进来，没有采用链接库，是为了方便调试。
PUBCPP = /project/public/_public.cpp

#编译参数
CFLAGS = -g

#all: procctl checkproc gzipfiles deletefiles ftpgetfiles ftpputfiles
all: tcpputfiles1 fileserver1
procctl:procctl.cpp
	g++ -o procctl procctl.cpp
	cp procctl ../bin/.
checkproc:checkproc.cpp
	g++ $(CFLAGS) -o checkproc  checkproc.cpp $(PUBINCL) -lm -lc
	cp checkproc ../bin/.
gzipfiles:gzipfiles.cpp
	g++ $(CFLAGS) -o gzipfiles  gzipfiles.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp gzipfiles ../bin/.
deletefiles:deletefiles.cpp
	g++ $(CFLAGS) -o deletefiles  deletefiles.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp deletefiles ../bin/.
ftpgetfiles:ftpgetfiles.cpp
	g++ $(CFLAGS) -o ftpgetfiles ftpgetfiles.cpp /project/public/_ftp.cpp  /project/public/_public.cpp -I/project/public -L/project/public -lftp -lm -lc
	cp ftpgetfiles ../bin/.
ftpputfiles:ftpputfiles.cpp
	g++ $(CFLAGS) -o ftpputfiles ftpputfiles.cpp /project/public/_ftp.cpp  /project/public/_public.cpp -I/project/public -L/project/public -lftp -lm -lc
	cp ftpputfiles ../bin/.

tcpputfiles:tcpputfiles.cpp
	g++ $(CFLAGS) -o tcpputfiles tcpputfiles.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp tcpputfiles1 ../bin/.

fileserver:fileserver.cpp
	g++ $(CFLAGS) -o fileserver fileserver.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp fileserver1 ../bin/.

tcpgetfiles:tcpgetfiles.cpp
	g++ $(CFLAGS) -o tcpgetfiles tcpgetfiles.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp tcpgetfiles ../bin/.

clean:
	rm -f procctl checkproc gzipfiles deletefiles ftpgetfiles ftpputfiles tcpputfiles
	fileserver  tcpgetfiles