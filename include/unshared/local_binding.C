# include "local_binding.H"
# include "local_reception_point.H"
# include "local_readwrite.H"
# include "base_message_header.H"

Local_Binding::Local_Binding(Reception_Point * dem_rec_point,
			     const int cli_sock ) :
  Binding_Point(dem_rec_point),
  client_socket(cli_sock)
{
  // empty
}


ssize_t Local_Binding::respond (Msg_Exit_Header * data, 
				const size_t data_size)  
{
  ASSERT(data_size >= sizeof(Msg_Exit_Header));
  
  data->set_response_size(data_size);
  
  data->set_invocation_size(0);
  
  int status = Local_Readwrite::writen(client_socket, data, data_size);

  if (status <= 0) 
    return 0;
  
  // Now, the service has been accomplished.
  set_still_busy(false);

  return status; 
}


ssize_t Local_Binding::respond (Msg_Exit_Header * data, 
				const size_t data_size,
				const void * buffer,
				const size_t buf_size)  
{
  ASSERT(data_size >= sizeof(Msg_Exit_Header));
  
  ASSERT(buffer != NULL || buf_size == 0); 

  data->set_response_size(data_size);
  
  data->set_invocation_size(buf_size); 
  
  int status = Local_Readwrite::writen(client_socket, data, data_size);

  if (status <= 0) 
    return 0;

  if (buf_size > 0)
    {
      status = Local_Readwrite::writen(client_socket, buffer, buf_size);
      ASSERT(status > 0);
    }

  // Now, the service has been accomplished.
  set_still_busy(false);
  
  return status;
}

