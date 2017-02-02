# include "remote_binding.H"
# include "remote_reception_point.H"
# include "base_message_header.H"

Remote_Binding::Remote_Binding( Reception_Point * dem_rec_point,
				MsgId _message_id ) :
  Binding_Point(dem_rec_point),
  message_id(_message_id)
{
  // empty
}

void Remote_Binding::write_to_client(void * data, size_t data_size)
{
  RawMsg reply_msg(data, data_size);

  MESSAGE("responding data_size %i", data_size);
  
  static_cast<Remote_Reception_Point *>(link_to_reception_point)->
    ptr_reception_deamon->send_reply(reply_msg, message_id);
}

ssize_t Remote_Binding::respond (Msg_Exit_Header * data, 
				 const size_t data_size)  
{
  data->set_response_size(data_size);

  data->set_invocation_size(0);

  write_to_client(data, data_size);
  
  // Now, the service has been accomplished.
  set_still_busy(false);
 
  return 0; 
}

ssize_t Remote_Binding::respond (Msg_Exit_Header * data, 
				 const size_t data_size,
				 const void * buffer,
				 const size_t buffer_size)  
{
  data->set_response_size(data_size);

  data->set_invocation_size(buffer_size);

  // data_size accounts for sizeof(Msg_Exit_Header) + sizeof(data).
  void * message = Malloc(data_size + buffer_size);

  // copy the first part of the message
  memcpy(message, data, data_size);
  
  // copy the second part of the message
  memcpy(static_cast<char*>(message) + data_size, buffer, buffer_size);

  // write it to the correspondant reception point
  write_to_client(message, data_size + buffer_size);
  
  Free(message);
    
  // Now, the service has been accomplished.
  set_still_busy(false);

  return 0;
}




