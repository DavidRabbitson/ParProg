all: time train guide my_functions series ssum dsum

my_functions: my_functions.c
	mpicc my_functions.c -o my_functions

time: time.c
	mpicc time.c -o time

train: train.c
	mpicc train.c -o train

guide: guide.c
	mpicc guide.c -o guide

series: series.c
	mpicc series.c -o series

ssum: looong_ssum.c
	mpicc looong_ssum.c -o ssum

dsum: looong_dsum.c
	mpicc looong_dsum.c -o dsum

clean:
	rm train guide time my_functions series ssum dsum
