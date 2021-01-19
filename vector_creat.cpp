#include<iostream>
#include<memory>
#include "my_alloctor.h"

using namespace std;

namespace lin_vector{

template<typename _Tp,typename _Tp1>
void construct(_Tp* p,const _Tp1& x){
    cout<<"成功构造了:"<<x<<"  "<<p<<endl;
    new(p) _Tp(x);
}

template<typename _Tp>
void _destrory(_Tp* p){
   
    p->~_Tp();
}

template<typename f_iterator>
void destrory(f_iterator first,f_iterator last){ 
    cout<<"成功删除了："<<last-first<<"个元素，首地址为： "<<first<<endl;
    for(;first<last;first++){
        _destrory(&*first);
    }
}


template <typename _Tp,typename Alloc>
class simple_alloc{
    public:
        static _Tp *allocate(size_t n){
            return 0==n?0 : (_Tp*)Alloc::allocate(n*sizeof(_Tp));
        }
        static _Tp *allocate(void){
            return (_Tp*)Alloc::allocate(sizeof(_Tp));
        }
        static void deallocate(_Tp *p,size_t n){
            if(n!=0){
                Alloc::deallocate(p,n*sizeof(_Tp));
            }
        }
        static void deallocate(_Tp *p){
            Alloc::deallocate(p,sizeof(_Tp));
        }
};
 

    template<class T,class Alloc=lin_allocator::alloc>
    class vector{
        public:
            typedef T value_type;
            typedef value_type* iterator;
            typedef value_type& reference;
            typedef size_t size_type;

        protected:
            iterator start;
            iterator finish;
            iterator end_of_storage;
            typedef simple_alloc<value_type,Alloc> data_allocator;
        
        public:
            iterator allocate_and_fill(size_type n,const T& x){
                iterator result =data_allocator::allocate(n);
                uninitialize_fill_n(result,n,x);
                return result;
            }
            void fill_initialize(size_type n,const T& value){
                start=allocate_and_fill(n,value);
                finish=start+n;
                end_of_storage=finish;
            }

            vector():start(0),finish(0),end_of_storage(0){}
            vector(size_type n,const T& value){
                fill_initialize(n,value);
            }
            vector(int n,const T& value){
                fill_initialize(n,value);
            }
            vector(long n,const T& value){
                fill_initialize(n,value);
            }
            ~vector(){
                destrory(start,finish);
                deallocate();
            }

            void deallocate(){
                if(start){
                    data_allocator::deallocate(start,end_of_storage-start);
                }
            }
            iterator begin(){
                return start;
            }
            iterator end(){
                return finish;
            }
            size_type size(){
                return (end() - begin());
            }
            size_type capacity() const{
                return size_type(end_of_storage-start);
            }
            bool empty() const{
                return begin()==end();
            }
            reference operator[](size_type n){
                return *(begin()+n);
            }
            reference front(){
                return *begin();
            }
            reference back(){
                return *(end()-1);
            }

            void push_back(const T& x){
                if(finish!=end_of_storage){
                    construct(finish,x);
                    ++finish;
                    cout<<"成功插进了一个元素："<<x<<"  "<<finish<<endl;
                }
                else{
                    insert_aux(end(),x);
                }
            }

            void pop_back(){
                --finish;
                _destrory(finish);
            }
            iterator erase(iterator first,iterator last){
                iterator i=copy(last,finish,first);
                destrory(i,finish);
                finish=finish-(last-first);
                return first;
            }
            iterator erase(iterator position){
                if(position+1!=end()){
                    copy(position+1,finish,position);
                }
                --finish;
                destrory(finish);
                return position;
            }

            void clear(){
                erase(begin(),end());
            }

        void insert_aux(iterator position,const T& x){
            if(finish!=end_of_storage){
                construct(finish,*(finish-1));
                ++finish;
                T x_copy=x;
                copy_backward(position,finish-2,finish-1);
                *position=x_copy;
            }
            else{
                const size_type old_size=size();
                const size_type len=old_size!=0 ? 2*old_size:1;

                iterator new_start=data_allocator::allocate(len);
                iterator new_finish=new_start;
                try{
                    new_finish=uninitialized_copy(start,position,new_start);
                    construct(new_finish,x);
                    ++new_finish;
                    new_finish=uninitialized_copy(position,finish,new_finish);
                }
                catch(...){
                    destrory(new_start,finish);
                    data_allocator::deallocate(new_start,len);
                    throw;
                }
                destrory(begin(),end());
                data_allocator::deallocate(begin(),(end_of_storage-begin()));

                start=new_start;
                finish=new_finish;
                end_of_storage=new_start+len;
            }
        }
    };
}

int main(){
    lin_vector::vector<int> v;
    int x=10;
    for(int i=0;i<10;i++)
        v.push_back(x);
    cout<<v.capacity()<<endl;
    cout<<"目前容器元素数量为："<<v.size()<<endl;
    v.erase(v.begin(),v.begin()+5);
    cout<<"目前容器元素数量为："<<v.size()<<endl;

   // cout<<v.front()<<endl;
    return 1;
}