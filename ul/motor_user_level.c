#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
int main(int argc, char* argv[])
{
	int fd;
	int flagg = 0;
	char args[128];
	char read_data[128];
	char *fn;
    	char *input;
	sprintf(args, "%s", argv[1]);
	//fd = open("/dev/motor", O_RDWR | O_APPEND);
	//if (fd < 0) {
	//	printf("Open /dev/motor failed!!!\n");
		//exit(1);
	//}
	
	if (argc == 2){

		input = argv[1];
		if (input[(strlen(input) - 1)] == '/')
		        input[(strlen(input) - 1)] = '\0';

	        (fn = strrchr(input, '/')) ? ++fn : (fn = input);
		printf("%s\n", fn);

		if(strstr(fn,"1") != NULL){
			 printf("File 1 found\n");
			 FILE *fp;
			 char str[1000];
		         const char s[2] = " ";
			 char *token;
			 int red=0,blue=0,green=0;
			 
			 char* filename = "/ad/eng/users/a/b/abharath/Project_EC535/motor_integrate/snap1.txt";
 
			 fp = fopen(filename, "r");
			 if (fp == NULL){
		         		printf("Could not open file %s",filename);
				        return 1;
			 }
			 while (fgets(str, 1000, fp) != NULL){
			 //printf("%s\n", str);
			 	token = strtok(str, s);
				int count=0;
			 	//printf("token is %s\n",token);
			 	while(token != NULL ) {

				//printf("Count is %d\n",count);
				if (count == 0){
					        green = atoi(token);
						//printf("Green value = %d\n",green);
						

				}else if (count == 1){
						blue = atoi(token);
						//printf("Blue value = %d\n",blue);
						

				}else if (count == 2){
						red = atoi(token);
						//printf("Red value = %d\n",red);
						//if((red < 255 && red > 65) && ((float)(blue/red) < 0.7 && (float)(blue/red) > 0) && ((float)(green/red) < 0.7 && (float)(green/red) > 0))
						//printf("division value is %f", (float)blue/red);
						if((65 < red < 255) && (0 < (float)(blue/red) < 0.7) && (0 < (float)(green/red) < 0.7))
						{
							flagg = 1;
							printf("flagg is %d\n",flagg);
						}
		
						else {
							flagg = 0;
						}
						

						
		
				}
					
				
    			        token = strtok(NULL, " ");
				count++;
				
				
			 }
			 }
			 fclose(fp);
			
		}else if(strstr(fn,"2") != NULL){		
			printf("File 2 found\n");
		}else if(strstr(fn,"3") != NULL){		
			printf("File 3 found\n");
		}else if(strstr(fn,"4") != NULL){		
			printf("File 4 found\n");
		}else if(strstr(fn,"6") != NULL){		
			printf("File 6 found\n");
		}else if(strstr(fn,"7") != NULL){		
			printf("File 7 found\n");
		}else if(strstr(fn,"8") != NULL){		
			printf("File 8 found\n");
			FILE *fp;
			 char str[1000];
		         const char s[2] = " ";
			 char *token;
			 int red=0,blue=0,green=0;
			 
			 char* filename = "/ad/eng/users/a/b/abharath/Project_EC535/motor_integrate/snap8.txt";
 
			 fp = fopen(filename, "r");
			 if (fp == NULL){
		         		printf("Could not open file %s",filename);
				        return 1;
			 }
			 while (fgets(str, 1000, fp) != NULL){
			 //printf("%s\n", str);
			 	token = strtok(str, s);
				int count=0;
			 	//printf("token is %s\n",token);
			 	while(token != NULL ) {

				//printf("Count is %d\n",count);
				if (count == 0){
					        green = atoi(token);
						//printf("red value = %d\n",red);

				}else if (count == 1){
						blue = atoi(token);
						//printf("Blue value = %d\n",blue);


				}else if (count == 2){
						red = atoi(token);

						if(red == 0){
							red = 60;
						}
						//printf("Green value = %d\n",green);
						//if((red < 255 && red > 65) && ((float)(blue/red) < 0.7 && (float)(blue/red) > 0) && ((float)(green/red) < 0.7 && (float)(green/red) > 0))
		
				}

				if (count >= 2){
					if((65 < red < 255) && (0 < (float)(blue/red) < 0.7) && (0 < (float)(green/red) < 0.7))
						{
							flagg = 1;
							printf("flagg is %d\n",flagg);
						}
		
						else {
							flagg = 0;
						}

				}					
				
    			        token = strtok(NULL, " ");
				count++;


			 }
			 }
			 fclose(fp);
		}else if(strstr(fn,"9") != NULL){
			printf("File 9 found\n");
		} else{
			printf("File not found\n");
		}
		//while(1){		
			//write(fd, argv[1], 4);
			//sleep(10);
			//write(fd,"stop",4);
			//sleep(5);
			//}
	}
	else{ 
        	printf("Error: invalid use.\n");
	}

	//close(fd);
	return 0;
}
