#include <iostream>
#include <thread>
#include <string>
using namespace std;

//Wrong Instance
//void myprint(const int& i, char* pmybuf) {
//	cout << i << endl;
//	cout << pmybuf << endl;
//	return;
//}

// Right Instance
void myprint(const int& i, const string& pmybuf) {
	cout << i << endl;
	cout << pmybuf.c_str() << endl;
	return;
}
int main() {

	//1. Passing temporary object as a thread parameter
	/*int mvar = 1;
	int& mvary = mvar;
	char mybuf[] = "This is a test!";
	thread mytobj(myprint, mvar, mybuf);
	mytobj.join();*/

	//2. Pitfalls to avoid (explain 1)
	int mvar = 1;
	int& mvary = mvar;
	char mybuf[] = "This is a test!";
	// pitfall 1: &mvar != &i, i is not the reference of mvar. It is a value pass.
						//it is safe to use detach() in child thread
	// pitfall 2 : A pointer is definitely a reference to an addr
	// Suggestion : Don't use pointer in the thread parameter
	
	// In fact , there are cases where child threads have not yet been executed but main thread has been ending.
	//thread mytobj(myprint, mvar, mybuf);
	
	//The one thing that's absolutely effective in the thread is to convert mybuf to a string object.
	//string() can generate a temporary object
	thread mytobj(myprint, mvar, string(mybuf));
	// It is safe to pass temp object as a thread parameter ,  SO  USE IT!
	mytobj.detach();

	cout << "End of the Main Thread!" << endl;
	return 0;
}