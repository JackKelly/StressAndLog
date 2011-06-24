/*
 * Singleton.h
 *
 *  Created on: 24 Jun 2011
 *      Author: jack
 */

#ifndef SINGLETON_H_
#define SINGLETON_H_

class Singleton {
public:
    virtual ~Singleton();
protected:
    Singleton();
    Singleton(const Singleton&);
    Singleton & operator=(const Singleton&);
};

#endif /* SINGLETON_H_ */
