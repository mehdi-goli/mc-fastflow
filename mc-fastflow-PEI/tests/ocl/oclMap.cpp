/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
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
/* Author: Massimo Torquati
 *         torquati@di.unipi.it  massimotor@gmail.com
 */


#if !defined(FF_OCL)
#define FF_OCL
#endif

#include <ff/map.hpp>
#include <ff/PEI/DSRIManagerNode.hpp>

using namespace ff;

FFMAPFUNC(mapf, float, elem, return (elem+1.0) );

template<typename T>
class oclTask: public baseTask {
public:
    typedef T base_type;

    oclTask():s(0) {}
    oclTask(base_type* t, size_t s):baseTask(t),s(s) {}
    size_t size() const     { return s;} 
    size_t bytesize() const { return s*sizeof(base_type); }    
protected:
    size_t s; 
};


int main(int argc, char * argv[]) {
    if (argc<2) {
        printf("use %s arraysize\n", argv[0]);
        return -1;
    }

    size_t size     =atoi(argv[1]);

    /* init data */
    float *M        = new float[size];
    for(size_t j=0;j<size;++j) M[j]=j;

    NEWMAP(oclmap, oclTask<float>, mapf, M, size);
    PROFILE(oclmap, true);
    oclmap->run_and_wait_end();
    
#if defined(CHECK)
    for(long i=0;i<size;++i)  printf("%.2f ", M[i]);
    printf("\n");
#endif

    printf("DONE\n");
    DELETEMAP(oclmap); // must be there if we want to use Profiler. otherewise the main automatcally does not call the 
    // destructor of a class created by macro.
    return 0;
}
