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

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

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

    The current research implementation remains under validation.

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

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
             Class uniGasReflectiveParticleMembranePatch Declaration
\*---------------------------------------------------------------------------*/

class uniGasReflectiveParticleMembranePatch
:
    public uniGasCyclicBoundary
{
    // Private data

        dictionary propsDict_;

        //- Reflection probability Pr; transmission probability Pt = 1 - Pr
        scalar p_;

        //- Temperature used on the physical front side (control patch)
        scalar temperatureFront_;

        //- Temperature used on the physical back side (neighbour patch)
        scalar temperatureBack_;

        vector velocity_;

        label nReflections_;

        //- Number of particles transmitted during the current reporting interval
        label nTransmissions_;

        //- Physical front-to-back/back-to-front transmission counts
        label nTransFrontToBack_;
        label nTransBackToFront_;

        scalar transmittedNpFrontToBack_;
        scalar transmittedNpBackToFront_;

        scalar transmittedMassFrontToBack_;
        scalar transmittedMassBackToFront_;

        vector transmittedMomentum_;

        //- Local accumulated impulse transferred from particles to membrane
        vector membraneImpulse_;

        //- Time of last local force report
        scalar lastReportTime_;

        //- Local force write interval
        scalar forceWriteInterval_;

        //- Has the local force file been initialised
        bool forceFileInitialised_;

public:

    //- Runtime type information
    TypeName("uniGasReflectiveParticleMembranePatch");

    // Constructors

        //- Construct from components
        uniGasReflectiveParticleMembranePatch
        (
            const polyMesh& mesh,
            uniGasCloud& cloud,
            const dictionary& dict
        );


    //- Destructor
    virtual ~uniGasReflectiveParticleMembranePatch() = default;


    // Member Functions

        void calculateProperties();

        void writeLocalMembraneForce();

        //- Create an initial configuration
        void initialConfiguration();

        //- Control the molecules
        void controlMol
        (
            uniGasParcel& mol,
            uniGasParcel::trackingData& td
        );

        //- Output data
        void output
        (
            const fileName& fixedPathName,
            const fileName& timePath
        );

        //- Update properties from a modified dictionary
        void updateProperties(const dictionary& dict);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
