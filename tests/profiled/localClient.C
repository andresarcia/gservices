# include <unistd.h>
# include <base_message_header.H>
# include <local_access_point.H>
# include <iostream>
# include <aleph.H>
//# include "performance.H"
# include <now.h>
# include <cycles.h>
# include <math.h> 

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

# define MSG_SIZE 128

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


int main(int argc, char * argv[])
{
  // TODO: verify if should be ommited. 12/03/2003
  // Bootstrapper::bootstrap();

  const int INVOCATION_SIZE = 11,
    INV_REPLY_SIZE = 11;

  Local_Locator loc("/tmp/channelTest");
    
  Local_Access_Point access_point(loc, 2);

  Local_Request_Msg request_msg;

  Local_Reply_Msg reply_msg;

  int i, n = argv[1] ? atoi(argv[1]) : 1000;

  /*
    Validating of services at client side
  */
  access_point.add_service(MYSERV_SERVICE_1, "service_1");
  access_point.add_service(MYSERV_SERVICE_2, "service_2");
  
  //  request_msg.set_data(ret_num);

  double acc = 0;

  unsigned long long start_cycle, end_cycle;
  double elapsed_time, start_time;

  cycles_per_second(1, 10);

  acc=0;

  double max = 0, min = 0;

  FILE * output_file;

  output_file = fopen("data.out","w");

  for (i = 0; i < n; i++)
    {
      start_cycle = cycles();
      access_point.send(&request_msg);
      access_point.receive(&reply_msg, sizeof(reply_msg)); 
      end_cycle = cycles();

      elapsed_time = cycles_diff(start_cycle, end_cycle);

      if (i==0) min = elapsed_time;
      else
	if (elapsed_time < min) 
	  min = elapsed_time;

      if (elapsed_time > max)
	max = elapsed_time;

      fprintf(output_file, "%f ",elapsed_time);

      acc+=elapsed_time;

      // access_point->send(&request_msg, invocation, INVOCATION_SIZE);	
      // access_point->receive(&reply_msg, sizeof(reply_msg), 
      //	    reply_to_invocation, INV_REPLY_SIZE);
      // MESSAGE("response for invocation: %s", reply_to_invocation);
      //cout << "Result from query:" << reply_msg.get_result() << flush << endl;
    }  
  
  fclose(output_file);

  output_file = fopen("data.out","r");

  double dev = 0;

  while(!feof(output_file))
    {
      fscanf(output_file, "%f",  &elapsed_time);
      dev+=pow(elapsed_time - acc/n, 2); // (Xi - Xmed)^2
    }

  dev/=n-1; //n-1

  printf("Accumulated time: %f",acc);
  
  printf("minimo tiempo %f\n", min);
  
  printf("maximo tiempo %f\n", max);
  
  printf("desviacion estandar %f\n", sqrt(dev));
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
      MESSAGE("Now, it works");
      }
  */
  //  MESSAGE("I'm gonna sleep for a while...");
  
  //  sleep(10);
}

