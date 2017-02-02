# include "base_message_header.H"
# include "remote_multiserver_point.H"
# include "local_access_point.H"
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
enum My_Services
{
  MYSERV_SERVICE_1,
  MYSERV_SERVICE_2,

  MYSERV_LAST
};

#define MSG_SIZE 128

Local_Access_Point * ptr_access_point;

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



class Services : public Remote_Multiserver_Point<Services>
{

  int service_1(Msg_Entry_Header * entry_msg,
		Remote_Multiserver_Binding<Services> * return_point)
  {
    Local_Request_Msg request_msg;
    Local_Reply_Msg reply_msg;
    ptr_access_point->send(&request_msg);
    ptr_access_point->receive(&reply_msg, sizeof(reply_msg)); 
    
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
  // Bootstrapper::bootstrap();

  Local_Locator loc("/tmp/channelTest");
  
  Local_Access_Point access_point(loc, 2);

  ptr_access_point = &access_point;

  access_point.add_service(MYSERV_SERVICE_1, "service_1");

  access_point.add_service(MYSERV_SERVICE_2, "service_2");

  Services server;

  char str_port[100];

  cout << "PORT: " << server.get_port().getStringPort(str_port, Port::stringSize);
  
  server.join_execution();
}
















