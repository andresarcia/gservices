/*
 *                      Local Communication. Local Binding.
 */
# ifndef REMOTE_BINDING_H
# define REMOTE_BINDING_H

# include "binding_point.H"
# include "base_message_header.H"
# include "ipcClient.H"
# include <pthread.h>

class Remote_Binding : public Binding_Point 
{
  MsgId message_id;

  void write_to_client(void *, size_t);

public:
  Remote_Binding( Reception_Point *, MsgId );

  ssize_t respond (Msg_Exit_Header *, const size_t);
  
  ssize_t respond (Msg_Exit_Header *, const size_t, const void *, const size_t);
};

# endif // REMOTE_BINDING_H










