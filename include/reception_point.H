/*
 *                Local Server Communication.
 */ 

# ifndef RECEPTION_POINT_H
# define RECEPTION_POINT_H

# include "binding_point.H"
# include <aleph.H>
# include <pthread.h>
# include <slink.H>

/**
   This class allows to have a reception point to give services.
   The use of this class is as follows:

   # define NO_OF_SERVICES ...whatever quantity of services.

   Local_Locator Local_locator("/dir1/dir2/common_file");

   Local_Reception_Point my_Local_rec_point(Local_locator,
                                              NO_OF_SERVICES);
   
   Then you have to register the services:

   my_Local_rec_point.add_service(SERVICE_CODE, "service_id", 
                                   service_function);
   
   ... remember to do it once for every service you pretend to have.

   And finally, you invoke the non-blocking funtion start_deamon().
 */

typedef int (*Service_Fct) (Binding_Point *, Msg_Entry_Header *);

typedef void * (* Deamon_Engine_Fct_t) (void *); 

class Reception_Point 
{
  /// Client_Binding needs to access some of the private members. 
  friend class Binding_Point;

protected:  
  /** 
      The engine function
  */
  Deamon_Engine_Fct_t deamon_engine_fct;

  /** 
      This variable indicates whether the server is requested 
      to be shutdown or not.
  */
  bool is_shutdown;

  /// main thread for the deamon.
  pthread_t deamon_thread;

  /** 
      Number of actual active services. It has to be 0 so that the server
      can safely be shutdown.
  */
  size_t active_services;

  /**
     This mutex helps in keeping safe the critical sections of code
     which has to do with shutdown procedure.
  */
  pthread_mutex_t shutdown_lock;

  /**
     Condition variable in which the system waits until there are no more
     active services.
  */
  pthread_cond_t exists_act_serv;

  /** 
      This routine deciphers the message and either invokes the service
      or takes the corresponding system action.
  */  

  void dispatcher(Binding_Point *, Msg_Entry_Header *); 

  /// Entry for the regist ering service table.
  class Services_Table_Entry 
  {
    char             service_name[80];
    Service_Fct      service_function;
    
  public:
    
    Services_Table_Entry(char * servc_name,
			 Service_Fct dmn_servc) :
      service_function(dmn_servc)
    {
      strcpy(service_name, servc_name);
    }
    
    Service_Fct get_service_function() const 
    {
      return service_function;
    }
    
    char * get_service_name() const
    {
      return const_cast<char*>(service_name);
    }
  };

  Slink binding_list;
  
  struct Binding_Node : public Slink 
  {
    Binding_Point * binding_point;
    void * request_message;
    
    Binding_Node(Binding_Point * _binding_point,
		 void * _request_message) :
      binding_point(_binding_point),
      request_message(_request_message)
      {
	// empty
      }
  };

  void binding_list_freeing_fct();

  /// number of services you pretend to have in your server.
  size_t   number_of_services;

  /** 
      Number of services you have actually registered. It is going to be
      incremented as services are being registered.
  */
  size_t   registered_services;

  Services_Table_Entry ** table;


  /**
     The following two routines manages the variable
     active_services. This is done in order to have a proper shutting
     down of the server when required.  
  */
  void increase_active_services();
  
  void decrease_active_services();

  /**
    It Shuts down the server. Before the complete shutting down, the server
    accomplish all the pending requests and then disapears.
    Future client requests are going to get a non-reachable server
    error, that must be handled at clients side.
  */
  void base_shutdown();
  

public:  
  /**
     This constructs a reception point at server's side.

     @param local_locator is the common point between the client
     and the server. Remember to specify the same path and file in the
     access point and the reception point.
     @param no_of_services especifies the number of services at server's side.
  */
  Reception_Point(unsigned int no_of_services, 
		  Deamon_Engine_Fct_t _engine_fct) :
    deamon_engine_fct(_engine_fct),
    is_shutdown(false),
    active_services(0),
    number_of_services(no_of_services),
    registered_services(0)
  {
    // empty
  }
  
  /** 
     Starts the deamon in a separate thread. So, remember the
     call to start_deamon() is a non-blocking one. You could also
     save the address of the thread in case you want to join it
     later.
  */
  pthread_t * start_deamon()
    throw (std::exception, UnexpectedException);
  
  /**
    This routine registers a service at server side.
    You have to be coherent with User_Service_Codes, and the string
    which helps to indentify a service specified in Service_Fct.

    Also remember that Service_Fct is of the type:
    typedef int (*Service_Fct) (Local_Binding, Local_Msg_Entry_Header *);

    See also the example testServer.C which shows the form of the service
    routine.    
  */
  void add_service(long, char *, Service_Fct)
    throw (std::exception, Duplicated);
  /** 
     Deletes a service at server's side, so that a new request after
     the deletion can be completed.
  */
  void delete_service(long)
    throw (std::exception, NotFound);

  /// Invokes shutdown() 
  virtual ~Reception_Point()
    {
	base_shutdown();
    }
};

# endif // RECEPTION_POINT_H











