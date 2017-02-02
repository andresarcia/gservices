# include "local_reception_point.H"
# include "local_access_point.H"
# include "local_readwrite.H"
# include "local_binding.H"
# include "base_message_header.H"
# include <aleph.H>
# include "signal.H"

Local_Reception_Point::
Local_Reception_Point(Local_Locator & deamon_loc, 
		      unsigned int no_of_services) :  
  Reception_Point(no_of_services, deamon_engine), 
  local_location(deamon_loc)
{
  int		        status;
  struct sockaddr_un	server_address;

  /* Create and initialize the table of services */
                           
  table = new (Services_Table_Entry *) [no_of_services];
  for (size_t i = 0; i < no_of_services; i++)
    table[i] = NULL;

  /* initialization of shutdown_mutex and the condition variable
     exists_act_serv */
  
  pthread_mutex_init(&shutdown_lock, NULL);

  /* unix domain socket initialization */

  listen_descriptor = socket(AF_LOCAL, SOCK_STREAM, 0);

  ASSERT(listen_descriptor > 0);

  unlink(deamon_loc.stringficate());

  bzero(&server_address, sizeof(server_address));

  server_address.sun_family      = AF_LOCAL;
  strcpy(server_address.sun_path, deamon_loc.stringficate());

  status = bind(listen_descriptor, (SA *) &server_address,
		sizeof(server_address));

  ASSERT(status >= 0);

  status = listen(listen_descriptor, LISTENQ);  // LISTENQ = 1024
  
  ASSERT(status >= 0);
}


void Local_Reception_Point::
unmark_local_binding_from_waiting_list(int closed_socket)
{
  ASSERT(socket > 0);

  Slink *cursor, 
        *head_element = &binding_list;

  cursor = head_element->getNext();

  while (cursor != head_element)
    {
      int searched_socket = static_cast<Local_Binding*>
	(static_cast<Binding_Node*>(cursor)->
	 binding_point)->get_client_socket();
      
      if (searched_socket == closed_socket) 
	{
	  static_cast<Binding_Node*>(cursor)->
	    binding_point->set_still_busy(false);
	  break;
	}
      cursor = cursor->getNext();
    }
}

void * Local_Reception_Point::deamon_engine(void * class_ptr)
{
  Local_Reception_Point * drp_ptr = static_cast<Local_Reception_Point *>
    (class_ptr);

  int		              i, maxi, maxfd, connfd, sockfd;
  int		              nready, client[FD_SETSIZE]; // FD_SETSIZE = 1024
  fd_set	              rset, allset;
  struct sockaddr_un	      client_address;
  socklen_t	              clilen;
  typedef Msg_Entry_Header    DMEH;
  char                        entry_msg[sizeof(DMEH)];
  void                      * where_to_place, * message; 
  

  maxfd = drp_ptr->listen_descriptor;	/* initialize */
  maxi = -1;		        /* index into client[] array */


  for (i = 0; i < FD_SETSIZE; i++)
    client[i] = -1;		/* -1 indicates available entry */

  FD_ZERO(&allset);
  FD_SET(drp_ptr->listen_descriptor, &allset);

  for ( ; !drp_ptr->is_shutdown ;  ) 
    {
      /* structure assignment */
      rset = allset;
      nready = select(maxfd+1, &rset, NULL, NULL, NULL);

      /* new client connection */
      if (FD_ISSET(drp_ptr->listen_descriptor, &rset)) 
	{	
	  clilen = sizeof(client_address);
	  connfd = accept(drp_ptr->listen_descriptor, (SA *) &client_address,
			  &clilen);

	  ASSERT(connfd >= 0);

	  for (i = 0; i < FD_SETSIZE; i++) 
	    if (client[i] < 0) 
	      {
		client[i] = connfd;	/* save descriptor */
		break;
	      }

	  ASSERT (i < FD_SETSIZE);          /* too many clients */
	  
	  FD_SET(connfd, &allset);	    /* add new descriptor to set */
	  if (connfd > maxfd)
	    maxfd = connfd;		/* for select */
	  if (i > maxi)
	    maxi = i;			/* max index in client[] array */
	  
          if (--nready <= 0)
	    continue;			/* no more readable descriptors */
	}
            
      for (i = 0; i <= maxi; i++) 
	{
	  /* check all clients for data */
   	  if ( (sockfd = client[i]) < 0)
	    continue;
	  
	  if (FD_ISSET(sockfd, &rset))
	    {	      
	      drp_ptr->binding_list_freeing_fct(); 
	      // call it every time, it tests whether there are entries
	      // in the binding_list to delete. Otherwise, in case the list
	      // is empty, the only cost is the empty-list comparison.

	      Local_Binding * client_bind;
	      
	      if (Local_Readwrite::readn(sockfd, entry_msg,sizeof(DMEH)) == 0) 
		{
		  /* connection closed by client */
		  close(sockfd);
		  FD_CLR(sockfd, &allset);
		  client[i] = -1;
		  drp_ptr->unmark_local_binding_from_waiting_list(sockfd);
		  MESSAGE("A client has just been disconnected.");
		  continue;
		}
	      else
		{
# define ENTRY_MSG reinterpret_cast<DMEH*>(entry_msg)
		  message = malloc(sizeof(DMEH) +
				   ENTRY_MSG->get_parameters_size() +
				   ENTRY_MSG->get_invocation_size());
		  
		  // Clients' list. 
		  client_bind = new (Local_Binding) (drp_ptr, sockfd);
		  
		  drp_ptr->binding_list.
		    insertNext(new (Binding_Node) (client_bind, message));
		  
		  // Prepare the message to be dispatched
		  memcpy(message, entry_msg, sizeof(DMEH));
		  
		  if (ENTRY_MSG->get_parameters_size() != 0)
		    {
		      where_to_place = 
			static_cast<char*>(message) + sizeof(DMEH);
		      
		      Local_Readwrite::
			readn(sockfd, where_to_place,
			      ENTRY_MSG->get_parameters_size());
		    }
		  
		  if (ENTRY_MSG->get_invocation_size() != 0)
		    {
		      where_to_place = 
			static_cast<char*>(message) + sizeof(DMEH) + 
			ENTRY_MSG->get_parameters_size();
		      
		      Local_Readwrite::readn(sockfd, where_to_place, 
					     ENTRY_MSG->get_invocation_size());
		    }
		  
		}

# undef ENTRY_MSG

	      drp_ptr->dispatcher(client_bind, 
				  reinterpret_cast<DMEH*>(message));

	      if (--nready <= 0) /* no more readable descriptors */
		break;
	    }  
	} 
    } /* end of main for( ; ; ) */  
  // now, I can safelly close all the conections.      
  for (i = 0; i <= maxi; i++) 
    {	
      /* check all the active clients */
      if ((sockfd = client[i]) < 0)
	continue;
      close(sockfd);
    }
  return 0;
}


void Local_Reception_Point::shutdown()
{
  Local_Access_Point dap_for_shutdown(local_location, 1);

  Shutdown_Message shut_msg;

  //  void * result;
  
  dap_for_shutdown.send(&shut_msg);

 loop:
  pthread_mutex_lock(&shutdown_lock);
  
  if (!is_shutdown)
    {
      pthread_mutex_unlock(&shutdown_lock);
      goto loop;
    }

  pthread_mutex_unlock(&shutdown_lock);
  // while (!is_shutdown);
  // pthread_join(deamon_thread, (void **)&result);
  
  // close the socket by which the server listens to all clients requests
  // for new connections. So, the connection for shut down is the last.
  close(listen_descriptor);

  // delete the connection point. If this hadn't been deleted it would cause
  // that other users could not execute the local_reception_point at the
  // same connection point. Generally, this is a difficult bug to detect.
  unlink(local_location.stringficate());
}

Local_Reception_Point::~Local_Reception_Point()
{
  if (!is_shutdown)
    shutdown();
}

