#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#define MAX 8192

int main(int argc, char *argv[ ]){
	FILE *fpin,*fpout;
	char buf[MAX]={0};
	char buf0[MAX]={0};
	char *p;
	char *ary[3];
	char lang[4];
	char store[MAX][10]={};
	char word[MAX][50]={}, coordinate[MAX][50]={};
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
	
	fpout = fopen("preout0.txt","w");
	fpin=fopen (argv[1],"r");

// Get Locale

	while(fgets(buf, MAX, fpin) != NULL ){

		if (strstr(buf, "\"locale\":") != NULL){
			fprintf(fpout,"%s",buf);
			fprintf(fpout,"\n");
			continue;
		}
	
		if (strstr(buf, "\"boundingPoly\":") != NULL){
			break;
		}
	}

// Delete lines below "fullTextAnnotation" tag

	while(fgets(buf, MAX, fpin) != NULL ){
		if (strstr(buf,"\"fullTextAnnotation\":") != NULL){
	 		break;
		}

//	Missing x and y

		if (strstr(buf,"              {}") != NULL){
			fprintf(fpout,"                \"x\": 0,\n");
			fprintf(fpout,"                \"y\": 0\n");
			memcpy(buf0, buf, MAX);
			continue;
		}

//	y=-1

		if (strstr(buf,"                \"y\": -1") != NULL){
			if (strstr(buf0,"{") != NULL){
				fprintf(fpout,"                \"x\": 0,\n");
			}
			fprintf(fpout,"                \"y\": 0\n");
			memcpy(buf0, buf, MAX);
			continue;
		}

//	x=-1

		if (strstr(buf,"                \"x\": -1") != NULL){
			fprintf(fpout,"                \"x\": 0,\n");
			if(strstr(buf,",") == NULL){
				fprintf(fpout,"                \"y\": 0\n");
			}
			memcpy(buf0, buf, MAX);
			continue;
		}

// x
		if (strstr(buf,"                \"x\"") != NULL){
			if(strstr(buf,",") != NULL){	 
				fprintf(fpout,"%s",buf);
				memcpy(buf0, buf, MAX);
				continue;
			}

// Missing y

			while ((p = strstr(buf,"\n")) != NULL) *p = ',';

			fprintf(fpout,"%s",buf);
			fprintf(fpout,"\n                \"y\": 0\n");
			memcpy(buf0, buf, MAX);
			continue;
		}

// Missing x

		if (strstr(buf,"                \"y\"") != NULL){
			if (strstr(buf0,"{") != NULL){
				fprintf(fpout,"                \"x\": 0,\n");
			}
// y
			fprintf(fpout,"%s",buf);
			memcpy(buf0, buf, MAX);
			continue;
		}

// Extract text

		if (strstr(buf,"          \"description\": ") != NULL){

		while ((p = strstr(buf,",")) != NULL) *p = ' ';

		fprintf(fpout,"%s",buf);
		memcpy(buf0, buf, MAX);
		continue;
		}

		memcpy(buf0, buf, MAX);
	}

	fclose(fpout);
	fclose(fpin);
	
	fpout = fopen("preout1.txt","w");
	fpin=fopen ("preout0.txt","r");

	while(fgets(buf, MAX, fpin) != NULL ){
	   		  
// Delete tags
		ary[0] = strtok(buf,":");
		ary[1] = strtok(NULL,",");

		if(ary[1] != NULL){
			fprintf(fpout,"%s",ary[1]);
		}else{
			fprintf(fpout,"\n");
		}
	}
	
	fclose(fpout);
	fclose(fpin);

	fpout= fopen("preout2.txt","w");


	if((fpin=fopen("preout1.txt","r"))!=NULL){
		while(fgets(buf,MAX,fpin) != NULL ){

			i++;
		  
			while ((p = strstr(buf,"\"")) != NULL) *p = ' ';

// Replace xml escape characters

	buf0[0] = '\0';
	int iter_xml = 0, single_char, len_xml;
	while ((single_char = buf[iter_xml++])) {
		switch (single_char) {
		case '<':
			strcat(buf0, "&lt;");
			break;
		case '>':
			strcat(buf0, "&gt;");
			break;
		case '&':
			strcat(buf0, "&amp;");
			break;
		default:
			len_xml = strlen(buf0);
			buf0[len_xml] = single_char;
			buf0[len_xml+1] = '\0';
			break;
		}
	}	
	memcpy(buf, buf0, MAX);

// Extract text and coordinates

			if ((i-1)%5 == 0|| (i-2)%5 == 0|| (i-4)%5 == 0){
				fprintf(fpout,"%s",buf);
				continue;
			}
		}
	}
	
	fclose(fpout);
	fclose(fpin);

	i = 0;
	
// Extract parameters

	if((fpin=fopen("preout2.txt","r"))!=NULL){
		while(fgets(buf,MAX,fpin) != NULL ){

			while ((p = strstr(buf,"\n")) != NULL) *p = '\0';

			i++;
			if(i == 1){
				if(strstr(buf, "en")){
				strcpy(&lang[0],"eng");
				offset = -5;
			}

			if(strstr(buf, "de")){
				strcpy(&lang[0],"deu");
				offset = -5;
			}

			if(strstr(buf, "ja")){
				strcpy(&lang[0],"jpn");
				offset = 0;
			}
			continue;
			}
			else{
 
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

   

    fpout= fopen( argv[2],"w");

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
       fprintf(fpout,"lang='%s'", &lang[0]);
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
