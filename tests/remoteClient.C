# include <unistd.h>
# include <base_message_header.H>
# include <remote_access_point.H>
# include <iostream>
# include <aleph.H>
//# include <ahExceptions.H>
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

class Request_Msg : public Msg_Entry_Header
{
  /*
    float data;
    char  v[30000];
  */
public:
  // remember to include the invocation_size at invocation time.
  Request_Msg(My_Services servc/*, size_t invocation_size*/) :
    Msg_Entry_Header(servc, sizeof(*this) /* + invocation_size*/)
    {
      // empty
    }

  /*  void set_data (float & num)
  {
    data = num;
  }*/
};

class Reply_Msg : public Msg_Exit_Header
{
  /*  float result;*/

public:
  Reply_Msg() :
    Msg_Exit_Header(STAT_LAST)
    // remember to invoke Msg_Exit_Header with at least
    // an invalid parameter.
    {
      // empty
    }
  /*  
  float get_result() const
    {
      return result;
    }
  */
};


int main(int argc, char * argv[])
{
  // TODO: check 12/3/3
  //  Bootstrapper::bootstrap();

  const int INVOCATION_SIZE = 11,
    INV_REPLY_SIZE = 11;

  Port port(argv[1]);
  
  Remote_Locator locator1(port);
    
  Remote_Access_Point access_point(locator1, 2);

  Request_Msg request_msg(MYSERV_SERVICE_1/*, INVOCATION_SIZE*/);

  Reply_Msg reply_msg;

  int i, n = argv[2] ? atoi(argv[2]) : 1000;

  //int deamon = 1;
  //deamon = argc[3] ? atoi(argc[3]) : 1;

  /*
  try 
    {
      //      if (deamon == 1)
	access_point = AllocNew(*objectAlloc, Remote_Access_Point) 
	  (locator1, 2);
      //   else
      //access_point = AllocNew(*objectAlloc, Remote_Access_Point) 
      //  (locator2, 2);
    }
  
  catch (NotFound) 
    {
      cout << "The deamon was not found or was not active..." << endl;
      return 0;
    }
  */
  /*
    Validating of services at client side
  */
  access_point.add_service(MYSERV_SERVICE_1, "service_1");
  access_point.add_service(MYSERV_SERVICE_2, "service_2");
  
  //  request_msg.set_data(ret_num);

  char invocation[11] = "1234567890";
  char reply_to_invocation[11];

  double acc = 0;

  unsigned long long start_cycle, end_cycle;
  double elapsed_time, start_time;

  for (i = 0; i < 100; i++)
    {
      //gettimeofday(&ti, NULL);

      start_time = now();

      access_point.send(&request_msg);
      access_point.receive(&reply_msg, sizeof(reply_msg)); 

      elapsed_time = now_delta(&start_time);
      acc += elapsed_time;

      //gettimeofday(&tf, NULL);
      //accumulated+=usec_diff(&ti, &tf);
    }

  cycles_per_second(acc, 100);

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

      //access_point->send(&request_msg, invocation, INVOCATION_SIZE);	
      //access_point->receive(&reply_msg, sizeof(reply_msg), 
      //		    reply_to_invocation, INV_REPLY_SIZE);
      MESSAGE("response for invocation: %s", reply_to_invocation);
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

