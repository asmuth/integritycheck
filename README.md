filecheck
=========

**filecheck** is a simple command-line tool for verifying and maintaining file
integrity. It computes checksums for files on disk and compares them against a list
of expected values stored in an *index file*.

An example index file looks like this:

```
> cat index.lst
file1.bin 1205 sha1:19b162f03f28273c383e5b834ec037518d751a05
file2.bin 1205 sha1:19b162f03f28273c383e5b834ec037518d751a05
```

When verifying an an index file, filecheck confirms not only that the files
listed in the index are intact, but also that the index itself is complete -
ensuring that every file in the data directory is accounted for. This helps
detect not only modified or missing files, but also unexpected additions.

In addition to verifying these index files, filecheck can create new ones,
update existing entries, and search for specific files within them.

filecheck is completely read-only; it never touches the actual files in your
data directory. It also doesn't store any copies of your data, but only the index
containing checksums and metadata. This means that it can detect corrupt or
missing files, but cannot restore them.

This design allows it to play nicely with your existing backup or version control
setup and scales easily to very large collections, such as photo archives or asset
libraries.


Usage
-----

The filecheck distribution consists of a single command-line program called
`filecheck`.

```
Usage: $ filecheck [OPTION...]
   -i, --index=<path>            Index file path
   -d, --directory=<path>        Data directory path (default: '.')
   -c, --check                   Check the integrity of files referenced by the index file (default)
   -u, --update                  Update the index file
   -s, --search                  Search in the index file
   -?, --help                    Display this help text and exit
   -V, --version                 Display the version of this program and exit

Output format:
   -o, --output=<format>         Output format (tty or text)
   -p, --progress                Enable progress output to STDERR
   -P, --noprogress              Disable progress output to STDERR

Options for the 'check' mode:
   -q, --quick                   Disable checksum verification, only verify file presence and size
```



Getting started
---------------

FIXME


Verification
------------

FIXME


Update
------

FIXME


Search
------

FIXME


Non-exhaustive Mode
-------------------

FIXME


Partial Verification and Updates
--------------------------------

FIXME


Index File Format
-----------------

FIXME



Build & Installation
--------------------

Before you can compile filecheck you need to install some build dependencies.
Currently you need a modern c++ compiler, cmake and openssl 3.

```
# Ubuntu
$ apt install clang cmake libssl-dev


# OSX
$ brew install cmake openssl@3
```

To build and install `filecheck`, check out this repo and run:

    $ cmake -B build && make -C build install


Test Suite
----------

If you have modified the source code, run the test suite:

    $ make -C build test

You can also run static analysis and test coverage checks using the following command:

    $ cmake -B build -DTEST_CPPCHECK=ON -DTEST_COVERAGE=ON -DCMAKE_BUILD_TYPE=DEBUG
    $ make -C build test


License
-------

    Copyright 2018 Paul Asmuth <paul@asmuth.com>

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
