// create by LeeShinYook
// 2019-11-3
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAX 1024


int Tokenize(char *buf,char * tokens[]){
 char * token;
 int tokenCount = 0;
 token = strtok(buf, " ");
 while(token != NULL) {
     tokens[tokenCount++] = token;
     token = strtok(NULL, " ");
 }
 tokens[tokenCount] = NULL;
 return tokenCount;
}

void InnerCmd(char *token) {
 if(chdir(token) == -1) perror("cd");
}

void Redirection(char *token[]) {
 pid_t pid;
 int fd_rd;
 printf("redirection!!\n");
 if((pid = fork()) == 0) {
     fd_rd = open(token[3], O_RDWR | O_CREAT | O_TRUNC, 0641);
     dup2(fd_rd, STDOUT_FILENO);
     close(fd_rd);
     execvp(token[0], token);
     exit(0); 
 }
 wait(); 
}

void BackgroundExec(char *token[]) {
 pid_t pid;
 int fd_bg;
 printf("!!! : %s\n",token[0]);
 if((pid = fork()) == 0) {
     fd_bg = open("/dev/null", O_RDONLY);
     dup2(fd_bg, STDOUT_FILENO);
     execvp(token[0],token);
     exit(0);
 }
}

bool Execute(char *cmd) {
 int tokenCount;
 int i = 0;
 char * tokens[MAX];
 pid_t pid;
 int additionalFlag = 0;
 
 tokenCount = Tokenize(cmd,tokens);
 if(strcmp(tokens[0], "cd") == 0) {
     InnerCmd(tokens[1]);
     return;
 }
 for(i = 0; i < tokenCount; i++) {
     if(!strcmp(tokens[i],">")) {
        additionalFlag++;
     	Redirection(tokens);
     }
     if(!strcmp(tokens[i],"&")) {
        additionalFlag++;
        BackgroundExec(tokens);
     } 
 }
 if(!additionalFlag) {
   if((pid = fork()) == 0) {
   	execvp(tokens[0],tokens);
	exit(0);
   }
   wait();
 }
 memset(tokens, '\0', MAX);
 return 1;
}
int main() {
 char cmd[MAX];
 while(1) {
     printf("%s$ ",get_current_dir_name());
     fgets(cmd, sizeof(cmd) - 1, stdin);
     if(strncmp("exit",cmd,4) == 0) {
     printf("Bye!");
     break;
     }
     Execute(cmd);
 }
}
