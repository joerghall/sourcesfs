# Sources Filesystem

Provides a virtual file system for mapping source files in debuggers such as GDB to source repositories

Currently supported providers:

* Git
* Perforce
* Local file system

## macOS

### Install prerequisites

* Install [FuseMacOSX][fuse-macos]
* Install Boost: `brew install boost`

### Build

### Run

```bash
mkdir ~/sourcesfs
bin/sourcesfs -f -d ~/sourcesfs
```

### Cleanup

Use `umount` to unmount the virtual file system if you kill the Fuse user space process before it can clean up properly:

```bash
umount ~/sourcesfs
```

## Linux

### Build

See [`docker/ubuntu.docker`][docker-file] for example Docker image for building `sourcesfs`.

```bash
mkdir debug
cd debug
cmake -DCMAKE_CC_COMPILER=gcc-6 -DCMAKE_CXX_COMPILER=g++-6 -DCMAKE_BUILD_TYPE=DEBUG -G "Unix Makefiles" ..
```

### Run

```bash
mkdir ~/sourcesfs
bin/sourcesfs -f -d ~/sourcesfs
```

## Windows

TBD

## Example configuration file

SourcesFS is configured using a `.sourcesfs` file in your home directory. Here's an example:

```json
{
  "github": {
    "type": "git",
    "args": ["user", "project"],
    "url": "git@github.com:${user}/${project}.git"
  },
  "perforce-server": {
    "type": "p4",
    "args": [],
    "url": "perforce-server:1667"
  },
  "cache": {
    "type": "cache",
    "args": [],
    "url": "/path/to/local/directory"
  }
}
```

## OSX brew

```bash
# Install tap
brew tap joerghall/homebrew-repo
# Install sourcesfs - this is work  in progress
brew install Sourcesfs
```

[docker-file]: docker/ubuntu.docker
[fuse-macos]: https://osxfuse.github.io
