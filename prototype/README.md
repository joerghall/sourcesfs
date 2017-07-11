# Prototype

sourcefs <mount point> --workdir <path> --config <path>

- Mapping of alias to git repository
- On request
    split path into alias / commit hash / path
    clone the repository and checkout commit hash
    serve the file for the path

# Git get files for a given commit hash
```bash
# Clone repo and checkout commit hash
git clone https://github.com/joerghall/sourcesfs.git sourcesfs-19fc775a34edbd2f560c9f7002299f728796ad5b
cd sourcesfs-19fc775a34edbd2f560c9f7002299f728796ad5b
git checkout 19fc775a34edbd2f560c9f7002299f728796ad5b
```

```bash
# Does not work
# from https://stackoverflow.com/questions/3489173/how-to-clone-git-repository-with-specific-revision-changeset

# make a new blank repository in the current directory
mkdir sourcesfs-19fc775a34edbd2f560c9f7002299f728796ad5b
cd sourcesfs-19fc775a34edbd2f560c9f7002299f728796ad5b

# add a remote
# git remote add origin git@github.com:joerghall/sourcesfs.git
git remote add origin https://github.com/joerghall/sourcesfs.git 

# fetch a commit (or branch or tag) of interest
# Note: the full history of this commit will be retrieved
git fetch origin 19fc775a34edbd2f560c9f7002299f728796ad5b..19fc775a34edbd2f560c9f7002299f728796ad5b

# reset this repository's master branch to the commit of interest
git reset --hard FETCH_HEAD

```
