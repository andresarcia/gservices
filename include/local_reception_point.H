/*
 *                Local Server Communication.
 */ 

# ifndef LOCAL_RECEPTION_POINT_H
# define LOCAL_RECEPTION_POINT_H

# include "reception_point.H"
# include "local_binding.H"
# include "local_locator.H"
# include "base_message_header.H"
# include <aleph.H>
# include <pthread.h>
# include <slink.H>

class Local_Reception_Point : public Reception_Point 
{
  /**
     This variable saves the socket by which our server listens
     to new connections.
  */
  int listen_descriptor;

  Local_Locator local_location;

  /// Deamon thread of control for incoming request.
  static void * deamon_engine(void *); 

  void unmark_local_binding_from_waiting_list(int);

public:
  
  /**
     This constructs a reception point at server's side.

     @param local_locator is the common point between the client
     and the server. Remember to specify the same path and file in the
     access point and the reception point.
     @param no_of_services especifies the number of services at server's side.
  */
  Local_Reception_Point(Local_Locator & local_locator,
			unsigned int no_of_services);

  /**
    It Shuts down the server. Before the complete shutting down, the server
    accomplish all the pending requests and then disapears.
    Future client requests are going to get a non-reachable server
    error, that must be handled at clients side.
  */
  void shutdown();
  
  /// Invokes shutdown() 
  ~Local_Reception_Point();
};

# endif // LOCAL_RECEPTION_POINT_H

