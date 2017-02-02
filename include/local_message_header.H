# ifndef MESSAGE_HEADER_H
# define MESSAGE_HEADER_H 

# include <stdint.h>
# include "binder.H"
# include "base_message_header.H"
# include "basic_defs.H"
# include "remote_access_point.H"

enum Locator_Call_Entry_Code
{
  /* service codes */ 
  GET_SITE_ID,
  REGISTER_CONTAINER, 
  UNREGISTER_CONTAINER, 
  REGISTER_OBJECT,
  UNREGISTER_OBJECT,
  TARGET_REG_MIG_OBJECT,
  SOURCE_UNREG_MIG_OBJECT,
  TARGET_REG_MIG_CONTAINER,
  SOURCE_UNREG_MIG_CONTAINER,
  STRONG_LOCATE,
  WEAK_LOCATE,
  IMPLICIT_LOCATE,
  CLIENT_INVOKE_SEND,
  LOCATOR_SUPER_RECEIVE,
  SERVER_INVOKE_REPLY,
  TEST_OBJECT_LOCATION,
  PING_N_OBJECTS,
  
  LOCATOR_CALL_ENTRY_CODE_LAST // the last code.
};

enum Reception_Type
{
  RECTYPE_INVOCATION_REQUEST,
  RECTYPE_INVOCATION_REPLY,

  RECTYPE_TYPE_LAST
};

enum Locator_Call_Exit_Code
{
  LOCEXIT_OK,
  LOCEXIT_CONTAINER_UNKNOWN,
  LOCEXIT_CONTAINER_STILL_WORKING,
  LOCEXIT_INTENDED_DUPLICATION,
  LOCEXIT_OBJECT_UNKNOWN,
  LOCEXIT_OBJECT_DEAD,
  LOCEXIT_OBJECT_NOT_FOUND,
  LOCEXIT_OBJECT_RECENT_BINDING,
  LOCEXIT_OBJECT_BUSY,
  
  LOCEXIT_INVALID
};

enum Cache_Update_Policy
{
  LOCCACHE_LRU,
  LOCCACHE_MRU,
  LOCCACHE_MIDDLE,
  
  LOCCACHE_INVALID
};

// This class works for transmision of Container_Id
class Param_Cnt_Msg : public Msg_Entry_Header
{
  Container_Id container_id;

public:
  Param_Cnt_Msg (Locator_Call_Entry_Code service, 
		 const Container_Id & cnt_id) :
    Msg_Entry_Header(service, sizeof(*this)),
    container_id(cnt_id)
    {
      // empty
    }

  const Container_Id & get_container_id() const
    {
      return container_id;
    }
};

// this class works for transmision of an Object_Id and
// a Container_Id.
class Param_ObjCnt_Msg : public Msg_Entry_Header
{
  Object_Id object_id;
  Container_Id container_id;

public:
  Param_ObjCnt_Msg(Locator_Call_Entry_Code service, 
		   const Object_Id & obj_id,
		   const Container_Id & cnt_id) :
    Msg_Entry_Header(service, sizeof(*this)),
    object_id(obj_id),
    container_id(cnt_id)
    {
      // empty
    }

  const Object_Id & get_object_id() 
    {
      return object_id;
    }
  
  const Container_Id & get_container_id() 
    {
      return container_id;
    }
};

// This class works for transmision of Object_Id
class Param_Obj_Msg : public Msg_Entry_Header
{
  Object_Id object_id;

public:
  Param_Obj_Msg(Locator_Call_Entry_Code service, 
		const Object_Id & obj_id) :
    Msg_Entry_Header(service, sizeof(*this)),
    object_id(obj_id)
    {
      // empty
    }

  const Object_Id & get_object_id() const
    {
      return object_id;
    }
};

// This class works for transmision of a Binding.
class Param_Bind_Msg : public Msg_Entry_Header
{
  Binding binding;

public:
  Param_Bind_Msg(Locator_Call_Entry_Code service, 
		 Binding _binding) :
    Msg_Entry_Header(service, sizeof(*this)),
    binding(_binding)
    {
      // empty
    }

  const Binding & get_binding() const
    {
      return binding;
    }
};


//  This class works for transmision of an Object_Id and a Binding
class Param_ObjBind_Msg : public Msg_Entry_Header
{
  Object_Id object_id;
  Binding binding;

public:
  Param_ObjBind_Msg(Locator_Call_Entry_Code service, 
		    const Object_Id & obj_id, 
		    const Binding & _binding) :
    Msg_Entry_Header(service, sizeof(*this)),
    object_id(obj_id),
    binding(_binding)
    {
      // empty
    }

  const Object_Id & get_object_id() const
    {
      return object_id;
    }
  
  const Binding & get_binding() const
    {
      return binding;
    }
  
};

// this class works for transmision of an Object_Id and
// a Container_Id.
class Param_ObjSite_Msg : public Msg_Entry_Header
{
  Object_Id object_id;
  Site_Id site_id;

public:
  Param_ObjSite_Msg(Locator_Call_Entry_Code service, 
		    const Object_Id & _object_id,
		    const Site_Id & _site_id) :
    Msg_Entry_Header(service, sizeof(*this)),
    object_id(_object_id),
    site_id(_site_id)
    {
      // empty
    }
  
  const Object_Id & get_object_id() const
    {
      return object_id;
    }
  
  const Site_Id & get_site_id() const
    {
      return site_id;
    }
};


class Site_Id_CallMsg : public Msg_Entry_Header
{
public:
  Site_Id_CallMsg() :
    Msg_Entry_Header(GET_SITE_ID, sizeof(*this))
  {
    // empty
  }
};


class Site_Id_RetMsg : public Msg_Exit_Header
{
  Site_Id site_id;
  
public:
  Site_Id_RetMsg(Locator_Call_Exit_Code status, Site_Id _site_id) :
    Msg_Exit_Header(status),
    site_id(_site_id)
    {
      // empty
    }
  
  Site_Id_RetMsg() :
    Msg_Exit_Header(LOCEXIT_INVALID),
    site_id(INVALID_SITE_ID)
    {
      // empty
    }

  const Site_Id & get_site_id() const
    {
      return site_id;
    }
};

class Register_Cnt_CallMsg : public Msg_Entry_Header
{
  pid_t pid;

public:
  Register_Cnt_CallMsg() :
    Msg_Entry_Header(REGISTER_CONTAINER, sizeof(*this)),
    pid(getpid())
    {
      // empty
    }

  pid_t get_pid() const
  {
    return pid;
  }
};

class Register_Cnt_RetMsg : public Msg_Exit_Header
{
  Container_Id container_id;
  
public:
  Register_Cnt_RetMsg(Locator_Call_Exit_Code status, 
		      const Container_Id & cnt_id) :
    Msg_Exit_Header(status),
    container_id(cnt_id)
    {
      // empty
    }
  
  Register_Cnt_RetMsg() :
    Msg_Exit_Header(LOCEXIT_INVALID)
    {
      // empty
    }

  const Container_Id & get_container_id() const
    {
      return container_id;
    }
};

class Unregister_Cnt_CallMsg : public Param_Cnt_Msg
{
public:
  Unregister_Cnt_CallMsg(const Container_Id & _container_id) :
    Param_Cnt_Msg(UNREGISTER_CONTAINER, _container_id)
    {
      // empty
    }
};

class Unregister_Cnt_RetMsg : public Msg_Exit_Header
{
public:
  Unregister_Cnt_RetMsg(Locator_Call_Exit_Code status) :
    Msg_Exit_Header(status)
    {
      // empty
    }

  Unregister_Cnt_RetMsg() :
    Msg_Exit_Header(LOCEXIT_INVALID)
    {
      // empty
    }
};


class Param_Loc_Exit_Msg : public Msg_Exit_Header
{
  Locator locator;
 
public:

  Param_Loc_Exit_Msg(Locator_Call_Exit_Code status, 
		     const Locator & _locator) :
    Msg_Exit_Header(status),
    locator(_locator)
    {
      // empty
    }

  Param_Loc_Exit_Msg() :
    Msg_Exit_Header(LOCEXIT_INVALID)
    {
      // empty
    }

  void set_locator(const Locator & _loc) 
    {
      locator = _loc;
    }  
  
  const Locator & get_locator() const
    {
      return locator;
    }
};

class Param_Obj_Exit_Msg : public Msg_Exit_Header
{
  Object_Id object_id;
 
public:

  Param_Obj_Exit_Msg(Locator_Call_Exit_Code status, 
		     const Object_Id & _object_id) :
    Msg_Exit_Header(status),
    object_id(_object_id)
    {
      // empty
    }

  Param_Obj_Exit_Msg() :
    Msg_Exit_Header(LOCEXIT_INVALID)
    {
      // empty
    }

  void set_object_id(const Object_Id & _object_id) 
    {
      object_id = _object_id;
    }  
  
  const Object_Id & get_object_id() const
    {
      return object_id;
    }
};

class Weak_Locate_CallMsg : public Param_Obj_Msg
{
public:
  Weak_Locate_CallMsg(const Object_Id & _object_id) :
    Param_Obj_Msg(WEAK_LOCATE, _object_id)
    {
      // empty
    }
};


class Weak_Locate_RetMsg : public Param_Loc_Exit_Msg
{

public:

  Weak_Locate_RetMsg(Locator_Call_Exit_Code status, 
		     const Locator & _locator) :
    Param_Loc_Exit_Msg(status, _locator)
    {
      // empty
    }

  Weak_Locate_RetMsg() :
    Param_Loc_Exit_Msg()
    {
      // empty
    }
};


class Strong_Locate_CallMsg : public Param_Obj_Msg
{
public:
  Strong_Locate_CallMsg(const Object_Id & _object_id) :
    Param_Obj_Msg(STRONG_LOCATE, _object_id)
    {
      // empty
    }
};


class Strong_Locate_RetMsg : public Param_Loc_Exit_Msg
{
 
public:

  Strong_Locate_RetMsg(Locator_Call_Exit_Code status, 
		       const Locator & _locator) :
    Param_Loc_Exit_Msg(status, _locator)
    {
      // empty
    }

  Strong_Locate_RetMsg() :
    Param_Loc_Exit_Msg()
    {
      // empty
    }
};

class Implicit_Locate_CallMsg : public Param_Bind_Msg
{
public:
  Implicit_Locate_CallMsg(const Binding & _binding) :
    Param_Bind_Msg(IMPLICIT_LOCATE, _binding)
    {
      // empty
    }
};

class Implicit_Locate_RetMsg : public Msg_Exit_Header
{
public:
  Implicit_Locate_RetMsg(Locator_Call_Exit_Code _status) :
    Msg_Exit_Header(_status)
    {
      // empty
    }
  
  Implicit_Locate_RetMsg() :
    Msg_Exit_Header(LOCEXIT_INVALID)
    {
      // empty
    }
};  

class Register_Obj_CallMsg : public Param_Cnt_Msg
{
public:
  Register_Obj_CallMsg(const Container_Id & _container_id) :
    Param_Cnt_Msg(REGISTER_OBJECT,
		  _container_id)
  {
    // empty
  }
};


class Register_Obj_RetMsg : public Param_Obj_Exit_Msg
{
public:
  Register_Obj_RetMsg(Locator_Call_Exit_Code _status, 
		      const Object_Id & _object_id) :
    Param_Obj_Exit_Msg(_status, _object_id)
    {
      // empty
    }

  Register_Obj_RetMsg() :
    Param_Obj_Exit_Msg()
    {
      // empty
    }
};


class Unregister_Obj_CallMsg : public Param_Obj_Msg
{
public:
  Unregister_Obj_CallMsg(const Object_Id & _object_id) : 
    Param_Obj_Msg(UNREGISTER_OBJECT, _object_id)
    {
      // empty
    }
};

class Unregister_Obj_RetMsg : public Msg_Exit_Header
{
public:
  Unregister_Obj_RetMsg(Locator_Call_Exit_Code _status) :
    Msg_Exit_Header(_status)
    {
      // empty
    }
  
  Unregister_Obj_RetMsg() :
    Msg_Exit_Header(LOCEXIT_INVALID)
    {
      // empty
    }
};  


class Clt_Invoke_Send_CallMsg : public Msg_Entry_Header
{
  Binding binding;
  size_t  request_size;

public:
  char request[0];

  Clt_Invoke_Send_CallMsg(const Binding & _binding,
			  const size_t _request_size) :
    Msg_Entry_Header(CLIENT_INVOKE_SEND, sizeof(*this)),
    binding(_binding),
    request_size(_request_size)
  {
    set_invocation_size(_request_size);
  }

  size_t get_request_size() const
  {
    return request_size;
  }
  
  Binding & get_binding() 
  {
    return binding;
  }
};

class Clt_Invoke_Send_RetMsg : public Msg_Exit_Header
{
  // Once the message is sent to the locator, it will assign a unique
  // identification to such message. The identification is returned in
  // the field message_id.
  Message_Id message_id;

  Locator locator;

public:
  Clt_Invoke_Send_RetMsg(const Message_Id & _message_id,
			 const Locator & _locator,
			 Locator_Call_Exit_Code _status) :
    Msg_Exit_Header(_status),
    message_id(_message_id),
    locator(_locator)
  {
    // empty
  }
  
  Clt_Invoke_Send_RetMsg() :
    Msg_Exit_Header(LOCEXIT_INVALID)
  {
    // empty
  }
  
  const Message_Id & get_message_id() const
  {
    return message_id;
  }
  
  const Locator & get_locator() const
  {
    return locator;
  }
};


class Locator_Super_Receive_CallMsg : public Msg_Entry_Header
{
  size_t        message_size;
  Binding       binding;
  Container_Id  receiving_container_id;
    
public:
  Locator_Super_Receive_CallMsg(const size_t & _message_size,
				const Binding & _binding,
				const Container_Id & _rec_cnt_id) :
    Msg_Entry_Header(LOCATOR_SUPER_RECEIVE, sizeof(*this)),
    message_size(_message_size),
    binding(_binding),
    receiving_container_id(_rec_cnt_id)
  {
    // empty
  }  
  
  size_t get_message_size() const
  {
    return message_size;
  }
  
  const Binding & get_binding() const
  {
    return binding;
  }
  
  const Container_Id & get_receiving_container_id() const
  {
    return receiving_container_id;
  }
};

class Locator_Super_Receive_RetMsg : public Msg_Exit_Header
{
  Message_Id message_id;
  
  // This binding carries the information of the source container who 
  // does the invocation. So, with this information I can answer to the
  // invocation.

  Binding binding;

  size_t message_size;
  
  Reception_Type reception_type;

public:

  Locator_Super_Receive_RetMsg(const Message_Id & _message_id,
			       const Binding & _binding,
			       size_t _msg_size,
			       Reception_Type _reception_type,
			       Locator_Call_Exit_Code _status) :
    Msg_Exit_Header(_status),
    message_id(_message_id),
    binding(_binding),
    message_size(_msg_size),
    reception_type(_reception_type)
  {
    // empty
  }
  
  Locator_Super_Receive_RetMsg() :
    Msg_Exit_Header(LOCEXIT_INVALID)
  {
    // empty
  }

  const Message_Id & get_message_id() const
  {
    return message_id;
  }

  size_t get_message_size() const
  {
    return message_size;
  }
  
  const Binding & get_binding() const
  {
    return binding;
  }
  
  Reception_Type get_reception_type() const
  {
    return reception_type;
  }
};

class Srv_Invoke_Reply_CallMsg : public Msg_Entry_Header
{
  Message_Id message_id;
  Binding binding;
  Container_Id  replying_container_id;
  size_t  reply_size;
  // Buffer for the invocation reply.
  char    reply_buffer[0];
  
public:

  
  Srv_Invoke_Reply_CallMsg(const Message_Id & _message_id,
			   const Binding & _binding,
			   const Container_Id & _rep_cnt_id,
			   const size_t _reply_size) :
    Msg_Entry_Header(SERVER_INVOKE_REPLY, sizeof(*this)),
    message_id(_message_id),
    binding(_binding),
    replying_container_id(_rep_cnt_id),
    reply_size(_reply_size)
    {
      set_invocation_size(reply_size);
    }
  
  const Message_Id & get_message_id() const
  {
    return message_id;
  }

  const Binding & get_binding() const
    {
      return binding;
    }
  
  size_t get_reply_size() const
    {
      return reply_size;
    }

  const Container_Id & get_replying_container_id() const
  {
    return replying_container_id;
  }

  char * get_reply_buffer() 
  {
    return &reply_buffer[0];
  }
};

class Srv_Invoke_Reply_RetMsg : public Msg_Exit_Header
{
public:
  Srv_Invoke_Reply_RetMsg(Locator_Call_Exit_Code _status) :
    Msg_Exit_Header(_status)
    {
      // empty
    }

  Srv_Invoke_Reply_RetMsg() :
    Msg_Exit_Header(LOCEXIT_INVALID)
    {
      // empty
    }
};


class Src_Unreg_Mig_Obj_CallMsg : public Msg_Entry_Header
{
  Object_Id object_id;
  Site_Id   tgt_site_id;

public:
  Src_Unreg_Mig_Obj_CallMsg(const Object_Id & _obj_id,
			    const Site_Id   & _site_id) :
    Msg_Entry_Header(SOURCE_UNREG_MIG_OBJECT, sizeof(*this)),
    object_id(_obj_id),
    tgt_site_id(_site_id)
  {
    // empty
  }

  const Object_Id & get_object_id() const
  {
    return object_id;
  }

  const Site_Id & get_tgt_site_id() const
  {
    return tgt_site_id;
  }
};

class Src_Unreg_Mig_Obj_RetMsg : public Msg_Exit_Header
{
public:
  Src_Unreg_Mig_Obj_RetMsg(Locator_Call_Exit_Code _status) :
    Msg_Exit_Header(_status)
  {
    // empty
  }
  
  Src_Unreg_Mig_Obj_RetMsg() :
    Msg_Exit_Header(LOCEXIT_INVALID)
  {
    // empty
  }
};

class Tgt_Reg_Mig_Obj_CallMsg : public Msg_Entry_Header
{
  Object_Id object_id;
  Container_Id container_id;
  Logical_Timestamp logical_timestamp;

public:
  Tgt_Reg_Mig_Obj_CallMsg(const Object_Id & _obj_id,
			  const Container_Id & _cnt_id,
			  const Logical_Timestamp _timestamp) :
    Msg_Entry_Header(TARGET_REG_MIG_OBJECT, sizeof(*this)),
    object_id(_obj_id),
    container_id(_cnt_id),
    logical_timestamp(_timestamp)
  {
    // empty
  }
  
  const Object_Id & get_object_id() const
  {
    return object_id;
  }
  
  const Container_Id & get_container_id() const
  {
    return container_id;
  }
  
  Logical_Timestamp get_logical_timestamp() const
  {
    return logical_timestamp;
  }
};

class Tgt_Reg_Mig_Obj_RetMsg : public Msg_Exit_Header
{
public:
  Tgt_Reg_Mig_Obj_RetMsg(Locator_Call_Exit_Code _status) :
    Msg_Exit_Header(_status)
  {
    // empty
  }
  
  Tgt_Reg_Mig_Obj_RetMsg() :
    Msg_Exit_Header(LOCEXIT_INVALID)
  {
    // empty
  }
};


class Test_Object_Location_CallMsg : public Msg_Entry_Header
{
  Locator locator;
  
public:
  Test_Object_Location_CallMsg(const Locator & _locator) :
    Msg_Entry_Header(TEST_OBJECT_LOCATION, sizeof(*this)),
    locator(_locator)
  {
    // empty
  }

  Locator & get_locator() 
  {
    return locator;
  }
};

class Test_Object_Location_RetMsg : public Msg_Exit_Header
{
  Locator new_locator;

public:
  Test_Object_Location_RetMsg(const Locator & _new_locator,
			      Locator_Call_Exit_Code _status) :
    Msg_Exit_Header(_status),
    new_locator(_new_locator)
  {
    // empty
  }
  
  Test_Object_Location_RetMsg() :
    Msg_Exit_Header(LOCEXIT_INVALID)
  {
    // empty
  }

  const Locator & get_new_locator() const 
  {
    return new_locator;
  }
};


class Ping_N_Objects_CallMsg : public Msg_Entry_Header
{
  unsigned int number_of_objects;
  Cache_Update_Policy policy;

public:
  Ping_N_Objects_CallMsg(unsigned int _number_of_objects,
			 Cache_Update_Policy _policy) :
    Msg_Entry_Header(PING_N_OBJECTS, sizeof(*this)),
    number_of_objects(_number_of_objects),
    policy(_policy)
  {
    // empty
  }
  
  unsigned int get_number_of_objects() const
  {
    return number_of_objects;
  }

  Cache_Update_Policy get_policy() const
  {
    return policy;
  }
};

class Ping_N_Objects_RetMsg : public Msg_Exit_Header
{
public:
  Ping_N_Objects_RetMsg(Locator_Call_Exit_Code _status) :
    Msg_Exit_Header(_status)
  {
    // empty
  }
  
  Ping_N_Objects_RetMsg() :
    Msg_Exit_Header(LOCEXIT_INVALID)
  {
    // empty
  }    
};

# endif // MESSAGE_HEADER_H 


