/*
	By Nroskill �����Զ�����ɿ�
	���԰�*nix�µ��ı��ļ�תΪWindows�µģ��Ӷ��ü��±��򿪾ͻ����ˣ�ת����ɺ����notepad�򿪣��������ɶ�����ļ� 
	��Gizeta˵�ܺ��ã�������ά������ 
	�ң��ͣ����� 
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main(int arg,char *argv[])
{
 FILE *fp,*fw;
 int i,j;
 char a,fn[999]="trans_",add[999],com[999]="notepad \"";
 if(arg==1) 
   {
    printf("No file was called\n");
    system("pause");
    return 0;
   }
 strcpy(add,argv[1]);
 for(i=strlen(add)-1;i>=0;i--)
   if(add[i]=='\\') 
     {
      i++;
      for(j=0;i+j<=strlen(add);j++)
        fn[6+j]=add[i+j];
      add[i]='\0';
      break;
     }
 strcat(add,fn);
 fw=fopen(add,"w");
 fp=fopen(argv[1],"r");
 while((a=fgetc(fp))!=EOF)
   {
    if(a==13) fprintf(fw,"\n");
    else fprintf(fw,"%c",a);
   }
 fclose(fp);
 fclose(fw);
 strcat(com,add);
 strcat(com,"\"");
 system(com);
 return 0;
}
