all:
	cd server && $(MAKE)
	./server/server
	python3 client/client.py

clean:
	rm -f inpipe outpipe
	cd server && $(MAKE) clean

run:
	cd server && $(MAKE)
	./server/server
	python3 client/client.py
	# gnome-terminal -- ./server/server
	# gnome-terminal -- python3 ./client/client.py