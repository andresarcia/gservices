# include "base_message_header.H"
# include "remote_multiserver_point.H"
# include "local_reception_point.H"
# include "port.H"
# include <iostream>
# include <aleph.H>
//# include <performance.H>

# define NO_OF_SERVICES 2
enum Service_Codes
{
  SERVICE_1,
  SERVICE_2
};

enum Service_Codes_2
{
  MYSERV_SERVICE_1,
  MYSERV_SERVICE_2
};

pthread_cond_t wait_to_finish = PTHREAD_COND_INITIALIZER;
pthread_mutex_t my_lock = PTHREAD_MUTEX_INITIALIZER;

#define MSG_SIZE 128

class Request_Msg : public Msg_Entry_Header
{
  char msg[MSG_SIZE];

public:
  Request_Msg() :
    Msg_Entry_Header(SERVICE_1, sizeof(*this))
  {
    for (int i=0; i<MSG_SIZE; i++)
      msg[i]='X';
  }
};

class Reply_Msg : public Msg_Entry_Header
{
  char msg[MSG_SIZE];

public:
  Reply_Msg() :
    Msg_Entry_Header(SERVICE_2, sizeof(*this))
  {
    for (int i=0; i<MSG_SIZE; i++)
      msg[i]='X';
  }
};

class Local_Request_Msg : public Msg_Entry_Header
{
  char msg[MSG_SIZE];

public:
  Local_Request_Msg() :
    Msg_Entry_Header(MYSERV_SERVICE_1, sizeof(*this))
  {
    for (int i=0; i<MSG_SIZE; i++)
      msg[i]='X';
  }
};

class Local_Reply_Msg : public Msg_Exit_Header
{
  char msg[MSG_SIZE];

public:
  Local_Reply_Msg() :
    Msg_Exit_Header(MYSERV_SERVICE_2)
  {
    for (int i=0; i<MSG_SIZE; i++)
      msg[i]='X';
  }
};

Port deamon;
Binding_Point * saved_client;

class Services : public Remote_Multiserver_Point<Services>
{

  int service_1(Msg_Entry_Header * entry_msg,
		Remote_Multiserver_Binding<Services> * return_point)
  {
    Reply_Msg reply;

    return_point->rpc_reply(&reply, sizeof(Reply_Msg));
    return 0;
  }

  int service_2(Msg_Entry_Header * entry_msg)
  {
    Local_Reply_Msg reply;
    saved_client->respond(&reply, sizeof(reply));
    return 0;
  }

public:
  
  Services() : 
    Remote_Multiserver_Point<Services>(NO_OF_SERVICES, this)
  {
    start_deamon();

    add_service(SERVICE_1, "service_1", &Services::service_1);
    add_service(SERVICE_2, "service_2", &Services::service_2);
  }
};

Services * ptr_server;

int service_1(Binding_Point * cli_id, Msg_Entry_Header * msg_head)
{
  Request_Msg request_msg;  
  ptr_server->rpc_send(deamon, &request_msg, sizeof(request_msg));
  
  saved_client = cli_id;

  return 0;
}

int service_2(Binding_Point * cli_id, Msg_Entry_Header * msg_head)
{
  return 0;
}

int main(int argn, char * argc[])
{
  //  Bootstrapper::bootstrap();
  
  Services server;

  ptr_server = &server;
  
  Local_Locator loc("/tmp/channelTest");
  Local_Reception_Point local_rp(loc, 2);

  local_rp.add_service(MYSERV_SERVICE_1, "service_1", service_1);
  local_rp.add_service(MYSERV_SERVICE_2, "service_2", service_2);
  
  pthread_t *t = local_rp.start_deamon();
  
  char str_port[100];

  deamon = Port(argc[1]);

  pthread_join(*t, NULL);

  /*
  MESSAGE("Press any key to continue...");
  char key;
  read(0, &key, 1);
  */
}
