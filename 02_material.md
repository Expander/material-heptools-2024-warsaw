# Standard Model

## Run SARAH

~~~.sh
cd ~/hep-software/SARAH
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
Calculate vertices, beta functions and self-energies:
~~~.m
MakeAll[]
Quit[]
~~~
~~~.sh
less Output/SM/EWSB/Vertices/VertexListFFV.m
less Output/SM/RGEs/BetaGauge.m
less less Output/SM/EWSB/One-Loop/SelfEnergy.m
~~~

## Create a SPheno spectrum generator

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

## Create a FlexibleSUSY spectrum generator

~~~.sh
cd ~/hep-software/FlexibleSUSY-2.8.0
~~~
Create the Standard Model (`SM`) spectrum genrator, configure it and compile it:
~~~.sh
LT=$HOME/hep-software/LoopTools/build
./createmodel -f --name=SM
./configure --with-models=SM --with-loop-libraries=looptools --with-looptools-incdir=$LT --with-looptools-libdir=$LT
make -j4
~~~
Run the spectrum generator (set `FlexibleSUSY[31] = 2`):
~~~.sh
./models/SM/run_SM.x --slha-input-file=models/SM/LesHouches.in.SM
~~~

# Standard Model + scalar singlet (SESM)

Let's create an extension of the SM by a new scalar singlet field that
mixes with the Higgs boson. We'll call it "Singlet Extended Standard
Model" (SESM). The Lagrangian should be:

$$\mathcal{L} = \mathcal{L}_{SM} - \left[\kappa_{SH} H^\dagger H s + \frac{\lambda_{SH}}{2} H^\dagger H s^2 + \frac{M_S^2}{2} s^2 + \frac{\kappa_S}{3} s^3 + \frac{\lambda_S}{2} s^4\right]$$

## Create SARAH model

1. We start from the SM and copy the SM model files:
   ~~~.sh
   cd ~/hep-software/SARAH
   mkdir -p Models/SESM
   cp Models/SM/parameters.m Models/SESM/
   cp Models/SM/particles.m Models/SESM/
   cp Models/SM/SM.m Models/SESM/SESM.m
   ~~~
1. We modify `Models/SESM/SESM.m`:
   ~~~.m
   ScalarFields[[2]] = {s, 1, Sing, 0, 1, 1};
   RealScalars = {s};

   LagNoHC = -(
       + mu2 conj[H].H
       + \[Lambda]/2 conj[H].H.conj[H].H
       + KapSH conj[H].H.s
       + LamSH/2 conj[H].H.s.s
       + MS2/2 s.s
       + KapS/3 s.s.s
       + LamS/2 s.s.s.s
   );

   DEFINITION[EWSB][VEVs] = {
     {H0, {v, 1/Sqrt[2]}, {Ah, \[ImaginaryI]/Sqrt[2]}, {phiH, 1/Sqrt[2]}},
     {Sing, {vS, 1}, {0, 0}, {phiS, 1}}
   };

   DEFINITION[EWSB][MatterSector] = {
     {{phiH, phiS}, {hh, ZH}},
     {{{dL}, {conj[dR]}}, {{DL,Vd}, {DR,Ud}}},
     {{{uL}, {conj[uR]}}, {{UL,Vu}, {UR,Uu}}},
     {{{eL}, {conj[eR]}}, {{EL,Ve}, {ER,Ue}}}
   };
   ~~~
1. We modify `Models/SESM/particles.m`:
   ~~~.m
   ParticleDefinitions[GaugeES] = {
     {Sing, { Description -> "Singlet",
              PDG -> {0},
              Width -> 0,
              Mass -> Automatic,
              ElectricCharge -> 0,
              FeynArtsNr -> 3,
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
1. Extend `Models/SESM/parameters.m`:
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
     {MS2, { LaTeX -> "M_S^2",
             Real -> True,
             OutputName -> MS2,
             LesHouches -> {HMIX,30} }},
     {LamS, { LaTeX -> "\\lambda_S",
              Real -> True,
              OutputName -> LamS,
              LesHouches -> {HMIX,31} }},
     {LamSH, { LaTeX -> "\\lambda_{SH}",
               Real -> True,
               OutputName -> LamSH,
               LesHouches -> {HMIX,32} }},
     {KapS, { LaTeX -> "\\kappa_S",
              Real -> True,
              OutputName -> KapS,
              LesHouches -> {HMIX,33} }},
     {KapSH, { LaTeX -> "\\kappa_{SH}",
               Real -> True,
               OutputName -> KapSH,
               LesHouches -> {HMIX,34} }}
   };
   ~~~
1. Check the model:
   ~~~.m
   math -run '<< SARAH`; Start["SESM"]; CheckModel[]; Quit[]'
   ~~~

## Create FlexibleSUSY model

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

   (* SESM input parameters *)

   MINPAR = {};

   EXTPAR = {
       {0, Qin},
       {1, QEWSB},
       {2, LambdaIN},
       {3, LamSIN},
       {4, LamSHIN},
       {5, KapSIN},
       {6, KapSHIN},
       {7, vSIN}
   };

   EWSBOutputParameters = { mu2, MS2 };

   HighScaleInput = {
       {\[Lambda], LambdaIN},
       {LamS, LamSIN},
       {LamSH, LamSHIN},
       {KapS, KapSIN},
       {KapSH, KapSHIN}
   };

   SUSYScaleInput = {
       {vS, vSIN}
   };

   };
   ~~~
1. Create, configure and compile the FlexibleSUSY spectrum generator:
   ~~~.sh
   LT=$HOME/hep-software/LoopTools/build
   ./createmodel -f --name=SESM
   ./configure --with-models=SESM --with-loop-libraries=looptools --with-looptools-incdir=$LT --with-looptools-libdir=$LT
   make -j4
   ~~~
1. Modify the SESM SLHA input file `model_files/SESM/LesHouches.in.SESM`:
   ~~~.sh
   Block EXTPAR                 # Input parameters
       0   1000                 # input scale Qin
       1   173.34               # scale QEWSB
       2   0.21                 # LambdaIN
       3   0.1                  # LamSIN
       4   0.1                  # LamSHIN
       5   100                  # KapSIN
       6  -100                  # KapSHIN
       7   3                    # vSIN
   ~~~
1. Run the spectrum generator with the SLHA input file
   ~~~.sh
   models/SESM/run_SESM.x --slha-input-file=model_files/SESM/LesHouches.in.SESM
   ~~~

## Create SPheno model

Now we turn to create a SPheno model file:

1. Copy the model file `SPheno.m` from the SM:
   ~~~.sh
   cd ~/hep-software/SARAH
   cp Models/SM/SPheno.m Models/SESM/
   ~~~
1. Modify the `SPheno.m` model file to set the new parameters:
   ~~~.sh
   MINPAR = {
       {2, LambdaIN},
       {3, LambdaSIN},
       {4, LambdaSHIN},
       {5, KappaSIN},
       {6, KappaSHIN},
       {7, vevSIN}
   };

   ParametersToSolveTadpoles = {mu2, MS2};

   BoundaryLowScaleInput = {
       {\[Lambda], LambdaIN},
       {LambdaS, LambdaSIN},
       {LambdaSH, LambdaSHIN},
       {KapS, KappaSIN},
       {KapSH, KappaSHIN},
       {vS, vevSIN}
   };

   DefaultInputValues = {
       LambdaIN -> 0.21,
       LambdaSIN -> 0.1,
       LambdaSHIN -> 0.1,
       KappaSIN -> 100,
       KappaSHIN -> -100,
       KappaSHIN -> 3
   };
   ~~~
1. Create the SPheno spectrum generator:
   ~~~.sh
   math -run '<< SARAH`; Start["SESM"]; MakeSPheno[]; Quit[]'
   ~~~
1. Copy the generated FORTRAN files to the SPheno directory and compile the spectrum generator
   ~~~.sh
   cd ~/hep-software/SPheno-4.0.5
   mv ~/hep-software/SARAH/Output/SESM/EWSB/SPheno SESM
   make F90=gfortran
   make F90=gfortran Model=SESM
   ~~~
1. Modify the SLHA input file `SESM/Input_Files/LesHouches.in.SESM`
   ~~~.sh
   Block MINPAR   # Input parameters 
    2   0.21      # LambdaIN
    3   0.1       # LamSIN
    4   0.1       # LamSHIN
    5   100       # KapSIN
    6  -100       # KapSHIN
    7   3         # vSIN
   ~~~
1. Run the SPheno spectrum generator:
   ~~~.sh
   ./bin/SPhenoSSM SESM/Input_Files/LesHouches.in.SESM
   ~~~

# Standard Model + two scalar singlets (TSESM)

Let's create an extension of the SM by a new scalar singlet field that
mixes with the Higgs boson. We'll call it "Singlet Extended Standard
Model" (TSESM). The Lagrangian should be:

$$\mathcal{L} = \mathcal{L}_{SM} - \left[\frac{\lambda_{SH}}{2} H^\dagger H s^2 + \frac{M_S^2}{2} s^2 + \frac{\lambda_S}{2} s^4\right]$$

## Create SARAH model

1. We start from the SM and copy the SM model files:
   ~~~.sh
   cd ~/hep-software/SARAH
   mkdir -p Models/TSESM
   cp Models/SM/parameters.m Models/TSESM/
   cp Models/SM/particles.m Models/TSESM/
   cp Models/SM/SM.m Models/TSESM/TSESM.m
   ~~~
1. We modify `Models/TSESM/TSESM/TSESM.m`:
   ~~~.m
   Global[[1]] = {Z[2], Z2};

   (* Gauge Groups *)

   Gauge[[1]]={B,   U[1], hypercharge, g1, False, 1};
   Gauge[[2]]={WB, SU[2], left,        g2, True , 1};
   Gauge[[3]]={G,  SU[3], color,       g3, False, 1};


   (* Matter Fields *)

   FermionFields[[1]] = {q, 3, {uL, dL},     1/6, 2,  3, 1};
   FermionFields[[2]] = {l, 3, {vL, eL},    -1/2, 2,  1, 1};
   FermionFields[[3]] = {d, 3, conj[dR],     1/3, 1, -3, 1};
   FermionFields[[4]] = {u, 3, conj[uR],    -2/3, 1, -3, 1};
   FermionFields[[5]] = {e, 3, conj[eR],       1, 1,  1, 1};

   ScalarFields[[1]] =  {H, 1, {Hp, H0},     1/2, 2,  1, 1};
   ScalarFields[[2]] = {s, 1, Sing, 0, 1, 1, -1};

   RealScalars = {s};

   LagNoHC = -(
       + mu2 conj[H].H
       + \[Lambda]/2 conj[H].H.conj[H].H
       + LamSH/2 conj[H].H.s.s
       + MS2/2 s.s
       + LamS/2 s.s.s.s
   );

   DEFINITION[EWSB][VEVs] = {
     {H0, {v, 1/Sqrt[2]}, {Ah, \[ImaginaryI]/Sqrt[2]}, {phiH, 1/Sqrt[2]}},
     {Sing, {vS, 1}, {0, 0}, {phiS, 1}}
   };

   DEFINITION[EWSB][MatterSector] = {
     {{phiH, phiS}, {hh, ZH}},
     {{{dL}, {conj[dR]}}, {{DL,Vd}, {DR,Ud}}},
     {{{uL}, {conj[uR]}}, {{UL,Vu}, {UR,Uu}}},
     {{{eL}, {conj[eR]}}, {{EL,Ve}, {ER,Ue}}}
   };
   ~~~
1. We modify `Models/TSESM/TSESM/particles.m`:
   ~~~.m
   ParticleDefinitions[GaugeES] = {
     {Sing, { Description -> "Singlet",
              PDG -> {0},
              Width -> 0,
              Mass -> Automatic,
              ElectricCharge -> 0,
              FeynArtsNr -> 3,
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
1. Extend `Models/TSESM/TSESM/parameters.m`:
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
     {LamSH, { LaTeX -> "\\kappa_1",
               Real -> True,
               OutputName -> LamSH,
               LesHouches -> {HMIX,31} }},
     {LamS, { LaTeX -> "\\lambda_S",
              Real -> True,
              OutputName -> LamS,
              LesHouches -> {HMIX,33} }},
     {MS2, { LaTeX -> "M_S^2",
             Real -> True,
             OutputName -> MS2,
             LesHouches -> {HMIX,34} }}
   };
   ~~~
1. Check the model:
   ~~~.m
   math -run '<< SARAH`; Start["TSESM"]; CheckModel[]; Quit[]'
   ~~~

## FlexibleSUSY model file

Now we turn to create a FlexibleSUSY model file:

1. We start from the SM and copy the SM model file:
   ~~~.sh
   cd ~/hep-software/FlexibleSUSY-2.8.0
   mkdir -p model_files/TSESM
   cp model_files/SM/FlexibleSUSY.m.in model_files/TSESM/
   cp model_files/SM/LesHouches.in.SM model_files/TSESM/LesHouches.in.TSESM
   ~~~
1. Now we edit the FlexibleSUSY model file `model_files/TSESM/FlexibleSUSY.m.in`
   ~~~.m
   FSDefaultSARAHModel = TSESM;
   [TODO]
   ~~~
1. Create, configure and copile the FlexibleSUSY spectrum generator:
   ~~~.sh
   LT=$HOME/hep-software/LoopTools/build
   ./createmodel -f --name=TSESM
   ./configure --with-models=TSESM --with-loop-libraries=looptools --with-looptools-incdir=$LT --with-looptools-libdir=$LT
   make -j4
   models/TSESM/run_TSESM.x --slha-input-file=models/TSESM/LesHouches.in.TSESM_generated
   ~~~

## micrOMEGAs

We can continue to pass the SLHA output to micrOMEGAs. For this, we
need to first generate appropriate CalcHep model files with SARAH:
~~~.sh
cd ~/hep-software/SARAH
math -run '<< SARAH`; Start["TSESM"]; MakeCHep[DMcandidate1 -> Z2 == -1, IncludeEffectiveHiggsVertices -> False]; Quit[]'
~~~
Now we switch over to micromegas and copy our just generated SARAH model files over:
~~~.sh
cd ~/hep-software/micromegas_6.1.15
./newProject TSESM
cd TSESM
cp ~/hep-software/SARAH/Output/TSESM/EWSB/CHep/* work/models/
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
