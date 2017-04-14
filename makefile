CC = g++
INCLUDE = -I`python -c 'from distutils.sysconfig import *; print get_python_inc()'`
BOOST = -lboost_python -lpython2.7 -lboost_serialization
CFLAGS = -std=c++11 -L/usr/local/lib -O3
CFLAGS_SO = -shared -fPIC -std=c++11 -L/usr/local/lib -O3 

install: ## NPYLMのビルド
	$(CC) model.cpp -o model.so $(INCLUDE) $(CFLAGS_SO) $(BOOST)

test: ## LLDB用
	$(CC) tools/test.cpp $(CFLAGS) $(INCLUDE) $(BOOST)

.PHONY: help
help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'
.DEFAULT_GOAL := help