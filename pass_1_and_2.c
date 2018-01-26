#include<stdio.h>
#include<math.h>
#include<string.h>
#include<stdlib.h>
#define BUFSIZE				1024
#define FILE_NAME_SIZE		15
int hexa(char nn[20]);
void scanprog(void);
void scanoptab(void);
int searchsym(char y[15],int );
int searchop(char ch[15]);
void record(int n);
void pass1(void);
int pass2(void);
int typecasting(char ch[15]);
struct{
     char loc_ctr[10];
     char label[15];
     char opcode[15];
     char oprand[15];
     char objcode[15];

}prog[150];
struct{
     char opcode1[15];
     char value[15];
     }optab[150];
struct{
     char label1[15];
     int lctr1;
     }symtab[150];
int countop=0,countprog=0,countsym=0,start=0;
int main()
{    int i=0,n;
     FILE *fsym,*fresult,*fim;
     char lctr[15],length[15],aisehi[15];
     fsym=fopen("SYMTAB.txt","w");
     fresult=fopen("pass1_result.txt","w");
    scanprog();
    scanoptab();
    pass1();
    printf("in PASS1 \n\n\tsymtab table is generated is in symtab.txt file and \n\n\tresult of pass1 is in pass1_result file\n");
    printf("\n\tlen of program==%s\n",itoa((symtab[countsym-1].lctr1-start+1),length,16));
    for(i=0;i<countsym;i++)
    {     itoa(symtab[i].lctr1,lctr,16);
        if((strcmp(symtab[i].label1,"____"))!=0)
          fprintf(fsym,"%s %s\n",symtab[i].label1,lctr);
    }i=0;
    fprintf(fresult,"%s %s %s %s\n",itoa(start,length,16),prog[i].label,prog[i].opcode,prog[i].oprand);
    for(i=1;i<=countprog;i++)
    {
         fprintf(fresult,"%s %s %s %s\n",prog[i-1].loc_ctr,prog[i].label,prog[i].opcode,prog[i].oprand);
    }
    n=pass2();
    fim=fopen("intermediate_file.txt","w");
    i=0;
     fprintf(fim,"%s %s %s %s %s\n",prog[i].loc_ctr,prog[i].label,prog[i].opcode,prog[i].oprand,prog[i].objcode);
   for(i=1;i<n;i++)
    {
         strcpy(aisehi,prog[i].oprand);
        fprintf(fim,"%s %s %s %s %s\n",prog[i-1].loc_ctr,prog[i].label,prog[i].opcode,aisehi,prog[i].objcode);
    }
     printf("In PASS2 \n\n\tINTERMEDIATE table is generated is in intermediate.txt file and \n\n\tresult of pass2 OBJECT PROGRAM  is in object_program.txt file\n");
    fclose(fsym);
    fclose(fresult);
    fclose(fim);
}
int pass2(void)
{    FILE *fp;
     int i,iprog=0,length,flag1,flag2,buf;
     char len[15],opadd[15],hex[15],temp[15];

     fp=fopen("object_program.txt","w");
     if(strcmp(prog[iprog].opcode,"START")==0)
        {
         strcpy(prog[iprog].objcode," ");
        }
           fprintf(fp,"H");
           length=6-strlen(prog[iprog].label);
           fprintf(fp,"%s",prog[iprog].label);
           for(i=0;i<length;i++)
             {fprintf(fp," ");}
           length=6-strlen(itoa(start,len,16));
           for(i=0;i<length;i++)
             {fprintf(fp,"0");}
          fprintf(fp,"%s",len);
           length=6-strlen( itoa((symtab[countsym-1].lctr1-start+1),len,16));
           for(i=0;i<length;i++)
             {fprintf(fp,"0");}
           fprintf(fp,"%s\n",itoa((symtab[countsym-1].lctr1-start+1),len,16));
          iprog++;
     while(strcmp(prog[iprog].opcode,"END")!=0)
     {
          flag1=searchop(prog[iprog].opcode);
          if(flag1!=-1)
            {
               if(strcmp(prog[iprog].oprand,"____")!=0)
                    {

                    flag2=searchsym(prog[iprog].oprand,countsym);
                    if(flag2!=-1)
                         {itoa(symtab[flag2].lctr1,opadd,16);

                         strcpy(prog[iprog].objcode,optab[flag1].value);
                         strcat(prog[iprog].objcode,opadd);
                         }
                    else if((strcmp(prog[iprog].oprand,"BUFFER,X")==0))
                         {
                              flag2=searchsym("BUFFER",countsym);
                          itoa(symtab[flag2].lctr1,opadd,16);
                          buf=hexa(opadd);
                          itoa(buf,opadd,16);
                          strcpy(prog[iprog].objcode,optab[flag1].value);
                          strcat(prog[iprog].objcode,opadd);
                         }
                    else if(flag2==-1)
                         {
                         strcpy(prog[iprog].objcode,optab[flag1].value);
                          strcat(prog[iprog].objcode,"0000");
                         }

               }

            }
          else if((strcmp(prog[iprog].opcode,"BYTE"))==0)
               {    i=0;

                    if(prog[iprog].oprand[i]=='C')
                         {
                              length=strlen(prog[iprog].oprand);
                              strcpy(temp," ");
                              for(i=2;i<length-1;i++)
                                   {strcat(temp,itoa((int)prog[iprog].oprand[i],hex,16));}
                                   strcpy(prog[iprog].objcode,temp);
                         }
                    else
                         {
                         length=strlen(prog[iprog].oprand);
                         for(i=2;i<length-1;i++)
                         {prog[iprog].objcode[i-2]=prog[iprog].oprand[i];}
                         prog[iprog].objcode[i-2]=' ';
                         }

               }
          else if(strcmp(prog[iprog].opcode,"WORD")==0)
               {
                    length=atoi(prog[iprog].oprand);
                    itoa(length,hex,16);
                    length=6-strlen(hex);
                    for(i=0;i<length;i++)
                        {prog[iprog].objcode[i]='0';}
                    strcat(prog[iprog].objcode,hex);
               }
               iprog++;

          }
          record(iprog);
          fclose(fp);
          return iprog;
     }
void record(int n)
{
     FILE *fp;
     char addr[15],lenobj[15];
     int length1;
     int i,countt=1,position,length,iprog,len;//count=1 because object program start from level of program file
     fp=fopen("object_program.txt","r+");
     fseek(fp,20,SEEK_SET);
     itoa(start,addr,16);
     fprintf(fp,"\n\n");
     fprintf(fp,"T");
     length=6-strlen(itoa(start,addr,16));

     for(i=0;i<length;i++)
             {fprintf(fp,"0");}
     fprintf(fp,"%s",addr);
     while(countt<n)
     {
          length=9;
          length1=9;
          len=0;
          countt--;
          while(length<70&&countt!=n)
          {
               if(strcmp(prog[countt+1].opcode,"RESW")==0||strcmp(prog[countt+1].opcode,"RESB")==0)
                 {length+=6;
                  len++;
                  countt++;
                 }
               else if(prog[countt+1].oprand[0]=='X')
                {
                  length1+=strlen(prog[countt+1].oprand)-3;
                  length+=6;
                  len++;
                  countt++;
                }
              else
               {
                  length1+=6;
                  length+=6;
                  len++;
                  countt++;
                }
          }
          length-=6;
           length1-=6;
           strcpy(lenobj," ");
           fprintf(fp,"%s",itoa((length1-9)/2,lenobj,16));
           countt=countt-len;
           strcpy(addr,prog[countt].loc_ctr);
           for(i=0;i<len-1;i++)
           {
                    fprintf(fp,"%s",prog[countt+1].objcode);
                    countt++;
           }countt++;
           fprintf(fp,"\n");
           fprintf(fp,"T");
          length=6-strlen(strcpy(addr,prog[countt-1].loc_ctr));
          for(i=0;i<length;i++)
             {fprintf(fp,"0");}
          fprintf(fp,"%s",addr);
     }
     fseek(fp,-7,SEEK_CUR);
     fprintf(fp,"\nE");
     length=6-strlen(itoa(start,addr,16));
           for(i=0;i<length;i++)
             {fprintf(fp,"0");}
          fprintf(fp,"%s",addr);
  fclose(fp);
}
void pass1(void)
{
     int lctr,i,iprog=0;
     int flag=0;
     char buffer[10];
     if(strcmp(prog[iprog].opcode,"START")==0)
          {lctr=typecasting(prog[iprog].oprand);
          start=lctr;
          printf("starting_address::%s",prog[iprog].oprand);
          }
     else
          {lctr=0;
          }
     iprog++;
     while(strcmp(prog[iprog].opcode,"END")!=0)
     {
                    if((strcmp(prog[iprog].label,"____"))!=0)
                    {
                    flag=searchsym(prog[iprog].label,iprog);
                    if(flag!=-1)
                         printf("duplicate symbol found at %d label",iprog);
                    else{
                         strcpy(symtab[countsym].label1,prog[iprog].label);
                         symtab[countsym].lctr1=lctr;
                         countsym++;
                         }
                    }
          strcpy(prog[iprog-1].loc_ctr,itoa(lctr,buffer,16));
         flag=searchop(prog[iprog].opcode);
          if(flag!=-1)
               lctr=lctr+3;
          else if((strcmp(prog[iprog].opcode,"WORD"))==0)
               lctr=lctr+3;
          else if((strcmp(prog[iprog].opcode,"RESW"))==0)
               lctr+=3*atoi(prog[iprog].oprand);
          else if((strcmp(prog[iprog].opcode,"RESB"))==0)
               lctr+=atoi(prog[iprog].oprand);
          else if((strcmp(prog[iprog].opcode,"BYTE"))==0)
               {
                    if(prog[iprog].oprand[0]=='X')
                         lctr=lctr+1;
                    else
                         lctr=lctr+strlen(prog[iprog].oprand)-3;
               }
          else{
               printf("invalid code!!");
               exit(1);
          }
           iprog++;
     }
}
int typecasting(char ch[15])
{
     int i,n,deci=0;
     int power;
     n=strlen(ch);

     for(i=0;i<=n-1;i++)
     {    power=n-i-1;
          deci+=((int)ch[i]-48)*pow(16,power);
     }
     return (deci);
}
int searchop(char ch[15])
{    int i;
     for(i=0;i<=countop;i++)
     {
          if((strcmp(optab[i].opcode1,ch))==0)
               return i;
     }
          return -1;
}

int searchsym(char y[15],int iprog)
{    int i;
     for(i=0;i<iprog;i++)
     {
          if((strcmp(symtab[i].label1,y))==0)
               return i;
     }
     return -1;
}
void scanprog(void)
{
     FILE *fp;
	char *token;
	char filename[FILE_NAME_SIZE];
	char buffer [BUFSIZE];
     int n=0;

		printf("File nane : ");
		scanf("%s", filename);

	if((fp = fopen(filename, "r")) == NULL) {
		/* when the file cannot be opened due to a reasons */
		printf("error! can't open the specified file\n");
		exit (1);
	}
	 while (fgets(buffer, BUFSIZE, fp) != NULL)
	{
		token = strtok (buffer,",:\n ");
  		while (token != NULL) {
    		if(token[0]!='*')
          {
               if(n==0)
                    {strcpy(prog[countprog].label,token);
                    }
               else if(n==1)
               {
                 strcpy(prog[countprog].opcode,token);
               }
               else if(n==2)
               {
                    strcpy(prog[countprog].oprand,token);
                    break;
               }
          }
          n++;
    		token = strtok (NULL, ",:\n ");
  		}
  		printf (" %s ",prog[countprog].label);
  		printf (" %s ",prog[0].opcode);
  		printf (" %s \n\n",prog[countprog].oprand);
  		countprog++;
  		n=0;

  	}
  	countprog--;
fclose(fp);

}
void scanoptab(void)
{
     FILE *fptr;
     fptr=fopen("optab.txt","r");
     while(!feof(fptr))
     {    fscanf(fptr," %s %s",&optab[countop].opcode1,&optab[countop].value);
          countop++;
     }
     countop--;
     fclose(fptr);
}
int hexa(char nn[20])
{

     int i,n,m,deci=0;
     int power;
     char ch[20];
     char pl[20];
     n=typecasting(nn);
     itoa(n,pl,2);
     m=16-strlen(pl);
     strcpy(ch," ");
     for(i=0;i<m-1;i++)
     {
         strcat(ch,"0");
     }
     ch[0]='1';
     strcat(ch,pl);
     strcpy(pl,ch);
     m=strlen(pl);
     for(i=0;i<m;i++)
     {    power=m-i-1;
          n=pow(2,power);
          deci+=((int)pl[i]-48)*n;
     }
     return (deci);
}











