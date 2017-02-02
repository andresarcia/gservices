# include <base_message_header.H>
# include <remote_reception_point.H>
# include <local_locator.H>
# include <iostream>
# include <pthread.h>
# include "signal.H"

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

Remote_Reception_Point * ptr_rp;
pthread_t main_thread_id;

class Request_Msg : public Msg_Entry_Header
{
  /*
  float data;
  */
public:
  Request_Msg(My_Services servc, unsigned long size) :
    Msg_Entry_Header(servc, size)
    {
      // empty
    }

  /*  void set_data (float & num)
  {
    data = num;
  }

  float get_data() const 
  {
    return data;
    }*/
};


class Reply_Msg : public Msg_Exit_Header
{
  /*
  float result;
  */
public:
  Reply_Msg(/*float r*/) :
    Msg_Exit_Header(STAT_CODE_1) 
    // Msg_Exit_Header(ResponseCode, ResponseSize = 0)
    //result(r)
  {
    // empty
  }
  /*
  float get_result() const
  {
    return result;
    }*/
};


int service_1_1(Binding_Point * cli_id, Msg_Entry_Header * msg_head)
{
  Request_Msg request(*(static_cast<Request_Msg *>(msg_head)));
  Reply_Msg reply/*(request.get_data())*/;
  MESSAGE("transmiting... "/*, request.get_data()*/);

  // TODO: check the data being transmited.
  
  cli_id->respond( &reply, sizeof(reply) );
  MESSAGE("Service 1_1 has been accomplished");
  return 0;
}

int service_2_1(Binding_Point * cli_id, Msg_Entry_Header * msg_head)
{
  Request_Msg request(*(static_cast<Request_Msg *>(msg_head)));
  Reply_Msg reply/*(request.get_data())*/;
  MESSAGE("transmiting... "/*, request.get_data()*/);

  // TODO: check the data being transmited.

  cli_id->respond( &reply, sizeof(reply) );
  MESSAGE("Service 2_1 has been accomplished");
  return 0;
}


int service_2(Binding_Point * cli_id, Msg_Entry_Header * msg_head)
{
  MESSAGE("Service 2 has been accomplished");
  return 0;
}

void * deamon_1 (void * arg)
{
  //  Local_Reception_Point * rp;

  //  rp = reinterpret_cast<Local_Reception_Point *>(arg);

  //  rp->start_deamon();

  return 0;
}

void * deamon_2 (void * arg)
{
  //  Local_Reception_Point * rp;

  //  rp = reinterpret_cast<Local_Reception_Point *>(arg);

  //rp->start_deamon();

  return 0;
}


void sig_term(int signo)
{
  if (pthread_self() == main_thread_id)
    {
      ptr_rp->shutdown();
    }
}


int main()
{
  //  Bootstrapper::bootstrap();

  main_thread_id = pthread_self();
  
  //  Local_Locator locator1("/tmp/.channelTest");
  
  Remote_Reception_Point reception_point_1(2);
  
  ptr_rp = &reception_point_1;
  
  //    reception_point_2(locator2, 2);
  
  Signal signalHandler(SIGTERM, sig_term);
  
  pthread_t thread1, thread2;

  reception_point_1.add_service(MYSERV_SERVICE_1, "service_1", service_1_1);
  reception_point_1.add_service(MYSERV_SERVICE_2, "service_2", service_2);

  /* 
     reception_point_2.add_service(MYSERV_SERVICE_1, "service_1", service_2_1);
     reception_point_2.add_service(MYSERV_SERVICE_2, "service_2", service_2);
     pthread_create(&thread1, NULL, deamon_1, &reception_point_1);
     pthread_create(&thread2, NULL, deamon_2, &reception_point_2);
     void ** result;
     pthread_join(thread2, result);
  */

  pthread_t *t = reception_point_1.start_deamon();

  char str_port[100];

  cout << "PORT: " << reception_point_1.get_port().getStringPort(str_port, Port::stringSize);
  
  pthread_join(*t, NULL);
 
  return 0;
}









