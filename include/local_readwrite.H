# ifndef  LOCCOM_READWRITE_H
# define  LOCCOM_READWRITE_H

# include <sys/types.h>	        /* basic system data types */

class Local_Readwrite
{
public:
  static ssize_t readn(int, void *, size_t);
  static ssize_t writen(int, const void *, size_t);
};

# endif // LOCCOM_MESSAGE_HEADER_H



