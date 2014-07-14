OBJS = main.cpp host.cpp
NAME = coordinator
CXX=clang++
FLAGS=-std=c++11 -stdlib=libc++

all: $(NAME)

coordinator: $(OBJS)
	$(CXX) $(FLAGS) $(OBJS) -o $(NAME)
	
clean:
	rm -rf $(NAME)