# Thread_01

thread() : 由c++头文件thread引入, 提供多线程的类thread

join() : 实现多线程的方法, 调用该方法会让子线程后于主线程结束, 即会先执行子线程

detach() : 实现多线程的方法, 但是子线程和主线程是并行的

其他实现线程的方法:

使用类 或者 lambda 表达式

# Thread_02

### 传递临时对象作为线程参数:

只要用临时构造的A类对象作为参数传递给线程, 那么就一定能够在主线程执行完毕前把线程函数的参数构造出来, 从而确保使用detach()时, 也能够正常的运行

#### 总结

1. 若传递int这种简单类型参数, 建议都是值传递, 不要用引用

2. 如果传递类对象, 避免隐式类型转换. 需要在创建线程时构造临时对象, 然后在函数参数里, 用引用方式来实现传递(否则会构造多个对象)

3. 以上问题都是在使用detach()的前提下, 建议不使用detach(), 只使用join(), 这样就不存在局部变量失效导致线程对内存的非法引用问题

### 线程ID概念

id是个数字, 每个线程(不管是主线程还是子线程), 实际上都对应着一个数字, 而且每个线程对应的数字都不同.

即不同的线程, 它的线程id(数字)必然不同.

```cpp
//线程id可以用c++标准库中的函数来获取: 
std::this_thread::get_id()
```

### 传递类对象, 智能指针作为线程参数

一般条件下使用引用, 不会真正的将地址传递, 只是拷贝一份参数传递给线程

```cpp
//使用该函数可以把真正的参数引用地址而非拷贝传递给线程函数参数
std::ref()
```

传递智能指针

```cpp
void myprint(unique_ptr<int> pzn){
    //子线程函数
}


int main(){
    //智能指针传递的是真正的参数地址, 不是拷贝
    unique_pt<int> myp(new int(100));
    std::thread mytobj(myprint, std::move(myp));
    //不能使用detach(), 不安全
    mytobj.join();
    return 0;
}
```

### 成员函数指针做线程函数

```cpp
class A{
    int m_i;
    A(int A) :m_i(a){
        cout<<"[A::A(int a)构造函数执行]"<<this<<"thread_id="<<std::this_thread::get_id()<<endl;
    }
    A(const A& a) :m_i(a, m_i){
        cout<<"[A::A(int a)拷贝构造函数执行]"<<this<<"thread_id="<<std::this_thread::get_id()<<endl;
    }
    ~A(){
        cout<<"[A::~A析构函数执行]"<<this<<"thread_id="<<std::this_thread::get_id()<<endl;
    }

    void thread_work(int num){
        cout<<"[子线程thread_work执行]"<<this<<" thread_id="<<std::this_thread::get_id()<<endl;
    }
}


int main(){
    A myobj(10);
    //(成员函数, 对象, 参数)
    std::thread myobj(&A::thread_work, std::ref(myobj), 15);
    mytobj,join();
    //注意,使用std:ref传参只能使用join()
        //而不使用std::ref可以使用detach()
    //可以具体根据是否调用拷贝构造函数来明确是否可以使用detach()
}    
```

```cpp
class A{
    int m_i;
    A(int A) :m_i(a){
        cout<<"[A::A(int a)构造函数执行]"<<this<<"thread_id="<<std::this_thread::get_id()<<endl;
    }
    A(const A& a) :m_i(a, m_i){
        cout<<"[A::A(int a)拷贝构造函数执行]"<<this<<"thread_id="<<std::this_thread::get_id()<<endl;
    }
    ~A(){
        cout<<"[A::~A析构函数执行]"<<this<<"thread_id="<<std::this_thread::get_id()<<endl;
    }

    void operator()(int num){
        cout<<"[子线程()执行]"<<this<<" thread_id="<<std::this_thread::get_id()<<endl;
    }
}


int main(){
    A myobj(10);
    //使用类对象创建线程  (对象, 参数)
    std::thread myobj(std::ref(myobj), 15);
    mytobj.join();
    //此时没有拷贝构造函数生成, 不能使用detach()
}
```

# Thread_03

普通的多个线程共同运行

```cpp
// Entry of threads
void myprint(int inum) {
    cout << "myprint thread begins to start, ID = " << inum << endl;

    cout << "myprint thread ended, ID = " << inum << endl;
}

int main() {
    // Create multi threads
    vector<thread> mythreads;

    // Create 10 threads 
    for (int i = 0; i < 10; i++) {
        mythreads.push_back(thread(myprint, i));
    }
    // Iterator
    for (auto iter = mythreads.begin(); iter != mythreads.end(); ++iter) {
        iter->join();
    }
    cout << "End of Main Thread" << endl;
    return 0;
}
```

##### 数据共享

1. 只读的数据: 安全稳定, 可以直接读取
   
   ```cpp
   vector<int> g_v = { 1, 2 }; // Shared Data, readonly
   void myprint(int inum) {
    cout << "id: " << std::this_thread::get_id << "线程 打印g_v" << g_v[0] << g_v[1] << edl;
    return;
   }
   
   int main() {
       vector<thread> mythreads;
   
       for (int i = 0; i < 10; ++i) {
           mythreads.push_back(thread(myprint, i));
       }
       for (auto iter = mythreads.begin(); iter != mythreads.end(); ++iter) {
           iter->join();
       }
       cout << "End of Main Thread" << endl;
       return 0;
   }
   ```

2. 可读可写: 如果代码没有特殊的处理, 程序肯定会崩溃
   
   简单的不崩溃处理: 读的时候不能写, 写的时候不能读, 同时用于写的线程不能同时写
   
   保护共享数据: 操作时, 用代码将共享数据锁住, 其他想操作共享数据的线程必须等待解锁
   
   代码化解决:  c++ 解决多线程保护共享数据问题的第一个概念“互斥量“

3. 互斥量(mutex): 类对象, 相当于一把“锁”, 多个线程尝试用l*lock()*成员函数来加锁, 只有一个线程可以成功, 读写成功后使用unlock()解锁, 其他线程必须等待直到加锁成功. 
   
   用法: 使用lock(), 操作共享数据, 再解锁unlock(); 
   
   注意: 加锁后必须解锁, lock()后根据退出分支不同,可能有多个unlock()
