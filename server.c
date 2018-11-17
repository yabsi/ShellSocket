/*
 *  @yalabsi
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <ctype.h>

#define CHARMAX          1024 // maximum chars in a buffer
#define ClIMAX          1024 // maximum client count

int port = 1234;
char* history[10];
int hist_track =0;

char * displayHistory()
{
    char* string_history = malloc(10240);

    fprintf(stdout, "%s", string_history);
    sprintf(string_history, "%s %s", string_history, "echo \"Shell command history:\n");


    if(hist_track == 0)
    	sprintf(string_history, "%s %s", string_history, "There's no commands in history yet\n");

    if(hist_track >= 10) {
	    int j, i= hist_track %10;
            for(j =i+1; j<10; j++)
		    sprintf( string_history, "%s %s", string_history, history[j]);
            for(j =0; j<i; j++)
		    sprintf( string_history, "%s %s", string_history, history[j]);
    }else{
	    int i;
	    for(i=0; i< hist_track; i++)
		    sprintf( string_history, "%s %s", string_history, history[i]);
   }

   sprintf(string_history, "%s %s", string_history, "\"");

   return string_history;
}

int main(int argc, char *argv[])
{
  //sockets keeps track of the clients
  int sock, sockets[ClIMAX];

  char buf[CHARMAX + 1], command[CHARMAX + 1];

  FILE *temp1, *temp2;
  char *cp;
  printf("Starting Server");

  /*
   *  Based the initialization of the socket off
   *  http://space.wccnet.edu/~chasselb/linux275/ClassNotes/sockets/sbasics.htm
   *
   */
  sock = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in server_address = { 0 }, Client_address = { 0 };
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(port);
  bind(sock, (struct sockaddr *) &server_address, sizeof(server_address));

  listen(sock, 128);

  int current = 0;
  pid_t pid=0;

  while (1) {
      fflush(stdout);
      //child only reaches this point after being disconnected
      //we terminate it so that only the parent handles connections
      if(pid != 0){
        exit(0);
      }

      printf("\nWaiting For Client\n");

      //parent waiting for a connection
      socklen_t Client_address_len = sizeof(Client_address);
      sockets[current] = accept(sock,(struct sockaddr *)&Client_address,&Client_address_len);
      pid = fork();


      if(pid == 0){
        current++;
        continue;
      }

      temp2 = fdopen(sockets[current],"r");
      while (1) {
          // if we cant get the string from the stream or
          // if the user inputs quit then end the while loop
          // to quit the program
          if (fgets(buf,CHARMAX,temp2) == NULL)
              break;

           int i;
	   for(i=0; i<1024; i++)
		if(buf[i] == '\n' || buf[i] == '\0')
			break;
		else
	 		buf[i] = buf[i] - 5;

	 if(strstr(buf,"history") != NULL) {
		 fprintf(stdout, "%s", displayHistory());
	  	 temp1 = popen(displayHistory(), "r");
   }else if(strstr(buf,"!!") != NULL){
     temp1 = popen(history[hist_track % 10], "r");
   }else if(strstr(buf,"!10") != NULL){
     temp1 = popen(history[(hist_track % 10)], "r"); // 10
   }else if(strstr(buf,"!1") != NULL){
     temp1 = popen(history[(hist_track+1) % 10], "r");
   }else if(strstr(buf,"!2") != NULL){
     temp1 = popen(history[(hist_track+2) % 10], "r");
   }else if(strstr(buf,"!3") != NULL){
     temp1 = popen(history[(hist_track+3) % 10], "r");
   }else if(strstr(buf,"!4") != NULL){
     temp1 = popen(history[(hist_track+4) % 10], "r");
   }else if(strstr(buf,"!5") != NULL){
     temp1 = popen(history[(hist_track+5) % 10], "r");
   }else if(strstr(buf,"!6") != NULL){
     temp1 = popen(history[(hist_track+6) % 10], "r");
   }else if(strstr(buf,"!7") != NULL){
     temp1 = popen(history[(hist_track+7) % 10], "r");
   }else if(strstr(buf,"!8") != NULL){
     temp1 = popen(history[(hist_track+8) % 10], "r");
   }else if(strstr(buf,"!9") != NULL){
     temp1 = popen(history[(hist_track+9) % 10], "r");
   }else{
	  temp1 = popen(buf, "r");
	 }
          // popen runs the shell command and stores the output
         history[hist_track % 10] = strdup(buf);
         hist_track++;



          if (temp1 == NULL)
            printf("Error");

          while (1) {
              // if there was an error somehow with copying the stream
              if (fgets(command, CHARMAX, temp1) == NULL)
                  break;
              // print to the appropriate client
              write(sockets[current], command, strlen(command));
          }
          // close the temp shell stream
          pclose(temp1);

          command[0] = 16;
          write(sockets[current],command,1);
        }
      //closes the socket and its stream
      fclose(temp2);
      close(sockets[current]);
  }

  return 0;
}
