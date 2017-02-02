# ifndef  BASE_MESSAGE_HEADER_H
# define  BASE_MESSAGE_HEADER_H

# include <unistd.h>
# include <pthread.h>
# include <string.h>
# include <aleph.H>

/**
   Response codes that goes from the server to the client. 
   Normally, DERESP_RESPONSE_TO_SERVICE is the one used for
   communicate the answer to a service request. 
*/
enum Response_Code
{
  DEMRESP_REGISTRATION_OK,
  DEMRESP_INTENDED_DOUBLE_REGISTRATION,
  DEMRESP_SERVICE_NOT_FOUND,
  DEMRESP_RESPONSE_TO_SERVICE,
  DEMRESP_SHUTDOWN_GOTTEN,
  
  DEMRESP_INVALID,
  
  DEMRESP_LAST
};


/// Service codes for the server operation

enum Service_Code
{
  /// Verifies if a proposed service exists at servers side  
  DEMSERV_ADD, 
  /// The message contains a request for a service
  DEMSERV_SEND,

  // The server's shutdown is requiered, only used at servers' side 
  DEMSERV_SHUTDOWN, 
  
  DEMSERV_INVALID, 

  DEMSERV_LAST
};



/**
   Header, which stablishes the difference between system messages and
   client messages (those whose Local_Response_Code is 
   DEMRESP_RESPONSE_TO_SERVICE). It is forbidden to directly use it.
   Client must work with  Local_Msg_Exit_Header.
*/
class Response_Header
{
  Response_Code deamon_response_code;
  size_t        response_size;
  size_t        invocation_size;

protected:

  Response_Header(Response_Code _deamon_response_code) : 
    deamon_response_code(_deamon_response_code),
    response_size(0),
    invocation_size(0)
  {
    // Empty
  }

  Response_Header() : 
    deamon_response_code(DEMRESP_INVALID),
    response_size(0),
    invocation_size(0)
  {
    // Empty
  }

public:
  Response_Code get_deamon_response_code() const
  {
    return deamon_response_code;
  }

  void set_response_size(const size_t size) 
  {
    response_size = size;
  }

  size_t get_response_size() const
  {
    return response_size;
  }

  void set_invocation_size(const size_t size) 
  {
    invocation_size = size;
  }

  size_t get_invocation_size() const
  {
    return invocation_size;
  }
};

/** 
   Header, which stablishes the difference between system messages and
   client messages (those whose Local_Service_Code is 
   DEMSERV_SEND). It is forbidden to directly use it.
   Client must work with  Local_Msg_Entry_Header.
*/
class Service_Header
{
  Service_Code deamon_service_code;

protected:

  Service_Header(Service_Code _service_code) : 
    deamon_service_code(_service_code)
  {
    // Empty
  }

  Service_Header() : 
    deamon_service_code(DEMSERV_INVALID)
  {
    // Empty
  }

public:
  Service_Code get_deamon_service_code() const
  {
    return deamon_service_code;
  }

};

/**
  Message used as identifier of client requests for service.
  It is used in the following way:

  class User_Message : public Local_Msg_Entry_Header
  {
  private:
    // this will be the data to be transmitted to the server.
    type_1 var1, var2 ... var_m
    ...
    type_n var_n_1, var_n_2, ... var_n_m;

  public:
    User_Message(User_Service_Codes u_s_c, others) : 
      Local_Msg_Entry_Header(u_s_c, sizeof(*this)) 
      {
       // whatever it takes to initialize your parameters.
      }

      ... other functions.
  }

  Remember then to have a class User_Service_Codes where you especify which
  services you actually have. This is the only way you have to differenciate
  your services. 

*/
class Msg_Entry_Header : public Service_Header
{
  // TODO: check for friendness of Register_Message.
  friend class Register_Message;
  friend class Shutdown_Message;

  long   client_service_code;
  size_t parameters_size;
  size_t invocation_size;
  
private:
  
  Msg_Entry_Header() :
    client_service_code(0),
    parameters_size(0),
    invocation_size(0)
  {
    // empty
  }
  
  Msg_Entry_Header(const Service_Code system_serv_code,
		   const long _client_service_code,
		   const size_t msg_size = 0) :
    Service_Header(system_serv_code),
    client_service_code(_client_service_code),
    parameters_size(msg_size),
    invocation_size(0)
  {
    // empty
  }
  
protected:
  
  Msg_Entry_Header(const long srvc_code,
		   const size_t msg_size = 0) :
    Service_Header(DEMSERV_SEND),
    client_service_code(srvc_code),
    parameters_size(msg_size - sizeof(Msg_Entry_Header)),
    invocation_size(0)
  {
    ASSERT(parameters_size >= 0);
    ASSERT(client_service_code >= 0);
  }

public:

  long get_client_service_code() const
  {
    return client_service_code;
  }

  void set_parameters_size(const size_t size)
  {
    parameters_size = size;
  }

  size_t get_parameters_size() const
  {
    return parameters_size;
  }

  void set_invocation_size(const size_t size) 
  {
    invocation_size = size;
  }

  size_t get_invocation_size() const
  {
    return invocation_size;
  }
};

/**
  
  Message used as identifier of client replys from services.  It is used
  in the following way:
  
  class User_Message_Response : public Local_Msg_Exit_Header
  {
  private:
  // this will be the response from the server.
  type_1 var1, var2 ... var_k;
  ...
  type_i var_i_1, var_i_2, ... var_i_j;
  
  public:
  User_Message_Response(User_Response_Codes u_r_c, others) : 
  Local_Msg_Exit_Header(USER_RESPONSE_CODES_LAST)
  {
  // whatever it takes to initialize your parameters.
  }
  
  ... other functions.
  }
  
  Remember then to have a class User_Response_Codes where you especify
  which response types you actually have. This is the only way you
  have to differenciate your reply's type.
  
  Also, look at the Local_Msg_Exit_Header initialization parameter
  (USER_RESPONSE_CODES_LAST). It means that at the time you are using
  a enumerative type User_Response_Codes, you must always initialize
  with at least a don't care value. This don't care value is used at
  clients side in which you don't care at the construction time of the
  reply_message. Later you will have fixed your answer when receiving
  the message.  

  At server's side you have to initialize Local_Msg_Exit_Header with
  the apropriate type. That will indicate to the client the status of
  the answer.
*/
class Msg_Exit_Header : public Response_Header
{
  friend class Response_For_Registration;
  friend class Shutdown_Response;

  long response_code;
  
private:  
  
  Msg_Exit_Header(const Response_Code resp_type,
		  const long resp_code) :
    Response_Header(resp_type),
    response_code(resp_code)
  {
    // empty
  }
  
  Msg_Exit_Header() :
    response_code(DEMRESP_INVALID)
  {
    // empty
  }
  
protected:
  
  Msg_Exit_Header(const long resp_code) :
    Response_Header(DEMRESP_RESPONSE_TO_SERVICE),
    response_code(resp_code)
  {
    // empty
  }
  
public:

  void set_response_code(const long resp_code)
  {
    response_code = resp_code;
  }

  long get_response_code() const
  {
    return response_code;
  }
};


class Register_Message : public Msg_Entry_Header 
{
  char service_name[80];

public:
  Register_Message(const long srvc_code, 
		   const char * src_serv_name) :
    Msg_Entry_Header(DEMSERV_ADD, srvc_code, 
		     sizeof(*this) - sizeof(Msg_Entry_Header))
  {
    strcpy(service_name, src_serv_name);
  }
  
  Register_Message()
  {
    // empty
  }
  
  char * get_service_name () const
  {
    return const_cast<char *>(service_name);
  }
};


// TODO: Check Response_For_Registration. It seems that the 2dn 
// TODO: argument is doing nothing.

class Response_For_Registration : public Msg_Exit_Header
{
  
public:
  /* 
     there are various kind of response:

     DEMRESP_REGISTRATION_OK,
     DEMRESP_INTENDED_DOUBLE_REGISTRATION,
     DEMRESP_SERVICE_NOT_FOUND,
     DEMRESP_RESPONSE_TO_SERVICE,
     
     DEMRESP_INVALID,
     
     those responses account for the action of validating certain
     service at client side. 
  */
      
  Response_For_Registration(const Response_Code deam_resp_code,
			    const long resp_type = 0) :
    Msg_Exit_Header(deam_resp_code, resp_type)
  {
    // empty
  }
  
  Response_For_Registration() :
    Msg_Exit_Header()
  {
    // empty
  }
  
};

class Shutdown_Message : public Msg_Entry_Header
{
public:
  Shutdown_Message() :
    Msg_Entry_Header(DEMSERV_SHUTDOWN, DEMSERV_INVALID, 0)
  {
    // empty
  }
};


class Shutdown_Response : public Msg_Exit_Header
{
public:
  Shutdown_Response() :
    Msg_Exit_Header(DEMRESP_SHUTDOWN_GOTTEN, 0)
    {
      // empty
    }
};


/**
  Local_Locator defines an especific location of the server at
  client's side.  At construction time, the only parameter is the path
  of the file which serves as access point for the server. Later the
  socket is set at client's construction time through the function
  set_deamon_socket(int).
*/
class Deamon_Locator 
{
protected:
  Deamon_Locator()
  {
    // empty
  }

public:
  /// This routine shows the string form of the Locator. 
  virtual const char * stringficate() const = 0;
};


# endif // BASE_MESSAGE_HEADER_H




