/*
 *                       Client Local Communication.
 */

# ifndef LOCAL_ACCESS_POINT_H
# define LOCAL_ACCESS_POINT_H

# include "access_point.H"
# include "local_locator.H"
# include "base_message_header.H"
# include <aleph.H>
# include <pthread.h>

# define SA	struct sockaddr
# define LISTENQ		1024	/* 2nd argument to listen() */

# ifndef BZERO
# define BZERO
# define	bzero(ptr,n)		memset(ptr, 0, n)
# endif

# include <sys/types.h>	        /* basic system data types */
# include <sys/socket.h>	/* basic socket definitions */
# include <errno.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/un.h>		/* for Unix domain sockets */
# include <sys/select.h>	/* for convenience */
# include <ostream.h>
# include <unistd.h>

# ifndef AF_LOCAL
# define AF_LOCAL	AF_UNIX
# endif



class Local_Access_Point : public Access_Point
{
public:

  /**
    Constructs an access point to a given server.

    @param \_deamon_locator gives the location of the server.
    @param \_no_of_services number of services to register.
    @param \_alloc memory allocator handler.
   */
  Local_Access_Point(const Local_Locator _deamon_locator,
		     unsigned int _no_of_services) 
    throw (std::exception, NotFound);
  
  /**
     This routine disconects a client from the system.
  */
  void shutdown();

  /**
     This routine invokes shutdown().
  */ 
  ~Local_Access_Point();
};

# endif // LOCAL_ACCESS_POINT_H














