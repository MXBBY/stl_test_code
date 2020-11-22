#include <cstdio>
#include <cstdlib>
#include <iostream>

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

template<typename T1,typename T2>
inline void construct(T1* p,const T2& value){
    new(p) T1(value);
}

template <typename T>
inline void destrory(T* pointer){
    pointer->~T();
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

template <typename _Tp,typename _Alloc=std::allocator<_Tp>>
class list{
    protected:
        typedef _list_node<_Tp> list_node;
        typedef simple_alloc<list_node,_Alloc> list_node_allocator;

    public:
        typedef list_node* link_type;
        typedef _list_iterator<_Tp> iterator;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        typedef _Tp value_type;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;

    protected:
        link_type node;

    public:
        iterator begin(){
            return (link_type)((*node)._M_next);
        }
        iterator end(){
            return node;
        }
        bool empty() const{
            return node->_M_next==node;
        }
        size_type size() const{
            size_type result=0;
            distance(begin(),end(),result);
            return result;
        }
        reference front(){
            return *begin();
        }
        reference back(){
            return *(--end());
        }
        protected:
            link_type get_node(){
                return list_node_allocator::allocate(sizeof(_Tp));
            }
            void put_node(link_type p){
                 list_node_allocator::deallocate(p);
            }
            link_type create_node(const_reference _x){
                link_type _p=get_node();
                construct(&_p->_M_data,_x);
                return _p;
            }
            void destroty_node(link_type _p){
                destrory(&_p->_M_data);
                put_node(_p);
            }
            void empty_initialize(){
                node=get_node();
                node->_M_next=node;
                node->_M_prev=node;
            }

            iterator insert(iterator position,const_reference x){
                link_type tmp=create_node(x);
                tmp->_M_next=position.node;
                tmp->_M_prev=position.node->_M_prev;
                position.node->_M_prev->_M_next=tmp;
                position.node->_M_prev=tmp;
                return tmp;
            }
    public:
            void push_back(const_reference x){
                insert(end(),x);
            }
            void push_front(const_reference x){
                insert(begin(),x);
            }
            iterator erase(iterator position){
                link_type next_node=link_type(position.node->_M_next);
                link_type prev_node=link_type(position.node->_M_prev);
                prev_node->_M_next=next_node;
                next_node->_M_prev=prev_node;
                destroty_node(position.node);
                return iterator(next_node);
            }
            void pop_front(){
                erase(begin());
            }
            void pop_back(){
                iterator tmp=end();
                erase(--tmp);
            }
            void transfer(iterator positon,iterator first,iterator last);
            void clear();
            void remove(const_reference value);
            void unique();
};

template <typename T,typename Alloc>
void list<T,Alloc>:: clear(){
    link_type cur=(link_type)node->_M_next;
    while(cur!=node){
        link_type tmp=cur;
        cur=(link_type)cur->_M_next;
        destroty_node(tmp);   
    }
    node->_M_next=node;
    node->_M_prev=node;
}

template <typename T,typename Alloc>
void list<T,Alloc>::remove(const_reference value){
    iterator first=begin();
    iterator last=end();
    while(first!=last){
        iterator next=first;
        ++next;
        if(*first==value) erease(first);
        first=next;
    }
}

template <typename T,typename Alloc>
void list<T,Alloc>::unique(){
    iterator first=begin();
    iterator last=end();
    if(first==last) return ;
    iterator next=first;
    while(++next!=last){
        if(*first==*next) erase(next);
        else first=next;
        next=first;
    }
}

template <typename T,typename Alloc>
void list<T,Alloc>::transfer(iterator position,iterator first,iterator last){
    if(position!=last){
        (*(link_type((*last.node)._M_prev)))._M_next=position.node;
        (*(link_type((*first.node)._M_prev)))._M_next=last.node;
        (*(link_type((*position.node).prev)))._M_next=first.node;
        link_type tmp=link_type((*position.node)._M_prev);
        (*position.node)._M_prev=(*last.node)._M_prev;
        (*last.node)._M_prev=(*first.node)._M_prev;
        (*first.node)._M_prev=tmp;
    }
}


#include <iostream>

int main(){
    list<int> c;

    for(int i=0;i<10;i++){
        c.push_back (i);
    }
    for(int i=0;i<10;i++){
        std::cout << c.front()<<std::endl;
        c.pop_front();
    }
    return 1;
}


