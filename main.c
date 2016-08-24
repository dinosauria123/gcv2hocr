#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX 2048

int main(int argc, char *argv[ ]){
    FILE *fpin,*fpout,*fpout1;
    char command[256];
    char buf[MAX]={0};
    char *p;
    char *ary[3];
    char lang[2];
    char store[MAX][10]={0,0};
    char word[MAX][50]={0,0}, coordinate[MAX][50]={0,0}, flame[50]={0};
    char imgheight[5], imgwidth[5];
    int i = 0, j = 0, k = 0;

    if (argc != 5){
       printf("Wrong argument.\n");
    exit(1);
    }

    if ((fpin=fopen (argv[1],"r")) == NULL ){
       printf("No input file.\n");
    exit(1);
    }

    if ((fpout=fopen (argv[2],"w")) == NULL){
       printf("No output file.\n");
    exit(1);
    }

    strcpy(&imgheight[0], argv[3]);
    strcpy(&imgwidth[0], argv[4]);

    fpout1 = fopen("preout1.txt","w");

    while(fgets(command, 256, fpin) != NULL){
       while(fgets(buf, MAX, fpin) != NULL ){

// Delete tags

          ary[0] = strtok(buf,":");
          ary[1] = strtok(NULL,",");

          if(ary[1] != NULL){
        	fprintf(fpout1,"%s",ary[1]);
          }
       }
    }

    fclose(fpout1);
    fclose(fpin);

    fpout1 = fopen("preout2.txt","w");

    if((fpin=fopen("preout1.txt","r"))!=NULL){
       while(fgets(buf,MAX,fpin) != NULL ){

          i++;

          while ((p = strstr(buf,"[")) != NULL) *p = '\0';
          while ((p = strstr(buf,"{")) != NULL) *p = '\0';
          while ((p = strstr(buf,"\"")) != NULL) *p = ' ';
          while ((p = strstr(buf,"   ")) != NULL) *p = '\n';
          while ((p = strstr(buf,"\n")) != NULL) *p = '\0';

          if(i % 2 == 0){
             fprintf(fpout1,"%s\n",buf);
          }
       }
    }

    fclose(fpout1);
    fclose(fpin);
    i = 0;

// Extract parameters

    if((fpin=fopen("preout2.txt","r"))!=NULL){
       while(fgets(buf,MAX,fpin) != NULL ){
          while ((p = strstr(buf,"\n")) != NULL) *p = '\0';

          i++;
          if(i == 1){
             strcpy(&lang[0],buf);
             continue;
          }
          else{
             if(i == 2){
                strcpy(&store[1][1], strtok(buf," "));
                strcpy(&store[2][1], strtok(NULL," "));
                strcat(&store[2][1]," ");
                strcat(&store[2][1],&store[1][1]);
                strcat(&flame[0],&store[2][1]);
                strcat(&flame[0]," ");
                continue;
             }

             if(i == 3){
                strcpy(&store[1][1], strtok(buf," "));
                strcpy(&store[2][1], strtok(NULL," "));
                strcat(&store[2][1]," ");
                strcat(&store[2][1],&store[1][1]);
                strcat(&flame[0],&store[2][1]);
                strcat(&flame[0]," ");
                continue;
             }

             if(i % 3 == 1){
                j++;
                strcpy(&word[j][1],buf);
                continue;
             }

             if(i % 3 == 2){
                strcpy(&store[1][1], strtok(buf," "));
                strcpy(&store[2][1], strtok(NULL," "));
                strcat(&store[2][1]," ");
                strcat(&store[2][1],&store[1][1]);
                strcat(&coordinate[j][1],&store[2][1]);
                strcat(&coordinate[j][1]," ");
                k = j;
                continue;
             }

             if(i % 3 == 0){
                strcpy(&store[1][1], strtok(buf," "));
                strcpy(&store[2][1], strtok(NULL," "));
                strcat(&store[2][1]," ");
                strcat(&store[2][1],&store[1][1]);
                strcat(&coordinate[k][1],&store[2][1]);
                strcat(&coordinate[k][1]," ");
                continue;
             }
          }
       }
    }

// Generate hocr output

    fprintf(fpout,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(fpout,"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"\n");
    fprintf(fpout,"\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n");
    fprintf(fpout,"<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n");
    fprintf(fpout,"<head>\n");
    fprintf(fpout,"<title></title>\n");
    fprintf(fpout,"<meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\n");
    fprintf(fpout,"<meta name='ocr-system' content='gcv2hocr' />\n");
    fprintf(fpout,"<meta name='ocr-capabilities' content='ocr_page ocr_carea ocr_par ocr_line ocrx_word'/>\n");
    fprintf(fpout,"</head>\n");
    fprintf(fpout,"<body>\n");
    fprintf(fpout,"%s%s%s%s%s","<div class='ocr_page' id='page_1' title='image \"test.jpg\"; bbox 0 0 ",imgwidth," ",imgheight,"; ppageno 0'>\n");

    for(int k = 1; k <= j; k++) {

       fprintf(fpout,"%s%i%s%s%s","<div class='ocr_carea' id='block_1_",k,"' title=\"bbox ", &coordinate[k][1],"\">\n");
       fprintf(fpout,"%s%i%s%s%s","<p class='ocr_par' dir='ltr' id='par_1_",k,"' title=\"bbox ",&coordinate[k][1],"\">\n");
       fprintf(fpout,"%s%i%s%s%s","<span class='ocr_line' id='line_1_",k,"' title=\"bbox ",&coordinate[k][1],"; baseline 0 -10; x_size 89; x_descenders 20; x_ascenders 21\">");

       while ((p = strstr(&coordinate[k][1],"\n"))!=NULL) *p = '\0';
       fprintf(fpout,"%s%i%s","<span class='ocrx_word' id='word_1_",k,"' title='bbox ");
       fprintf(fpout,"%s%s",&coordinate[k][1],"; x_wconf 85' ");
//       printf("%s\n",&coordinate[k][1]);
       while ((p = strstr(&word[k][1],"\n"))!=NULL) *p = '\0';
       fprintf(fpout,"lang='eng' dir='ltr'>%s</span> ", &word[k][1]);
//       printf("%s\n",&word[k][1]);

       fprintf(fpout,"\n");
       fprintf(fpout,"</span>\n");
       fprintf(fpout,"</p>\n");
       fprintf(fpout,"</div>\n");
    }

    fprintf(fpout,"</div>\n");
    fprintf(fpout,"</body>\n");
    fprintf(fpout,"</html>\n");

    fclose(fpout);

    return 0;
}