#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <thread>
//The sign of whether the entire progress is completed is 
//whether the main thread has completed execution
using namespace std;
//in general, other child threads will be forced to terminate by 
//the operating system if they do not finish executing after main
//thread terminated.


// Create threads

// 1. Header file <thread>
// 2.Create a thread from a functionc
void myprint() {
	cout << "Child thread begins to execute" << endl;
	cout << "End of child thread execution" << endl;
}

class TA {

public:
	void operator()() {
		cout << "Thread operator() begins to execute" << endl;
		cout << "End of thread operator()" << endl;

	}
};


//  Main thread starts execution from the main()
int main() {

	// 3. Write codes in main

	// 4. thread is a class of stl, set myprint() as the starting point for thread execution
	//thread mytobj(myprint);

	// 5, join(), : block main thread until child thread executes to end  (Traditional method)
	//mytobj.join();

	// 5.5 detach() : main thread could end before child threads  (New method)
	// 5.6 join() and detach() cannot be called at the same time
	//mytobj.detach();

	// 5.7 joinable() : judge if succeeding in using join() or detach(),  Return true or false.
	/*if (!mytobj.joinable()) {
		cout << "joinable() == false" << endl;
	}
	else {
		cout << "joinable()==true" << endl;
	}*/

	 //6. Create other thread through class
	/*TA ta;
	thread mytobj3(ta);
	mytobj3.join();*/

	// 7. Create thread by lambda expression
	auto mylamthread = [] {
		cout << "Start up my thread3" << endl;
		cout << "End of my thread3" << endl;
	};
	thread mytobj5(mylamthread);
	mytobj5.join();
	// 8. execute main thread
	cout << "End of main thread execution" << endl;
	return 0;
}

