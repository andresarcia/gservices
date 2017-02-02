# include "base_message_header.H"
# include "remote_multiserver_point.H"
# include "local_reception_point.H"
# include "port.H"
# include <iostream>
# include <aleph.H>

# define NO_OF_SERVICES 2
enum Service_Codes_2
{
  MYSERV_SERVICE_1,
  MYSERV_SERVICE_2
};

#define MSG_SIZE 128


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

int service_1(Binding_Point * cli_id, Msg_Entry_Header * msg_head)
{
  Local_Reply_Msg reply;
  cli_id->respond(&reply, sizeof(reply));
  return 0;
}

int service_2(Binding_Point * cli_id, Msg_Entry_Header * msg_head)
{
  return 0;
}


int main(int argn, char * argc[])
{
  //Bootstrapper::bootstrap();
  
  Local_Locator loc("/tmp/channelTest");
  Local_Reception_Point local_rp(loc, 2);

  local_rp.add_service(MYSERV_SERVICE_1, "service_1", service_1);
  local_rp.add_service(MYSERV_SERVICE_2, "service_2", service_2);
  
  pthread_t *t = local_rp.start_deamon();
  
  pthread_join(*t, NULL);

}

