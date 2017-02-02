# include "base_message_header.H"
# include "remote_multiserver_point.H"
# include "port.H"
# include <iostream>
# include <aleph.H>
//# include <performance.H>
# include <pthread.h>
#include <now.h>
#include <cycles.h>
#include <math.h>

# define NO_OF_SERVICES 2
enum Service_Codes
{
  SERVICE_1,
  SERVICE_2,

  LAST
};

int my_n = 0, counter = 0;
bool real_measure = false;
long long cycles = 0, start_cycle, end_cycle;
double elapsed_time, start_time;
double acc = 0;
  
double mmin = 0, mmax = 0;
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

FILE * output_file;

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
    elapsed_time = now_delta(&start_time);
    end_cycle = cycles();
    
    if (!real_measure)
      {
	acc += elapsed_time;

	pthread_cond_signal(&wait_to_finish);
      }
    else
      {
	double elapsed_time_2 = cycles_diff(start_cycle, end_cycle);
	pthread_mutex_lock(&my_lock);
	pthread_cond_signal(&wait_to_finish);
	pthread_mutex_unlock(&my_lock);
	
	  acc+=elapsed_time_2;

	if (counter==0) mmin = elapsed_time_2;
	else
	  if (elapsed_time_2 < mmin) 
	  mmin = elapsed_time_2;
	
	if (elapsed_time_2 > mmax)
	  mmax = elapsed_time_2;
	
	fprintf(output_file, "%lf ",elapsed_time_2);
      }

    if (real_measure) counter++;

    if (counter == my_n)
      pthread_cond_signal(&wait_to_finish);
    
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


int main(int argn, char * argc[])
{
  //  Bootstrapper::bootstrap();
  
  Services server;
  
  
  Port deamon(argc[1]);

  int i;

  my_n = argc[2] ? atoi(argc[2]) : 1000;

  Request_Msg request_msg;

  /*  for (i = 0; i < 100; i++)
    { 
      start_time = now(); 
      server.rpc_send(deamon, &request_msg, sizeof(request_msg));
      pthread_cond_wait(&wait_to_finish, &my_lock);
      }*/

  cycles_per_second(1, 10);
  acc = 0;
  output_file = fopen("data.out","w");
  real_measure = true;

  for (i = 0; i < my_n; i++)
    {
      //     MESSAGE("request N %i  has been sent", i);
      start_cycle = cycles();
      server.rpc_send(deamon, &request_msg, sizeof(request_msg));
      pthread_cond_wait(&wait_to_finish, &my_lock);
    }  

  pthread_mutex_unlock(&my_lock);

  //  pthread_cond_wait(&wait_to_finish, &my_lock);

  fclose(output_file);

  output_file = fopen("data.out","r");
  
  double dev = 0;

  while(!feof(output_file))
    {
      fscanf(output_file, "%lf",  &elapsed_time);
      dev+=pow(elapsed_time - acc/my_n, 2); // (Xi - Xmed)^2
    }

  dev/=my_n-1; //n-1
  
  printf("Cliente, IPC con Ipc_Multiclient_Server y dispatcher.\n");
  printf("Tiempo total en seg %f\n", acc);
  printf("Minimo tiempo %f\n", mmin);
  printf("Maximo tiempo %f\n", mmax);
  printf("Desviacion estandar %f\n\n", sqrt(dev));

  /*
  MESSAGE("Press any key to continue...");
  char key;
  read(0, &key, 1);
  */

}
