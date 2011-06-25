/*
 * Singleton.h
 *
 *  Created on: 24 Jun 2011
 *      Author: jack
 *
 * Adapted from: http://www.yolinux.com/TUTORIALS/C++Singleton.html
 *
 * Known limitations:
 * This code isn't thread-safe.  This shouldn't be a problem for us because
 * we're not running mutiple threads.  We do use signals but that shouldn't
 * be a problem because we instantiate each singleton long before the signals
 * will ever fire.
 * For more info, see: http://www.aristeia.com/Papers/DDJ_Jul_Aug_2004_revised.pdf
 *
 * The singleton design pattern is pretty controversial. See:
 * http://stackoverflow.com/questions/86582/singleton-how-should-it-be-used
 * Their advice, applicable to my situation, seems to be to make a global object like std::cout
 *
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
