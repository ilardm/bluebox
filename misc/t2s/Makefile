# $^ - parametre
# $@ - goal

result	=	t2w
src	=	main.c
obj	=	main.o

all: debug

debug: $(src)
	gcc -g -O0 -c $(src)
	g++ -o $(result) -lsndfile $(obj)

release: $(src)
	gcc -O3 -c $(src)
	g++ -o $(result) -lsndfile $(obj)

clean:
	rm -f *.o
	rm -f $(result)
