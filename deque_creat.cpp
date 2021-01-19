#include<iostream>
#include <memory>
#include "my_alloctor.h"

using namespace std;

namespace lin_dqeue{

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

    inline size_t _deque_buf_size(size_t n,size_t sz){
        return n!=0 ? n:(sz<20 ? size_t(20/sz):size_t(1));
    }

    template<class T,class Ref,class Ptr,size_t BufSiz>
    struct _deque_iterator{
        typedef _deque_iterator<T, T&, T*,BufSiz> iterator;
        typedef _deque_iterator<T, const T&, const T*, BufSiz> const_iterator;
        static size_t buffer_size(){
            return _deque_buf_size(BufSiz,sizeof(T));
        }

        typedef T value_type;
        typedef Ptr pointer;
        typedef Ref reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef T** map_pointer;

        typedef _deque_iterator self;

        T* cur;
        T* first;
        T* last;
        map_pointer node;

        void set_node(map_pointer new_node){
            node=new_node;
            first=*new_node;
            last=first+difference_type(buffer_size());
        }

        reference operator*() const{
            return *cur;
        }
        pointer operator->() const{
            return &(operator*());
        }
        difference_type operator-(const self& x) const{
            return difference_type(buffer_size())*(node-x.node-1)+(cur-first)+(x.last-x.cur);
        } 
        self& operator++(){
            ++cur;
            if(cur==last){
                set_node(node+1);
                cur=first;
            }
            return *this;
        }
        self operator++(int ){
            self tmp=*this;
            ++*this;
            return tmp;
        }
        self& operator--(){
            if(cur==first){
                set_node(node-1);
                cur=last;
            }
            --cur;
            return *this;
        }
        self operator--(int){
            self tmp=*this;
            --*this;
            return tmp;
        }
        self& operator+=(difference_type n){
            difference_type offset=n+(cur-first);
            if(offset>=0 && offset<difference_type(buffer_size( ))){
                cur+=n;
            }
            else{
                difference_type node_offset=offset>0 ? offset/difference_type(buffer_size()):-difference_type((-offset-1)/buffer_size())-1;
                set_node(node+node_offset);
                cur=first+(offset-node_offset*difference_type(buffer_size()));
            }
            return *this;
        }
        self operator+(difference_type n) const{
            self tmp=*this;
            return tmp+=n;
        }
        self& operator-=(difference_type n) const{
            return *this+=-n;
        }
        self operator-(difference_type n) const{
            self tmp=*this;
            return this-=n;
        }
        reference operator[](difference_type n) const{
            return *(*this+n);
        }
        bool operator==(const self& x){
            return cur==x.cur;
        }
        bool operator!=(const self& x){
            return !(*this==x);
        }
        bool operator<(const self& x) const{
            return (node==x.node) ? (cur<x.cur):(node<x.noce);
        }
    };

    template<typename T,typename Alloc=lin_allocator::alloc,size_t BufSiz=0>
    class deque{
        public:
            typedef T  value_type;
            typedef value_type* pointer;
            typedef T& reference;
            typedef ptrdiff_t difference_type;
            typedef size_t size_type;
            typedef _deque_iterator<T, T&, T*,BufSiz> iterator;
            typedef simple_alloc<value_type,Alloc> data_allocator;
            typedef simple_alloc<pointer,Alloc> map_allocator;

        protected:
            typedef pointer* map_pointer;
            map_pointer map;
            
            size_type map_size;

        public:
            iterator start;
            iterator finish;
            deque():start(),finish(),map(0),map_size(0){};
            deque(int n,const value_type& value):start(),finish(),map(0),map_size(0) {
                fill_initialize(n,value);
            }
            static size_t buffer_size(){
            return _deque_buf_size(BufSiz,sizeof(T));
        }
            iterator begin(){
                return start;
            }
            iterator end(){
                return finish();
            }
            reference operator[](size_type n){
                return start[difference_type(n)];
            }
            reference front(){
                return *start;
            }

            void fill_initialize(size_type n,const value_type& value){
                creat_map_and_nodes(n);
                cout<<"buffer_size()是："<<buffer_size()<<endl;
                map_pointer cur;
                try{
                    for(cur=start.node; cur<finish.node; ++cur){
                        uninitialized_fill(*cur,*cur+buffer_size(),value);
                    }
                    uninitialized_fill(finish.first,finish.cur,value);
                }
                catch(...){
                }
            }
            //用于结构体，未分配内存。
            void fill_initialize(size_type n){
                creat_map_and_nodes(n);
                cout<<"buffer_size()是："<<buffer_size()<<endl;
                map_pointer cur;
            }


            void creat_map_and_nodes(size_type num_elements){
                size_type num_nodes=num_elements/ buffer_size()+1;

                if(num_nodes+2>=8) map_size=num_nodes+2;
                else map_size=8;
                cout<<"map_size的值是："<<map_size<<endl;
                map=map_allocator::allocate(map_size);

                map_pointer nstart=map+(map_size-num_nodes)/2;
                map_pointer nfinish=nstart+num_nodes-1;

                map_pointer cur;
                try{
                    for(cur=nstart;cur<=nfinish;++cur){
                        *cur=allocate_node();
                    }
                }
                    catch(...){
                    }
                    start.set_node(nstart);
                    finish.set_node(nfinish);
                    start.cur=start.first;
                    finish.cur=finish.first+num_elements%buffer_size();
            }
            pointer allocate_node(){
                return data_allocator::allocate(buffer_size());
            }

            void push_front(const value_type& t){
                if(start.cur!=start.first){
                    construct(start.cur-1,t);
                    --start.cur();
                }
                else{
                    push_front_aux(t);
                }
            }

            void push_front_aux(const value_type& t){
                value_type t_copy=t;
                reserve_map_at_front();
                *(start.node-1)=allocate_node();
                try{
                    start.set_node(start.node-1);
                    start.cur=start.last-1;
                    construct(start.cur,t_copy);
                }
                catch(...){}
            }

            void reserve_map_at_front(size_type nodes_to_add=1){
                if(nodes_to_add>start.node-map){
                    reallocate_map(nodes_to_add,true);
                }
            }

            void reallocate_map(size_type nodes_to_add,bool add_at_front){
                size_type old_num_nodes=finish.node-start.node+1;
                size_type new_num_nodes=old_num_nodes+nodes_to_add;

                map_pointer new_nstart;

                if(map_size>2*new_num_nodes){
                    new_nstart=map+(map_size-new_num_nodes)/2+(add_at_front ? nodes_to_add:0);
                    if(new_nstart<start.node){
                        copy(start.node,finish.node+1,new_nstart);
                    }
                }
            }

            void push_back(const value_type& t){
                if(finish.cur!=finish.last-1){
                    construct(finish.cur,t);
                    ++finish.cur;
                }
                else{
                    push_back_aux(t);
                }
            }

            void push_back_aux(const value_type& t){
                value_type t_copy=t;
                reserve_map_at_back();
                *(finish.node+1)=allocate_node();
                try{
                    construct(finish.cur,t_copy);
                    finish.set_node(finish.node+1);
                    finish.cur=finish.first;
                }
                catch(...){}
            }
    };
}

struct test{
    int i;
    char s;
};

int main(){
    lin_dqeue::deque<test> d;
    cout<<sizeof(test)<<endl;
    d.fill_initialize(39);
    //cout<<d.front()<<endl;
    return 1;
}