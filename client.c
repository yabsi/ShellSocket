/*
 *  @yalabsi
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define CHARMAX          1024 // maximum chars in a bufferfer
int port = 1234;

int main(int argc, char *argv[])
{
  char *ip;

  if(argc > 1)
    ip = argv[1];
  else
    ip = "127.0.0.1";

  int sock;
  char *string;
  char buffer[1025];
  int bytesRead;

  if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
      printf("\n Error : Could not create socket \n");
      return 1;
  }

  /*
   *  Based the initialization of the socket off
   *  http://space.wccnet.edu/~chasselb/linux275/ClassNotes/sockets/sbasics.htm
   *
   */
  struct sockaddr_in server_address = { 0 };
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);
  server_address.sin_addr.s_addr = inet_addr(ip);

  if((connect(sock, (struct sockaddr *) &server_address, sizeof(server_address))) < 0){
      printf("\n Error : Could not connect to server \n");
      return 1;
  };

  while (1) {
      // if we can't copy the input to buffer or
      // if we can't write to the socket
      // or if the command was quit
      if (fgets(buffer,CHARMAX,stdin) == NULL ||
           write(sock, buffer, strlen(buffer)) < 0 ||
           strcmp(buffer,"quit\n") == 0)
          break;

      //as long as there's more output to read
      while (string != NULL) {
          // bytesRead indicated how how many bytes read
          bytesRead = read(sock, buffer, 1024);

          // if there's an error or if we're done, break
          if (bytesRead <= 0)
              break;

          //looks for a data segement
          string = memchr(buffer, 16 , bytesRead);
          //writes the string with the proper indexes
          write(1,buffer, string != NULL ? string - buffer : bytesRead);
      }
  }
  //closes socket with server
  close(sock);
  return 0;
}
