/*
Steph Oro
I pledge my honor that I have abided by the Stevens Honor System.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "list.h"
#include "my.h"

#define OUTPUTSIZE 512

const char * ss_mess = "::server:: '";
int len_ss_mess;
const char * ss_left = "' has left the chat";
int len_ss_left;

struct sockaddr_in serv_addr;
typedef struct client_obj{
  int fd, name_len, exists;
  char * name;
} Client;
typedef struct s_node Node;
int portno;
int sockfd;
Node * never_use = NULL;
Client * client;
Node ** client_list = &never_use;
char safe[256];

/*
  zeros things
 */
void my_zero(char * c, int size){
  int i;
  for(i = 0; i < size; ++i){
    c[i] = 0;
  }
}

/*
removes \n's from things
 */
void str_rem_nl(char * str){
  int len = my_strlen(str);
  if(len > 0 && str[len - 1] == '\n')
    str[len - 1] = 0;
}

/*
quits
 */
int error(const char *msg){
  my_str(msg);
  return -1;
}

/*
initalize saftey enforcer
 */
void init_safe(){
  int i;
  for(i = 0; i < 256; ++i){
    safe[i] = 0;
  }
  for(i = 32; i <= 126; ++i){
    safe[i] = 1;
  }
  safe[58] = 0;
  safe[0] = safe[7] = safe[9] = 1;
}

/*
make a str safe
 */
void makestr_safe(char * str){
  unsigned char c;
  str_rem_nl(str);
  while((c=(unsigned char)*(str++))){
    if(safe[(int)c] == 0){
      *(str - 1) = 0;
      my_str("We do not allow ascii character #");
      my_int((int)c);
      my_str(" on this chat. Instead this will be considered a null.\n");
      break;
    }
  }
}

/*
delete a client
 */
void deleteClient(Client * client){
  close(client->fd);
  if(client->name != NULL)
    free(client->name);
  free(client);
}

/*
make a client
 */
Client * makeClient(){
  Client * c = (Client *)malloc(sizeof(Client));
  c->name = NULL;
  c->exists = 0;
  return c;
}

/*
write a message to a client, and force it to send
 */
int writeToClient(Client * client, const char * message){
  int n, len;
  len = my_strlen(message);
  if(len <= 0)
    return 1;
  my_str("writing '");
  my_str(message);
  my_str("' to client '");
  my_str(client->name);
  my_str("'\n");
  n = write(client->fd,message,len);
  fsync(client->fd);
  return n;
}

/*
broadcast a message to all clients except
those matching the ignore file descriptor
 */
void broadcast(const char * message, int ignore){
  Node * temp = NULL;
  for(temp = *client_list; temp != NULL; temp = temp->next){
    client = (Client *)temp->elem;
    /*if(ignore != client->fd)*/
    writeToClient(client, message);
  }
}

/*
send a message that a certain client has joined
 */
void sendJoinMessage(Client * client){
  const char * mainmessage = "' has joined the chat";
  const char * servermessage = "::server:: '";
  int lenserver = my_strlen(servermessage);
  int lenmain = my_strlen(mainmessage);
  int len = lenserver + client->name_len + lenmain;
  char * message = malloc(sizeof(char) * (len + 1));
  my_strcpy(message, servermessage);
  my_strcpy(message + lenserver, client->name);
  my_strcpy(message + (lenserver + client->name_len), mainmessage);
  message[len] = 0;
  broadcast(message, client->fd);
}

/*
recieve a new client
 */
int recieveClient(int fd){
  Client * client = makeClient();
  client->fd = fd;
  
  add_elem((void *)client, client_list);
  return 0;
}

/*
  free all the clients and delete them
  destroy the list
 */
void freeClientList(){
  client = (Client *)remove_node(client_list);
  while(client != NULL){
    deleteClient(client);
    client = (Client *)remove_node(client_list);
  }
}

/*
  remove a client
 */
int removeCL(Node ** head, Node * temp){
  if(temp == *head)
    deleteClient(remove_node(head));
  else
    deleteClient(remove_node(&temp));
  return -1;
}

/*
  print a leaving message
 */
void printLeavingMessage(char * outbuffer, Client * client){
  if(client->name == NULL) return;
  my_strcpy(outbuffer, ss_mess);
  my_strcpy(outbuffer + len_ss_mess, client->name);
  my_strcpy(outbuffer + len_ss_mess + client->name_len, ss_left);
  broadcast(outbuffer, client->fd);
}

/*
remove a client, but expected.
 */
int removeCLU(Node ** head, Node * temp){
  my_str("client '");
  my_str(client->name);
  my_str("' unexpectedly terminated.\n");
  return removeCL(head, temp);
}

/*
read a message from a client and handle it
search for the client based on their file
descriptor
returns less than 0 if the client should
be removed from the active set
 */
int readFromClient(int fd){
  char * nick;
  Node * temp = NULL;
  int n, oldlen;
  char outbuffer[OUTPUTSIZE];
  char buffer[256];
  my_zero(buffer, 256);
  my_zero(outbuffer, OUTPUTSIZE);
  temp = *client_list;
  client = NULL;
  while(temp != NULL){
    client = (Client *)temp->elem;
    if(client->fd == fd)
      break;
    /*move on*/
    temp = temp->next;
  }
  
  if(temp == NULL || client == NULL)
    return 0;
  
  n = read(client->fd,buffer,255);
  
  if (n <= 0){
    printLeavingMessage(outbuffer, client);
    return removeCLU(client_list, temp);
  }
  makestr_safe(buffer);

  if(client->name == NULL){
    if(buffer[0] == '/' || buffer[0] == 0){
      writeToClient(client, "::server:: we're not accepting that nickname, try another");
      return 0;
    }
    if(my_strlen(buffer) > 20){
      writeToClient(client, "::server:: maximum namelength of 20.");
      return 0;
    }

    my_str("new user: "), my_str(buffer), my_str("\n");
    client->name = my_strdup(buffer);
    client->name_len = my_strlen(client->name);
    
    if (writeToClient(client, "Welcome!") < 0){      
      return removeCLU(client_list, temp);
    }
    sendJoinMessage(client);
  
  }else if(buffer[0] == '/'){
    if(my_strcmp(buffer, "/exit") == 0){
      writeToClient(client, "/exit");
      printLeavingMessage(outbuffer, client);
      return removeCL(client_list, temp);
    }else if(my_strkcmp("/me ",buffer, 4) == 0){
      my_strcpy(outbuffer, client->name);
      my_strcpy(outbuffer + client->name_len, " ");
      my_strcpy(outbuffer + client->name_len + 1, buffer + 4);
      
      broadcast(outbuffer, client->fd);
    }else if(my_strkcmp("/nick ",buffer, 6) == 0){
      nick = buffer + 6;
      if(nick[0] == '/' || nick[0] == 0){
	writeToClient(client, "::server:: we're not accepting your new nickname.");
	return 0;
      }
      oldlen = client->name_len + len_ss_mess;
      my_strcpy(outbuffer, ss_mess);
      my_strcpy(outbuffer + len_ss_mess, client->name);
      free(client->name);
      client->name = my_strdup(nick);
      client->name_len = my_strlen(nick);
      my_strcpy(outbuffer + oldlen, "' has changed their name to '");
      oldlen += 29;
      my_strcpy(outbuffer + oldlen, client->name);
      oldlen += client->name_len;
      my_strcpy(outbuffer + oldlen, "'");
      
      broadcast(outbuffer, client->fd);
    }else{
      writeToClient(client, "::server:: Invalid command!");
    }
  }else{
    
    my_strcpy(outbuffer, client->name);
    my_strcpy(outbuffer + client->name_len, ": ");
    my_strcpy(outbuffer + client->name_len + 2, buffer);
    
    broadcast(outbuffer, client->fd);
  }
  return 0;
}

/*
socket handler, essentially the main loop
accepts new connections and reads in active ones
 */
int socketHandler () {
  fd_set active_fd_set, read_fd_set;
  int i, times = 10;
  struct sockaddr_in clientname;
  socklen_t size;
  int enable = 1;
  /* init the socket */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) 
    return error("ERROR opening socket");
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
		 &enable, sizeof(int)) < 0)
    return error("setsockopt(SO_REUSEADDR) failed");
  my_zero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr *) &serv_addr,
	   sizeof(serv_addr)) < 0)
    return error("ERROR on binding");
  
  /* set up the set of sockets. */
  FD_ZERO (&active_fd_set);
  FD_SET (sockfd, &active_fd_set);
  listen(sockfd, SOMAXCONN);
  while (times){
    /* wait for things to get interesting */
    read_fd_set = active_fd_set;
    if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0){
      return error ("select");
    }

    /* make sure the interesting things happen. */
    for (i = 0; i < FD_SETSIZE; ++i){
      if (FD_ISSET (i, &read_fd_set)){
	if (i == sockfd){
	  /* new connection. */
	  int new;
	  size = sizeof (clientname);
	  new = accept(sockfd,
		       (struct sockaddr *) &(clientname),
		       &(size));

	  
	  if (new < 0){
	    
	  }else{
	    FD_SET (new, &active_fd_set);
	    recieveClient(new);
	  }
	}else{
	  /* old connection wants attention. */
	  if (readFromClient(i) < 0){
	    FD_CLR (i, &active_fd_set);
	  }
	}
      }
    }
  }
  return 0;
}

/*
runs a neat little server that accepts ascii text
 */
int main(int argc, char *argv[])
{
  len_ss_left = my_strlen(ss_left);
  len_ss_mess = my_strlen(ss_mess);
  init_safe();
  if(argc < 2){
    my_str("./"), my_str(argv[0]), my_str(" [port]");
    return 1;
  }
  portno = my_atoi(argv[1]);
  if(portno < 1){
    my_str("bad port\n");
    return 1;
  }
  socketHandler();
  
  broadcast("/exit", -1);
  freeClientList();
  close(sockfd);
  
  return 0; 
}
