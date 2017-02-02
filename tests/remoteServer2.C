# include "base_message_header.H"
# include "remote_multiserver_point.H"
# include "port.H"
# include <iostream>
# include <aleph.H>


# define NO_OF_SERVICES 2
enum Service_Codes
{
  SERVICE_1,
  SERVICE_2,

  LAST
};

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
    MESSAGE("Response Gotten!");
    return 0;
  }

  pthread_t * serv_thread;

public:
  
  Services() : 
    Remote_Multiserver_Point<Services>(NO_OF_SERVICES, this)
  {
    serv_thread = start_deamon();

    add_service(SERVICE_1, "service_1", &Services::service_1);
    add_service(SERVICE_2, "service_2", &Services::service_2);
  }

  void join_execution()
  {
    pthread_join(*serv_thread, NULL);
  }
};


int main(int argn, char * argc[])
{
  //  Bootstrapper::bootstrap();
  
  Services server;

  char str_port[100];

  cout << "PORT: " << server.get_port().getStringPort(str_port, Port::stringSize);
  
  server.join_execution();
}
















