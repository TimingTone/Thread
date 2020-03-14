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

##### 普通的多个线程共同运行

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
   
   为了防止忘记unlock(), 引入了一个叫std::lock _guard的类模版
   
   std::lock_guard会智能的自动解锁
   
   std::lock_guard可以直接取代lock()和unlock()
   
   ```cpp
   std::mutex my_mutex;
   std::lock_guard<std::mutex> SSS(my_mutex) //put it on the where you need to lock and unlock
   ```

4. 死锁: 存在两个或多个互斥量相互lock(), 无法解开时会产生死锁
   
   解决: 只要保证两个互斥量上锁顺序一致就不会死锁

5. std::lock()函数模版:  一次锁住两个或两个以上的互斥量
   
   它不存在因为锁的顺序问题导致的死锁风险问题, 因为std::lock()会将每个互斥量都锁住, 或都解锁, 只要有一个没锁住, 就会释放之前锁住的互斥量
   
   作用: 用于处理多个互斥量
   
   ```cpp
   std::lock(my_mutex1, my_mutex1……); //可以有多个互斥量
   …………
   my_mutex1.unlock();
   my_mutex2.unlock();
   …………    
   ```

6. std::lock_guard的std ::adopt_lock参数
   
   和std::lock()配合自动实现多个互斥量的上锁和解锁
   
   adopt_lock: 是一个结构体对象, 起一个标记作用, 作用是表示这个互斥量已经lock(), 不需要在std::lock_guard中再次lock()
   
   ```cpp
   std::lock(my_mutex1, my_mutex2);
   std::lock_guard<std::mutex> sss1(my_mutex1, std::adopt_lock);
   std::lock_guard<std::mutex> sss2(my_mutex2, std::adopt_lock);
   ```

# Thread_04

##### unique_lock 取代 lock_guard

unique_lock是一个类模版, 工作中, 一般使用lock_guard(推荐使用)

lock_guard取代了mutex的lock()和unlock(), unique_lock也有一样的功能

unique_lock比lock_guard灵活很多, 但是效率更差, 占用内存更多

```cpp
std::mutex my_mutex;
std::unique_lock<std::mutex> sss(my_mutex);
```

##### 参数:

1. std::adopt_lock: 表示互斥量已经被lock;  
   
   使用条件: 先lock互斥量, 在使用该参数

2. std::try_to_lock: 尝试用lock()去锁定mutex, 但是如果没有锁定成功, 会立即返回, 并不会阻塞在那里
   
   使用前提: 用try_to_lock前不能lock()

3. std::defer_lock: 初始化一个没有加锁的mutex
   
   使用前提: 不能先给mutex加锁

##### unique_lock的成员函数

1. lock()
   
   ```cpp
   std::mutex my_mutex;
   std::unique_lock<std::mutex> sss(my_mutex, std::defer_lock);
   sss.lock();//不需要unlock()
   ```

2. unlock()
   
   ```cpp
   std::mutex my_mutex;
   std::unique_lock<std::mutex> sss(my_mutex, std::defer_lock);
   sss.lock();
   //处理共享代码
   …………
   sss.unlock();
   //处理非共享代码
   …………
   sss.lock();
   //处理共享代码
   …………
   ```

3. try_lock()
   
   尝试给互斥量加锁, 如果拿不到锁, 则返回false, 如果拿不到锁, 返回true, 这个函数不会阻塞

4. release()
   
   返回他所管理的mutex对象指针, 并释放所有权, 也就是说, 这个unique_lock和mutex不再有关系
   
   ```cpp
   std::mutex my_mutex;
   std::unique_lock<std::mutex> sss(my_mutex);//相当于将mutex与unique_lock绑在一起
   std::mutex *pt = sss.release(); //返回值是一个mutex指针, 此时ptx指向my_mutex
   //需要自己手动解锁
   ptx->unlock();
   ```

##### 粒度

把lock()的代码多少称为锁的**粒度** , 用粗(代码多)细(代码少)来描述

##### unique_lock对mutex所有权的传递

unique_lock对象可以将mutex的所有权转移给其他的unique_lock对象

但是所有权不能被复制

```cpp
std::mutex my_mutex;
std::unique_lock<std::mutex> sss1(my_mutex);//sss1拥有my_mutex的所有权
std::unique_lock<std::mutex> sss2(std::move(sss1));//解除sss1对my_mutex的所有权, 将所有权转移给sss2
```

也可以通过函数返回

移动构造函数: 返回局部对象会使系统生成临时的unique_lock对象, 并调用unique_lock的移动构造函数

```cpp
std::mutex my_mutex;
class A{
    public:
        std::unique_lock<std::mutex> rtn_unique_lock(){
            std::unique_lock<std::mutex> temp_sss(my_mutex);
            return temp_sss;//从函数返回一个局部的unique_lock对象
        }
    …………
        std::unique_lock<std::mutex> sss = rtn_unique_lock();
}
```

# Thread_05

单例设计模式共享数据分析, 解决, call_once()

单例类只能创建一个对象

std::call_once():   c++11引入的函数, 该函数的第二个 参数是一个函数名func();

功能: 保证函数func()只会被调用一次

call_once()具备互斥量的能力, 而且效率上, 比互斥量消耗的资源更少

call_once()需要与一个标记(第一个参数)结合使用,这个标记为: std::once_flag; 

call_once()就是通过这个标记来决定对应的函数func()是否执行

调用call_once()成功后,call_once就把这个标记设置为一种“已调”状态

只要once_flag为 “已调用” 状态, 那么对应的函数func()就不会再次执行了

```cpp
std::mutex resource_mutex;
std::once_flag g_flag;

class MyCAS{
    static void createInstance(){
        m_instance = new MyCAS();
        static NewRecycle c;
    }
    ………
    static MyCAS *GetInstance(){
        //注释掉double if那段代码

        //两个线程同时执行到这里,其中一个线程要等另一个线程执行完返回,才能继续执行
        //但是由于g_flag被第一个执行的设置为“已调用”, 因此第二个不再执行该语句
        //会跳过该语句
        std::call_once(g_flag, createInstance);
        return m_instance;
    }
}
```

# Thread_06

##### 条件变量

std::condition_variable,   wait(),    notify_one()

线程A: 等待一个条件满足

线程B: 专门往消息队列中传递消息(数据)

条件变量的作用: 让线程A等待一个条件, 若线程B满足这个条件后触发线程A, 线程A就从等待的位置处继续执行

std::condition_variable :一个和条件相关的一个类, 需要和互斥量配合工作, 用的时候生成这个类的对象;

wait(): 用来等待一个条件

如果wait()没有第二个参数: 如my_cond.wait(sss): 那么第二个参数默认返回false

如果wait()第二个参数lambda表达式返回false,那么wait()将解锁互斥量,并堵塞在本行, 堵塞直到其他某个线程调用notify_one()成员函数为止;  如果返回true, 那么wait()直接返回,让程序继续执行

当其他线程用notify_one()将本wait从堵塞状态唤醒之后, wait就开始不断的尝试重新获取互斥量锁, 如果获取不到,那么流程就会卡在wait这里等着获取,如果获取到了锁(等于加了锁), 那么wait就继续执行:

1.  如果wait()有第二个参数(lambda), 就判断这个lambda表达式, 如果lambda表达式为false, 那wait又对互斥量解锁,继续休眠, 等待被notify_one()唤醒

2. 如果返回值为true, 那么wait返回, 流程走下来(此时互斥锁被锁着)

3. 如果没有第二个参数(相当于返回true),则wait返回,流程走下来.

```cpp
//Thread_03
std::condition_variable my_cond;//生成一个条件变量对象
………………
my_cond.notify_one();
………………
std::mutex my_mutex;
while(true){
    std::unique_lock<std::mutex> sss(my_mutex);
    my_cond.wait(sss, [this]{
        if(!msgRecvQueue.empty()) return true;
        return false;
    });
}
```

notify_all()………

# Thread_07

### std::async     std::future创建后台任务并返回

###### 希望线程返回一个结果

std::async: 是一个函数模版, 用来启动一个异步任务, 并且返回一个std::future对象

std::future: 类模版

###### 启动一个异步任务

自动创建一个线程并开始执行对应的线程入口函数, 它返回一个std::future对象

这个std::future对象里就含有线程入口函数所返回的结果<线程返回的结果>

我们可以通过future对象的成员函数get()来获取结果

**在程序中使用get()函数, 会持续等待子线程执行结束并返回结果, 才能执行下一步**

**在程序中使用wait()函数, 会持续等待子线程执行结束, 但是不会返回结果, 才能执行下一步**

**get()函数只能调用一次**

###### 标记

std::launch: 枚举类型

std::launch::deferred: 延迟调用, 并且没有创建新线程, 是在主线程中调用的线程入口函数

表示线程入口函数调用被延迟到std::future的wait()或者get()函数调用才执行

如果wait()或者get()没有被调用, 那么线程就不会被执行.  实际上, 线程根本就没有被创建

```cpp
std::future<int> result = std::async(std::launch::deferred, &A::mythread, &a, temppar);
result.get();
```

std::launch::async: 在调用async函数时就开始创建新线程

不需要get()或者wait()函数的调用才创建线程, 但需要它们来返回结果以及继续执行

~~async()函数默认用的就是std::launch::async标记~~

```cpp
std::future<int> result = std::async(std::launch::async, &A::mythread, &a, temppar);
```

###### std::packaged_task

打包任务, 把任务包装起来

类模版, 它的模版参数是各种可调用对象

通过std::packaged_task 来把各种可调用对象包装起来, 方便将来作为线程入口函数来调用

```cpp
//std::package_task<函数返回值类型(函数参数类型)>
std::packaged_task<int(int)> mypt(mythread);//把函数mythread通过packaged_task包装起来
std::thread t(std::ref(mypt), 1);
t.join();

std::future<int> result = mypt.get_future();
result.get();
```

```cpp
//包装lambda表达式
std::packaged_task<int(int)> mypt([](int mypar){
    //线程函数
})
std::thread t(std::ref(mypt), 1);
t.join();

mypt(105);//直接调用,函数调用
```

###### std::promise

类模版

我们可以在某个线程中给他赋值, 然后我们可以在其他线程中, 把这个值取出来用

```cpp
void mythread(std::promise<int> &temp, int calc){
    //线程函数
}

std::promise<int> myprom; //声明一个std::promise对象mypron, 保存的值类型为int
std::thread t(mythread, std::ref(myprom), 180);
t.join();
//获取结果值
std::future<int> fu = mypron.get_future();//promise和future绑定, 用于获取线程返回值
auto result = fu.get(); //get只能调用一次, 不能调用多次
```

通过promise保存一个值, 在将来某个时刻我们通过把一个future绑定到这个promise上来, 使用get()函数得到这个绑定的值

###### std::future的其他成员函数

###### std::shared_future

# Thread_08

### 原子操作std::atomic

###### 原子操作概念及范例

有两个线程, 对一个变量进行操作, 一个线程读该变量值, 另一个线程往这个变量中写值

但是两个线程同时操作容易产生bug, 被打断, 不安全

为了防止两个线程同时运行不会被打断, 引入了原子操作

***原子操作: 一般是指“不可分割的操作”, 也就是说这种操作状态要么是完成的, 要么是没完成的, 不可能出现其他的状态***

原子操作可以理解为一种: 不需要用到**互斥量加锁(无锁)技术**的多线程并发编程方式, 是一种在多线程中不会被打断的程序执行片段.

原子操作的效率要高于互斥量操作.



###### 原子操作与互斥量的作用对象

原子操作: 一般是一个变量(非代码段)

互斥量: 代码段



###### std::atomic

类模版

用来封装某个类型的值

一般用于计数或者统计(累计收发的数据包数, )

---

一般atomic原子操作, 针对++, --, +=, -=, |=, &=是支持的, 其他的可能不支持(自己测试)

```cpp
g_mycout++;   //支持
g_mycout+=1;  //支持
g_mycout=g_mycout+1; //不支持
```

# Thread_09

### std::async深入

###### std::async参数详述

1. std::launch::deferred: 延迟调用; 但是并不会创建一个新线程, 需要future对象调用get()或wait()函数才能执行线程入口函数

2. std::launch::async: 强制创建一个新线程,在新线程上执行异步任务

3. std::launch::async | std::launch::deferred
   
    "创建线线程并立即执行" 或者 “没有创建线线程并且延迟到调用result.get()才开始执
   
   行线程入口函数”
   
   两者居其一(不确定性)

4. 如果不带额外参数, 只给async函数一个入口函数名, 其默认额外参数是:
   
   std::launch::async  |  std::launch::deferred
   
   其效果和3完全一致

###### std::async 和 std::thread 的区别

1. std::thread() 如果系统资源紧张, 那么可能创建线程就会失败, 那么执行std::thread()时整个程序可能崩溃;  并且不容易拿到新线程的返回值

2. std::async()  一般不叫做创建线程(当然async可以创建线程), 一般叫做创建一个异步任务;  一般不会报异常不会崩溃, 因为如果系统资源紧张导致无法创建新线程时, std::async这种不加额外参数的调用就不会创建新线程, 而是后续谁调用了get()来请求结果, 这个异步任务就运行在执行这条get()语句所在的线程上

3. std::async和std::thread最明显的不同就是, async有时候并不创建线程

###### 如何判断std::async默认参数执行的是哪一个

通过std::future对象的wait_for函数

```cpp
std::future<int> result = std::async(mythread);
std::future_status status = result.wait_for(0s);
if(status==std::future_status::deferred){
    //此时std::async默认参数是std::launch::deferred
}
else{
    //此时std::async默认参数是std::launch::async
}
```

# Thread_10

以上都是介绍c++跨平台的多线程编程, 以下介绍一些只能在windows平台上的多线程编程

###### windows临界区

windows临界区和mutex(互斥量)是类似的

###### 多次进入临界区试验

###### 自动析构技术

---

以下仍然是c++11跨平台的多线程

###### recursive_mutex递归的独占互斥量

std::mutex: 独占互斥量(不能同时lock, 一次只允许一次lock)

在c++11中不允许同一个线程中lock同一个互斥量多次, 否则会报异常

std::recursive_mutex: 递归的独占互斥量, 可以解决同一个线程多次lock的问题

```cpp
std::recursive_mutex my_mutex;
//其他用法同std::mutex相似
```

###### 带超时的互斥量std::timed_mutex和std::recursive _timed_mutex

以往的互斥量必须lock后等到unlock才能继续执行读写操作, 会被卡住

超时功能指即便无法执行互斥量的读写操作, 也不会被卡住, 会继续执行

---

std::timed_mutex提供了两个新接口

1. try_lock_for():  参数是时间段, 等待一段时间后, 如果拿到锁(return true)或未拿到锁(return false)会继续执行
   
   ```cpp
   std::chrono::milliseconds timeout(100);
   if(my_mutex.try_lock_for(timeout)){}//等待100ms尝试获取锁
   ………
   ```

2. try_lock_until():   参数是未来的时间点,  在未来时间还没到, 如果拿到了锁, 就继续执行, 到了时间没有拿到锁, 也继续执行, 返回值也是bool值
   
   ```cpp
   std::chrono::milliseconds timeout(100);
   if(my_mutex.try_lock_until(chrono::steady_clock::now()+timeout)){}//当前时间点+100ms
   ………
   ```

std::recursive_timed_mutex允许一个线程多次获取互斥量

---

###### 补充知识

1. 虚假唤醒
   
   使用wait()函数时, 多次调用notify_one去唤醒wait(), 而wait()函数中队列的数据为空,  此时需要判断队列是否为空来防止虚假唤醒.
   
   wait()中要有第二参数(lambda)并且这个lambda中要正确判断要处理的公共数据是否存在

2. atomic
   
   ```cpp
   atomic<int> atom;
   while(true){
       cout<<atom<<endl;//读atom是一个原子操作, 但是整个一行代码并不是原子操作
       // 如果同时有对atom写的操作, 可能这行代码在屏幕上输出的只是原来的未写的atom值, 直到写完, 输出的才是正确的值
   }    
   ```
   
   ```cpp
   atomic<int> atom;
   atom = 0
   //注意原子操作不允许直接赋值初始化
   atomic<int> atom_1 = atom;//错误赋值
   
   //load(): 以原子方式读atomic对象的值
   atomic<int> atom_2(atom.load());//正确赋值(读)
   auto atom_3(atom.load());
   
   //store(): 以原子方式写入内容
   atom_3.store(12);       
   ```

###### 线程池

1. 不再每次新建线程, 而是统一管理调度, 循环利用旧的线程的方式, 叫做线程池

2. 一般在程序启动时, 一次性的创建好一定数量的线程, 防止每次创建新线程浪费系统大量的资源, 同时程序也更稳定

###### 线程的创建数量

1. 极限:2000/程序, 再创建程序会崩溃

2. 根据具体要求
