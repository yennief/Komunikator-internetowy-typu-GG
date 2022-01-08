#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define SERVER_PORT 1234
#define QUEUE_SIZE 5
#define MAX 100
#define BUF 1024

struct index
{
    int id;
};

typedef struct
{
 int fd;
 char username[MAX];
 char password[MAX];
 int if_logged_in;
 char messages[MAX][(4+MAX+BUF)*MAX];
 int friends_list[MAX];

}thread_data_t;

thread_data_t clients[MAX];

int users_number=0;

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void add_friend(char *login_friend, int idx){

    int found;
    for (int i=0;i<users_number;i++){
        found = strcmp(clients[i].username,login_friend);
        if(found == 0 && clients[idx].friends_list[i]==0 && idx != i){

            clients[idx].friends_list[i]=1;
            clients[i].friends_list[idx]=1;
            char buf[]="Added friend\n";
            write(clients[idx].fd ,buf, sizeof(buf));

            //Send a username to the added client of a friend who just added them, so they can be added to them automatically too
            char buf2[4+MAX]="a\t";
            strcat(buf2,clients[idx].username);
            strcat(buf2,"\n");
            write(clients[i].fd ,buf2, sizeof(buf2));
            return;
        }
        if(found == 0 && clients[idx].friends_list[i]==1 && idx != i){
            char buf[]="User is already a friend\n";
            write(clients[idx].fd ,buf, sizeof(buf));
            return;
        }
    }

    char buf[]="User does not exist\n";
    write(clients[idx].fd ,buf, sizeof(buf));

}

void load_conversation(char *username,int idx){
    int found;
    int length=0;
    for (int i=0;i<users_number;i++){
         found = strcmp(clients[i].username,username);

         if(found == 0 && clients[idx].friends_list[i]==1 && idx != i){
             char message[4+MAX+BUF]="";
             length = strlen(clients[idx].messages[i]);
             int k=0;
             for(int j=0;j<length;j++){
                 if (clients[idx].messages[i][j]=='\n'){
                     write(clients[idx].fd,message,strlen(message));
                     k=0;
                     memset(message,0,strlen(message));

                 }
                message[k]=clients[idx].messages[i][j];
                k++;
             }
             write(clients[idx].fd,"\n",1);
             write(clients[idx].fd,"stop\n",5);
         }
    }
}

void send_message(char* username,int idx,char received_msg[]){

    char message[4+MAX+BUF]="m\t";
    int found;
    //printf("%d\n", idx);
    for (int i=0;i<users_number;i++){
        found = strcmp(clients[i].username,username);
        //printf("%d\n", found);
        if(found == 0 && clients[idx].friends_list[i]==1 && idx != i){
            strcat(message,clients[idx].username);
            strcat(message,":");
            strcat(message,"\t");
            strcat(message,received_msg);
            strcat(message,"\n");
            strcat(clients[i].messages[idx],message);
            strcat(clients[idx].messages[i],message);
            /*if(clients[idx].if_logged_in==1){
                char message2[4+MAX]="m\t";
                strcat(message2,clients[idx].username);
                strcat(message2,"\t");
                strcat(message2,"\n");
                write(clients[i].fd,message2,sizeof(message2));
            }*/
            //printf("Check sending\n");
            char buf[]="sent\n";
            write(clients[idx].fd ,buf, sizeof(buf));

            return;
        }
    }
    char buf[]="User does not exist\n";
    write(clients[idx].fd ,buf, sizeof(buf));

}

void delete_friend(int idx, char *username){
    int found;
    for (int i=0;i<users_number;i++){
        found = strcmp(clients[i].username,username);
        if(found == 0 && idx != i && clients[idx].friends_list[i]==1){

            clients[idx].friends_list[i]=0;
            clients[i].friends_list[idx]=0;
            char buf[]="Deleted friend\n";
            write(clients[idx].fd ,buf, sizeof(buf));

            char buf2[4+MAX]="d\t";
            strcat(buf2,clients[idx].username);
            strcat(buf2,"\n");
            write(clients[i].fd ,buf2, sizeof(buf2));
            return;
        }
    }

    char buf[]="User does not exist\n";
    write(clients[idx].fd ,buf, sizeof(buf));

}

void *ThreadBehavior(void *t_data)
{

    pthread_detach(pthread_self());
    struct index *th_data = (struct index*)t_data;

    while(1){
        char buf[4+MAX+BUF]="";
        read(clients[th_data->id].fd, buf, sizeof(buf));
        char username[MAX]="";
        char message[BUF]="";
        int login_size=0;
        int message_size=0;
        for(int i=2;i<MAX+2 && buf[i]!='\t';i++){
            username[i-2]=buf[i];
            login_size=i;
        }

        if(buf[0]=='A'){

           add_friend(username,th_data->id);

        }
        else if(buf[0]=='l'){

            load_conversation(username, th_data->id);

        }
        else if(buf[0]=='M'){

            for(int i=2+login_size;i<2+login_size+BUF && buf[i]!='\t';i++){
                  message[i-2-login_size]=buf[i];
                  message_size=i;
            }

            send_message(username,th_data->id,message);

        }
        else if(buf[0]=='Q'){
            clients[th_data->id].if_logged_in=0;
        }
        else if(buf[0]=='D'){
            delete_friend(th_data->id,username);
        }
    }

    pthread_exit(NULL);
}

void register_user(int connection_socket_descriptor, char buf[MAX]){

   char username[MAX]="";
   char password[MAX]="";
   int idx;
   int found = 1;
   int log_size=0;

   for(int i=2;i<MAX && buf[i]!='\t';i++){
              username[i-2]=buf[i];
              log_size=i;
       }

   for (int i=0;i<MAX;i++){

            found = strcmp(clients[i].username,username);
            if(found==0){
                idx=i;
                break;
            }
    }

   if(found==0){
            char buf[]="User already exists\n";
            write(connection_socket_descriptor, buf, sizeof(buf));
            close(connection_socket_descriptor);
    }
    else{

         for(int i=log_size+2;i<MAX && buf[i]!='\t';i++){
               password[i-log_size-2]=buf[i];
         }

        strcpy(clients[users_number].username,username);
        strcpy(clients[users_number].password,password);

        users_number+=1;

        char buf[]="User registered\n";
        write(connection_socket_descriptor, buf, sizeof(buf));

    }

}

int load_friends_list(int idx, char* list){

    for (int i=0; i<MAX;i++){
        if(clients[idx].friends_list[i]==1){
            strcat(list,clients[i].username);
            strcat(list,"\t");
        }
    }
    int size = strlen(list);
    list = realloc(list,size+1);
    strcat(list,"\n");

    return size;

}

int login(int connection_socket_descriptor, char buf[MAX]){

   char username[MAX]="";
   char password[MAX]="";
   int correct_psswd=0;
   int idx;
   int found = 1;
   int log_size=0;

   for(int i=2;i<MAX && buf[i]!='\t';i++){
           username[i-2]=buf[i];
           log_size=i;
       }

   for (int i=0;i<MAX;i++){
        found = strcmp(clients[i].username,username);
        if(found==0){
             idx=i;
             break;
        }
    }

    if(found!=0){

        char buf[]="User not found\n";
        //printf("User not found\n");
        write(connection_socket_descriptor, buf, sizeof(buf));
        close(connection_socket_descriptor);
        return -1;
    }
    else{
        for(int i=log_size+2;i<MAX && buf[i]!='\t';i++){
            password[i-log_size-2]=buf[i];

        }
        int pass_correct = strcmp(password,clients[idx].password);
        if(pass_correct!=0){
            char buf[]="Wrong password\n";
            write(connection_socket_descriptor, buf, sizeof(buf));
            close(connection_socket_descriptor);
            return -1;

        }
        else{
            if(clients[idx].if_logged_in == 1){
                char buf[]="User is already logged in!\n";
                write(connection_socket_descriptor, buf, sizeof(buf));
                close(connection_socket_descriptor);
                return -1;
            }
            else{

                int size=0;
                char *list = malloc(MAX*(MAX+2)*sizeof(char));
                size = load_friends_list(idx,list);

                char buf[]="Login successful\n";
                write(connection_socket_descriptor, buf, sizeof(buf));

                if(size!=0){
                    char buf3[MAX]="full\n";
                    write(connection_socket_descriptor, buf3, sizeof(buf3));
                    char list2[size+1];
                    strcpy(list2, list);
                    strcat(list2, "\n");

                    write(connection_socket_descriptor, list2, sizeof(list2));
                }
                else{
                    char buf3[MAX]="empty\n";
                    write(connection_socket_descriptor, buf3, sizeof(buf3));
                }
                clients[idx].fd = connection_socket_descriptor;
                clients[idx].if_logged_in = 1;
                //printf("Login successful\n");

            }
            return idx;
        }
    }

   return 1;
}

int main(int argc, char* argv[])
{
   int server_socket_descriptor;
   int connection_socket_descriptor;
   int bind_result;
   int listen_result;
   char reuse_addr_val = 1;
   struct sockaddr_in server_address;
   struct sockaddr_in cli_addr;

   pthread_t thread, thread2;
   int create_result = 0;

   memset(&server_address, 0, sizeof(struct sockaddr));
   server_address.sin_family = AF_INET;
   server_address.sin_addr.s_addr = htonl(INADDR_ANY);
   server_address.sin_port = htons(SERVER_PORT);

   server_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
   if (server_socket_descriptor < 0)
   {
       fprintf(stderr, "%s: Błąd przy próbie utworzenia gniazda..\n", argv[0]);
       exit(1);
   }
   setsockopt(server_socket_descriptor, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse_addr_val, sizeof(reuse_addr_val));

   bind_result = bind(server_socket_descriptor, (struct sockaddr*)&server_address, sizeof(struct sockaddr));
   if (bind_result < 0)
   {
       fprintf(stderr, "%s: Błąd przy próbie dowiązania adresu IP i numeru portu do gniazda.\n", argv[0]);
       exit(1);
   }

   listen_result = listen(server_socket_descriptor, QUEUE_SIZE);
   if (listen_result < 0) {
       fprintf(stderr, "%s: Błąd przy próbie ustawienia wielkości kolejki.\n", argv[0]);
       exit(1);
   }

   socklen_t clilen = sizeof(cli_addr);

   while(1)
   {

       connection_socket_descriptor = accept(server_socket_descriptor, (struct sockaddr*)&cli_addr, &clilen);
       if (connection_socket_descriptor < 0)
       {
           fprintf(stderr, "%s: Błąd przy próbie utworzenia gniazda dla połączenia.\n", argv[0]);
           exit(1);
       }

       if((users_number) == MAX){

        write(connection_socket_descriptor, "Too many users\n", 14* sizeof(char));
        close(connection_socket_descriptor);

       }
       else{

           char option[MAX];

           memset(option, 0, sizeof(option));
           read(connection_socket_descriptor,option,sizeof(option));

           if(option[0] == 'R'){

               register_user(connection_socket_descriptor, option);

           }

           else if(option[0]=='L'){
            int cur_idx;

            cur_idx = login(connection_socket_descriptor, option);
            if(cur_idx != -1){

                struct index *t_data = (struct index*)malloc(sizeof(struct index));
                t_data->id = cur_idx;
                //printf("Check\n");

                create_result = pthread_create(&thread, NULL, ThreadBehavior, (void *)t_data);
                if (create_result){
                    printf("Błąd przy próbie utworzenia wątku, kod błędu: %d\n", create_result);
                    exit(-1);
                }

            }

           }

       }

   }

   close(server_socket_descriptor);
   return(0);
}