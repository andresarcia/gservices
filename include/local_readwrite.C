# include "local_readwrite.H"
# include <sys/types.h>	        /* basic system data types */
# include <sys/socket.h>	/* basic socket definitions */
# include <errno.h>
# include <unistd.h>
# include <aleph.H>


ssize_t Local_Readwrite::readn(int fd, void *vptr, size_t n)
{
  size_t  nleft;
  ssize_t nread;
  char    *ptr;

  ptr = static_cast<char *>(vptr);
  nleft = n;
  while (nleft > 0) 
    {
      if ( (nread = read(fd, ptr, nleft)) < 0) 
	{
          if (errno == EINTR)
	    nread = 0;              /* and call read() again */
	  else
	    { 
	      MESSAGE("*** ERROR: in function read. errno = %i, %s", errno,
		      strerror(errno));
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

   writes the chunk of "n" bytes stored at "vptr" in the 
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

ssize_t Local_Readwrite::writen(int fd, const void *vptr, size_t n)
{
  size_t          nleft;
  ssize_t         nwritten;
  const char      *ptr;

  ptr = static_cast<const char *>(vptr);
  nleft = n;
  while (nleft > 0) 
    {
      nwritten = write(fd, ptr, nleft);
      if ( nwritten <= 0) 
	{
	  if (errno == EINTR)
	    nwritten = 0;           /* call write() again */
	  else
	    { 
	      MESSAGE("*** ERROR: in function write. errno = %i, %s", errno,
		      strerror(errno));
	      return(-1);
	    }
	}
      nleft -= nwritten;
      ptr   += nwritten;
    }
  return(n);
}




