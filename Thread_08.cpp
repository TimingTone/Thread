#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

using namespace std;
// int g_mycout = 0;

//packaging a object whose type is int
//we can operate g_mycout just like operating an int-type variable
atomic<int> g_mycout = 0;
// mutex g_my_mutex;
void mythread()
{
    for (int i = 0; i < 1000; ++i){
        // g_my_mutex.lock();
        // g_mycout++;
        // g_my_mutex.unlock();

        //atomic operation can't be interrupted
        g_mycout++;
    }
    return;
}
int main()
{
    thread myobj1(mythread);
    thread myobj2(mythread);
    myobj1.join();
    myobj2.join();

    cout << "Two threads have executed over, eventually the result of g_mycout is: " << g_mycout << endl;
}