#include "list_creat.cpp"
//#include <cstdio>
#include <iostream>
//#include <cstdlib>

int main(){
    list<int> c;

    for(int i=0;i<10;i++){
        c.push_back (i);
    }
    for(int i=0;i<10;i++){
        std::cout << c.front()<<endl;
        c.pop_front();
    }
    return 1;
}