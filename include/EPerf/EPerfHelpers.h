/*
 * EPerfHelpers.h
 *
 *  Created on: Apr 19, 2013
 *      Author: bzcdress
 */

#ifndef EPERFHELPERS_H_
#define EPERFHELPERS_H_

#ifdef DEBUG
    #include <iostream>
    #define \
        DMSG(m) \
        _Pragma("omp critical") \
        std::cerr << "EPerF: " << m << "\n";
#else
    #define DMSG(m)
#endif

#endif /* EPERFHELPERS_H_ */
