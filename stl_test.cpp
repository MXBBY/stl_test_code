#include <cstdio>
#include <cstdlib>
#include <iostream>

using std::cin;
using std::cout;
using std::string;

long get_a_target_long(){
    long target=0;

    cout << "target(o~" << RAND_MAX <<"):";
    cin >> target;
    return target;
}

string get_a_target_string(){
    long target=0;
    char buf[10];

    cout << "target(0~" << RAND_MAX <<"):";
    cin >> target;
    snprintf(buf,10,"%d",target);
    return string(buf); 
}

int compareLongs(const void* a, const void* b){
    return (*(long*)a-*(long*)b);
}

int compareStrings(const void* a,const void* b){
    if(*(string*)a > *(string*)b)    return 1;
    else if (*(string*)a < *(string*)b)    return -1;
    else    return 0;
}


#include <array>
#include <iostream>
#include <ctime>
#include <cstdlib>

namespace jj01{
    void test_array(){
        cout << "\n 以下是对stl中的array进行测试的内容\n";
    }

    array<long,ASIZE> c;
    
    clock_t timeStart=clock();
    

}