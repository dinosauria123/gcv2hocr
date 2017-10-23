#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX 8192

int main(int argc, char *argv[ ]){
    FILE *fpin,*fpout,*fpout0,*fpout1;
    char command[256];
    char buf[MAX]={0};
    char *p;
    char *ary[3];
    char lang[3];
    char store[MAX][10]={0,0};
    char word[MAX][50]={0,0}, coordinate[MAX][50]={0,0}, frame[50]={0};
    char imgheight[5]="0" , imgwidth[5]="0";
    int offset = -5; //Adjust for proper y-position. English = -5, Japanese = 0
    int i = 0, j = 0, k = 0;

    if ((fpin=fopen (argv[1],"r")) == NULL ){
       printf("No input file.\n");
    exit(1);
    }

    if ((fpout=fopen (argv[2],"w")) == NULL){
       printf("No output file.\n");
    exit(1);
    }

    if (argc > 5){
       printf("Wrong argument.\n");
    exit(1);
    }

    if ((argv[3] != NULL) && (argv[4] != NULL)){
       strcpy(&imgheight[0], argv[3]);
       strcpy(&imgwidth[0], argv[4]);
    }

    if (!((isdigit(imgheight[0]) && (isdigit(imgwidth[0]))))){
       printf("Wrong image size.\n");
    exit(1);
    }
	
	fpout0 = fopen("preout0.txt","w");
	fpin=fopen (argv[1],"r");

// Delete lines below "fullTextAnnotation" tag
	
	while(fgets(buf, MAX, fpin) != NULL ){

       if (strstr(buf,"fullTextAnnotation") != NULL){
		   break;	   
	   }

	fprintf(fpout0,"%s",buf);
   
	}	

    fclose(fpout0);
    fclose(fpin);
	
	fpout1 = fopen("preout1.txt","w");
	fpin=fopen ("preout0.txt","r");
	
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

          // Replace xml escape characters

          while ((p = strstr(buf,"&")) != NULL){
             memmove(p,"&amp;",5);
             break;
          }
          while ((p = strstr(buf,"<")) != NULL) strcpy(buf,"&lt;");
          while ((p = strstr(buf,">")) != NULL) strcpy(buf,"&gt;");
  
          if(i % 2 == 1 && i != 1){
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
             if(strstr(buf, "en")){
                strcpy(&lang[1],"eng");
                offset = -5;
             }

             if(strstr(buf, "de")){
                strcpy(&lang[1],"deu");
                offset = -5;
             }


             if(strstr(buf, "ja")){
                strcpy(&lang[1],"jpn");
                offset = 0;
             }

             continue;
          }
          else{
             if(i == 2){
                strcpy(&store[1][1], strtok(buf," "));
                strcpy(&store[2][1], strtok(NULL," "));
                strcat(&store[2][1]," ");
                strcat(&store[2][1],&store[1][1]);
                strcat(&frame[0],&store[2][1]);
                strcat(&frame[0]," ");
                continue;
             }

             if(i == 3){
                strcpy(&store[1][1], strtok(buf," "));
                strcpy(&store[2][1], strtok(NULL," "));
                strcat(&store[2][1]," ");
                strcat(&store[2][1],&store[1][1]);
                strcat(&frame[0],&store[2][1]);
                strcat(&frame[0]," ");
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
                strcat(&store[1][1]," ");
                strcat(&store[1][1],&store[2][1]);
                strcpy(&coordinate[j][1],&store[1][1]);
                strcat(&coordinate[j][1]," ");
                k = j;
                continue;
             }

             if(i % 3 == 0){
                strcpy(&store[1][1], strtok(buf," "));
                strcpy(&store[2][1], strtok(NULL," "));
                strcat(&store[1][1]," ");
                strcat(&store[1][1],&store[2][1]);
                strcat(&coordinate[k][1],&store[1][1]);
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

//       fprintf(fpout,"%s%i%s%s%s","<div class='ocr_carea' id='block_1_",k,"' title=\"bbox ", &coordinate[k][1],"\">\n");
//       fprintf(fpout,"%s%i%s%s%s","<p class='ocr_par' dir='ltr' id='par_1_",k,"' title=\"bbox ",&coordinate[k][1],"\">\n");
       fprintf(fpout,"%s%i%s%s%s%i%s","<span class='ocr_line' id='line_1_",k,"' title=\"bbox ",&coordinate[k][1],"; baseline 0 ",offset,"; x_size 89; x_descenders 20; x_ascenders 21\">");

       while ((p = strstr(&coordinate[k][1],"\n"))!=NULL) *p = '\0';
       fprintf(fpout,"%s%i%s","<span class='ocrx_word' id='word_1_",k,"' title='bbox ");
       fprintf(fpout,"%s%s",&coordinate[k][1],"; x_wconf 85' ");
//       printf("%s\n",&coordinate[k][1]);
       while ((p = strstr(&word[k][1],"\n"))!=NULL) *p = '\0';
       fprintf(fpout,"lang='%s'", &lang[1]);
       fprintf(fpout," dir='ltr'>%s</span> ", &word[k][1]);
//       printf("%s\n",&word[k][1]);

       fprintf(fpout,"\n");
       fprintf(fpout,"</span>\n");
//       fprintf(fpout,"</p>\n");
//       fprintf(fpout,"</div>\n");
    }

    fprintf(fpout,"</div>\n");
    fprintf(fpout,"</body>\n");
    fprintf(fpout,"</html>\n");

    fclose(fpout);

    return 0;
}
