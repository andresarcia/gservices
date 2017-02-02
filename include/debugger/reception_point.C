# include "reception_point.H"
# include <pthread.h>

pthread_t * Reception_Point::start_deamon()
  throw (std::exception, UnexpectedException)
{
  if (pthread_create(&deamon_thread, NULL, deamon_engine_fct, this) != 0)
    Throw (UnexpectedException) ();
  
  return &deamon_thread;
}


void Reception_Point::add_service(long servc_code, 
				  char * servc_name,
				  Service_Fct service) 
  throw (std::exception, Duplicated)
{
  if (table[servc_code] == NULL)
    {
      ASSERT(registered_services < number_of_services);

      /* service not found, therefore register it */
      table[servc_code] = 
	new (Services_Table_Entry) (servc_name, service);
      registered_services++;

      // MESSAGE("Service %s registered.", servc_name);
    }
  else
    {
      MESSAGE("Was the service alredy there?");
      Throw (Duplicated) ();
    }
}


void Reception_Point::delete_service(const long service_code)
  throw (std::exception, NotFound)
{
  if (table[service_code] == NULL)
    Throw (NotFound) ();
  else
    {
      delete table[service_code];
      table[service_code] = NULL;
    }
}


void Reception_Point::dispatcher(Binding_Point  * to_whom, 
				 Msg_Entry_Header * in_message)
{
  Response_For_Registration * resp_for_reg;
  
  switch (in_message->get_deamon_service_code())
    {
    case DEMSERV_ADD :
      /*
	Client wants to register a service
      */
      if ((table[in_message->get_client_service_code()] == NULL) ||
	  (strcmp(table[in_message->get_client_service_code()]
		  ->get_service_name(), 
		  static_cast<Register_Message *>(in_message)
		  ->get_service_name()) != 0))
	{
	  /* 
	     service not found, therefore report it back to client 
	  */
	  resp_for_reg = new (Response_For_Registration) 
	    (DEMRESP_SERVICE_NOT_FOUND);
	  ERROR("Service %s, WAS NOT FOUND!", 
		table[in_message->get_client_service_code()]
		->get_service_name());
	}
      else
	{
	  resp_for_reg = new (Response_For_Registration) 
	    (DEMRESP_REGISTRATION_OK);
	  /*
	    MESSAGE("Service %s, Registered.", 
	    table[in_message->get_client_service_code()]
	    ->get_service_name());
	  */
	}
	      
      to_whom->respond(resp_for_reg, sizeof(Response_For_Registration));
      
      delete resp_for_reg;
      break;

    case DEMSERV_SEND:
      ASSERT(registered_services == number_of_services);

      to_whom->set_request_message(in_message);
      
      if (table[in_message->get_client_service_code()] == NULL)
	/* service not found, therefore ERROR */
	ERROR("Wrong service requested");
      else
	/*
	  either run the service in the same server's thread or 
	  in a different one.
	*/
	(table[in_message->get_client_service_code()]->
	 get_service_function()) (to_whom, in_message);
      break;
    
    case DEMSERV_SHUTDOWN:
      MESSAGE("A SHUTDOWN message has arrived.");
      pthread_mutex_lock(&shutdown_lock);
      // mark binding node of shutdown-message client as a non-busy.
      to_whom->set_still_busy(false);

      is_shutdown = true; 

      pthread_mutex_unlock(&shutdown_lock);

      MESSAGE("SHUTDOWN message processed.");
      // activate the shutdown flag.
      break;
    
    case DEMSERV_INVALID:
    default:
      MESSAGE("Panic: An invalid message appeared");
    }

}


void Reception_Point::base_shutdown()
{
  // free the registering table  
  for (size_t i=0; i < number_of_services; i++)
    if (table[i] != NULL)
        delete (table[i]);
  
  delete [] table;

  /* destoy mutex and condition variable */

  pthread_mutex_destroy(&shutdown_lock);

  // Destroy the binding points who last from the last freeing burst. 
  binding_list_freeing_fct();

  MESSAGE("The server has been shut down...");
}


void Reception_Point::binding_list_freeing_fct()
{
  Slink * list_cursor;

  for ( list_cursor = &binding_list; 
	list_cursor->getNext() != &binding_list; ) 
    {
      if ((static_cast<Binding_Node *>(list_cursor->getNext()) ->
	  binding_point->get_still_busy() == false) || (is_shutdown == true))
	{
	  free(static_cast<Binding_Node *>
	       (list_cursor->getNext())->request_message);

	  delete (static_cast<Binding_Node *>(list_cursor->getNext())
		  ->binding_point);
	  
	  delete (list_cursor->removeNext());
	}
      else
	list_cursor = list_cursor->getNext();
    }
}










