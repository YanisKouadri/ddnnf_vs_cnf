µ-toksia : SAT-based Solver for Argumentation Frameworks
========================================================

Version    : ICCMA'23

Author     : [Andreas Niskanen](mailto:andreas.niskanen@helsinki.fi), University of Helsinki

Compiling
---------

SAT solvers [Glucose](https://www.labri.fr/perso/lsimon/research/glucose/) (version 4.1) and [CryptoMiniSat](https://github.com/msoos/cryptominisat) (version 5.11.4) are included in this release. To compile µ-toksia with Glucose, run `SAT_SOLVER=glucose make` in this directory. Similarly, to compile with CryptoMiniSat, run `SAT_SOLVER=cryptominisat make` in this directory. The binary file will be built as `./build/release/bin/mu-toksia`. To remove all object files, issue `make clean`.

Command-line usage
------------------

```
./build/release/bin/mu-toksia -p <task> -f <file> [-a <query>] [-fo <format>]

  <task>      computational problem
  <file>      input argumentation framework
  <format>    file format for input AF
  <query>     query argument

Options:
  --help      Displays this help message.
  --version   Prints version and author information.
  --formats   Prints available file formats.
  --problems  Prints available computational problems.
```

For a description of possible tasks, see the [ICCMA'23 subtracks](https://iccma2023.github.io/tracks.html#main).

As of ICCMA'23, the input file format flag `-fo` is optional. Without specifying this flag, µ-toksia assumes the [ICCMA'23 format](https://iccma2023.github.io/rules.html#input-format) for the input AF file. (Note that µ-toksia still supports the APX and TGF formats; for a description of these formats, see e.g. Section 4 [here](http://argumentationcompetition.org/2021/SolverRequirements.pdf).)

API usage
---------

µ-toksia also supports [IPAFAIR](https://bitbucket.org/coreo-group/ipafair), an incremental API for AF solvers.

```
$ python3
>>> from python.mu_toksia import mu_toksia as AFSolver
>>> s = AFSolver("CO")
>>> s.add_argument(2)
>>> s.add_argument(3)
>>> s.add_attack(2,3)
>>> s.solve_cred([3])
False
>>> s.add_argument(1)
>>> s.add_attack(1,2)
>>> s.solve_cred([3])
True
>>> s.extract_witness()
[1, 3]
```

Older versions
--------------

For the ICCMA'19 version (which supports the dynamic track of ICCMA'19), see the [dynamic](https://bitbucket.org/andreasniskanen/mu-toksia/src/dynamic) branch of this repository. For the ICCMA'21 version, please see the commit tagged with [iccma21](https://bitbucket.org/andreasniskanen/mu-toksia/commits/tag/iccma21).

Citation
--------

Please cite the following paper if you use `µ-toksia` in a publication:

```
@inproceedings{DBLP:conf/kr/NiskanenJ20a,
  author    = {Andreas Niskanen and
               Matti J{\"{a}}rvisalo},
  editor    = {Diego Calvanese and
               Esra Erdem and
               Michael Thielscher},
  title     = {{\(\mathrm{\mu}\)}-toksia: An Efficient Abstract Argumentation Reasoner},
  booktitle = {Proceedings of the 17th International Conference on Principles of
               Knowledge Representation and Reasoning, {KR} 2020, Rhodes, Greece,
               September 12-18, 2020},
  pages     = {800--804},
  year      = {2020},
  url       = {https://doi.org/10.24963/kr.2020/82},
  doi       = {10.24963/kr.2020/82},
  timestamp = {Fri, 09 Apr 2021 18:52:15 +0200},
  biburl    = {https://dblp.org/rec/conf/kr/NiskanenJ20a.bib},
  bibsource = {dblp computer science bibliography, https://dblp.org}
}
```

Contact
-------

Please direct any questions, comments, bug reports etc. directly to [the author](mailto:andreas.niskanen@helsinki.fi).
