/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */

/*!
 *  \link
 *  \file ff_fib.cpp 
 *  \ingroup application_level
 *
 *  \brief This file contains the implementation of fibonacci program written
 *  in FastFlow with skeletal programming patterns.
 *
 *  It computes the n-th Fibonacci sequence number using a 
 *  simple stream parallel approach
 */

/* ***************************************************************************
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as 
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *  As a special exception, you may use this file as part of a free software
 *  library without restriction.  Specifically, if other files instantiate
 *  templates or use macros or inline functions from this file, or you compile
 *  this file and link it with other files to produce an executable, this
 *  file does not by itself cause the resulting executable to be covered by
 *  the GNU General Public License.  This exception does not however
 *  invalidate any other reasons why the executable file might be covered by
 *  the GNU General Public License.
 *
 ****************************************************************************
 */

//#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <stdint.h>
#include <ff/farm.hpp>
#include <ff/PEI/DSRIManagerNode.hpp>

using namespace ff;

inline uint64_t fib(uint64_t n) {
    if (n<2) return n;    
    return fib(n-1)+fib(n-2);
}

/*!
 *  \class Worker
 *  \ingroup application_level
 *
 *  \breif It defines the workers of the form.
 *
 *  This class is defined in \ref ff_fib.cpp
 */

class Worker: public ff_node {
public:
    void * svc(void * task) {
        return (void*)fib((uint64_t)task);
    }
};

/*!
 *  \class Emitter
 *  \ingroup application_level
 *
 *  \breif It defines the emitter of the form.
 *
 *  This class is defined in \ref ff_fib.cpp
 */

class Emitter: public ff_node {
private:
    inline void generate_stream(int k) {
    if (k<=b) {
        ff_send_out((void*)k);
        ++streamlen;
        return;
    }
    generate_stream(k-1);
    generate_stream(k-2);
    }
public:
    Emitter(int n, int b):n(n),b(b),streamlen(0),result(0) {};

    void * svc(void * task) {
    if (task==NULL) generate_stream(n);
    else {
        result +=(uint64_t)task;
        if (--streamlen == 0) return NULL;
    }
    return GO_ON;
    }

    uint64_t get_result() const { return result; }
private:
    int n;
    int b;
    int streamlen;
    uint64_t result;
};

void usage(char * name) {
    std::cerr << "usage: \n";
    std::cerr << "      " << name << " n [n-stop n-workers]\n";
}

int main(int argc, char * argv[]) {
    bool check=false;
    int n,b=20; 
#ifdef HAVE_SYSCONF_NPROCESSORS
    int nworkers = sysconf(_SC_NPROCESSORS_ONLN);
#else
    int nworkers = 2;
#endif

    if (argc>=3) {
        n = atoi(argv[1]); b = atoi(argv[2]);
        if (argc==4)  nworkers = atoi(argv[3]);
        if (argc==5)  check=true;
        if (b>=n) {
            std::cerr << "ERROR: n-stop should be less than n\n";
            usage(argv[0]);
            return -1;
        }
    } else if (argc == 2) {
        n = atoi(argv[1]);
    } else {
        usage(argv[0]);
        return -1;
    }
    
    if (n<=30) {
        std::cout << "**SEQUENTIAL** fib(" << n << ")= " << fib(n) << "\n";
        return 0;
    }

    /* The mapping policy is very simple: the emitter is mapped along on CPU 0, 
     * the workers on the remaining cores.
     */
    
    // the worker_mapping array, should be dinamically built
    //const char worker_mapping[] = "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15, 17,18,19,20,21,22,23,24,25,26,27,28,29,30,31";
      //const char worker_mapping[] = "0,1,2,3,0,1,2,3";

    //threadMapper::instance()->setMappingList(worker_mapping);  
    
    ff_farm<> farm;    
    Emitter E(n, b);
//    E.setAffinity(0); // the emitter is statically mapped on core 0

    farm.add_emitter(&E);
    std::vector<ff_node *> w;
    for(int i=0;i<nworkers;++i) w.push_back(new Worker);
    farm.add_workers(w);
    farm.wrap_around();
    PROFILE(&farm);
    if (farm.run_and_wait_end()<0) {
        error("running farm\n");
        return -1;
    }

    std::cout << "fib(" << n << ")= " << E.get_result() << "\n";
    printf("Time: %g (ms)\n", farm.ffTime());
    printf("n. of workers used: %d\n", nworkers);
    printf("n-stop value used is: %d\n", b);

    if (check && n>30) {
        if (fib(n) != E.get_result()) {
            std::cerr << "Wrong result\n";
            return -1;
        }
        std::cout << "Ok\n";
    }

    return 0;
}

/*!
 *  @}
 *  \endlink
 */
