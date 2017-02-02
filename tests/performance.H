#include <sys/time.h>


struct timeval my_ti, my_tf;

long my_usec_diff(const struct timeval * ti, const struct timeval * tf)
{
  if (ti->tv_sec == tf->tv_sec)
    {
      return tf->tv_usec - ti->tv_usec;
    }
      
  long usec_diff = (tf->tv_sec - ti->tv_sec + 1) * 1000000;

  usec_diff-= ti->tv_usec;

  usec_diff-= (1000000 - tf->tv_usec);

  return usec_diff;
}


# define INIT_MARK gettimeofday(&my_ti, NULL)
# define END_MARK gettimeofday(&my_tf, NULL)
# define MEASURED_TIME my_usec_diff(&my_ti, &my_tf)

