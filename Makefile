all: CImg.h a3.cpp Classifier.h NearestNeighbor.h Haar.h
	mkdir -p train_a train_b trainaverage trainextract traintest traintotal
	g++ a3.cpp -o a3 -lX11 -lpthread -I. -Isiftpp siftpp/sift.cpp
init:
	tar xvf overfeat.tgz
	./overfeat/download_weights.py

clean:
	rm a3
