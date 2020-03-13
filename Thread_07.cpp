#include <iostream>
#include <mutex>
#include <string>
#include <vector>
#include <future>
#include <thread>

using namespace std;

//Thread entrance function
class A{
    public:
        int mythread(int mypar){
            std::cout << mypar << std::endl;
            std::cout << "mythread() start\t"
                << "thread_ID: "
                << std::this_thread::get_id()
                << std::endl;
            std::chrono::milliseconds dura(5000); //5s
            std::cout << "mythread() end\t"
                << "thread_ID: "
                << std::this_thread::get_id()
                << std::endl;
            return 5;
    }
};

int main()
{
    A a;
    int temppar = 12;
    std::cout << "main\t"
         << "thread_ID: "
         << std::this_thread::get_id
         << std::endl;
    //Ordinary function call approach
    // std::future<int> result = std::async(mythread);

    //Class member function call approach
    std::future<int> result = std::async(&A::mythread, &a, temppar);

    std::cout << "continue………" << std::endl;
    int def;
    def = 0;
    //get() can only be used once
    std::cout << result.get() << std::endl;//Waitting for the end of mythread execution to get the result.
    std::cout << "Main Thread ends" << std::endl;
    return 0;
}
