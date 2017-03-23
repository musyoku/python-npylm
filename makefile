CC = g++
CFLAGS = -I`python -c 'from distutils.sysconfig import *; print get_python_inc()'` -std=c++11 -L/usr/local/lib -lboost_serialization -lboost_python -lpython2.7 -O0 -g
CFLAGS_SO = -I`python -c 'from distutils.sysconfig import *; print get_python_inc()'` -shared -fPIC -std=c++11 -L/usr/local/lib -lboost_python -lpython2.7 -O2

install: ## NPYLMのビルド
	$(CC) model.cpp -o model.so $(CFLAGS_SO)

fix: ## Macで.soファイルを修正する
	sudo install_name_tool -change libboost_python.dylib /usr/local/lib/libboost_python.dylib model.so

test: ## LLDB用.
	$(CC) test.cpp $(CFLAGS)

.PHONY: help
help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'
.DEFAULT_GOAL := help