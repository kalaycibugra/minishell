#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#define ANSI_COLOR_BLUE    "\x1b[34m"//color for terminal
#define ANSI_COLOR_RESET   "\x1b[0m"//reset color

void sigintHandler(int sig_num);
int background=0;
int flag=0;
pid_t pid,pidl;
void rhandle(int fd_1, int fd_2) {
  if (fd_1 != fd_2) {
    if (dup2(fd_2, fd_2) == -1)
        printf("Error\n");
    else
        close(fd_1); /* successfully redirected */
    
  }
}
void filtering(char *options[][10], int pos, int in_fd,int size,int output,int input,char filename_i[],char filename_o[]){
  
        int fd[2];
        int status;
        pid_t pid1,pid2,pid3,pid4;
        
        if (pipe(fd) == -1) {
            perror("pipe");
            exit(1);
        }
    
        if ( (pid1 = fork()) < 0) {
            perror("fork");
            exit(1);
        }

        else if ( pid1 == 0 ) {
        //    setsid();
            if(pos==size-1){
                setpgid(getpid(),pidl);
                printf("%d",pidl);
            if(output == 1)//output redirection
                {    
                    printf("here");
                    int outfile = open(filename_o, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                    output=0;
                    if (outfile < 0){
                        fprintf(stderr,"Error\n");
                    }
                    else{
                        if(dup2(outfile, STDOUT_FILENO) != STDOUT_FILENO){
                            fprintf(stderr, "Error\n");
                            close(outfile);
                        }
                    }
                    
                }
            printf("%schild's process group id is now %d %d\n",options[pos][0], (int) getpgrp(),(int)getppid());

            rhandle(in_fd, STDIN_FILENO);
            int exec_check =execvp(options[pos][0],options[pos]);
            if (exec_check == -1) {
                printf("command not found\n");
            }
            exit(2); 
          
        }   else{
                if(pos==0){
                    pidl=getpid();
                    setpgid(getpid(),pidl);
                    
                    
                    if (input==1){//input redirection
                        int inf = open(filename_i, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                        if (inf < 0){
                            fprintf(stderr,"Error\n");
                            }
                        input = 0;
                        if(dup2(inf,0)<0){
                            close(inf);
                            fprintf(stderr, "Error\n");
                        }
                    }    
                    printf("%schild's process group id is now %d %d\n",options[pos][0], (int) getpgrp(),(int)getppid());

                    close(fd[0]); 
                    
                    rhandle(in_fd, STDIN_FILENO);  
                    rhandle(fd[1], STDOUT_FILENO);
                    int exec_check =execvp(options[pos][0],options[pos]);
                    if (exec_check == -1) {
                        printf("command not found\n");
                    }
                    exit(2);  
                }
                else{
                printf("%schild's process group id is now %d %d\n",options[pos][0], (int) getpgrp(),(int)getppid());

                    setpgid(getpid(),pidl);
                    close(fd[0]); 
                    rhandle(in_fd, STDIN_FILENO); 
                    rhandle(fd[1], STDOUT_FILENO);
                    int exec_check =execvp(options[pos][0],options[pos]);
                    if (exec_check == -1) {
                        printf("command not found\n");
                    }
                    exit(2); 
                }
            }
        }
        else{
            if(pos<size-1){
                
                close(fd[1]);
                close(in_fd);
                filtering(options, pos + 1, fd[0],size,output,input,filename_i,filename_o);
            }
            else{
                if(background!=0){
                while ((pid2 = waitpid(0, &status, 0)) != -1)
                    continue;
                }
            }
        }
}
void filterArray(char *options[][10],char *cmd[],int fil,size_t size1){
    
    int index=0;
    for(int i=0;i<fil;i++){
        for(int z=0;z<10;z++){
            options[i][z]=NULL;
        }
    }
    for(int k=0;k<fil;k++)
    {   
        for(int i=index;i<size1-1;i++){
            if(strcmp(cmd[i], "|") == 0){
                index=i;
                i=size1;
            } 
        }
        if(k==0){
            for(int i=0;i<index;i++){
                options[k][i]=cmd[i];
            }
            options[k][index]=NULL;
        }else if(k==fil-1){
            int it=0;
            for(int i=index+1;i<size1;i++){
                options[k][it]=cmd[i];
                it++;
            }
        }
        else{
            int temp;   
            for(int i=index+1;i<size1-1;i++){
                if(strcmp(cmd[i], "|") == 0){
                    temp=i;
                    i=size1;
                }
            }
            int id=0;
            
            for(int i=index+1;i<temp;i++){
                options[k][id]=cmd[i];
                id++;
            }
            options[k][temp]=NULL;
            temp=0;
            index=index+1;
        }
    }
}
void sigintHandler(int sig_num) //Control C handler
{   

   flag=1;
    printf("\n Cannot be terminated using Ctrl+C use exit\n");
    printf(ANSI_COLOR_BLUE "minish>" ANSI_COLOR_RESET);
    signal(SIGINT, sigintHandler); 
    fflush(stdout); 

} 

int main(int argc, char *argv[]){
    int status;
    int back_count=0;
    int getpid1;
    char command[80];
    char filename_i[80];    
    char filename_o[80];
    int output = 0;
    int input = 0;
    
    int background=0;
    printf(ANSI_COLOR_BLUE "minish>" ANSI_COLOR_RESET);

    while(1){
        background=0;
        int filter=0;
        output=0;
        signal(SIGINT, sigintHandler);
        fgets(command, 100, stdin);   
        int len=strlen(command);
        if(command[len-1]=='\n')//for handling what fget returns
           command[len-1]='\0';
        if(strcmp("exit",command) == 0){
            break;
        }
        char temp[80];
        strcpy(temp,command);
        if(len!=1){
            int spaces=0;
            for(int i = 0; command[i] != '\0'; i++){//space counting in order to count the words in the input
                if (command[i] == ' '){
                    spaces++;
                }
                if (command[i]=='|'){
                    filter++;
                }
            }
            char *p = strtok (command," ");
        
            char *array[spaces+1];
            int j=0;
            while (p != NULL)
            {
                array[j++] = p;
                p = strtok (NULL, " ");
            }
            char *first=">";
            size_t n = sizeof(array)/sizeof(array[0]);
            int check1=0;
            int ind=0;
            char *cmd[(int)n+1];
            for(int k=0;k<=n;k++){
                
                if(k==n)
                    cmd[k]=(char *)0;
                else
                    cmd[k]=array[k];
                
            }
            size_t size = sizeof(cmd)/sizeof(cmd[0]);
            if(size>3){
                if(strcmp(cmd[size-3], ">") == 0){ //removing > symbol and filename from command 
                    strcpy(filename_o,cmd[size-2]);
                    cmd[size-2] = NULL;
                    cmd[size-3] = NULL;
                    output = 1;
                    
                }
                else if(strcmp(cmd[size-3], "<") == 0&&filter==0){  //removing < symbol and filename from command 
                    strcpy(filename_i,cmd[size-2]);
                    cmd[size-2] = NULL;
                    cmd[size-3] = NULL;
                    input = 1;                   
                }
            }
            if(strcmp(array[n-1], "&") == 0){ //if command is a background command delete "&" because execvp won't operate it
                cmd[n-1] = NULL;
                background = 1;
                back_count++; 
            }
            if(filter!=0){
                int fil=filter+1;
                char *options[fil][10];
                int ind=0;
                size_t size1 = sizeof(cmd)/sizeof(cmd[0]);
                for(int l=0;l<n-1;l++){
                    if(strcmp(array[l], "<") == 0){
                        ind=l;
                        input=1;
                        strcpy(filename_i,cmd[ind+1]);
                        break;
                    }
                }
                if(ind!=0){
                    char *cmd1[(int)size1-2];
                    int ite=0;
                    for(int k=0;k<size1;k++){
                        if(k!=ind&&k!=ind+1){
                            if(k==size1)
                                cmd1[ite]=(char *)0;
                            else
                                cmd1[ite]=cmd[k];
                            ite++;
                            }
                        }
                    filterArray(options,cmd1,fil,size1);//parse filtered command input
                }
                else{
                    char *cmd1[(int)size1];
                    int ite=0;
                    for(int k=0;k<size1;k++){    
                        if(k==size1)
                            cmd1[k]=(char *)0;
                        else
                            cmd1[k]=cmd[k];
                        ite++;
                            
                    }
                    filterArray(options,cmd1,fil,size1);//parse filtered command input
                }
                int stdin_bak = dup(0);
                int stdout_bak = dup(1);
                for(int i=0;i<fil;i++){
                    for(int j=0;j<10;j++)
                        printf("%s",options[i][j]);
                    printf("\n");
                }
                filtering(options,0,0,fil,output,input,filename_i,filename_o);//handle filter chain

                //**********fixing stdin and stdout **********
                dup2(stdin_bak,0);
                close(stdin_bak);  
                dup2(stdout_bak,0);
                close(stdout_bak);                 
            
                printf(ANSI_COLOR_BLUE "minish>" ANSI_COLOR_RESET);   
                while(waitpid(-1, NULL, WNOHANG) > 0)
                        continue;
                continue;
            }
            else{
          //  pid_t pid; 
                pid = fork();
                if(pid < 0){  //error handling for fork
                    printf("Error");
                    break;
                }   
                else if ( pid == 0){

                    if(background==1){
                        setpgid(getpid(), pid);
                        getpid1=getpid();
                        printf("Process %d in the background mode\n",getpid1);
                        background=0;
                    }   
                    if(output == 1)//output redirection
                        {    
                            int outfile = open(filename_o, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                            output=0;
                            if (outfile < 0){
                                fprintf(stderr,"Error\n");
                            }
                            else{
                                if(dup2(outfile, STDOUT_FILENO) != STDOUT_FILENO){
                                    fprintf(stderr, "Error\n");
                                    close(outfile);
                                }
                            }
                            
                        }
                    else if (input==1){//input redirection
                        int inf = open(filename_i, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                        if (inf < 0){
                            fprintf(stderr,"Error\n");
                            }
                        input = 0;
                        if(dup2(inf,0)<0){
                            close(inf);
                            fprintf(stderr, "Error\n");
                        }
                    }                
                    int exec_check = execvp(cmd[0], cmd);//run command
                    if (exec_check == -1) {
                        printf("command not found\n");
                    }
                    exit(2);
                }
                else if(background!=1){
                    signal(SIGINT, sigintHandler);
                    printf(ANSI_COLOR_BLUE "minish>" ANSI_COLOR_RESET);
                    waitpid(pid,0,0);
                }
            
               
                    while(waitpid(-1, NULL, WNOHANG) > 0)
                        continue;
                
            }
        }
        else
            printf(ANSI_COLOR_BLUE "minish>" ANSI_COLOR_RESET);
       
   }
    return 0;
}
