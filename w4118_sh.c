#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#define max_size 32
#define max_tok 10

int path_size = 10;
int path_num;
int tok_num;
	
char **tok;
char **path;
char *cur_path;

int execution ();
int attach_path(char*);

int main() 
{
	int c,j,i=0, pipe_no = 0;
	int *pipefd;
	int cur_size = max_size;
	int status;
	//int tok_num = 0;
	//int path_num = 0;
	pipefd = malloc(sizeof(int) * 2);
	pipefd[0] = 0;
	pipefd[1] = -1;
	tok = malloc(sizeof(char*) * max_tok);
	path = malloc(sizeof(char*) * path_size);
	cur_path = malloc(sizeof(char*) * 128);
	memset(cur_path, '\0',128);
	strcpy(cur_path,"/home/w4118/");
	char *input = malloc(cur_size);
	//*tok = (char*) malloc(max_size); // later use at time of saving to allocate space 

	printf("$");
	while ( (c=getchar()) != EOF ) {
		
		if ( c == '|') {
		        int pid;
			pipe_no ++;
			pipefd = (int*)realloc(pipefd, (sizeof(int) * (pipe_no*2 +2)));
			pipe(pipefd+(2*pipe_no));
			pid = fork();
			if(pid == 0){
				printf("At pipe_no %d, %s \n",pipe_no,tok[0]);
				fflush(stdout);
				dup2(0, pipefd[2*pipe_no - 2]); 
				dup2(1, pipefd[2*pipe_no + 1]);
				if (pipe_no == 1) 
				        close(0);
				for(j = 0; j < (2 * pipe_no); j++)
					close(pipefd[j]);
				tok[tok_num] = NULL;
				if(tok[0][0] != '/')
					printf("This is value of ap : %d", attach_path(tok[0]));
				for(j = 0; j < tok_num; j++){
				        printf("%s\n", tok[j]);
				}
				execve(tok[0], tok,NULL);
				fprintf(stderr,"error:60 pip_no %d:%s \n",pipe_no, strerror(errno));
				break;//exit(0);
			} else if(pid == -1)
				printf("error:64 didn't fork and pid is: %d", pid);
			wait(&status);
			i = 0;
			for(j=0; j<tok_num; j++) 
				if (tok[j]!=NULL)
					free(tok[j]);
			tok_num = 0;
			cur_size = max_size;
			input = realloc(input,sizeof(char)*max_size);
			memset(input,'\0',max_size);

		} else if (c == '\n' && pipe_no > 0){
			int pid;
			if ( (pid=fork()) ==0) {
				close(0);
				printf("the final stage\n");
				fflush(stdout);
				dup2(0, pipefd[2*pipe_no]);
				//for(j=0; j<2*pipe_no;j++)
				//	close(pipefd[j]);
				tok[tok_num] = NULL;
				if(tok[0][0] !='/')
					attach_path(tok[0]);
				execve(tok[0], tok,NULL);
				fprintf(stderr,"error:82 fin_pip %d:%s\n",pipe_no, strerror(errno));
				break;
			} else if (pid < 0)
				printf("error:87 didnt fork");
			printf("final thing child pid:%d",pid);
			wait(&status);
			printf("AT:97");
			for(j=2; j<2*pipe_no;j++)
				close(pipefd[j]);
			if (pipefd != NULL)
				free(pipefd);
			pipe_no =0;
			
			i = 0;
			for(j=0; j<tok_num; j++) 
				if (tok[j] != NULL) 
					free(tok[j]);
			tok_num = 0;
			cur_size = max_size;
			input = realloc(input,sizeof(char)*max_size);
			memset(input,'\0',max_size);
			printf("$");
			fflush(stdout);
					
		} else if (c == '\n' && pipe_no == 0) {
			if (i > 0) {
				input[i] = '\0';
				tok[tok_num] = malloc(sizeof(char) * strlen(input) + 1);
				strcpy(tok[tok_num],input);
				tok [++tok_num] = NULL;
			}
			
			if (tok_num > 0 && execution() == 0) 
				break;
			for (j = 0; j < tok_num; j++) {
				if (tok[j] != NULL)
					free(tok[j]);
			}
			printf("\n");
			//tok[tok_num][i] = '\0';
			//if (strcmp("exit",tok[0]) == 0) 
			//	break;
			tok_num = 0;
			cur_size = max_size;
			i = 0;
			input = (char*) realloc(input,sizeof(char) * max_size);
			memset(input,'\0',max_size);
			printf("$");
			//chk = 1;  // ????
		} else if (c == ' ') {
			if (i > 0) {
				input[i] = '\0';
				tok[tok_num] = malloc(sizeof(char) * strlen(input) + 1);
				strcpy(tok[tok_num++],input);
			}
			//tok[tok_num][i] = '\0';
			i = 0;
			cur_size = max_size;
			//tok[tok_num] = (char*)malloc(max_size);
			input = (char*) realloc(input,sizeof(char) * max_size);
			memset(input,'\0',max_size);
		} 
		else {
			input[i++] = c;
		}
		
		if (i == cur_size) {
			cur_size+= max_size;
			input = realloc(input,cur_size);
		}
		
		if (tok_num == 10) { 
			printf("error:146 Token overflow");
			break;
		}
	}
	for (j = 0; j < path_num; j++) {
		printf("path %d:%s\n",j,path[j]);
		if (path[j] != NULL) 
			free(path[j]);
	}
//	for (j = 0; j <= tok_num; j++) {
//		//printf("%s ",tok[j]); 
//		free(tok[j]);
//	}
        if (input != NULL) 
		free(input);
	return 0;
}

int execution () 
{
	int pid;
	int status;
	int i;
	if (strcmp("exit",tok[0]) == 0) 
		return 0;
	else if (strcmp("cd",tok[0]) == 0) {
		//do chdir
		if ( tok_num == 2 ) { 
			if (chdir(tok[1]) == -1)	
				fprintf(stderr,"error: cd1:%s",strerror(errno));
		} else 
			printf("error:175 cd invalid arguments");
		
		return 1;
	} else if (strcmp("path",tok[0]) == 0) {
		if (path_num==path_size) {
			path_size++;
			path = realloc(path,sizeof(char*) * path_size);
		}
	
		if (tok[1] == NULL || tok[2] == NULL || tok_num > 3)
			printf("error:185 invalid arguments for path");
		else if (strcmp(tok[1],"+")==0) {
			for(i=0; i<path_num ; i++)
				if (strcmp(path[i],tok[2])==0)
					return 1;	
			path[path_num]=malloc(strlen(tok[2])+1);
			strcpy(path[path_num++],tok[2]);
		} else if (strcmp(tok[1],"-")==0) {	
			for(i=0; i<path_num ; i++) {
				if (strcmp(path[i],tok[2])==0) {
					(path_num)--;
					if (path[i] != NULL)
						free(path[i]);
					path[i]=path[path_num];
					path[path_num]=NULL;
				} else // error come for every check in loop 
					printf("error:200 '%s' not found in path list\n",tok[2]);
			}
		}	
		return 1;
	} else {
		//char *test[] = {"/bin/ls","/",NULL};
		//printf("----%s---",test[0]);
		if (tok[0][0] != '/')
			attach_path(tok[0]);
 
		pid = fork();
		if (pid == 0) { // child
			tok[tok_num]= NULL;
			if (execve(tok[0],tok,NULL) == -1) {
				fprintf(stderr,"error:213 exec:%s",strerror(errno));
				return 0;
			}
		}
		else if (pid > 0) // parent
			wait(&status);
		else 
			fprintf(stderr,"error:220 %s",strerror(errno));
	}
	
	return 1;
}
			

int attach_path(char *cmd) { 
	char tmp[128];
	int i,j,fd;
	for (j = 0; j < path_num; j++) 
		printf("path %d:*%s*\n",j,path[j]);
	for (i=0;path[i] != NULL;i++) {
		memset(tmp,'\0',128);
		strncpy(tmp, path[i], strlen(path[i]));
		strncat(tmp, cmd, strlen(cmd));
		if ((fd = open(tmp, O_RDONLY)) != -1) {
			close(fd);
			cmd = realloc(cmd, strlen(tmp));
			strncpy(cmd, tmp, strlen(tmp));
			printf(" opened\n");
			return 1;
		} else {
		        fprintf(stderr, "Error on line 270 %s:\n", strerror(errno));
		}
	}
	return 0;
}
