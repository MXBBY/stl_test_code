#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

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

#define ASIZE 500000

namespace jj01{
    void test_array(){
        cout << "\n 以下是对stl中的array进行测试的内容\n";

        array<long,ASIZE> c;
    
        clock_t timeStart=clock();
        for(long i=0;i<ASIZE;i++){
            c[i]=rand();
        }
        cout << "milli-seconds:" << (clock()-timeStart) << endl;
        cout << "array.size():" << c.size() << endl;
        cout << "array.front():" << c.front() << endl;
        cout << "array.back():" << c.back() << endl;
        cout << "array.data():" << c.data() << endl;

        long target=get_a_target_long();

        timeStart=clock();
        qsort(c.data(),ASIZE,sizeof(long),compareLongs);
        long* pItem=(long*)bsearch(&target,(c.data()),ASIZE,sizeof(long),compareLongs);
        cout << "qsort()+bsearch(),milli-seconds:" << (clock()-timeStart) <<endl;
        if(pItem!=NULL)    cout<< "fond:"<< *pItem <<endl;
        else    cout << "Not fond."<<endl;
    }
}

#include <vector>
#include <stdexcept>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <ctime>
#include <algorithm>

namespace jj02{
    void test_vector(){
        cout << "\n以下是对array中的vector进行测试的内容。\n";

        vector <string> c;
        char buf[10];

        clock_t timestart=clock();
        for(long i=0;i<ASIZE;i++){
            try{
               snprintf(buf,10,"%d",rand());
               c.push_back(string(buf));
            }
            catch(exception& p){
                cout << "i="<< i << p.what() <<endl;
                abort();
            }
        }
           cout <<"millli-second:" << (clock()-timestart) <<endl;
           cout <<"vector.size():" << c.size() << endl;
           cout <<"vector.front():" << c.front() <<endl;
           cout <<"vector.back():" << c.back() <<endl;
           cout << "vector.capacity():" << c.capacity()<<endl;
            
            string target=get_a_target_string();
            {
                timestart=clock();
                auto pItem=::find(c.begin(),c.end(),target);
                cout <<"::find(),milli-second:"<<(clock()-timestart) <<endl;

                if(pItem!=c.end()){
                    cout << "found,"<< *pItem<<endl;
                }
                else{
                    cout << "not found!" <<endl;
                }
            }
            {
                timestart=clock();
                sort(c.begin(),c.end());
                string* pItem=(string*)bsearch(&target,(c.data()),c.size(),sizeof(string),compareStrings);
                cout << "sort()+bsratch(),milli-second():" << (clock()-timestart)<<endl;

                if(pItem!=NULL){
                    cout<< "found"<<*pItem<<endl;
                }
                else{
                    cout<<"not found!"<<endl;
                }
            }
    }
}

#include <list>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <cstdio>
#include <stdexcept>

namespace jj03{
    void test_list(){
        cout << "\n以下是对stl中list的测试！\n";

        list <string> c;
        char buf[10];

        clock_t timestart=clock();
        for(int i=0;i<ASIZE;i++){
            try{
                snprintf(buf,10,"%d",rand());
                c.push_back(string(buf));
            }
            catch(exception& p){
                cout << "i=" <<i <<" "<<p.what() <<endl;
                abort();
            }
        }
        cout << "milli-second:"<<(clock()-timestart)<<endl;
        cout << "list.size():" << c.size()<<endl;
        cout << "list.front():" <<c.front()<<endl;
        cout << "list.back():" <<c.back()<<endl;
        cout<< "list.max_size():" <<c.max_size()<<endl;

        string target=get_a_target_string();
        timestart=clock();
        auto pItem=::find(c.begin(),c.end(),target);
        cout<<"::find(),milli-second():"<<(clock()-timestart)<<endl;

        if(pItem!=c.end()){
            cout<< "found,"<<*pItem <<endl;
        }
        else{
            cout<<"not found!"<<endl;
        }

        timestart=clock();
        c.sort();
        cout <<"sort(),milli-second():"<<(clock()-timestart)<<endl;
    }
}

#include <forward_list>

namespace jj04{
    void test_forward_list(){
        cout<<"以下是对stl中的foraward_list进场测试的内容。";

        forward_list<string> c;
        char buf[10];

        clock_t timestart=clock();
        for(int i=0;i<ASIZE;i++){
            try{
                snprintf(buf,10,"%d",rand());
                c.push_front(string(buf));
            }
            catch(exception& p){
                cout <<"i="<<i<<" "<<p.what()<<endl;
                abort();
            }
        }
        cout<<"milli-second:"<<(clock()-timestart)<<endl;
        cout<<"forward-list.max_sieze():"<<c.max_size()<<endl;
        cout<<"forward-list.front():"<<c.front()<<endl;

        string target=get_a_target_string();
        timestart=clock();
        auto pItem=::find(c.begin(),c.end(),target);
        cout<<"::find(),milli-second:"<<(clock()-timestart)<<endl;

        if(pItem!=c.end()){
            cout<<"found,"<<*pItem<<endl;
        }
        else{
            cout<<"not found!"<<endl;
        }

        timestart=clock();
        c.sort();
        cout<<"milli-second:"<<(clock()-timestart)<<endl;
    }
}
/*
int main(){
    jj04::test_forward_list();
    return 0;
}
*/