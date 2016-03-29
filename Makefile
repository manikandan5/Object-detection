all: CImg.h a3.cpp Classifier.h NearestNeighbor.h Haar.h
	mkdir -p train_a train_b trainaverage trainextract traintest traintotal n
	module load opencv; \
	g++ a3.cpp  -o a3 -lX11 -lpthread `pkg-config opencv --cflags` `pkg-config opencv --libs` -I. -O3 -Isiftpp siftpp/sift.cpp
init:
	tar xvf overfeat.tgz
	./overfeat/download_weights.py

clean:
	rm a3
