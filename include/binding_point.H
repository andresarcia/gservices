# ifndef BINDING_POINT_H
# define BINDING_POINT_H

# include "base_message_header.H"
# include <pthread.h>

class Reception_Point;

class Binding_Point
{
  friend class Reception_Point;

  Binding_Point()
    {
      // empty
    }

  // Determines wheter the service has been accomplish (false) or not (true).

  bool still_busy; 
  

protected:

  Reception_Point        * link_to_reception_point;
 
  /**
     Message's address to free.
  */
  void                   * request_message;
  
  static pthread_mutex_t   free_mutex;

  /**
    To be constructed Local_Binding needs a client socket and a 
    Local_Reception_Point pointer. The client socket is the way Local_Binding
    will communicate the reply to the client. And Local_Reception_Point
    works as a communication link to the Server attending the services.
    This link is important because it helps with the control of the active
    services.
  */
  Binding_Point(Reception_Point * dem_rec_point) :
    // it is true because the service hasb't been accomplished 
    still_busy(true),
    link_to_reception_point(dem_rec_point) 
  {
    // empty
  }
  
public:
  /**
     Remember from loccom_message_header.H the response class has the form:
     class Your_Inherited_Class : public Local_Msg_Exit_Header
     {
     ...
     };
     
     Since you already have a Response class which has inherited  
     Local_Msg_Exit_Header, then you simply 
     respond(your_inherited_class, sizeof(your_inherited_class)).
     
     this is because your_inherited_class is_a Local_Msg_Exit_Header.
  */

  void set_request_message(void *ptr);

  void set_still_busy(const bool _still_busy)
    {
      still_busy = _still_busy;
    }

  bool get_still_busy() const
    {
      return still_busy;
    }
  
  virtual ssize_t respond (Msg_Exit_Header *, const size_t) = 0;
  
  virtual ssize_t respond (Msg_Exit_Header *, const size_t,
			   const void *, const size_t) = 0;

  virtual ~Binding_Point()
  {
    // empty
  }
};

# endif // BINDING_POINT_H





