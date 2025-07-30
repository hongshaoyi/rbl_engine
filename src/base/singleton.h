#ifndef __SINGLETON_H__
#define __SINGLETON_H__

template <typename T>
class Singleton {
    Singleton() {};
    ~Singleton() {};

    Singleton(Singleton &s) = delete;
    Singleton& operator=(Singleton &s) = delete;
    Singleton(Singleton &&s) = delete;
    Singleton& operator=(Singleton &&s) = delete;

public:
    static T& get_instance() {
        static T instance;

        return instance;
    }
};

#endif