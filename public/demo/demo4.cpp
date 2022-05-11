/*
 *  程序名：demo4.cpp，此程序演示开发框架中STRCAT函数的使用。
 *  作者：吴从周
*/
#include "../_public.h"

int main()
{
  char str[11];   // 字符串str的大小是11字节。
  STRCPY(str,sizeof(str),"www");  

  STRCAT(str,sizeof(str),".fr");  // str原有的内容加上待追加的内容没有超过str可以存放的大小。
  printf("str=%s=\n",str);        // 出输结果是str=www.fr=

  STRCAT(str,sizeof(str),"eecplus.net");  // str原有的内容加上待追加的内容超过了str可以存放的大小。
  printf("str=%s=\n",str);                // 出输结果是str=www.freecp=

  char  addr[]= "12345678";
  //char addr[] = {'1','2','3','4','5','6','7','8','\0'};
  //strcat(addr,"nanchg");            
  printf("addr=%s=\n",addr);        // nanchang  产生数组越界不安全

  int i,j = strlen(addr)-1;
  char temp;
  //printf("%d",i);
  printf("%c\n",addr[7]);
  //int len = 
  for(i = 0;i < strlen(addr)/2;i++,j--){
      temp = addr[i];
      addr[i] = addr[j];
      addr[j] = temp;
  }
  printf("%s\n",addr);
}

