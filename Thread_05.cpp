#include <iostream>
#include <thread>
#include <string>
#include <mutex>

using namespace std;

// Singleton Class
std::mutex resource_mutex;
class MyCAS
{
private:
	MyCAS() {}				  //privative constructor function
	static MyCAS *m_instance; //Static member variable

public:
	static MyCAS *GetInstance()
	{
		//improve efficiency by double if.
		if (m_instance == NULL)
		{ //double check

			std::unique_lock<std::mutex> my_mutex(resource_mutex);
			if (m_instance == NULL)
			{
				m_instance = new MyCAS();
				//when program is over, the destructor of class will be called.
				static NewRecycle c;
			}
		}
		return m_instance;
	}

	//Release the object
	class NewRecycle
	{
	public:
		~NewRecycle()
		{
			if (MyCAS::m_instance)
			{
				delete MyCAS::m_instance;
				MyCAS::m_instance = NULL;
			}
		}
	};

	void func()
	{
		cout << "Test" << endl;
	}
};

//Class static variable initialization
MyCAS *MyCAS::m_instance = NULL;

// int main()
// {
// 	//Create a object, which return pointer.
// 	MyCAS *p_a = MyCAS::GetInstance();
// 	MyCAS *p_b = MyCAS::GetInstance(); //p_a points to p_b
// 	//cannot create a object by class name
// 	// MyCAS a;
// 	p_a->func();
// 	MyCAS::GetInstance()->func();
// 	return 0;
// }


void mythread(){
	cout << "my thread begins to execute" << endl;
	MyCAS *p_a = MyCAS::GetInstance();
	p_a->func();
	cout << "my thread ends" << endl;
	return;
}

int main(){
	std::thread mytobj1(mythread);
	std::thread mytobj2(mythread);
	mytobj1.join();
	mytobj2.join();
	return 0;
}