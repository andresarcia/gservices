# include "local_locator.H"
# include "local_access_point.H"
# include <aleph.H>
# include <pthread.h>
/** includes for unix domain sockets */  
# include <sys/types.h>	        /* basic system data types */
# include <sys/socket.h>	/* basic socket definitions */
# include <errno.h>
# include <unistd.h>


# ifdef New
# define OLDNEW New
# define OLDDELETE Delete
# undef New
# undef Delete
# endif

static ssize_t readn_f(Deamon_Locator * deamon_loc, void *in_buffer, size_t n)
{
  size_t  nleft;
  ssize_t nread;
  char    *ptr;

  ptr = static_cast<char *>(in_buffer);
  nleft = n;
  while (nleft > 0) 
    {
      if ( (nread = read(static_cast<Local_Locator*>(deamon_loc)->
			 get_deamon_socket(), ptr, nleft)) < 0) 
	{
          if (errno == EINTR)
	    nread = 0;              /* and call read() again */
	  else
	    { 
	      MESSAGE("Read error #%i, %s", errno, strerror(errno));
	      return(-1);
	    }
	} 
      else 
	if (nread == 0)
	  break;                          /* EOF */

      nleft -= nread;
      ptr   += nread;
    }

  return(n - nleft);              /* return >= 0 */
}


/* 

   writes the chunk of "n" bytes stored at "in_buffer" in the 
   socket "fd" 

   IMPORTANT NOTE: There is a common trouble with sockets when the
   server is down and a client tries to write a large amount of data
   to that server.  The error can be reproduced when writting more
   than 98256 bytes to a server.  The system call "write" operates as
   writting a first chunk of data to the broken server. Then, because
   of the absence of the server a RST message is received at client's
   side. Due to the implementation of the function
   "Loccom_Readwrite::writen", when trying to write the sencond chunk
   of data the operating system aborts the program.  This program
   aborting is due to the intent of writing twice on a closed socket.

   Between the first and the second writing it is impossible to detect
   the absence of the server. Even when a naive aproach would be to
   read the RST message, what if the chunck written doesn't split as
   espected so that we could read the RST in the middle. Then, the
   proposed solution is to be "prepared" for this situation.

   You can find a lot more information in the book: UNIX Nerwork Programming
   Networking APIs: Sockets and XTI, W. Richard Stevens, Chapter 5.  

*/

static 
ssize_t writen_f(Deamon_Locator * deamon_loc, const void *out_buffer, size_t n)
{
  size_t          nleft;
  ssize_t         nwritten;
  const char      *ptr;

  ptr = static_cast<const char *>(out_buffer);
  nleft = n;
  while (nleft > 0) 
    {
      nwritten = write(static_cast<Local_Locator*>(deamon_loc)->
		       get_deamon_socket(), ptr, nleft);
      if ( nwritten <= 0) 
	{
	  if (errno == EINTR)
	    nwritten = 0;           /* call write() again */
	  else
	    { 
	      ERROR("Unexpected error in write function.");
	      return(-1);
	    }
	}
      nleft -= nwritten;
      ptr   += nwritten;
    }
  return(n);
}


Local_Access_Point::Local_Access_Point(const Local_Locator _deamon_loc,
				       unsigned int _no_of_services)
  throw (std::exception, NotFound)
  : Access_Point(_no_of_services, readn_f, writen_f)
{
  int			status;  
  struct sockaddr_un	server_address; 



  link_to_deamon = new (Local_Locator)(_deamon_loc);
 
  static_cast<Local_Locator*>(link_to_deamon)
    ->set_deamon_socket(socket(AF_LOCAL, SOCK_STREAM, 0));

  /* Create and initialize the table of services */

  ASSERT(number_of_services > 0);

  ASSERT(static_cast<Local_Locator*>
	 (link_to_deamon)->get_deamon_socket() > 0);  
 
  bzero(&server_address, sizeof(server_address));

  server_address.sun_family = AF_LOCAL;
  strcpy(server_address.sun_path, _deamon_loc.stringficate());
 
  status = connect(static_cast<Local_Locator*>
		   (link_to_deamon)->get_deamon_socket(), 
		   (SA *) &server_address, 
		   sizeof(server_address)); 

  // MESSAGE ( ("Client connect status: %i", status);

  if (status < 0)
    {
      delete (link_to_deamon);
      MESSAGE("ERROR: The local server was not found!");
      Throw (NotFound) ();
    }
}


void Local_Access_Point::shutdown()
{
  is_shutdown = true;
  close(static_cast<Local_Locator*>(link_to_deamon)->get_deamon_socket());
  delete (link_to_deamon);
}

Local_Access_Point::~Local_Access_Point()
{
  if (!is_shutdown)
    shutdown();
}












