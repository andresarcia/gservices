# include "binding_point.H"
# include "reception_point.H"
# include <pthread.h>

pthread_mutex_t Binding_Point::free_mutex = PTHREAD_MUTEX_INITIALIZER;

void Binding_Point::set_request_message(void *ptr)
{
  request_message = ptr;
}



