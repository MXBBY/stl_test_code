#include "my_alloctor.h"
#include<vector>

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
 

template<typename _Tp>
class test{
    typedef simple_alloc<_Tp,lin_allocator::alloc> test_allocator;
public:
    _Tp* get_node(){
        return test_allocator::allocate(1);
    }
    void put_node(_Tp* p){
        test_allocator::deallocate(p);
    }
};

struct _list_node_base{
    _list_node_base* _M_next;
    _list_node_base* _M_prev;
};

template <class _Tp>
struct _list_node:public _list_node_base{
    _Tp _M_data;
};


template<typename _Tp>
struct _list_iterator{
    typedef _list_iterator<_Tp>  iterator;
    typedef _list_iterator<_Tp> self;

    typedef _Tp              value_type;
    typedef _Tp*            pointeer;
    typedef _Tp&           reference;
    typedef  size_t        size_type;
    typedef ptrdiff_t    difference_type;

    typedef _list_node<_Tp>* link_type;

    link_type node;

    _list_iterator(link_type x):node(x){}
    _list_iterator(){}
    _list_iterator(const iterator& x):node(x.node){}

    bool operator==(const self& x) const{
        return node==x.node;
    }
    bool operator!=(const self& x) const{
        return node!=x.node;
    }
    reference operator*() const{
        return (*node)._M_data;
    }    
    pointeer operator->() const{
        return &(operator*());
    }
    self& operator++(){
        node=(link_type)((*node)._M_next);
        return *this;
    }
    self operator++(int){
        self tmp=*this;
        ++*this;
        return tmp;
        }
        self& operator--(){
            node=(link_type)(*node)._M_prev;
            return *this;
        }
        self operator--(int){
            self tmp=*this;
            --*this;
            return tmp;
        }
};

int main(){
    typedef _list_node<int> node;
    test<node> t;
    node* p=t.get_node();
    cout<<p<<endl;
    t.put_node(p);
    return 1;
}