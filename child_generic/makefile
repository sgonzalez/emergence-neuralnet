OBJS = main.cpp childhost.cpp
NAME = generic
CXX=clang++
FLAGS=-std=c++11 -stdlib=libc++

all: $(NAME)

generic: $(OBJS)
	$(CXX) $(FLAGS) $(OBJS) -o $(NAME)
	
clean:
	rm -rf $(NAME)