# Standard Model

## Run SARAH

~~~.sh
cd ~/hep-software/SARAH
math
~~~
Start Mathematica:
~~~.sh
math
~~~
Start SARAH and start the SM
~~~.sh
<< SARAH`
Start["SM"];
~~~
Show SARAH's main commands
~~~.m
SARAH`Commands
~~~
Inspect some model details:
~~~.m
SARAHparameters  (* model parameters *)
Particles[EWSB]  (* fields *)
MassMatrix[Fe]   (* mass matrix of lepton fields Fe *)
~~~

## Create SPheno spectrum generator

~~~.sh
cd ~/hep-software/SARAH
~~~
Create the Standard Model (`SM`) spectrum genrator, configure it and compile it:
~~~.sh
math -run '<< SARAH`; Start["SM"]; MakeSPheno[]; Quit[]'
~~~
Move the generated files to the SPheno directory:
~~~.sh
mv Output/SM/EWSB/SPheno ~/hep-software/SPheno-4.0.5/SM
~~~
Compile the SPheno spectrum generator:
~~~.sh
cd ~/hep-software/SPheno-4.0.5
make F90=gfortran
make F90=gfortran Model=SM
~~~
Run the spectrum generator:
~~~.sh
bin/SPhenoSM SM/Input_Files/LesHouches.in.SM
~~~
Inspect the output files:
~~~.sh
less SPheno.spc.SM
~~~

## Create FlexibleSUSY spectrum generator

~~~.sh
cd ~/hep-software/FlexibleSUSY-2.8.0
~~~
Create the Standard Model (`SM`) spectrum genrator, configure it and compile it:
~~~.sh
./createmodel -f --name=SM
./configure --with-models=SM --with-loop-libraries=looptools --with-looptools-incdir=$HOME/hep-software/LoopTools/x86_64-Linux/include/ --with-looptools-libdir=$HOME/hep-software/LoopTools/x86_64-Linux/lib64/
make -j4
~~~
Run the spectrum generator:
~~~.sh
./models/SM/run_SM.x --slha-input-file=models/SM/LesHouches.in.SM
~~~

# Standard Model + scalar singlet (SESM)

Let's create an extension of the SM by a new scalar singlet field that
mixes with the Higgs boson. We'll call it "Singlet Extended Standard
Model" (SESM). The Lagrangian should be:

$$\mathcal{L} = \mathcal{L}_{SM} - \left(K_1 H^\dagger H s + \frac{K_2}{2} H^\dagger H s^2 + \frac{M_s}{2} s^2 + \frac{\kappa}{3} s^3 + \frac{\lambda_s}{2} s^4\right)$$

1. We start from the SM and copy the SM model files:
   ~~~.sh
   cd ~/hep-software/SARAH
   mkdir -p Models/SESM
   cp Models/SM/parameters.m Models/SESM/
   cp Models/SM/particles.m Models/SESM/
   cp Models/SM/SM.m Models/SESM/SESM.m
   ~~~
1. Extend `Models/SESM/SESM/particles.m`:
   ~~~.m
   ParticleDefinitions[GaugeES] = {
     {Sing, { Description -> "Singlet",
              PDG -> {0},
              Width -> 0,
              Mass -> Automatic,
              ElectricCharge -> 0,
              FeynArtsNr -> 1,
              LaTeX -> "s",
              OutputName -> "s" }},
     ...
   };

   ParticleDefinitions[EWSB] = {
     {hh,  { Description -> "Higgs",
             PDG -> {25,35},                  (* <-- two PDG numbers *)
             PDG.IX -> {101000001,101000002}  (* <-- two PDG numbers *)
           }
     },
     ...
   };

   WeylFermionAndIndermediate = {
     {phiH, { PDG -> {0},       (* <-- name changed H -> phiH *)
              Width -> 0,
              Mass -> Automatic,
              LaTeX -> "H",
              OutputName -> "" }},
     ...
   };
   ~~~
1. Extend `Models/SESM/SESM/parameters.m`:
   ~~~.m
   ParameterDefinitions = {
     ...
     {ZH, { Description -> "Scalar-Mixing-Matrix" }},
     {\[Alpha], { Description -> "Scalar mixing angle" }},
     {vS, { Dependence -> None,
            DependenceNum -> None,
            DependenceOptional -> None,
            DependenceSPheno -> None,
            Real -> True,
            LesHouches -> {HMIX, 51},
            LaTeX -> "vS",
            OutputName -> vS}},
     {K1, { LaTeX -> "\\kappa_1",
            OutputName -> K1,
            LesHouches -> {HMIX,31} }},
     {K2, { LaTeX -> "\\kappa_2",
            OutputName -> K2,
            LesHouches -> {HMIX,32} }},
     {\[Kappa], { LaTeX -> "\\kappa",
                  OutputName -> Kap,
                  LesHouches -> {HMIX,35} }},               
     {LambdaS, { LaTeX -> "\\lambda_S",
                 OutputName -> LS,
                 LesHouches -> {HMIX,33} }},
     {MS, { LaTeX -> "M_S",
            OutputName -> MuS,
            LesHouches -> {HMIX,34} }}
   };
   ~~~
1. Extend `Models/SESM/SESM/SESM.m`:
   ~~~.m
   ScalarFields[[2]] = {s, 1, Sing, 0, 1, 1};
   RealScalars = {s};

   LagNoHC = -(
       + mu2 conj[H].H
       + \[Lambda]/2 conj[H].H.conj[H].H
       + K1 conj[H].H.s
       + K2/2 conj[H].H.s.s
       + MS/2 s.s
       + \[Kappa]/3 s.s.s
       + LambdaS/2 s.s.s.s
   );

   DEFINITION[EWSB][VEVs] = {
     {H0, {v, 1/Sqrt[2]}, {Ah, \[ImaginaryI]/Sqrt[2]}, {phiH, 1/Sqrt[2]}},
     {Sing, {vS, 1/Sqrt[2]}, {0, 0}, {phiS, 1}}
   };

   DEFINITION[EWSB][MatterSector] = {
     {{phiH, phiS}, {hh, ZH}},
     {{{dL}, {conj[dR]}}, {{DL,Vd}, {DR,Ud}}},
     {{{uL}, {conj[uR]}}, {{UL,Vu}, {UR,Uu}}},
     {{{eL}, {conj[eR]}}, {{EL,Ve}, {ER,Ue}}}
   };
   ~~~
1. Check the model:
   ~~~.m
   math -run '<< SARAH`; Start["SESM"]; CheckModel[]; Quit[]'
   ~~~

Now we turn to create a FlexibleSUSY model file:

1. We start from the SM and copy the SM model file:
   ~~~.sh
   cd ~/hep-software/FlexibleSUSY-2.8.0
   mkdir -p model_files/SESM
   cp model_files/SM/FlexibleSUSY.m.in model_files/SESM/
   cp model_files/SM/LesHouches.in.SM model_files/SESM/LesHouches.in.SESM
   ~~~
1. Now we edit the FlexibleSUSY model file `model_files/SESM/FlexibleSUSY.m.in`
   ~~~.m
   FSDefaultSARAHModel = SESM;
   [TODO]
   ~~~
1. Create, configure and copile the FlexibleSUSY spectrum generator:
   ~~~.sh
   ./createmodel -f --name=SESM
   ./configure --with-models=SESM --with-loop-libraries=looptools --with-looptools-incdir=$HOME/hep-software/LoopTools/build/ --with-looptools-libdir=$HOME/hep-software/LoopTools/build/
   make -j4
   models/SESM/run_SESM.x --slha-input-file=models/SESM/LesHouches.in.SESM_generated
   ~~~

We can continue to pass the SLHA output to micrOMEGAs. For this, we
need to first generate appropriate CalcHep model files with SARAH:
~~~.sh
cd ~/hep-software/SARAH
math -run '<< SARAH`; Start["SESM"]; MakeCHep[]; Quit[]'
~~~
Now we switch over to micromegas and copy our just generated SARAH model files over:
~~~.sh
cd ~/hep-software/micromegas_6.1.15
./newProject SESM
cd SESM
cp ~/hep-software/SARAH/Output/SESM/EWSB/CHep/* work/models/
~~~
We need to make some minor adaptations to the model files:
~~~.sh
model_files=`find . -name '*.mdl'`
sed -i~ 's/_dp/   /g' $model_files
~~~
Now we build the CalcHep model file and run the point
~~~.sh
make main=main.cpp
./main data.par
~~~
