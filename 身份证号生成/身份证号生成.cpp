/*
	By Nroskill �����Զ�����ɿ�
	����Ҫ�����ɺϷ������֤�ţ�����㷨�������������ҵ�
	��Ҫ����ˮ��
*/
#include<cstdio>
#include<cstring>
#include<cstdlib>
#include"xzqh.h"
int main()
{int w[]={7,9,10,5,8,4,2,1,6,3,7,9,10,5,8,4,2},i,j,s,sex=0,birth[3],date[]={0,31,29,31,30,31,30,31,31,30,31,30,31};
 char jiaoyan[11]={'1','0','X','9','8','7','6','5','4','3','2'},result[20];
 FILE *out;
 out=fopen("id.txt","w+");
 printf("��ס�������ڵأ�\n");
 for(i=0;i<3507;i++)
   if(xzqh[i][0][2]=='0'&&xzqh[i][0][3]=='0') printf("%d.%s\n",i,xzqh[i][1]);
 scanf("%d",&j);
 system("cls");
 for(i=j+1;i<3507;i++)
   {
    if((xzqh[i][0][4]!='0'||xzqh[i][0][5]!='0')&&strcmp(xzqh[i][1],"��Ͻ��")!=0&&strcmp(xzqh[i][1],"��")!=0) printf("%d.%s\n",i,xzqh[i][1]);
    if(xzqh[i][0][2]=='0'&&xzqh[i][0][3]=='0'&&strcmp(xzqh[i][1],"��")!=0) break;
   }
 scanf("%d",&i);
 system("cls");
 strcpy(result,xzqh[i][0]); 
 memset(birth,0,sizeof(birth));
 while( birth[0]<=0 || (birth[1]<=0||birth[1]>=13) || (birth[2]<=0||birth[2]>date[birth[1]]) )
 {
  printf("����������(birth)��\n���磺1949.10.1\n");
  scanf("%d.%d.%d",&birth[0],&birth[1],&birth[2]);
 }
 result[6]='0'+birth[0]/1000;
 result[7]='0'+(birth[0]/100)%10;
 result[8]='0'+(birth[0]/10)%10;
 result[9]='0'+birth[0]%10;
 result[10]='0'+birth[1]/10;
 result[11]='0'+birth[1]%10;
 result[12]='0'+birth[2]/10;
 result[13]='0'+birth[2]%10;
 system("cls");
 while(sex!=1&&sex!=2)
   {
    printf("�Ա�(sex)��\n1.��(male)\n2.Ů(female)\n");
    scanf("%d",&sex);
   }
 sex%=2;
 while(sex<1000)
   {
    s=0;
    result[14]='0'+sex/100;
    result[15]='0'+(sex/10)%10;
    result[16]='0'+sex%10;
    for(j=0;j<17;j++)
      s+=(result[j]-'0')*w[j];
    result[17]=jiaoyan[s%11];
    fprintf(out,"%s\n",result);
    sex+=2;
   }
 fclose(out);
 return 0;
}
