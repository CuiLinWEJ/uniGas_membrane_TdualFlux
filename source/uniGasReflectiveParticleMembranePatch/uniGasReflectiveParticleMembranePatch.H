/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
    Copyright (C) 2019-2023 OpenCFD Ltd.
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

Class
    Foam::uniGasReflectiveParticleMembranePatch

Description
    Probabilistic cyclic permeable-membrane boundary for uniGasFoam.

    Incident particles are diffusely reflected with probability Pr or
    transmitted through the coupled membrane with probability Pt = 1 - Pr.
    The implementation supports physical front/back membrane temperatures,
    front-to-back/back-to-front transmitted-particle statistics, and
    membrane-force output.

    Side convention for the revised single-entry two-sided setup:

        front = configured/control cyclic patch
        back  = cyclic neighbour patch

Usage
    Example boundary-condition specification:

    \verbatim
    cyclicBoundaryProperties
    {
        patch       plate_front;
    }

    boundaryModel   uniGasReflectiveParticleMembranePatch;

    uniGasReflectiveParticleMembranePatchProperties
    {
        reflectionProbability   0.5;
        temperatureFront        1866.7;
        temperatureBack         200;
        velocity                (0 0 0);
        forceWriteInterval      1e-3;
    }
    \endverbatim

SourceFiles
    uniGasReflectiveParticleMembranePatch.C

\*---------------------------------------------------------------------------*/

#ifndef uniGasReflectiveParticleMembranePatch_H
#define uniGasReflectiveParticleMembranePatch_H

#include "uniGasCyclicBoundary.H"
#include "vector.H"
#include "polyMesh.H"
#include "uniGasCloud.H"

namespace Foam
{

class uniGasReflectiveParticleMembranePatch
:
    public uniGasCyclicBoundary
{
    dictionary propsDict_;

    scalar p_;

    //- Temperature used on the physical front side
    scalar temperatureFront_;

    //- Temperature used on the physical back side
    scalar temperatureBack_;

    vector velocity_;

    label nReflections_;
    label nTransmissions_;

    //- Physical front-to-back/back-to-front transmission counts
    label nTransFrontToBack_;
    label nTransBackToFront_;

    scalar transmittedNpFrontToBack_;
    scalar transmittedNpBackToFront_;

    scalar transmittedMassFrontToBack_;
    scalar transmittedMassBackToFront_;

    vector transmittedMomentum_;
    vector membraneImpulse_;

    scalar lastReportTime_;
    scalar forceWriteInterval_;
    bool forceFileInitialised_;

public:

    TypeName("uniGasReflectiveParticleMembranePatch");

    uniGasReflectiveParticleMembranePatch
    (
        const polyMesh& mesh,
        uniGasCloud& cloud,
        const dictionary& dict
    );

    virtual ~uniGasReflectiveParticleMembranePatch() = default;

    void calculateProperties();
    void writeLocalMembraneForce();
    void initialConfiguration();

    void controlMol
    (
        uniGasParcel& mol,
        uniGasParcel::trackingData& td
    );

    void output
    (
        const fileName& fixedPathName,
        const fileName& timePath
    );

    void updateProperties(const dictionary& dict);
};

}

#endif

// ************************************************************************* //
