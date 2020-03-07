all: train guide

train: train.c
	mpicc train.c -o train

guide: guide.c
	mpicc guide.c -o guide

clean:
	rm train guide
