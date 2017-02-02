/*
 *                       Client Remote Communication.
 */
# ifndef REMOTE_ACCESS_POINT_H
# define REMOTE_ACCESS_POINT_H

# include "access_point.H"
# include "ipcClient.H"
# include "port.H"
# include "base_message_header.H"
# include <aleph.H>
# include <pthread.h>

class Remote_Locator : public Deamon_Locator
{
  IpcRemoteClient * deamon_link;
  Port remote_server_port;

public:
  Remote_Locator(const Port & _server_port) :
    remote_server_port(_server_port)
  {
    // empty
  }

  void set_deamon_link(IpcRemoteClient * _deamon_link)
  {
    deamon_link = _deamon_link;
  }

  IpcRemoteClient * get_deamon_link() const
  {
    return deamon_link;
  }

  Port get_remote_server_port() const
  {
    return remote_server_port;
  }

  const char * stringficate() const 
  {
    return NULL;
  }
};

class Remote_Access_Point : public Access_Point
{
  IpcRemoteClient  * ptr_access_point;

public:

  /**
    Constructs an access point to a given server.

    @param \_deamon_locator gives the location of the server.
    @param \_no_of_services number of services to register.
    @param \_alloc memory allocator handler.
   */
  Remote_Access_Point(Remote_Locator & _deamon_locator,
		      unsigned int _no_of_services);

  /**
     This routine disconects a client from the system.
  */
  void shutdown();

  /**
     This routine invokes shutdown().
  */ 
  ~Remote_Access_Point();
};

# endif // REMOTE_ACCESS_POINT_H














