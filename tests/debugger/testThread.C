# include <pthread.h>
# include <iostream>
# include <unistd.h>

using namespace std;

class a_thread 
{
  pthread_t main_thread;
  
  int a_number;

  static void * stat_fnc (void * arg)
  {
    a_thread * c = static_cast<a_thread *>(arg);
    cout << "Entering into " << arg << " number " << c->a_number << endl << flush;
    sleep(10);
    cout << "Exiting from " << arg << endl << flush;
  }
   
public:
  a_thread(int n) :
    a_number(n)
    {
      // empty
    }
  void go()
    {
      pthread_create(&main_thread,  NULL, stat_fnc, this);
    }
  
  void wait()
    {
      void ** result;
      pthread_join(main_thread, result);
    }

};

int main()
{
  a_thread thr_1(10), thr_2(20);

  thr_1.go();
  sleep(5);
  thr_2.go();
  thr_1.wait();
  thr_2.wait();
}
