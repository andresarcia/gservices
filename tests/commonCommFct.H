# include <base_message_header.H>
# include <iostream>
# include <pthread.h>
# include <aleph.H>
# include "locator_comm_binding.H"


static int count_results = 0;

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


class Request_Msg : public Msg_Entry_Header
{
  float data;
  char  v[100];

public:
  char invocation[0];

  Request_Msg(My_Services servc, size_t invocation_size) :
    // REMEMBER TO ALWAYS INCLUDE THE INVOCATION SIZE AS A PARAMETER
    // FOR INVOCATION MESSAGES. OTHERWISE, THE LIBRARY WILL NOT BE
    // ABLE TO COMPUTE THE TOTAL MESSAGE SIZE.
    Msg_Entry_Header(servc, sizeof(*this) + invocation_size )
    {
      // empty
    }

  void set_data (float & num)
    {
      data = num;
    }
  
  const float get_data() const
  {
    return data;
  }

};


class Reply_Msg : public Msg_Entry_Header
{
  float result;
  char invoc[0];

public:

  Reply_Msg(float r, size_t invocation_size) :
    Msg_Entry_Header(MYSERV_SERVICE_2, sizeof(*this) + invocation_size), 
    result(r)
  {
    // empty
  }
  
  float get_result() const
  {
    return result;
  }

  const char * get_invoc() const
  {
    return &invoc[0];
  }
};


int service_2(Msg_Entry_Header * msg_head)
{
  
  MESSAGE("The result of the invocation was: %f, %s", 
	  static_cast<Reply_Msg *>(msg_head)->get_result(),
	  static_cast<Reply_Msg *>(msg_head)->get_invoc());
  

  count_results++;

  MESSAGE("I've received %i messages.",count_results);

  return 0;
}

/*
struct Param_For_Thread
{
  Locator_Comm_Binding * cli_id;
  Request_Msg copy_of_request;
  
  Param_For_Thread(Locator_Comm_Binding * _cli_id, 
		   Request_Msg & _request) :
    cli_id(_cli_id),
    copy_of_request(_request)
  {
    //empty
  }

};

// Service with thread. This rather means that the service is long.
void * service_thread (void * arg)
{
  MESSAGE("Entro");
  Param_For_Thread * p_pft = static_cast<Param_For_Thread *>(arg);

  sleep(2);

  char response[11]="Respuesta!"; // sizeof(response) == 11
  
  Reply_Msg reply(p_pft->copy_of_request.get_data(), 11);
  
  MESSAGE("transmiting... %f", p_pft->copy_of_request.get_data());

  Locator_Comm_Binding * local_cli_id = p_pft->cli_id;

  AllocDelete(*objectAlloc, p_pft);

  /
  TODO: check the data being transmited.
  line 1: This should be the very last instruction. When this line
          is executed, the current thread could be mistakingly deleted.
          The resulting problem could be that the line 2 would never
          execute.
  /
  local_cli_id->rpc_reply( &reply, sizeof(reply), response, 11 );  
  /
    line 2:
    // AllocDelete(*objectAlloc, p_pft);
  /

  MESSAGE("Service 1_1 has been accomplished");
  return 0;
}

int service_1(Msg_Entry_Header * msg_head, Locator_Comm_Binding * cli_id)
{
  pthread_t t; 
  pthread_attr_t thread_attr;

  pthread_attr_init(&thread_attr);
  pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
          
  MESSAGE("Giving service...");
  
 
  //    The message should be copied, so that it could be safely freed when
  //   returning to dispatcher 
 
  Param_For_Thread * pft = 
    AllocNew (*objectAlloc, Param_For_Thread)
    (cli_id, *static_cast<Request_Msg*>(msg_head));

  pthread_create(&t, &thread_attr, service_thread, pft);
  
  pthread_attr_destroy(&thread_attr);
  return 0;
}
*/


// NO THREADS: short services.
int service_1(Msg_Entry_Header * msg_head, Locator_Comm_Binding * cli_id)
{
  usleep(500000);

  Request_Msg * request = static_cast<Request_Msg *>(msg_head);
  
  // Manipulating the invocation.
  
  //  MESSAGE("The invocation was: %s", request->invocation);

  Reply_Msg reply(request -> get_data(), 11);

  //  MESSAGE("transmiting... %f", request -> get_data());

  // TODO: check the data being transmited.

  char response[11]="0987654321";

  cli_id->rpc_reply( &reply, sizeof(reply), response, 11 );

  //  MESSAGE("Service_1 has been accomplished");
  
  return 0;
}
