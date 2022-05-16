#include "_ftp.h"

// 程序运行参数的结构体。
struct st_arg
{
  char host[31];           // 远程服务端的IP和端口。
  int  mode;               // 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。
  char username[31];       // 远程服务端ftp的用户名。
  char password[31];       // 远程服务端ftp的密码。
  char remotepath[301];    // 远程服务端存放文件的目录。
  char localpath[301];     // 本地文件存放的目录。
  char matchname[101];     // 待下载文件匹配的规则。
  char listfilename[301];  // 下载前列出服务器文件名的文件。
} starg;

struct st_fileinfo{
    char filename[301]; //文件名
    char mtime[21];     //文件时间
};

vector<struct st_fileinfo> vfilelist; //存放下载前列出服务器的文件名容器

//把ftp.nlist()方法获取的list文件加载到容器vfilelist中
bool LoadListFile();

CLogFile logfile;  //日志文件

Cftp ftp;          //ftp类

//程序退出和信号2 15的处理函数
void EXIT(int sig);
//函数说明
void _help();
// 把xml解析到参数starg结构中
bool _xmltoarg(char *strxmlbuffer); 
// 下载文件功能的主函数。
bool _ftpgetfiles();


int main(int argc, char *argv[])
{
    if(argc!=3){_help();return -1;}
    // 关闭全部的信号和输入输出。
    // 设置信号,在shell状态下可用 "kill + 进程号" 正常终止些进程。
    // 但请不要用 "kill -9 +进程号" 强行终止。
    //CloseIOAndSignal(); 
    //signal(SIGINT,EXIT); signal(SIGTERM,EXIT);

    // 打开日志文件。
    if (logfile.Open(argv[1],"a+")==false)
    {
        printf("打开日志文件失败(%s).\n",argv[1]); return -1;
    }
    // 解析xml，得到程序运行的参数。
    if (_xmltoarg(argv[2])==false) return -1;

    // 登录ftp服务器。
    if (ftp.login(starg.host,starg.username,starg.password,starg.mode)==false){
      logfile.Write("ftp.login(%s,%s,%s) failed.\n",starg.host,starg.username,starg.password); return -1;
    }
    logfile.Write("ftp.login ok.\n");

    _ftpgetfiles(); // 下载文件功能的主函数

    ftp.logout(); //断开连接

    return 0;
}

bool _ftpgetfiles(){
  // 进入ftp服务器存放文件的目录。
  if (ftp.chdir(starg.remotepath)==false)
  {
    logfile.Write("ftp.chdir(%s) failed.\n",starg.remotepath); return false;
  }
  // 调用ftp.nlist()方法列出服务器目录中的文件，结果存放到本地文件中。
  if (ftp.nlist(".",starg.listfilename)==false)
  {
    logfile.Write("ftp.nlist(%s) %s failed.\n",starg.remotepath,starg.listfilename); return false;
  }

  //3.把ftp.nlist()方法获取到的list文件加载到容器vlistfile中。
  if (LoadListFile()==false)
  {
    logfile.Write("LoadListFile() failed.\n");  return false;
  }
  //4.遍历容器vfilelist

}

//把ftp.nlist()方法获取的list文件加载到容器vfilelist中
bool LoadListFile(){

    vfilelist.clear();//清空容器

    CFile File;       //文件类

    if(File.Open(starg.listfilename,"r")==false){
        logfile.Write("File.Open(%s) filad! \n",starg.listfilename);return false;
    }
    struct st_fileinfo stfileinfo;
    while (true)
    {
      memset(&stfileinfo,0,sizeof(stfileinfo));

      if(File.Fgets(stfileinfo.filename,300,true)==false)break;

      vfilelist.push_back(stfileinfo);
    }

    //遍历容器vfilelist
    char strremotefilename[301],strlocalfilename[301];
    for (int i = 0; i < vfilelist.size(); i++)
    {
        SNPRINTF(strremotefilename,sizeof(strremotefilename),300,"%s%s",starg.remotepath,vfilelist[i].filename);
        SNPRINTF(strlocalfilename,sizeof(strlocalfilename),300,"%s%s",starg.localpath,vfilelist[i].filename);
        //调用ftp.get()方法从服务器下载文件
        logfile.WriteEx("%s \n",strremotefilename);
        logfile.WriteEx("%s \n",strlocalfilename);
        logfile.Write("get %s ... \n",strremotefilename);
        if(ftp.get(strremotefilename,strlocalfilename) == false){
            logfile.WriteEx("failed ... \n",strremotefilename);break;
        }
        logfile.WriteEx("get %s ok ... \n",strremotefilename);

    }

    return true;
}


void EXIT(int sig){
    printf("程序退出. sig=%d\n\n",sig);

}

void _help()
{
  printf("\n");
  printf("Using:/project/tools1/bin/ftpgetfiles logfilename xmlbuffer\n\n");

  printf("Sample:/project/tools1/bin/procctl 30 /project/tools1/bin/ftpgetfiles /log/idc/ftpgetfiles_surfdata.log \"<host>127.0.0.1:21</host><mode>1</mode><username>book</username><password>123456</password><localpath>/idcdata/surfdata</localpath><remotepath>/tmp/idc/surfdata</remotepath><matchname>SURF_ZH*.XML,SURF_ZH*.CSV</matchname><listfilename>/idcdata/ftplist/ftpgetfiles_surfdata.list</listfilename><ptype>1</ptype><remotepathbak>/tmp/idc/surfdatabak</remotepathbak><okfilename>/idcdata/ftplist/ftpgetfiles_surfdata.xml</okfilename><checkmtime>true</checkmtime><timeout>80</timeout><pname>ftpgetfiles_surfdata</pname>\"\n\n\n");

  printf("本程序是通用的功能模块,用于把远程ftp服务端的文件下载到本地目录。\n");
  printf("logfilename是本程序运行的日志文件。\n");
  printf("xmlbuffer为文件下载的参数,如下：\n");
  printf("<host>127.0.0.1:21</host> 远程服务端的IP和端口。\n");
  printf("<mode>1</mode> 传输模式,1-被动模式,2-主动模式，缺省采用被动模式。\n");
  printf("<username>wucz</username> 远程服务端ftp的用户名。\n");
  printf("<password>wuczpwd</password> 远程服务端ftp的密码。\n");
  printf("<remotepath>/tmp/idc/surfdata</remotepath> 远程服务端存放文件的目录。\n");
  printf("<localpath>/idcdata/surfdata</localpath> 本地文件存放的目录。\n");
  printf("<matchname>SURF_ZH*.XML,SURF_ZH*.CSV</matchname> 待下载文件匹配的规则。"\
         "不匹配的文件不会被下载，本字段尽可能设置精确，不建议用*匹配全部的文件。\n");
  printf("<listfilename>/idcdata/ftplist/ftpgetfiles_surfdata.list</listfilename> 下载前列出服务端文件名的文件。\n");
  printf("<ptype>1</ptype> 文件下载成功后,远程服务端文件的处理方式,1-什么也不做,2-删除;3-备份,如果为3,还要指定备份的目录。\n");
  printf("<remotepathbak>/tmp/idc/surfdatabak</remotepathbak> 文件下载成功后,服务端文件的备份目录,此参数只有当ptype=3时才有效。\n");
  printf("<okfilename>/idcdata/ftplist/ftpgetfiles_surfdata.xml</okfilename> 已下载成功文件名清单,此参数只有当ptype=1时才有效。\n");
  printf("<checkmtime>true</checkmtime> 是否需要检查服务端文件的时间,true-需要,false-不需要,此参数只有当ptype=1时才有效,缺省为false。\n");
  printf("<timeout>80</timeout> 下载文件超时时间，单位：秒，视文件大小和网络带宽而定。\n");
  printf("<pname>ftpgetfiles_surfdata</pname> 进程名，尽可能采用易懂的、与其它进程不同的名称，方便故障排查。\n\n\n");
}

// 把xml解析到参数starg结构中。
bool _xmltoarg(char *strxmlbuffer)
{
  memset(&starg,0,sizeof(struct st_arg));

  GetXMLBuffer(strxmlbuffer,"host",starg.host,30);   // 远程服务端的IP和端口。
  if (strlen(starg.host)==0)
  { logfile.Write("host is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"mode",&starg.mode);   // 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。
  if (starg.mode!=2)  starg.mode=1;

  GetXMLBuffer(strxmlbuffer,"username",starg.username,30);   // 远程服务端ftp的用户名。
  if (strlen(starg.username)==0)
  { logfile.Write("username is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"password",starg.password,30);   // 远程服务端ftp的密码。
  if (strlen(starg.password)==0)
  { logfile.Write("password is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"remotepath",starg.remotepath,300);   // 远程服务端存放文件的目录。
  if (strlen(starg.remotepath)==0)
  { logfile.Write("remotepath is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"localpath",starg.localpath,300);   // 本地文件存放的目录。
  if (strlen(starg.localpath)==0)
  { logfile.Write("localpath is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"matchname",starg.matchname,100);   // 待下载文件匹配的规则。
  if (strlen(starg.matchname)==0)
  { logfile.Write("matchname is null.\n");  return false; }

  GetXMLBuffer(strxmlbuffer,"listfilename",starg.listfilename,300);   // 下载前列出服务器文件名的文件。
  if (strlen(starg.listfilename)==0)
  { logfile.Write("listfilename is null.\n");  return false;}
  return true;
}
