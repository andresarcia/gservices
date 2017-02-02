# ifndef LOCAL_LOCATOR_H
# define LOCAL_LOCATOR_H 

# include "base_message_header.H"

class Local_Locator : public Deamon_Locator 
{
  char deamon_location[255];
  int  deamon_socket; 

public:
  Local_Locator(const char * dem_loc)
  {
    strcpy(deamon_location, dem_loc);
  }

  const char * stringficate() const
  {
    return deamon_location;
  }

  void set_deamon_socket(int sock_id) 
  {
    deamon_socket = sock_id;
  }

  int get_deamon_socket() const
  {
    return deamon_socket;
  }
};

# endif // LOCAL_LOCATOR_H

