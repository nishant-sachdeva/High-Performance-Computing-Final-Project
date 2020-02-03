// #include "functions.h"
#include <iostream>


struct Slice
{
	uint8_t size;
	char*   data;
};


using namespace std;
#include "get.cpp"
#include "put.cpp"





class kvStore {
	public:
		kvStore(uint64_t max_entries)
		{
			// make space for max entries number of entries I guess ? 
			
		}

		bool get(); //returns false if key didn’t existbool 
		bool put(Slice &key, Slice &value); //returns true if value overwrittenbool 
		bool remove(Slice &key);
		bool get(int N, Slice &key, Slice &value);
 		//returns Nth key-value pairbool 
		bool remove(int N); //delete Nth key-value pair
};


int main()
{
	kvStore obj1(10000);
	obj1.get();

	return 0;
}