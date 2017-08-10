.PHONY: default
default: ubuntusourcesfs centossourcesfs

.PHONY: ubuntusourcesfs centossourcesfs

ubuntusourcesfs:
	docker build -t ubuntusourcesfs -f ubuntu.docker .

centossourcesfs:
	docker build -t centossourcesfs -f centos.docker .
