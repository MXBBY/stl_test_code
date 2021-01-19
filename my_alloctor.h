#include <iostream>

using namespace std;

namespace lin_allocator{
    #define _THROW_BAD_ALLOC cerr<<"out of memory"<<endl;exit(1)

    template<int inst>
    class _malloc_alloc_template{
        private:
            static void* oom_malloc(size_t);
            static void* oom_realloc(void*,size_t);
            static void (*_malloc_alloc_oom_headler)();

        public:
            static void* allocate(size_t n){
                void* result=malloc(n);
                if(0==result){
                    result=oom_malloc(n);
                }
                return result;
            }
            static void deallocate(void* p,size_t){
                free(p);
            }
            static void* reallocate(void* p,size_t,size_t new_sz){
                void* result=realloc(p,new_sz);
                if(0==result){
                    result=oom_realloc(p,new_sz);
                }
                return result;
            }
            static void (*set_malloc_headler(void(*f)()))(){
                void(*old)()=_malloc_alloc_oom_headler;
                _malloc_alloc_oom_headler=f;
                return(old);
            }
    };

    template<int inst>
    void (*_malloc_alloc_template<inst>::_malloc_alloc_oom_headler )()=0;

    template<int inst>
    void* _malloc_alloc_template<inst>::oom_malloc (size_t n){
        void (*my_malloc_handler)();
        void* result;

        for(;;){
            my_malloc_handler=_malloc_alloc_oom_headler;
            if(0==my_malloc_handler){
                _THROW_BAD_ALLOC;
            }
            (*my_malloc_handler)();
            result=malloc(n);
            if(result){
                return(result);
            }
        }
    }

    template<int inst>
    void *_malloc_alloc_template<inst>::oom_realloc(void *p,size_t n){
        void (*my_malloc_headler)();
        void* result;

        for(;;){
            my_malloc_headler=_malloc_alloc_oom_headler;
            if(0==my_malloc_headler){
                _THROW_BAD_ALLOC;
            }
            (*my_malloc_headler)();
            result=realloc(p,n);
            if(result){
                return(result);
            }
        }
    }

    typedef _malloc_alloc_template<0> malloc_alloc;

    template<class T,class Alloc>
    class simple_alloc{
        public:
            static T*allocate(size_t n){
                return 0==n ? 0 : (T*)Alloc::allocate(n*sizeof(T));
            }
            static T* allocate(void){
                return (T*)Alloc::allocate(sizeof(T));
            }
            static void deallocate(T* p,size_t n){
                if(0!=n){
                    Alloc::deallocate(p,n*sizeof(T));
                }
            }
            static void deallocate(T* p){
                Alloc::deallocate(p,sizeof(T));
            }
    };

    enum{_ALIGN=8};
    enum{_MAX_BYTES=128};
    enum{_NFREELISTS=_MAX_BYTES/_ALIGN};

    template<bool threads,int inst>
    class _default_alloc_template{
        private:
            static size_t _ROUND_UP(size_t bytes){
                return (((bytes)+_ALIGN-1)&~(_ALIGN-1));
            }
            private:
                union obj{
                    union obj* free_list_link;
                };
            private:
                static obj* free_list[_NFREELISTS];
                static size_t FREELIST_INDEX(size_t bytes){
                    return (((bytes)+_ALIGN-1)/_ALIGN-1);
                }

                static void *refill(size_t n);

                static char *chunk_alloc(size_t size,int &nobjs);

                static char* start_free;
                static char* end_free;
                static size_t heap_size;

            public:
                static void *allocate(size_t n){
                    obj* *my_free_list;
                    obj* result;

                    if(n>(size_t)_MAX_BYTES){
                        cout<<"内存太大，调用全局的allocate分配内存。"<<endl;
                        return (malloc_alloc::allocate(n));
                    }

                    my_free_list=free_list+FREELIST_INDEX(n);
                    result=*my_free_list;
                    if(0==result){
                        void* r=refill(_ROUND_UP(n));
                        cout<<"成功分配了内存，大小为："<<n<<endl;
                        return r;
                    }
                    *my_free_list=result->free_list_link;
                    cout<<"成功分配了内存，大小为："<<n<<endl;
                    return (result);       
                }

                static void deallocate(void *p,size_t n){
                    obj* q=(obj*)p;
                    obj* *my_free_list;

                    if(n>(size_t)_MAX_BYTES){
                        malloc_alloc::deallocate(p,n);
                        cout<<"成功调用全局deallocate回收内存。"<<endl;
                        return;
                    }
                    cout<<"成功回收了大小为："<<n<<" 的内存p:"<<p<<endl;
                    my_free_list=free_list+FREELIST_INDEX(n);
                    q->free_list_link=*my_free_list;
                    *my_free_list=q;
                }

                static void *reallocate(void *p,size_t old_sz,size_t new_sz);
    };

    template<bool threads,int inst>
    char* 
    _default_alloc_template<threads,inst>::
    chunk_alloc(size_t size,int& nobjs){
        char* result;
        size_t total_bytes=size*nobjs;
        size_t bytes_left=end_free-start_free;

        if(bytes_left>=total_bytes){
            result=start_free;
            start_free+=total_bytes;
            cout<<"成功分配了："<<nobjs<<"块大小为："<<size<<"的内存。"<<endl;            
            cout<<"分配的内存首地址为："<<(obj*)result<<endl;
            return (result);
        }
        else if(bytes_left>=size){
            nobjs=bytes_left/size;
            total_bytes=size*nobjs;
            result=start_free;
            start_free+=total_bytes;
             cout<<"成功分配了："<<nobjs<<" 块大小为："<<size<<" 的内存。"<<endl;
             cout<<"分配的内存首地址为："<<(obj*)result<<endl;
            return (result);
        }
        else{
            size_t bytes_to_get=2*total_bytes+_ROUND_UP(heap_size>>4);
            if(bytes_left>0){
                obj* *my_free_list=free_list+FREELIST_INDEX(bytes_left);
                cout<<"对剩余内存碎片进行处理。"<<endl;
                ((obj*)start_free)->free_list_link=*my_free_list;
                *my_free_list=(obj*)start_free;
            }
            start_free=(char*)malloc(bytes_to_get);
            if(0==start_free){
                cout<<"系统没有内存了，接下来将从链表中回收。"<<endl;
                int i;
                obj* *my_free_list;
                obj *p;

                for(i=size;i<_MAX_BYTES;i+=_ALIGN){
                    my_free_list=free_list+FREELIST_INDEX(i);
                    p=*my_free_list;
                    if(0!=p){
                        *my_free_list=p->free_list_link;
                        start_free=(char*)p;
                        end_free=start_free+i;
                        return(chunk_alloc(size,nobjs));
                    }
                }
                end_free=0;
                start_free=(char*)malloc_alloc::allocate(bytes_to_get);
            }
            cout<<"成功用malloc为内存池分配了："<<bytes_to_get<< " 块内存。"<<endl;
            cout<<"为内存池分配的首地址为："<<(obj*)start_free<<endl;
            heap_size+=bytes_to_get;
            end_free=start_free+bytes_to_get;
            return(chunk_alloc(size,nobjs));
        }
    }

    template<bool threads,int inst>
    void* _default_alloc_template<threads,inst>::
    refill(size_t n){
        int nobjs=20;
        char* chunk=chunk_alloc(n,nobjs);
        obj* *my_free_list;
        obj* result;
        obj* current_obj;
        obj* next_obj;
        int i;

        if(1==nobjs){
            return(chunk);
        }
        my_free_list=free_list+FREELIST_INDEX(n);
        result=(obj*)chunk;
        *my_free_list=next_obj=(obj*)(chunk+n);
        for(i=1;;++i){
            current_obj=next_obj;
            next_obj=(obj*)((char*)next_obj+n);
            if(nobjs-1==i){
                current_obj->free_list_link=0;
                break;
            }
            else{
                current_obj->free_list_link=next_obj;
            }
        }
        return(result);
    }

    template<bool threads,int inst>
    char* _default_alloc_template<threads,inst>::start_free=0;

    template<bool threads,int inst>
    char* _default_alloc_template<threads,inst>::end_free=0;

    template<bool threads,int inst>
    size_t _default_alloc_template<threads,inst>::heap_size=0;

    template<bool threads,int inst>
    typename _default_alloc_template<threads,inst>::obj*
    _default_alloc_template<threads,inst>::free_list [_NFREELISTS]
    = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };

    typedef _default_alloc_template<false,0> alloc;
/*
    void test_alloc(){
        cout<<"\ntest_global_allocator_with_16_freelist().....\n";

        void* p1=alloc::allocate(120);
        void* p2=alloc::allocate(72);
        void* p3=alloc::allocate(60);

        cout<<p1<<"    "<<p2<<"    "<<p3<<"    "<<endl;

        alloc::deallocate(p1,120);
        alloc::deallocate(p2,72);
        alloc::deallocate(p3,60);

        cout<<endl;
    }*/
}