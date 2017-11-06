QMAKE = /home/tl/qt-arm-4.8.3/bin/qmake

all:
	make -C dsp all
	cd host && ${QMAKE} && make

clean:
	make -C dsp clean
	cd host && ${QMAKE} && make clean && rm Makefile
