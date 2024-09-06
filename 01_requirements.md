# Requirements

Table of contents

- [Requirements](#requirements)
   * [Wolfram Mathematica / Wolfram Engine](#wolfram-mathematica-wolfram-engine)
   * [Compilers and libraries](#compilers-and-libraries)
   * [HEP software](#hep-software)
      + [SARAH 4.15.2](#sarah-4152)
      + [SPheno](#spheno)
      + [FlexibleSUSY 2.8.0](#flexiblesusy-280)
      + [FeynArts and FormCalc](#feynarts-and-formcalc)
      + [micrOMEGAs](#micromegas)

During the workshop we will build and use HEP software.  It would be
best to bring a laptop with a UNIX-based operating system, like Linux
(recommended) or MacOS. Ubuntu or Debian would be the best choice,
because all software compiles and runs smoothly on these operating
systems.

## Wolfram Mathematica / Wolfram Engine

In the workshop we will need Wolfram Mathematica or Wolfram Engine.
If you do not have any of these, please install Wolfram Engine as
follows:

1. Get a free licence for WolframEngine here:
   https://www.wolfram.com/engine/free-license/
2. Download and install WolframEngine 14.1 from:
   https://www.wolfram.com/engine/
3. Ensure the Wolfram Language Kernel can be called from the command line by running `math`:
   ~~~.sh
   $PATH=$PATH:/usr/local/Wolfram/WolframEngine/14.1/Executables/
   ~~~
4. Test it:
   ~~~.sh
   math -run "Print[Hello World]; Quit[]"
   ~~~

## Compilers and libraries

During the workshop we will also need:

* C++ compiler
* FORTRAN compiler
* BOOST library [https://www.boost.org/]
* Eigen3 library [https://eigen.tuxfamily.org/index.php?title=Main_Page]
* GNU Scientific Library [https://www.gnu.org/software/gsl/]

Please install all of these.

Installation on Ubuntu:
~~~.sh
sudo apt install gcc g++ gfortran libboost-dev libeigen3-dev libgsl-dev dpkg-dev
~~~
Installation on MacOS:
~~~.sh
brew install gcc boost eigen gsl
~~~

## HEP software

### SARAH 4.15.2

We will make heavy use of SARAH [https://sarah.hepforge.org/]. Before
the workshop starts, SARAH should be installed:
~~~.sh
mkdir -p ~/hep-software && cd ~/hep-software
wget https://sarah.hepforge.org/downloads/SARAH-4.15.2.tar.gz
tar -xf SARAH-4.15.2.tar.gz
ln -s SARAH-4.15.2 SARAH
echo 'AppendTo[$Path, FileNameJoin[{$HomeDirectory, "hep-software", "SARAH"}]];' >> ~/.WolframEngine/Kernel/init.m
~~~
Afterwards, please test the SARAH installation:
~~~.sh
math -run '<< SARAH`; Quit[]'
~~~

### SPheno

We will also use SPheno [https://spheno.hepforge.org/]. Please
download and extract it before the workshop starts:

~~~.sh
mkdir -p ~/hep-software && cd ~/hep-software
wget https://spheno.hepforge.org/downloads/SPheno-4.0.5.tar.gz
tar -xf SPheno-4.0.5.tar.gz
~~~

### FlexibleSUSY 2.8.0

We will also use FlexibleSUSY [https://flexiblesusy.hepforge.org/]. Please
download and extract it before the workshop starts:
~~~.sh
mkdir -p ~/hep-software && cd ~/hep-software
wget https://flexiblesusy.hepforge.org/downloads/FlexibleSUSY-2.8.0.tar.gz
tar -xf FlexibleSUSY-2.8.0.tar.gz
~~~

### FeynArts and FormCalc

For the calculation of decays with
[FlexibleDecay](https://arxiv.org/pdf/2106.05038) we'll need FeynArts,
FormCalc and LoopTools [https://feynarts.de/]. They can be most
convenienty installed as follows (hit `y` everytime you get asked to
install a package):
~~~.sh
mkdir -p ~/hep-software && cd ~/hep-software
wget https://feynarts.de/FeynInstall
chmod 755 FeynInstall
./FeynInstall
echo 'AppendTo[$Path, FileNameJoin[{$HomeDirectory, "hep-software", "FeynArts"}]];' >> ~/.WolframEngine/Kernel/init.m
echo 'AppendTo[$Path, FileNameJoin[{$HomeDirectory, "hep-software", "FormCalc"}]];' >> ~/.WolframEngine/Kernel/init.m
~~~
Afterwards, please test the FeynArts/FormCalc installations:
~~~.sh
math -run '<< FeynArts`; << FormCalc`; Quit[]'
~~~
Finally, we have to re-compile LoopTools with `-fPIC`:
~~~.sh
cd ~/hep-software/LoopTools
make clean
FFLAGS="-O3 -fPIC" ./configure
make
~~~

### micrOMEGAs

We will make use of micrOMEGAs [https://lapth.cnrs.fr/micromegas/].
Please download and compile it as follows:
~~~.sh
wget https://zenodo.org/records/13376690/files/micromegas_6.1.15.tgz
tar -xzf micromegas_6.1.15.tgz
cd micromegas_6.1.15
make
~~~
