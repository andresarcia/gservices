# include <unistd.h>
# include "base_message_header.H"
# include "remote_access_point.H"
# include "port.H"
# include <iostream>
# include <aleph.H>
# include <ahExceptions.H>

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
  char  v[300];

public:
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
};

class Reply_Msg : public Msg_Exit_Header
{
  float result;

public:

  Reply_Msg() :
    Msg_Exit_Header(STAT_LAST)
    // remember to invoke Msg_Exit_Header with at least
    // an invalid parameter.
    {
      // empty
    }
  
  float get_result() const
    {
      return result;
    }
};


int main(int argn, char * argc[])
{
  // Bootstrapper::bootstrap();

  Remote_Access_Point * access_point;

  Request_Msg request_msg(MYSERV_SERVICE_1, 11);

  char invocation[11] = "0123456789";
  char response_invocation[11];

  Reply_Msg reply_msg;

  int i, n = argc[1] ? atoi(argc[1]) : 1000;

  Port deamon(argc[3]);

  Remote_Locator rem_locator(deamon);

  float ret_num = argc[2] ? atof(argc[2]) : 3.141592;

  access_point = AllocNew(*objectAlloc, Remote_Access_Point) (rem_locator, 2);

  /*
    Validating of services at client side
  */
  access_point->add_service(MYSERV_SERVICE_1, "service_1");
  access_point->add_service(MYSERV_SERVICE_2, "service_2");
  
  request_msg.set_data(ret_num);

  size_t size = 11;

  for (i = 0; i < n; i++)
    {
      /* 
	 A very normal request for a service.
      */
      access_point->send(&request_msg, invocation, 11);
      MESSAGE(("request has been sent"));
      //      usleep(500000);
      access_point->receive(&reply_msg, sizeof(reply_msg), 
			    response_invocation, size);
      MESSAGE(("reply has been received"));
      MESSAGE(("response for invocation: %s", response_invocation));
      /*
	Printing the result...
      */
      cout << "Result from query:" << reply_msg.get_result() << flush << endl;
    }  
  /*  
      access_point->delete_service(MYSERV_SERVICE_1);
      
      try
      {
      access_point->send(&request_msg, sizeof(request_msg));
      }
      catch (NotFound)
      {
      access_point->add_service(MYSERV_SERVICE_1, "service_1");
      access_point->send(&request_msg, sizeof(request_msg));
      access_point->receive(&reply_msg, sizeof(reply_msg));
      MESSAGE ( ("Now, it works") );
      }
  */
  MESSAGE ( ("I'm gonna sleep for a while...") );
  
  sleep(10);
  
  AllocDelete (*objectAlloc, access_point);
}
















