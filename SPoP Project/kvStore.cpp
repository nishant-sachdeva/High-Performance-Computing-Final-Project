#include<bits/stdc++.h>
using namespace std;



class kvstore{
public:
	bool get(string key);
	bool put(string key, string value);
	bool del(string key);
	pair<string,string> get(int N);
	bool del(int N);
};


bool kvstore :: get(string key)
{
	//Your Code Here

	return false;
}

bool kvstore :: put(string key, string value){
	//Your Code Here
	return false;
}

bool kvstore :: del(string key){
	//Your Code Here
	return false;
}

pair<string,string> kvstore :: get(int N){
	// Your Code Here
	pair<string,string> temp = make_pair("key","value");
	return temp;
}

bool kvstore :: del(int N)
{
	// Your Code Here
	return true;
}