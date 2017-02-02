# include <aleph.H>
# include "ipcClient.H"
# include "reception_point.H"
# include "remote_reception_point.H"
# include "base_message_header.H"

Remote_Reception_Point::
Remote_Reception_Point(unsigned int no_of_services) :
  Reception_Point(no_of_services, deamon_engine)
{
  table = new (Services_Table_Entry *) [no_of_services];
  for (size_t i = 0; i < no_of_services; i++)
    table[i] = NULL;

  /* initialization of shutdown_mutex and the condition variable
     exists_act_serv */
  
  pthread_mutex_init(&shutdown_lock, NULL);

  pthread_cond_init(&exists_act_serv, NULL);

  ptr_reception_deamon = new (IpcServer);

  char strP[Port::stringSize];
  
  MESSAGE("Port: %s", ptr_reception_deamon 
	   ->getServerPort().getStringPort(strP, Port::stringSize));
}


void Remote_Reception_Point::shutdown()
{
  if (!is_shutdown)
    {
      IpcRemoteClient shutdown_point(ptr_reception_deamon->getServerPort());

      // getStringPort(strP, Port::stringSize));
      void * result;

      Shutdown_Message shutdown_msg;
      Shutdown_Response shutdown_resp;
      
      // TODO ------------------------------------------------------
      // preparing the messages.
      // TODO: THIS MESSAGE SHOULD BE AN ASSINCRONOUS SENDING. 
      // TODO: THIS SHOULD BE INMMEDIATELY DELETED ONCE THE ROUTINE IS READY.
      RawMsg send_message(&shutdown_msg, sizeof(shutdown_msg), 
			  RawMsg::DEFAULT_FLAG, 10000);
      RawMsg resp_message(&shutdown_resp, sizeof(shutdown_resp), 
			  RawMsg::DEFAULT_FLAG, 10000);
      
      // sending the shutdown message.
      shutdown_point.send_request(send_message);
      shutdown_point.receive_reply(resp_message);
      
      // TODO --------------------------------------------------------

      // waiting for normal termination (through shutdown flag).
      pthread_join(deamon_thread, (void **)&result);
      
      // really shutdown the server.
      delete (ptr_reception_deamon);

      MESSAGE("<<< Finishing shutdown routine >>>");
    }
}

void * Remote_Reception_Point::deamon_engine(void * class_ptr)
{
  typedef Msg_Entry_Header  DMEH; 
  Remote_Reception_Point   * drp_ptr = static_cast<Remote_Reception_Point *>
    (class_ptr);

  char                     message_buffer[4096]; 
  RawMsg                   incomming_message(&message_buffer[0], 4096,
					     RawMsg::DEFAULT_FLAG);

  Remote_Binding           * ptr_client_bind;
  MsgId                    message_id;

  // this message is going to be freed later on other rutine.  
  void                     * message_to_dispatcher;

  // while is not required to be shutdow
  for ( ; !drp_ptr->is_shutdown ;  ) 
    {
      MESSAGE("Waiting For A Remote Request...");
      
      // Look out for this...
      incomming_message.setBodySize(4096);

      //    wait for incoming message
      message_id = (drp_ptr->ptr_reception_deamon)->
	receive_request(incomming_message);

      // TODO: This should be deleted once the assincronous_send routine
      // TODO: is ready.
      if (reinterpret_cast<Msg_Entry_Header*>(message_buffer)
	  ->get_deamon_service_code() == DEMSERV_SHUTDOWN)
	{
	  Shutdown_Response shut_resp;
	  RawMsg resp(&shut_resp, sizeof(shut_resp));
	  (drp_ptr->ptr_reception_deamon)->send_reply(resp, message_id);
	}

      //    take off the system message from ipc message
      message_to_dispatcher = malloc(incomming_message.getBodySize());
            
      //    copy the system message into new location
      memcpy(message_to_dispatcher, incomming_message.getBodyAddr(), 
	     incomming_message.getBodySize());      


      //    put into the binding list the new request for service.
      //    It will be freed later at destruction time.
      ptr_client_bind = new (Remote_Binding) (drp_ptr, message_id);      
      drp_ptr->binding_list.
	insertNext(new(Binding_Node) (ptr_client_bind, message_to_dispatcher));

      //    check for binding-list entries ready to be freed.
      drp_ptr->binding_list_freeing_fct(); 

      //    pass it to the dispatcher
      drp_ptr->dispatcher(ptr_client_bind, 
			  reinterpret_cast<DMEH*>(message_to_dispatcher)); 
    }
  return 0;
}







