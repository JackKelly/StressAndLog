/*
 * Singleton.h
 *
 *  Created on: 24 Jun 2011
 *      Author: jack
 */

#ifndef SINGLETON_H_
#define SINGLETON_H_

#include <cassert>
#include <iostream>

template<class T>
class Singleton
{
public:
    static T* get_instance() {
        if ( instance == NULL ) {
            instance = new T;
        }
        assert( instance != NULL );
        return instance;
    }
protected:
    Singleton();
    ~Singleton();
private:
    Singleton(const Singleton&);
    Singleton& operator=(const Singleton&);
    static T * instance;
};

template<class T> T* Singleton<T>::instance=NULL;

#endif /* SINGLETON_H_ */
