/*
 *                Remote Server Communication.
 */ 

# ifndef REMOTE_RECEPTION_POINT_H
# define REMOTE_RECEPTION_POINT_H

# include "reception_point.H"
# include "remote_binding.H"
// # include "remote_locator.H"
# include "base_message_header.H"
# include <aleph.H>
# include <pthread.h>

// Carlos' library.
# include "ipcClient.H"


class Remote_Reception_Point : public Reception_Point 
{
  friend class Remote_Binding;

  IpcServer * ptr_reception_deamon;

  /// Deamon thread of control for incoming request.
  static void * deamon_engine(void *); 
  
public:
  
  /**
     This constructs a reception point at server's side.

     @param local_locator is the common point between the client
     and the server. Remember to specify the same path and file in the
     access point and the reception point.
     @param no_of_services especifies the number of services at server's side.
  */
  Remote_Reception_Point(unsigned int no_of_services);

  /**
    It Shuts down the server. Before the complete shutting down, the server
    accomplish all the pending requests and then disapears.
    Future client requests are going to get a non-reachable server
    error, that must be handled at clients side.
  */

  void shutdown();
  
  /**
     It returns the port in which it is receiving messages.
  */

  Port get_port() const
  {
    return  ptr_reception_deamon->getServerPort();
  }

  /// Invokes shutdown() 
  ~Remote_Reception_Point()
  {
    if (!is_shutdown)
      shutdown();
  }
};

# endif // LOCAL_RECEPTION_POINT_H








