# ---------------------------------------------------------------------------
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License version 2 as 
#  published by the Free Software Foundation.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#  As a special exception, you may use this file as part of a free software
#  library without restriction.  Specifically, if other files instantiate
#  templates or use macros or inline functions from this file, or you compile
#  this file and link it with other files to produce an executable, this
#  file does not by itself cause the resulting executable to be covered by
#  the GNU General Public License.  This exception does not however
#  invalidate any other reasons why the executable file might be covered by
#  the GNU General Public License.
#
# ---------------------------------------------------------------------------
CXX      	     = g++ 
CXXFLAGS 	     =
FF_ROOT  	     = ../../.. # add your own fastflow root
ZMQ_ROOT	     =/users/mehdi/local # add your zmq library root

#Non-standard OpenCL Location
OCL_ROOT 	     = /opt/gridware/libs/amd-opencl/2.8 # add your own openCL root
OCLFLAGS 	     = -DFASTFLOW_OPENCL
OPTFLAGS 	     = -g -finline-functions
LDFLAGS  	     = -L$(ZMQ_ROOT)/lib/ -L$(OCL_ROOT)/lib/x86_64
LIBS     	     = -lzmq -lOpenCL -lpthread -ldl
INCLUDES 	     = -I$(FF_ROOT) -I$(OCL_ROOT)/include -I$(ZMQ_ROOT)/include
TRACEFASTFLOW 	     = -DTRACE_FASTFLOW -DZMQ_DSRI
PEI		     = -DPROFILER -DFF_ESAVER
TARGET               = simple-convolution/ff_conv sobel-filter/ff_sob

.PHONY: all clean cleanall distclean
.SUFFIXES: .c .cpp .o


all: $(TARGET)

simple-convolution/ff_conv: simple-convolution/conv.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OCLFLAGS) $(OPTFLAGS) $(TRACEFASTFLOW) $(LDFLAGS) $< -o $@ $(LIBS) $(PEI)
sobel-filter/ff_sob: sobel-filter/sob.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(OCLFLAGS) $(OPTFLAGS) $(TRACEFASTFLOW) $(LDFLAGS) $< -o $@ $(LIBS) $(PEI)

clean: 
	-rm -fr *.o *~

cleanall: clean
	-rm -rf $(TARGET) *.d 
