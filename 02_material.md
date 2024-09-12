Table of contents

- [Standard Model](#standard-model)
   * [Run SARAH](#run-sarah)
   * [Create a SPheno spectrum generator](#create-a-spheno-spectrum-generator)
   * [Create a FlexibleSUSY spectrum generator](#create-a-flexiblesusy-spectrum-generator)
- [Standard Model + real scalar gauge singlet (SESM)](#standard-model--real-scalar-gauge-singlet-sesm)
   * [Create SARAH model](#create-sarah-model)
   * [Create a FlexibleSUSY spectrum generator](#create-a-flexiblesusy-spectrum-generator-1)
   * [Create a SPheno spectrum generator](#create-a-spheno-spectrum-generator-1)
- [Standard Model + two scalar singlets (TSESM)](#standard-model--two-scalar-singlets-tsesm)
   * [Create SARAH model](#create-sarah-model-1)
   * [FlexibleSUSY model file](#flexiblesusy-model-file)
   * [micrOMEGAs](#micromegas)
   * [SModelS](#smodels)

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
Inspect some output:
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
Move the generated files to the SPheno directory and
compile the SPheno spectrum generator:
~~~.sh
cd ~/hep-software/SPheno-4.0.5
mv ~/hep-software/SARAH/Output/SM/EWSB/SPheno SM
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

Go to the FlexibleSUSY directory:
~~~.sh
cd ~/hep-software/FlexibleSUSY-2.8.0
~~~
Create the Standard Model (`SM`) spectrum genrator, configure it and compile it:
~~~.sh
LT=$HOME/hep-software/LoopTools/build
./createmodel -f --name=SM
./configure --with-models=SM --with-loop-libraries=looptools --with-looptools-incdir=${LT} --with-looptools-libdir=${LT}
make -j4
~~~
Run the spectrum generator (set `FlexibleSUSY[31] = 2`):
~~~.sh
./models/SM/run_SM.x --slha-input-file=models/SM/LesHouches.in.SM
~~~

# Standard Model + real scalar gauge singlet (SESM)

Let's create an extension of the SM by a new real scalar gauge singlet
field that mixes with the Higgs boson. We'll call it "Singlet Extended
Standard Model" (SESM). The Lagrangian should be:

$$\mathcal{L} = \mathcal{L}_{SM} - \left[\kappa_{SH} H^\dagger H s + \frac{\lambda_{SH}}{2} H^\dagger H s^2 + \frac{M_S^2}{2} s^2 + \frac{\kappa_S}{3} s^3 + \frac{\lambda_S}{2} s^4\right]$$

## Create SARAH model

1. We start from the SM and copy the SM model files to a new `SESM` directory:
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

## Create a FlexibleSUSY spectrum generator

Now we will create a FlexibleSUSY model file:

1. We start from the SM and copy the SM model file to a new `SESM` directory:
   ~~~.sh
   cd ~/hep-software/FlexibleSUSY-2.8.0
   mkdir -p model_files/SESM
   cp model_files/SM/FlexibleSUSY.m.in model_files/SESM/
   cp model_files/SM/LesHouches.in.SM model_files/SESM/LesHouches.in.SESM
   ~~~
1. Now we edit the FlexibleSUSY model file `model_files/SESM/FlexibleSUSY.m.in` to define new input parameters and fix the new model parameters:
   ~~~.m
   FSDefaultSARAHModel = SESM;

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

   (* Remove these: *)
   (* UseSM3LoopRGEs = True; *)
   (* UseSM4LoopRGEs = True; *)
   (* UseSM5LoopRGEs = True; *)
   (* UseHiggs2LoopSM = True; *)
   (* UseHiggs3LoopSM = True; *)
   (* UseHiggs4LoopSM = True; *)
   (* UseYukawa3LoopQCD = True; *)
   (* UseYukawa4LoopQCD = True; *)
   (* UseSMAlphaS3Loop = True; *)
   (* UseSMAlphaS4Loop = True; *)
   ~~~
1. Create, configure and compile the FlexibleSUSY spectrum generator:
   ~~~.sh
   LT=$HOME/hep-software/LoopTools/build
   ./createmodel -f --name=SESM
   ./configure --with-models=SESM --with-loop-libraries=looptools --with-looptools-incdir=${LT} --with-looptools-libdir=${LT}
   make -j4
   ~~~
1. Modify the SESM SLHA input file `model_files/SESM/LesHouches.in.SESM` to set the input parameters to reasonable values:
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

## Create a SPheno spectrum generator

Now we turn to create a SPheno model file:

1. Go to the SPheno directory and copy the model file `SPheno.m` from the SM to the SESM:
   ~~~.sh
   cd ~/hep-software/SARAH
   cp Models/SM/SPheno.m Models/SESM/
   ~~~
1. Modify the `SPheno.m` model file to define new input parameters and fix the new model parameters:
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
   and inspect the output
   ~~~.sh
   less SPheno.spc.SESM
   ~~~

# Standard Model + two scalar singlets (TSESM)

As a second example, let us consider an extension of the SESM by
another real scalar gauge singlet. The model should be called Two
Singlet Extension of the Standard Model (\TSESM). The Lagrangian
should have a global $Z_2$ symmetry with all non-SM fields having
$Z_2$ charge $-1$. The Lagrangian should be:

$$\mathcal{L}_{TSESM} = \mathcal{L}_{SESM} - \left[\frac{\tilde\lambda_{SH}}{2} H^\dagger H \tilde{s}^2 + \frac{\tilde{M}_S^2}{2} \tilde{s}^2 + \frac{\tilde{\lambda}_S}{2} \tilde{s}^4\right]$$

## Create SARAH model

1. We start from the SESM and copy the SESM model files to a new TSESM directory:
   ~~~.sh
   cd ~/hep-software/SARAH
   mkdir -p Models/TSESM
   cp Models/SESM/parameters.m Models/TSESM/
   cp Models/SESM/particles.m Models/TSESM/
   cp Models/SESM/SESM.m Models/TSESM/TSESM.m
   ~~~
1. We modify `Models/TSESM/TSESM.m`: Add a global $Z_2$ symmetry and assign the charge +1 to each SESM field and -1 to each non-SESM field
   ~~~.m
   Global[[1]] = {Z[2], Z2};

   Gauge[[1]]={B,   U[1], hypercharge, g1, False, 1};
   Gauge[[2]]={WB, SU[2], left,        g2, True , 1};
   Gauge[[3]]={G,  SU[3], color,       g3, False, 1};

   FermionFields[[1]] = {q, 3, {uL, dL},     1/6, 2,  3, 1};
   FermionFields[[2]] = {l, 3, {vL, eL},    -1/2, 2,  1, 1};
   FermionFields[[3]] = {d, 3, conj[dR],     1/3, 1, -3, 1};
   FermionFields[[4]] = {u, 3, conj[uR],    -2/3, 1, -3, 1};
   FermionFields[[5]] = {e, 3, conj[eR],       1, 1,  1, 1};

   ScalarFields[[1]] =  {H, 1, {Hp, H0},     1/2, 2,  1, 1};
   ScalarFields[[2]] = {s, 1, Sing, 0, 1, 1, 1};
   ScalarFields[[3]] = {ts, 1, TSing, 0, 1, 1, -1};

   RealScalars = {s, ts};

   LagNoHC = -(
       + mu2 conj[H].H
       + \[Lambda]/2 conj[H].H.conj[H].H
       + KapSH conj[H].H.s
       + LamSH/2 conj[H].H.s.s
       + MS2/2 s.s
       + KapS/3 s.s.s
       + LamS/2 s.s.s.s
       + TLamSH/2 conj[H].H.ts.ts
       + TMS2/2 ts.ts
       + TLamS/2 ts.ts.ts.ts
   );
   ~~~
1. We modify `Models/TSESM/particles.m`:
   ~~~.m
   ParticleDefinitions[EWSB] = {
     ...
     {TSing,  { Description -> "Singlet",
                PDG -> {6666635},
                PDG.IX -> {101000002},
                FeynArtsNr -> 10,
                Mass -> LesHouches,
                LaTeX -> "\\tilde{s}",
                ElectricCharge -> 0,
                LHPC -> {"gold"},
                OutputName -> "ts" }}
   };
   ~~~
1. Extend `Models/TSESM/parameters.m`:
   ~~~.m
   ParameterDefinitions = {
     ...
     {TMS2, { Description -> "Singlet mass term",
              LaTeX -> "\\tilde{M}_{S}^2",
              Real -> True, 
	      OutputName -> TMS2,
              LesHouches -> {HMIX,35} }}, 

     {TLamSH, { OutputName -> TLamSH,
                LaTeX -> "\\tilde{\\lambda}_{SH}",
                Real -> True, 
                LesHouches -> {HMIX,36} }},

     {TLamS, { OutputName -> TLamS,
               LaTeX -> "\\tilde{\\lambda}_{S}",
               Real -> True, 
               LesHouches -> {HMIX,37} }},
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
   cp model_files/SESM/FlexibleSUSY.m.in model_files/TSESM/
   cp model_files/SESM/LesHouches.in.SESM model_files/TSESM/LesHouches.in.TSESM
   ~~~
1. Now we edit the FlexibleSUSY model file `model_files/TSESM/FlexibleSUSY.m.in`
   ~~~.m
   FSDefaultSARAHModel = TSESM;

   EXTPAR = {
       {0, Qin},
       {1, QEWSB},
       {2, LambdaIN},
       {3, LamSIN},
       {4, LamSHIN},
       {5, KapSIN},
       {6, KapSHIN},
       {7, vSIN},
       {8, TLamSIN},
       {9, TLamSHIN},
       {10, TMS2IN}
   };

   HighScaleInput = {
       {\[Lambda], LambdaIN},
       {LamS, LamSIN},
       {LamSH, LamSHIN},
       {KapS, KapSIN},
       {KapSH, KapSHIN},
       {TLamS, TLamSIN},
       {TLamSH, TLamSHIN},
       {TMS2, TMS2IN}
   };
   ~~~
1. Create, configure and compile the FlexibleSUSY spectrum generator:
   ~~~.sh
   LT=$HOME/hep-software/LoopTools/build
   ./createmodel -f --name=TSESM
   ./configure --with-models=TSESM --with-loop-libraries=looptools --with-looptools-incdir=${LT} --with-looptools-libdir=${LT}
   make -j4
   ~~~
1. Modify the TSESM SLHA input file `model_files/TSESM/LesHouches.in.TSESM` to set the input parameters to reasonable values:
   ~~~.sh
   Block EXTPAR  # Input parameters
       ...
       8   0.1   # TLamSIN
       9   0.1   # TLamSHIN
      10   1e3   # TMS2IN
   ~~~
1. Run the spectrum generator with the SLHA input file
   ~~~.sh
   models/TSESM/run_TSESM.x --slha-input-file=model_files/TSESM/LesHouches.in.TSESM
   ~~~

## micrOMEGAs

We continue to pass the SLHA output of FlexibleSUSY to micrOMEGAs. For
this, we need to first generate appropriate CalcHEP model files with
SARAH:
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
~~~
We run FlexibleSUSY to generate an SLHA output file with the particle spectrum. The SLHA output file must be named `SPheno.spc.TSESM` and must be placed next to the `main` exectutable of micrOMEGAs:
~~~.sh
FS=$HOME/hep-software/FlexibleSUSY-2.8.0
$FS/models/TSESM/run_TSESM.x --slha-input-file=$FS/model_files/TSESM/LesHouches.in.TSESM > SPheno.spc.TSESM
~~~
Finally, we run micrOMEGAs:
~~~.sh
./main data.par
~~~

## SModelS

In order to get an SLHA output for `smodels` we add the following line
to `~/hep-software/micromegas_6.1.15/TSESM/main.cpp`:
~~~.cpp
smodels(LHC8+LHC13, 5, 0., const_cast<char*>("smodels.slha"), const_cast<char*>("3.0.0"), 0);
~~~
We recompile and re-run micrOMEGAs:
~~~.sh
make main=main.cpp
./main data.par
~~~
We now find the SLHA output of micrOMEGAs in `smodels.slha`.

W. Kotlarski has prepared a custom program [`CalcOmega-1DM.cpp`](TSESM/CalcOmega-1DM.cpp).
Compile and run it as follows:
~~~
cd ~/hep-software/micromegas_6.1.15
mkdir nlohmann && cd nlohmann
wget https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp
cd -
make main=CalcOmega-1DM.cpp
./CalcOmega-1DM
~~~
