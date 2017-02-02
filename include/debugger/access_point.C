# include "access_point.H"

void Access_Point::delete_service(const long service_code)
  throw (std::exception, NotFound)
{
  if (table[service_code] == NULL)
    Throw (NotFound) ();
  else
    {
      delete (table[service_code]);
      table[service_code] = NULL;
      MESSAGE("Service Deleted!");
    }
}


void Access_Point::send(const Msg_Entry_Header * request) const  
  throw (std::exception, NotFound)
{
  pthread_mutex_lock(lock_send);
  switch (request->get_deamon_service_code())
    {
    case DEMSERV_SEND :
      if (table[request->get_client_service_code()] == NULL)
	{
	  MESSAGE("*** ERROR: Not registered service!");
	  Throw (NotFound) ();
	}
      break;
    case DEMSERV_ADD : 
    case DEMSERV_SHUTDOWN :
      break;
    default:
      ERROR("*** ERROR: Unknown header");
    }
  
  size_t size = sizeof(Msg_Entry_Header) + request->get_parameters_size();
  
  if ((*writen)(link_to_deamon, request, size) < 0)
    ERROR("Unexpected error");
  
}


void Access_Point::send(Msg_Entry_Header * request,
			const void * buffer, const size_t buffer_size)   
  throw (std::exception, NotFound)
{
  pthread_mutex_lock(lock_send);
  switch (request->get_deamon_service_code())
    {
    case DEMSERV_SEND :
      if (table[request->get_client_service_code()] == NULL)
	{
	  MESSAGE("*** ERROR: Not registered service!");
	  Throw (NotFound) ();
	}
      break;
    case DEMSERV_ADD : 
    case DEMSERV_SHUTDOWN :
      break;
    default:
      ERROR("*** ERROR: Unknown header");
    }

  size_t size = 
    sizeof(Msg_Entry_Header) + request->get_parameters_size() + buffer_size; 
  
  void * message = malloc(size);

  // copy the first part of the message
  memcpy(message, request, sizeof(Msg_Entry_Header) + request->get_parameters_size());
  
  // copy the second part of the message
  memcpy(static_cast<char*>(message) + 
	 sizeof(Msg_Entry_Header) + 
	 request->get_parameters_size(), 
	 buffer, buffer_size);

  // write it to the correspondent reception point
  if ((*writen)(link_to_deamon, message, size) < 0)
    ERROR("Unexpected error");
 
  free(message);
}



void Access_Point::receive(Msg_Exit_Header * reply, 
			   const size_t size)
  throw (std::exception, SizeFault, NotFound)
{
  ASSERT(size >= sizeof(Msg_Exit_Header));
  
  if ((*readn)(link_to_deamon, reply, size) == 0)
    {
      pthread_mutex_unlock(lock_send);
      MESSAGE("*** ERROR: The socket has been closed by the server ***");
      Throw (NotFound) ();
    }
  
  if (size != reply->get_response_size())
    {
      pthread_mutex_unlock(lock_send);
      Throw (SizeFault) (size);
    }
  
  pthread_mutex_unlock(lock_send);
}


void Access_Point::receive(Msg_Exit_Header * reply, 
			   const size_t size_msg, 
			   void * buffer_addr, 
			   size_t & size_buf) 
  throw (std::exception, SizeFault, NotFound)
{
#define INVOCATION_SIZE reinterpret_cast<Msg_Exit_Header*>(reply)->get_invocation_size()
      
  ASSERT(size_msg >= sizeof(Msg_Exit_Header));

  ASSERT(size_buf >= INVOCATION_SIZE);
  
  if ((*readn)(link_to_deamon, reply, size_msg) == 0)
    {
      pthread_mutex_unlock(lock_send);
      MESSAGE("*** ERROR: The socket has been closed by the server ***");
      Throw (NotFound) ();
    }
  else
    {  
      // buffer_addr should be left untouched. This is because we 
      // don't know where the address comes from. If it comes from
      // a Malloc, assigning NULL to the pointer could lead to memory
      // leaks.
      // Don't do this: buffer_addr = NULL;
      if (INVOCATION_SIZE > 0)
	{
	  if (INVOCATION_SIZE > size_buf)
	    {
	      pthread_mutex_unlock(lock_send);
	      Throw (SizeFault) (INVOCATION_SIZE);
	    }
	  else
	    {
	      size_buf = (*readn)(link_to_deamon, 
				  buffer_addr, 
				  INVOCATION_SIZE);

	      ASSERT(INVOCATION_SIZE == size_buf);
	    }
	}
    }
  
  if (size_msg != reply->get_response_size())
    {
      pthread_mutex_unlock(lock_send);
      Throw (SizeFault) (size_msg);
    }
  
  pthread_mutex_unlock(lock_send);
}  
#undef INVOCATION_SIZE

 
void Access_Point::add_service(const long service_code,
			       char * name)
  throw (std::exception, Duplicated)
{
  Register_Message reg_msg(service_code, name);
  Response_For_Registration response_msg;

  send(&reg_msg);
  receive(&response_msg, sizeof(response_msg));

  switch (response_msg.get_response_code())
    {
    case DEMRESP_REGISTRATION_OK :
      table[service_code] = new (Register_Table_Entry) (name);
      // MESSAGE("Registration of %s was OK", name);
      break;
    case DEMRESP_SERVICE_NOT_FOUND:
      MESSAGE("Service requiered was not found!");
      Throw (Duplicated) ();
      break;
    default : 
      ERROR("*** ERROR: Header not recognized!");
    }
}


