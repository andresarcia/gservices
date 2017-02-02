/*
 *                      Local Communication. Local Binding.
 */
# ifndef LOCAL_BINDING_H
# define LOCAL_BINDING_H

# include "binding_point.H"
# include "base_message_header.H"
# include "local_reception_point.H"
# include <pthread.h>

class Local_Binding : public Binding_Point 
{
  friend class Local_Reception_Point;

  int client_socket;
  
public:
  Local_Binding( Reception_Point *,
		 const int cli_sock );

  int get_client_socket() const
  {
    return client_socket;
  }

  ssize_t respond (Msg_Exit_Header *, const size_t);
  
  ssize_t respond (Msg_Exit_Header *, const size_t, const void *, const size_t);
};

# endif // LOCAL_BINDING_H










