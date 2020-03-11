#include <iostream>
#include <list>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;

class A{
    public:
        void inMsgRecvQueue(){
            for (int i = 0; i < 10; ++i){
                cout << "inMsgRecvQueue() executes, insert one element: " << i << endl;

                my_mutex.lock();
                msgRecvQueue.push_back(i);
                my_mutex.unlock();
            }
        }
        bool outMsgLULProc(int &command){
            my_mutex.lock();
            if (!msgRecvQueue.empty())
            {
                //msg is not empty

                //Return the first element, but don't check if it exists.
                int command = msgRecvQueue.front();
                //Remove the first element, but don't return.
                msgRecvQueue.pop_front();
                my_mutex.unlock();
                return true;
            }
            my_mutex.unlock();
            return false;
        }
        void outMsgRecvQueue(){
            int cmd = 0;
            for (int i = 0; i < 10; ++i){
                bool result = outMsgLULProc(cmd);
                if(result == true){
                    cout << "outMsgRecvQueue() executes, get out a element: " <<cmd<< endl;
                }
                else
                {
                    //msg queue is empty
                    cout << "outMsgRecvQueue() executes, but the message queue is currently empty" << endl;
                }
            }
        }

    private:
        //msg queue
        std::list<int> msgRecvQueue;
        //Create a mutex
        std::mutex my_mutex;
};

int main(){
    A myobja;
    //To make sure that the thread using the same object, the second parameter is the reference,
    std::thread myOutMsgObj(&A::outMsgRecvQueue, &myobja);
    std::thread myInMsgObj(&A::inMsgRecvQueue, &myobja);
    myOutMsgObj.join();
    myInMsgObj.join();
    return 0;
}