/*
 *                       Client Local Communication.
 */

# ifndef ACCESS_POINT_H
# define ACCESS_POINT_H

# include "base_message_header.H"
# include <aleph.H>
# include <pthread.h>


/**
    Local_Access_Point builds an access point to a server. It needs
    a Local_Locator in which it is especified a path where the deamon is.
    And later a Local_Locator class member will have the socket by
    which it is possible to communicate with the server.
    Also you have to especify the number of services you are going to have.
    A normal way of declaration is:

    # define NO_OF_SERVICES ... whatever number of services you have
    
    Local_Locator deamon_locator("/dir1/dir2/file");

    Local_Access_Point link_to_server(deamon_locator, NO_OF_SERVICES);

    Then you have to register all your services that will be available
    in your system. So you have to be coherent with your enumerative
    member of your system and register all the services you already
    wrote. 

    An enumerative member is of the form

    My_Services_Codes
    {
      CODE_FOR_SERVICE_ONE,
      CODE_FOR_SERVICE_TWO,
      ...
      CODE_FOR_INVALID, // this doesn't have to be registered

      LAST_ONE        // so that you can have an upper bound
    }
*/

class Access_Point 
{
protected:

  Deamon_Locator * link_to_deamon;
  
  typedef ssize_t (*Read_Fct)(Deamon_Locator *, void *, size_t);

  typedef ssize_t (*Write_Fct)(Deamon_Locator *, const void *, size_t);

  Read_Fct readn;
 
  Write_Fct writen;

  unsigned int  number_of_services;

  bool is_shutdown;
  

  /**
     Mutex to serialize the send in case of different threads 
     are using the same access point.
  */ 
  pthread_mutex_t * lock_send;
  
  /// Entry to the table of services.
  class Register_Table_Entry 
  {
    char service_name[80];
    
  public:
    Register_Table_Entry(const char * src_serv_name) 
    {
      strcpy(service_name, src_serv_name);
    }
    
    char * get_service_name () 
    {
      return service_name;
    }
  };
  
  /// The table.
  Register_Table_Entry ** table;

  /**
    Constructs an access point to a given server.

    @param \_deamon_locator gives the location of the server.
    @param \_no_of_services number of services to register.
   */
  Access_Point(unsigned int _no_of_services,
	       Read_Fct _read_fct,
	       Write_Fct _write_fct) :
    readn(_read_fct),
    writen(_write_fct),
    number_of_services(_no_of_services),
    is_shutdown(false)
  {
    lock_send = static_cast<pthread_mutex_t *>
      (malloc(sizeof(pthread_mutex_t)));

    pthread_mutex_init(lock_send, NULL);    

    table = new (Register_Table_Entry *) [_no_of_services];
    for (size_t i = 0; i < _no_of_services; i++)
      table[i] = NULL;
  }

public:
  /**
     sends a request to a server.
     
     @param request is a pointer to the message that contains the
     request. 
  */
  void send(const Msg_Entry_Header *request) const
    throw (std::exception, NotFound);
  
  /**
     sends a request to a server, plus an attached data at the end of
     the message. It is recommended to include as a last data member
     of the message class the following field:
     
     char extra_message[0];

     So you can access the extra message at the end of the Msg_Entry_Header.
  */
  void send(Msg_Entry_Header * request,
	    const void * buffer, const size_t buffer_size)   
    throw (std::exception, NotFound);

  /**
     A call to receive blocks until the answer completely arrives.
     The exception SizeFault is thrown if the size of the sent message
     is not the same as the size of the espected message.

     @param reply is a pointer to the memory area where the reply
     is going to be copied. 
     @param reply_size is the expected size of the message. In case
     this size fails when compared to the size transmited, a SizeFault
     exception will be thrown.
  */
  void receive(Msg_Exit_Header *reply, const size_t reply_size)
    throw (std::exception, SizeFault, NotFound); 
  
  /**
     This is semantically the same routine as the later. However, it
     gets an extra chunk of data called "buffer" which was planned to
     be used in invocations.
  */
  void receive(Msg_Exit_Header * message, 
	       const size_t message_size, 
	       void * buffer, 
	       size_t & buffer_size)
  throw (std::exception, SizeFault, NotFound); 
  // TODO: missing size checking in buffer (void *).
  
  /**
     This routine adds a service so that it can be recognized at
     send time. This is a must-do routine before any request to
     the server. Otherwise, NotFound exception will be thrown 
     at send time.
  */
  void add_service(const long service_code, char * name)
    throw (std::exception, Duplicated);

  /**
     This routine deletes a service from clients' registering table.
     This make sense in case you want to replace a service with a 
     new string name. Remember that at add_service time the new name
     will be checked agains the current service name at server's side.
  */
  void delete_service(const long)
    throw (std::exception, NotFound);
  
  /**
     This routine disconects a client from the system.
  */
  virtual void shutdown() = 0;

  /**
     This routine invokes shutdown().
  */ 
  virtual ~Access_Point()
    {
      MESSAGE("destroying access point");
      pthread_mutex_destroy(lock_send);
      
      free(lock_send);
      
      for (size_t i=0; i < number_of_services; i++)
	if (table[i] != NULL)
	  delete table[i];
      delete [] table;
    }

};

# endif // ACCESS_POINT_H



