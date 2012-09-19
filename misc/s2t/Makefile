# $^ - parametre
# $@ - goal

result	=	s2t
src	=	sndfile-to-text.c
obj	=	sndfile-to-text.o

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
