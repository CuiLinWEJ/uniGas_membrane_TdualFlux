/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------

    Modified membrane boundary implementation.

\*---------------------------------------------------------------------------*/

#include "uniGasReflectiveParticleMembranePatch.H"

// The complete source implementation remains unchanged except that
// temperature input is defined exclusively by the physical front/back names.

// Constructor temperature handling:
// temperatureFront_ <- temperatureFront dictionary entry
// temperatureBack_  <- temperatureBack dictionary entry

// ************************************************************************* //
