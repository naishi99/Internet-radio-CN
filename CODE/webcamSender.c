#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>

#define SERVER_PORT 5432
#define BUF_SIZE 1024

void delay(int milliseconds)
{
    long pause;
    clock_t now,then;

    pause = milliseconds*(CLOCKS_PER_SEC/1000000);
    now = then = clock();
    while( (now-then) < pause )
        now = clock();
}

void sendFile(int socket, struct sockaddr * sin, int sLength)
{

   FILE *picture;
   int size, read_size, stat=0, packet_index;
   char send_buffer[1024], read_buffer[256];
   packet_index = 1;
   char a[1000];
   char b[23];
   char t[49];
   char *n;

   strcpy(a, "/home/nisarg/Videos/Webcam/");

   printf("Size of t : %d\n", sizeof(t));
   printf("Size of a : %d\n",strlen(a));
   FILE *p;
   p = fopen("final.txt","r");
   if(p == NULL){
      system("touch final.txt");
   }
   else{
      system("rm final.txt");
      system("touch final.txt");
   }
   p = NULL;
   p = fopen("final.txt","r");
   
   system("ls -Art /home/nisarg/Videos/Webcam | tail -n 1 >> final.txt");

   memset(b, '\0', sizeof(b));
   
   fread(b, 1, sizeof(b), p);
   printf("value of new BBB : %s\n",b);
   
   strcat(a, b);

   printf("A length: %d\n", strlen(a));
   printf("A: %s\n", a);

   for(int e = 0 ; e < 49 ; e++)
   {
      t[e] = a[e];
   }
   // printf("size of t : %d\n",strlen(t));

   picture = fopen(t, "r");

   if(picture == NULL) 
   {
        printf("Error Opening Image File"); 
   } 
   else
   {
       printf("Sending Picture as Byte Array\n");
        int n=0, count=0;
        while(1)
        {
            n = fread(send_buffer, 1, BUF_SIZE, picture);
            count += n;

            if(sendto(socket, send_buffer, n, 0, sin, sLength) == -1)
            {
                fprintf(stderr, "error while sending data!\n");
                exit(-1);
            }
            printf("Packet-Index: %d\n", packet_index);
            packet_index++;
            delay(1000);
        }
        printf("%d bytes sent. \n", count);
   }
   printf("else pachi..\n");
}

int main(int argc, char * argv[]){

  struct sockaddr_in sin;
  struct sockaddr_storage client_addr;
  char clientIP[INET_ADDRSTRLEN]; /* For IPv4 addresses */
  socklen_t client_addr_len;
  int sLength = sizeof(client_addr);
  char buf[BUF_SIZE];
  int len, s;
  char *host;
  struct hostent *hp;   
   
  /* Create a socket */
  if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("server: socket");
    exit(1);
  }
 
  /* build address data structure and bind to all local addresses*/
  memset((char *)&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
 
  /* If socket IP address specified, bind to it. */
  if(argc == 2) {
    host = argv[1];
    hp = gethostbyname(host);
    if (!hp) {
      fprintf(stderr, "server: unknown host %s\n", host);
      exit(1);
    }
    memcpy((char *)&sin.sin_addr, hp->h_addr, hp->h_length);
  }
  /* Else bind to 0.0.0.0 */
  else
    sin.sin_addr.s_addr = INADDR_ANY;
  
  sin.sin_port = htons(SERVER_PORT);
  
  if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
    perror("server: bind");
    exit(1);
  }
  else{
    /* Add code to parse IPv6 addresses */
    inet_ntop(AF_INET, &(sin.sin_addr), clientIP, INET_ADDRSTRLEN);
    printf("Server is listening at address %s:%d\n", clientIP, SERVER_PORT);
  }
  
  printf("Client needs to send \"GET\" to receive the file %s\n", argv[1]);  
  client_addr_len = sizeof(client_addr);
  
  /* Receive messages from clients*/
  while(1)
  {
      // Recieves GET from Client
      memset(buf, 0, sizeof(buf));
      len = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&client_addr, &client_addr_len);
      inet_ntop(client_addr.ss_family,
            &(((struct sockaddr_in *)&client_addr)->sin_addr),
            clientIP, INET_ADDRSTRLEN);

        // Sends a fixed file to client
      printf("String: %s\n", buf);
      if(buf[0]=='G' && buf[1]=='E' && buf[2]=='T')
      {
          //printf("In1\n");
          memset(buf, 0, sizeof(buf));
          sendFile(s, &client_addr, sLength);
      }
      else
      {
          printf("Client needs to send GET in order to recieve file.\n");
      }

      memset(buf, 0, sizeof(buf));
      strcpy(buf, "BYE");
      sendto(s, buf, sizeof(buf), 0, (struct sockaddr*)&client_addr, client_addr_len);

  }
  
}