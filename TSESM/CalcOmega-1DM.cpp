#include "../include/micromegas.h"
#include "lib/pmodel.h"
#include"../include/micromegas_aux.h"

#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>

int main(int argc, char** argv)
{
   int err;
   char lspname[10], nlspname[10];

   // no idea
   // Florian's comment:
   //    CalcHep sometimes hangs up, if a partial width vanishes exactly.
   //    This can happen for instance in the flavour violating decay of fermion
   //    if no flavor violation is presented in the parameter. The problematic
   //    channels involve massive goldstones. For transveral vectors I didn't
   //    find the problem.
   //    to Force Unitary Gauge assign 1
   ForceUG = 0;

   // determines how the particle widths are computed,
   // they are either
   // read from an SLHA file (1)
   // calculated internally (0, the default option)
   useSLHAwidth = 0;

   //pWidthPref(const_cast<char*>("SOc"), 3);
   //pWidthPref(const_cast<char*>("POc"), 3);
   // switches to turn on/off processes with off-shell gauge bosons
   // in the final state for DM annihilation and particle decays.
   // 1: 3-body final state in annihilation
   // 2: 3-body final state also in coanihilation
   // Florian suggests: VZdecay=0; VWdecay=0;
   // His reason:
   //    Sometimes, the calculation of the 3-body final states with W,Z
   //    causes a seg-fault
   VZdecay = atoi(argv[2]);
   VWdecay = atoi(argv[2]);
   cleanDecayTable();

   err = sortOddParticles(lspname);

   printMasses (stdout, 1);
   std::cout << std::flush;

   // -1 = best accuracy, 0 = good accuracy, 1 = "fast option" (accuracy ~1%)
   const int fast = atoi(argv[3]);
   // criterium for including co-annihilations (1 = no coann.)
   constexpr double Beps = 1e-5;
   double Xf = -1;
	const double Omega = darkOmega(&Xf,fast,Beps,&err);
   std::cout << "Omegah2 = " << Omega << '\n';

   static constexpr double cut = 0.01;		// cut-off for channel output
	printChannels(Xf,cut,Beps,1,stdout);

   std::cout << std::flush;

   // ==================================

   double pA0[2], pA5[2], nA0[2], nA5[2];
   constexpr double Nmass = 0.939;
   constexpr double GeVm2ToPb {3.8937966e+8};

   std::cout << "\n==== Calculation of CDM-nucleons amplitudes  =====\n\n";

   // CDM[0]
   // Amplitudes (in  GeV−2)
   nucleonAmplitudes (CDM[1], pA0, pA5, nA0, nA5);
   printf("%s[%s]-nucleon micrOMEGAs amplitudes\n", CDM[1], antiParticle(CDM[1]));
   printf("proton:  SI  %.3E [%.3E]  SD  %.3E [%.3E]\n", pA0[0], pA0[1],  pA5[0], pA5[1] );
   printf("neutron: SI  %.3E [%.3E]  SD  %.3E [%.3E]\n", nA0[0], nA0[1],  nA5[0], nA5[1] );

   std::cout << '\n';
   // DM-nucleon reduced mass
   const double reduced_mass_dm1 = Nmass*McdmN[1]/(Nmass + McdmN[1]);
   const double SCcoeff1 = 4./M_PI * std::pow(reduced_mass_dm1, 2) * GeVm2ToPb;
   printf("%s[%s]-nucleon cross sections [pb]:\n", CDM[1], antiParticle(CDM[1]));
   printf(" proton  SI %.3E [%.3E] SD %.3E [%.3E]\n",
      SCcoeff1*std::norm(pA0[0]), SCcoeff1*std::norm(pA0[1]),
      3*SCcoeff1*std::norm(pA5[0]), 3*SCcoeff1*std::norm(pA5[1])
   );
   printf(
      " neutron SI %.3E [%.3E] SD %.3E [%.3E]\n",
      SCcoeff1*std::norm(nA0[0]),   SCcoeff1*std::norm(nA0[1]),
      3*SCcoeff1*std::norm(nA5[0]), 3*SCcoeff1*std::norm(nA5[1])
   );
   std::cout << std::flush;

   char* expName;
   printf("\n===== Direct detection exclusion:======\n");
   const double pval = DD_pval(AllDDexp, Maxwell, &expName);
   if (pval < 0.1) {
      printf("Excuded by %s at %.1f%% level\n", expName, 100*(1-pval));
   }
   else {
      printf("Not excluded by DD experiments at 90%% level (p-value = %.2f)\n", pval);
   }
   std::cout << std::flush;

   if (argc > 1 && atoi(argv[1])) {
      smodels (LHC8+LHC13, 5, 0.0001, const_cast<char*>("smodels.slha"), const_cast<char*>("2.3.0"), 0);
   }

   double cs_lep;
   LspNlsp_LEP(&cs_lep);
   std::cout << "\nLEP limit... " << cs_lep << '\n';

   const double CL = monoJet();
   std::cout << "Mono-jet " << CL << '\n';
   std::cout << "\nExcluded by invisible Z width? " << Zinvisible() << '\n';

  	return 0;
}

