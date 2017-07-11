# Sources Filesystem

## Mac

### Install perquisites 
install FuseMacOSX - https://osxfuse.github.io

### Build

### Run
```bash
mkdir ~/sourcesfs
bin/sourcesfs -f -d ~/sourcesfs
```
### Cleanup 
umount if you killed the fuse user space process

```bash
umount ~/sourcesfs
```

## Linux

### Build

See [`buildtools/ubuntu.docker`][docker-file] for example Docker image for building `sourcesfs`.

```
mkdir debug
cd debug
cmake -DCMAKE_CC_COMPILER=gcc-6 -DCMAKE_CXX_COMPILER=g++-6 -DCMAKE_BUILD_TYPE=DEBUG -G "Unix Makefiles" ..
```

### Run

```
mkdir ~/sourcesfs
bin/sourcesfs -f -d ~/sourcesfs
```

## Windows

<tbd>

[docker-file]: buildtools/ubuntu.docker
