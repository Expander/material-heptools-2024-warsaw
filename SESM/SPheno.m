OnlyLowEnergySPheno = True;

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

DEFINITION[MatchingConditions]= 
{{v, vSM}, 
 {Ye, YeSM},
 {Yd, YdSM},
 {Yu, YuSM},
 {g1, g1SM},
 {g2, g2SM},
 {g3, g3SM}};


ListDecayParticles = {Fu,Fe,Fd,hh};
ListDecayParticles3B = {{Fu,"Fu.f90"},{Fe,"Fe.f90"},{Fd,"Fd.f90"}};

DefaultInputValues = {
    LambdaIN -> 0.21,
    LambdaSIN -> 0.1,
    LambdaSHIN -> 0.1,
    KappaSIN -> 100,
    KappaSHIN -> -100,
    KappaSHIN -> 3
};

AddTreeLevelUnitarityLimits=True;

RenConditionsDecays={
{dCosTW, 1/2*Cos[ThetaW] * (PiVWp/(MVWp^2) - PiVZ/(mVZ^2)) },
{dSinTW, -dCosTW/Tan[ThetaW]},
{dg2, 1/2*g2*(derPiVPheavy0 + PiVPlightMZ/MVZ^2 - (-(PiVWp/MVWp^2) + PiVZ/MVZ^2)/Tan[ThetaW]^2 + (2*PiVZVP*Tan[ThetaW])/MVZ^2)  },
{dg1, dg2*Tan[ThetaW]+g2*dSinTW/Cos[ThetaW]- dCosTW*g2*Tan[ThetaW]/Cos[ThetaW]}
};
