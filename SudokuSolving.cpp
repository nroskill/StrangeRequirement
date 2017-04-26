/*
	By Nroskill 年代久远，不可考
	解数独，当时只测了据说世界上最难的数独，不保证都能解
	需要同目录下建立shudu.txt，待填位置用0表示
	代码丑的不行，懒得改了 
*/ 
#include<cstdio>
#include<cstdlib>
#include<cstring>
struct sd
{
 int fb[10][10];
 bool hang[10][10],lie[10][10],kuai[10][10];
};

int uf(struct sd a)
{
 int i,j,n=0;
 for(i=1;i<=9;i++)
   for(j=1;j<=9;j++)
     if(a.fb[i][j]==0) n++;
 return n;
}

struct sd change(struct sd a,int x,int y)
{
  a.hang[x][a.fb[x][y]]=1;
  a.lie[y][a.fb[x][y]]=1;
  a.kuai[(x-1)/3*3+(y-1)/3+1][a.fb[x][y]]=1;
  return a;
 }


struct sd cal(struct sd a)
{
 bool chance[10][10][10],judge=1;
 int i,j,k,t,num,save;
 memset(chance,0,sizeof(chance));
 for(i=1;i<=9;i++)
   for(j=1;j<=9;j++)
     if(a.fb[i][j]!=0)
       for(k=1;k<=9;k++)
         chance[i][j][k]=1;
     else
       for(k=1;k<=9;k++)
         if(a.hang[i][k]||a.lie[j][k]||a.kuai[(i-1)/3*3+(j-1)/3+1][k]) chance[i][j][k]=1;
 while(judge&&uf(a))
   {
    judge=0;
    for(i=1;i<=9;i++)
      for(j=1;j<=9;j++)
        {
         for(k=1,num=0;num<2&&k<=9&&a.fb[i][j]==0;k++)
           if(chance[i][j][k]==0) {num++;save=k;}
         if(num==1)
           {
            a.fb[i][j]=save;
            a=change(a,i,j);
            for(k=1;k<=9;k++)
              {
               chance[k][j][save]=1;
               chance[i][k][save]=1;
              }
            for(i=(i-1)/3*3+1,j=(j-1)/3*3+1,k=0;k<=2;k++)
              for(t=0;t<=2;t++)
                chance[i+k][j+t][save]=1;
            judge=1;
           }
        }
   }
 return a;
}

struct sd t(struct sd a)
{
 int i,j,k,q,y,x,n=10;
 struct sd b;
 a=cal(a);
 if(uf(a)!=0)
 {
  for(i=1;i<=9;i++)
    for(j=1;j<=9;j++)
      if(a.fb[i][j]==0)
        {
         for(q=0,k=1;k<=9;k++)
           if(a.hang[i][k]==0&&a.lie[j][k]==0&&a.kuai[(i-1)/3*3+(j-1)/3+1][k]==0) q++;
         if(q<n) {n=q;x=i;y=j;}
        } 
  for(k=1;k<=9;k++)
    if(a.hang[x][k]==0&&a.lie[y][k]==0&&a.kuai[(x-1)/3*3+(y-1)/3+1][k]==0)
      {
       b=a;
       b.fb[x][y]=k;
       b=t(change(b,x,y));
       if(uf(b)==0) return b;
      }
 }     
 return a;
}

int main()
{
 struct sd in;
 int i,j;
 FILE *a,*b;
 a=fopen("shudu.txt","r");
 if(a==NULL)
   {
    printf("没有找到数独文件，请在程序同目录下建立\"shudu.txt\"，里面写入要解的数独，需填处以\"0\"代替\n例如\n");
    printf("8 0 0 0 0 0 0 0 0\n0 0 3 6 0 0 0 0 0\n0 7 0 0 9 0 2 0 0\n0 5 0 0 0 7 0 0 0\n0 0 0 0 4 5 7 0 0\n0 0 0 1 0 0 0 3 0\n0 0 1 0 0 0 0 6 8\n0 0 8 5 0 0 0 1 0\n0 9 0 0 0 0 4 0 0\n\n");
    system("pause");
    return 0;
   }
 memset(in.hang,0,sizeof(in.hang));
 memset(in.lie,0,sizeof(in.lie));
 memset(in.kuai,0,sizeof(in.kuai));
 for(i=1;i<=9;i++)
   for(j=1;j<=9;j++)
     {
      fscanf(a,"%d",&in.fb[i][j]);
      in=change(in,i,j);
     }
 in=t(in);
 for(i=1;i<=9;i++)
   {
    for(j=1;j<=9;j++)
      printf("%d ",in.fb[i][j]);
    printf("\n");
   }
 system("pause");
 return 0;
}
