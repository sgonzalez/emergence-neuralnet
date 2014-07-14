OBJS = main.cpp neuralhost.cpp neuralnet.cpp genetic.cpp
NAME = feedforward
CXX=clang++
FLAGS=-std=c++11 -stdlib=libc++

all: $(NAME)

feedforward: $(OBJS)
	$(CXX) $(FLAGS) $(OBJS) -o $(NAME)
	
clean:
	rm -rf $(NAME)