/*
	By Nroskill 2017/04/26 ����
	ò���Ǽ���ǰд�� .h�ǳ��ģ���Դ���� 
*/
#include "md5.h"
int main()
{
  char s[65],*p;
  gets(s);
  p = md5(s);
  printf("md5(%s,32)=%s\n",s,p);
  p[24]='\0';
  printf("md5(%s,16)=%s\n",s,p+8);
  free(p);
  return 0;
}
