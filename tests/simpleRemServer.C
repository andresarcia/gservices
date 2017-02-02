# include <base_message_header.H>
# include <remote_reception_point.H>
# include <iostream>
# include <pthread.h>
# include <aleph.H>

enum My_Services
{
  MYSERV_SERVICE_1,
  MYSERV_SERVICE_2,

  MYSERV_LAST
};

enum Status_Codes
{
  STAT_CODE_1,
  STAT_CODE_2,
  
  STAT_LAST
};

struct Param_For_Thread
{
  Binding_Point * cli_id;
  Msg_Entry_Header * msg_head;
  
  Param_For_Thread(Binding_Point * _cli_id, 
		   Msg_Entry_Header * _msg_head) :
    cli_id(_cli_id),
    msg_head(_msg_head)
  {
    //empty
  }
};


class Request_Msg : public Msg_Entry_Header
{
  float data;
  char  v[300];
  
public:
  char  invocation[0];

  Request_Msg(My_Services servc) :
    Msg_Entry_Header(servc, sizeof(*this))
    {
      // empty
    }

  void set_data (float & num)
  {
    data = num;
  }

  float get_data() const 
  {
    return data;
  }
};


class Reply_Msg : public Msg_Exit_Header
{
  float result;

public:
  Reply_Msg(float r) :
    Msg_Exit_Header(STAT_CODE_1), 
    // Msg_Exit_Header(ResponseCode)
    result(r)
  {
    // empty
  }
  
  float get_result() const
  {
    return result;
  }
};


int service_2(Binding_Point * cli_id, Msg_Entry_Header * msg_head)
{
  MESSAGE (("Service 2 has been accomplished"));
  return 0;
}
/*
// Service with thread. This rather means that the service is long.
void * service_thread (void * arg)
{
  MESSAGE(("Entro"))
  Param_For_Thread * p_pft = static_cast<Param_For_Thread *>(arg);

  //sleep(1);
  Request_Msg * request = static_cast<Request_Msg *>(p_pft->msg_head);
  
  Reply_Msg reply(request -> get_data());
  
  MESSAGE ("transmiting... %f", request -> get_data());

  // TODO: check the data being transmited.

  // line 1:
  p_pft->cli_id->respond( &reply, sizeof(reply) );
  // line 2:
  AllocDelete(*objectAlloc, p_pft);

  MESSAGE ("Service 1_1 has been accomplished");
  return 0;
}

int service_1_1(Binding_Point * cli_id, Msg_Entry_Header * msg_head)
{
  pthread_t t; 
  pthread_attr_t thread_attr;

  pthread_attr_init(&thread_attr);
  pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
          
  MESSAGE ("Giving service...");
  Param_For_Thread * pft = AllocNew (*objectAlloc,
				     Param_For_Thread)(cli_id, msg_head);

  pthread_create(&t, &thread_attr, service_thread, pft);
  
  pthread_attr_destroy(&thread_attr);
  return 0;
}
*/


// NO THREADS: short services.
int service_1_1(Binding_Point * cli_id, Msg_Entry_Header * msg_head)
{
  Request_Msg * request = static_cast<Request_Msg *>(msg_head);
  
  // Manipulating the invocation.
  MESSAGE ("The invocation was: %s", request->invocation);

  Reply_Msg reply(request->get_data());
  
  MESSAGE ("transmiting... %f", request->get_data());

  // TODO: check the data being transmited.

  char response[11]="0987654321";

  cli_id->respond( &reply, sizeof(reply), response, 11 );

  MESSAGE ("Service 1_1 has been accomplished");
  return 0;
}


int main()
{
  // Bootstrapper::bootstrap();

  Remote_Reception_Point reception_point_1(2);
  
  reception_point_1.add_service(MYSERV_SERVICE_1, "service_1", service_1_1);
  reception_point_1.add_service(MYSERV_SERVICE_2, "service_2", service_2);

  reception_point_1.start_deamon();

  MESSAGE(("Press any key to continue..."));
  char key;
  read(0, &key, 1);
  //  sleep(10);
  reception_point_1.shutdown();

  /*
    The has to be a way to wait until all service threads finish.
    Otherwise there is an important probability of having inconsistency
    in number of  Malloc and number of Free.

    Beleive it or not, look at line 1 and 2 of service_thread.
    A simple but dangerous way to solve it is to wait. Tha's the 
    reason of the following sleep(1),
  */
  sleep(1);

  return 0;
}












