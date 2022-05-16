#include "_public.h"

//增加生成历史数据文件的功能，为压缩文件和清理文件模块准备历史数据文件
//增加信号处理函数,处理2和15的信号。
//解决调用exit函数退出时局部对象没有调用析构函数的问题。
CFile File;
//把心跳信息写入共享内存.
CPActive Active;  //进程心跳

//省   站号  站名 纬度   经度  海拔高度
//安徽,58015,砀山,34.27,116.2,44.2
struct st_stcode{
    char provname[31]; //省
    char obtid[11];    //站号
    char obtname[31];   //站名
    double lat;        //纬度
    double lon;        //经度
    double height;     //海拔
};
//将全国气象站点参数的容器
vector<struct st_stcode> vstcode;
//将站点参数文件加载到vstcode容器中
bool LoadSTCode(const char *inifile);

// 全国气象站点分钟观测数据结构
struct st_surfdata
{
  char obtid[11];      // 站点代码。
  char ddatetime[21];  // 数据时间：格式yyyymmddhh24miss
  int  t;              // 气温：单位，0.1摄氏度。
  int  p;              // 气压：0.1百帕。
  int  u;              // 相对湿度，0-100之间的值。
  int  wd;             // 风向，0-360之间的值。
  int  wf;             // 风速：单位0.1m/s
  int  r;              // 降雨量：0.1mm。
  int  vis;            // 能见度：0.1米。
};
vector<struct st_surfdata> vsurfdata;  // 存放全国气象站点分钟观测数据的容器

// 模拟生成全国气象站点分钟观测数据，存放在vsurfdata容器中。
void CrtSurfData();

// 获取当前时间，当作观测时间。
char strddatetime[21];

// 把容器vsurfdata中的全国气象站点分钟观测数据写入文件。
bool CrtSurfFile(const char *outpath,const char *datafmt);

CLogFile logfile;   //日志类



// 程序退出和信号2 15处理函数
void EXIT(int sig){
    logfile.Write("crtsurfdata 程序退出. sig=%d\n",sig);
    exit(0);
}

int main(int argc,char const *argv[]){
    //inifile outpath logfile
    if ((argc!=5) && ( argc!=6)) 
    {
        // 如果参数非法，给出帮助文档。
        printf("Using:./crtsurfdata inifile outpath logfile datafmt [datetime]\n");
        printf("Example:\
        /project/idc1/bin/crtsurfdata /project/idc1/ini/stcode.ini  /tmp/idc/surfdata /log/idc/crtsurfdata.log xml,json,csv\n\n");
        printf("Example:\
        /project/idc1/bin/crtsurfdata /project/idc1/ini/stcode.ini  /tmp/idc/surfdata /log/idc/crtsurfdata.log xml,json,csv  20210520052030\n\n");

        printf("inifile 全国气象站点参数文件名。\n");
        printf("outpath 全国气象站点数据文件存放的目录。\n");
        printf("logfile 本程序运行的日志文件名。\n");
        printf("datafmt 生成数据文件的格式,支持xml、json和csv三种格式,中间用逗号分隔。\n\n");
        printf("datetime 这是一个可选参数，表示生成指定时间的数据和文件。\n\n\n");
        return -1;
    }
    // 关闭全部的信号和输入输出。
    // 设置信号,在shell状态下可用 "kill + 进程号" 正常终止些进程。
    // 但请不要用 "kill -9 +进程号" 强行终止。
    CloseIOAndSignal(true); signal(SIGINT,EXIT);  signal(SIGTERM,EXIT);

    if(logfile.Open(argv[3]) == false){
        printf("logfile.Open(%s) faild .\n",argv[3]);return -1;
    }
    logfile.Write("crtsurfdata 开始运行. \n");

    Active.AddPInfo(20,"crtsurfdata");

    // 把站点参数文件中加载到vstcode容器中。 
    if (LoadSTCode(argv[1])==false) return -1;

    //2.获取当前时间，当作观测时间。
    //char strddatetime[21];
    memset(strddatetime,0,sizeof(strddatetime));
    if(argc==5){
        LocalTime(strddatetime,"yyyymmddhh24miss");
    }else{
        STRCPY(strddatetime,sizeof(strddatetime),argv[5]);
    }
    

    // 模拟生成全国气象站点分钟观测数据，存放在vsurfdata容器中。
    CrtSurfData();

      // 把容器vsurfdata中的全国气象站点分钟观测数据写入文件。
    if (strstr(argv[4],"xml")!=0) CrtSurfFile(argv[2],"xml");
    if (strstr(argv[4],"json")!=0) CrtSurfFile(argv[2],"json");
    if (strstr(argv[4],"csv")!=0) CrtSurfFile(argv[2],"csv");

    logfile.Write("crtsurfdata 结束运行. \n");

    return 0;

}

//将站点参数文件加载到vstcode容器中
bool LoadSTCode(const char *inifile){

    CFile File;
    CCmdStr CCmdStr;
    struct st_stcode stcode;
    //1.打开站点参数文件
    if(File.Open(inifile,"r")==false){
         printf("logfile.Open(%s) faild .\n",inifile);return-1;
    }
    char strBuffer[301];

    while(true){
        //2.将从站点参数文件中读取一行 如果已读取完 跳出循环
        memset(strBuffer,0,sizeof(strBuffer));
        if(File.Fgets(strBuffer,300,true)==false)break;
        //3.将读取到的一行进行拆分
        //printf("%s\n",strBuffer);
        //4.将站点参数的每一个数据项保存带站点参数结构体中
        CCmdStr.SplitToCmd(strBuffer,",",true);  
        if(CCmdStr.CmdCount()!=6)continue;    
        CCmdStr.GetValue(0,stcode.provname,30);   //省
        CCmdStr.GetValue(1,stcode.obtid,10);      //站号
        CCmdStr.GetValue(2,stcode.obtname,30);    //站名
        CCmdStr.GetValue(3,&stcode.lat);          //纬度
        CCmdStr.GetValue(4,&stcode.lon);          //经度
        CCmdStr.GetValue(5,&stcode.height);       //海拔高度
        //5.将站点参数结构体放入站点参数容器中
        vstcode.push_back(stcode);
    }

    /*
    for (int ii=0;ii<vstcode.size();ii++)
        logfile.Write("provname=%s,obtid=%s,obtname=%s,lat=%.2f,lon=%.2f,height=%.2f\n",\
                   vstcode[ii].provname,vstcode[ii].obtid,vstcode[ii].obtname,vstcode[ii].lat,\
                   vstcode[ii].lon,vstcode[ii].height);
    */
    
    return true;
}


// 模拟生成全国气象站点分钟观测数据，存放在vsurfdata容器中。
void CrtSurfData(){
    //1.播随机数的种子
    srand(time(0));

    struct st_surfdata stsurfdata;

  // 遍历气象站点参数的vstcode容器。
  for (int ii=0;ii<vstcode.size();ii++)
  {
    memset(&stsurfdata,0,sizeof(struct st_surfdata));

    // 用随机数填充分钟观测数据的结构体。
    strncpy(stsurfdata.obtid,vstcode[ii].obtid,10); // 站点代码。
    strncpy(stsurfdata.ddatetime,strddatetime,14);  // 数据时间：格式yyyymmddhh24miss
    stsurfdata.t=rand()%351;       // 气温：单位，0.1摄氏度
    stsurfdata.p=rand()%265+10000; // 气压：0.1百帕
    stsurfdata.u=rand()%100+1;     // 相对湿度，0-100之间的值。
    stsurfdata.wd=rand()%360;      // 风向，0-360之间的值。
    stsurfdata.wf=rand()%150;      // 风速：单位0.1m/s
    stsurfdata.r=rand()%16;        // 降雨量：0.1mm
    stsurfdata.vis=rand()%5001+100000;  // 能见度：0.1米

    // 把观测数据的结构体放入vsurfdata容器。
    vsurfdata.push_back(stsurfdata);
    }
}


// 把容器vsurfdata中的全国气象站点分钟观测数据写入文件。
bool CrtSurfFile(const char *outpath,const char *datafmt)
{


  // 拼接生成数据的文件名，例如：/tmp/idc/surfdata/SURF_ZH_20210629092200_2254.csv
  char strFileName[301];
  sprintf(strFileName,"%s/SURF_ZH_%s_%d.%s",outpath,strddatetime,getpid(),datafmt);
  //printf("%s\n",strFileName);

  // 打开文件。
  if (File.OpenForRename(strFileName,"w")==false)
  {
    logfile.Write("File.OpenForRename(%s) failed.\n",strFileName); return false;
  }

  // 写入第一行标题。
  if (strcmp(datafmt,"csv")==0) File.Fprintf("站点代码,数据时间,气温,气压,相对湿度,风向,风速,降雨量,能见度\n");
  if (strcmp(datafmt,"xml")==0) File.Fprintf("<data>\n");
  if (strcmp(datafmt,"json")==0) File.Fprintf("{\"data\":[\n");

  // 遍历存放观测数据的vsurfdata容器。
  for (int ii=0;ii<vsurfdata.size();ii++)
  {
    // 写入一条csv记录。
    if (strcmp(datafmt,"csv")==0)
      File.Fprintf("%s,%s,%.1f,%.1f,%d,%d,%.1f,%.1f,%.1f\n",\
         vsurfdata[ii].obtid,vsurfdata[ii].ddatetime,vsurfdata[ii].t/10.0,vsurfdata[ii].p/10.0,\
         vsurfdata[ii].u,vsurfdata[ii].wd,vsurfdata[ii].wf/10.0,vsurfdata[ii].r/10.0,vsurfdata[ii].vis/10.0);
    if (strcmp(datafmt,"xml")==0)
      File.Fprintf("<obtid>%s</obtid><ddatetime>%s</ddatetime><t>%.1f</t><p>%.1f</p>"\
                   "<u>%d</u><wd>%d</wd><wf>%.1f</wf><r>%.1f</r><vis>%.1f</vis><endl/>\n",\
         vsurfdata[ii].obtid,vsurfdata[ii].ddatetime,vsurfdata[ii].t/10.0,vsurfdata[ii].p/10.0,\
         vsurfdata[ii].u,vsurfdata[ii].wd,vsurfdata[ii].wf/10.0,vsurfdata[ii].r/10.0,vsurfdata[ii].vis/10.0);

    if (strcmp(datafmt,"json")==0){
      File.Fprintf("{\"obtid\":\"%s\",\"ddatetime\":\"%s\",\"t\":\"%.1f\",\"p\":\"%.1f\","\
                   "\"u\":\"%d\",\"wd\":\"%d\",\"wf\":\"%.1f\",\"r\":\"%.1f\",\"vis\":\"%.1f\"}",\
         vsurfdata[ii].obtid,vsurfdata[ii].ddatetime,vsurfdata[ii].t/10.0,vsurfdata[ii].p/10.0,\
         vsurfdata[ii].u,vsurfdata[ii].wd,vsurfdata[ii].wf/10.0,vsurfdata[ii].r/10.0,vsurfdata[ii].vis/10.0);
      if (ii<vsurfdata.size()-1) File.Fprintf(",\n");
      else   File.Fprintf("\n"); 
    }

  }
  if (strcmp(datafmt,"xml")==0) File.Fprintf("</data>\n");
  if (strcmp(datafmt,"json")==0) File.Fprintf("]}\n");

  //sleep(50);
  // 关闭文件。
  File.CloseAndRename();

  UTime(strFileName,strddatetime);  //修改文件的时间属性

  logfile.Write("生成数据文件%s成功,数据时间%s,记录数%d.\n",strFileName,strddatetime,vsurfdata.size());

  return true;
}
