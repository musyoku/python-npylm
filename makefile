CC = g++
CFLAGS_SO = -I`python -c 'from distutils.sysconfig import *; print get_python_inc()'` -shared -fPIC -std=c++11 -L/usr/local/lib -lboost_python -lpython2.7 -O2

model:
	$(CC) model.cpp -o model.so $(CFLAGS_SO)

install:
	sudo install_name_tool -change libboost_python.dylib /usr/local/lib/libboost_python.dylib model.so