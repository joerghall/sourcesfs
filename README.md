# Sources Filesystem

Provides a virtual file system for mapping source files in debuggers such as GDB to source repositories

Currently supported providers:

* Git
* Perforce
* Local file system

## Mac

### Install perquisites 
install FuseMacOSX - https://osxfuse.github.io
brew install boost

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

See [`docker/ubuntu.docker`][docker-file] for example Docker image for building `sourcesfs`.

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

### Example configuration file

SourcesFS is configured using a `.sourcesfs` file in your home directory. Here's an example:

```json
{
  "gitlab.com": {
    "type": "git",
    "url": "git@gitlab.com:${group}/${name}.git"
  },
  "perforce-server": {
    "type": "p4",
    "url": "perforce-server:1667"
  },
  "cache": {
    "type": "cache",
    "url": "/path/to/local/directory"
  }
}
```

[docker-file]: docker/ubuntu.docker
