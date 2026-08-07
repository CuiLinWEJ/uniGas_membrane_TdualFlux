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

\*---------------------------------------------------------------------------*/

#include "uniGasReflectiveParticleMembranePatch.H"
#include "addToRunTimeSelectionTable.H"
#include "OSspecific.H"
#include <fstream>

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
defineTypeNameAndDebug(uniGasReflectiveParticleMembranePatch, 0);

addToRunTimeSelectionTable
(
    uniGasCyclicBoundary,
    uniGasReflectiveParticleMembranePatch,
    dictionary
);
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::uniGasReflectiveParticleMembranePatch::uniGasReflectiveParticleMembranePatch
(
    const polyMesh& mesh,
    uniGasCloud& cloud,
    const dictionary& dict
)
:
    uniGasCyclicBoundary(mesh, cloud, dict),
    propsDict_(dict.subDict(typeName + "Properties")),
    p_(propsDict_.get<scalar>("reflectionProbability")),
    temperatureMaster_
    (
        propsDict_.getOrDefault<scalar>
        (
            "temperatureMaster",
            propsDict_.getOrDefault<scalar>("temperature", 200.0)
        )
    ),
    temperatureSlave_
    (
        propsDict_.getOrDefault<scalar>
        (
            "temperatureSlave",
            propsDict_.getOrDefault<scalar>("temperature", 200.0)
        )
    ),
    velocity_(propsDict_.get<vector>("velocity")),
    nReflections_(0),
    nTransmissions_(0),
    nTransAtoB_(0),
    nTransBtoA_(0),
    transmittedNpAtoB_(0.0),
    transmittedNpBtoA_(0.0),
    transmittedMassAtoB_(0.0),
    transmittedMassBtoA_(0.0),
    transmittedMomentum_(Zero),
    
    //
    membraneImpulse_(Zero),
    lastReportTime_(mesh.time().value()),
    forceWriteInterval_
    (
       propsDict_.lookupOrDefault<scalar>
       (
          "forceWriteInterval",
          mesh.time().deltaTValue()
       )
    ),
    forceFileInitialised_(false)
{
    writeInTimeDir_ = false;
    writeInCase_ = false;
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::uniGasReflectiveParticleMembranePatch::calculateProperties()
{
    label nReflections = nReflections_;
    label nTransmissions = nTransmissions_;
    vector membraneImpulse = membraneImpulse_;

    if (Pstream::parRun())
    {
        reduce(nReflections, sumOp<label>());
        reduce(nTransmissions, sumOp<label>());
        reduce(membraneImpulse, sumOp<vector>());
    }

    const scalar currentTime = mesh_.time().value();

    const scalar reportDeltaT = max
    (
        currentTime - lastReportTime_,
        mesh_.time().deltaTValue()
    );

    const vector membraneForce = membraneImpulse/max(reportDeltaT, VSMALL);

    const label nTotal = nReflections + nTransmissions;

    if (Pstream::master())
    {
        Info<< "    no Reflections: " << nReflections
            << ", no Transmissions: " << nTransmissions;

        if (nTotal > 0)
        {
            Info<< " reflection fraction: "
                << scalar(nReflections)/scalar(nTotal);
        }
        else
        {
            Info<< " reflection fraction: 0";
        }

        Info<< nl
            << "    membrane impulse [N s]: " << membraneImpulse
            << nl
            << "    membrane force [N]: " << membraneForce
            << nl
            << "    membrane force averaging time [s]: " << reportDeltaT
            << endl;
    }

    // Reset interval statistics after reporting
    membraneImpulse_ = Zero;
    nReflections_ = 0;
    nTransmissions_ = 0;

    nTransAtoB_ = 0;
    nTransBtoA_ = 0;

    transmittedNpAtoB_ = 0.0;
    transmittedNpBtoA_ = 0.0;

    transmittedMassAtoB_ = 0.0;
    transmittedMassBtoA_ = 0.0;

    transmittedMomentum_ = Zero;
    lastReportTime_ = currentTime;
}



void Foam::uniGasReflectiveParticleMembranePatch::writeLocalMembraneForce()
{
    const scalar currentTime = mesh_.time().value();

    if ((currentTime - lastReportTime_) < forceWriteInterval_)
    {
        return;
    }

    const scalar reportDeltaT = max
    (
        currentTime - lastReportTime_,
        mesh_.time().deltaTValue()
    );

    const vector localForce = membraneImpulse_/max(reportDeltaT, VSMALL);

    const label procNo = Pstream::myProcNo();

    fileName outDir =
        mesh_.time().globalPath()
       /"postProcessing"
       /"membraneForce"
       /patchName_
       /("processor" + name(procNo));

    mkDir(outDir);

    fileName outFile = outDir/"membraneForce.dat";

    if (!forceFileInitialised_)
    {
        std::ofstream os(outFile.c_str(), std::ios_base::out);

        os  << "# time dt proc nReflections nTransmissions "
            << "impulseX impulseY impulseZ "
            << "forceX forceY forceZ"
            << std::endl;

        forceFileInitialised_ = true;
    }

    std::ofstream os(outFile.c_str(), std::ios_base::out | std::ios_base::app);

    os  << currentTime << " "
        << reportDeltaT << " "
        << procNo << " "
        << nReflections_ << " "
        << nTransmissions_ << " "
        << membraneImpulse_.x() << " "
        << membraneImpulse_.y() << " "
        << membraneImpulse_.z() << " "
        << localForce.x() << " "
        << localForce.y() << " "
        << localForce.z()
        << std::endl;
    // Write transmitted-particle flux statistics
    {
        scalar localArea = 0.0;

        const vectorField& Sf = mesh_.faceAreas();

        forAll(coupledFacesA_, i)
        {
            localArea += mag(Sf[coupledFacesA_[i]]);
        }

        if (localArea < VSMALL)
        {
            forAll(coupledFacesB_, i)
            {
                localArea += mag(Sf[coupledFacesB_[i]]);
            }
        }

        const scalar areaDt = max(localArea*reportDeltaT, VSMALL);

        const scalar absNp = transmittedNpAtoB_ + transmittedNpBtoA_;
        const scalar netNp = transmittedNpAtoB_ - transmittedNpBtoA_;

        const scalar absMass = transmittedMassAtoB_ + transmittedMassBtoA_;
        const scalar netMass = transmittedMassAtoB_ - transmittedMassBtoA_;

        const scalar absNumberFlux = absNp/areaDt;
        const scalar netNumberFlux = netNp/areaDt;

        const scalar absMassFlux = absMass/areaDt;
        const scalar netMassFlux = netMass/areaDt;

        const vector momentumFlux = transmittedMomentum_/areaDt;

        fileName fluxDir =
            mesh_.time().globalPath()
           /"postProcessing"
           /"membraneFlux"
           /patchName_
           /("processor" + name(procNo));

        mkDir(fluxDir);

        fileName fluxFile = fluxDir/"membraneFlux.dat";

        std::ifstream fluxCheck(fluxFile.c_str());
        const bool fluxFileExists = fluxCheck.good();

        std::ofstream fs
        (
            fluxFile.c_str(),
            std::ios_base::out | std::ios_base::app
        );

        if (!fluxFileExists)
        {
            fs  << "# time dt proc localArea "
                << "nTransAtoB nTransBtoA "
                << "transmittedNpAtoB transmittedNpBtoA "
                << "transmittedMassAtoB transmittedMassBtoA "
                << "absNumberFlux netNumberFlux "
                << "absMassFlux netMassFlux "
                << "momentumFluxX momentumFluxY momentumFluxZ"
                << std::endl;
        }

        fs  << currentTime << " "
            << reportDeltaT << " "
            << procNo << " "
            << localArea << " "
            << nTransAtoB_ << " "
            << nTransBtoA_ << " "
            << transmittedNpAtoB_ << " "
            << transmittedNpBtoA_ << " "
            << transmittedMassAtoB_ << " "
            << transmittedMassBtoA_ << " "
            << absNumberFlux << " "
            << netNumberFlux << " "
            << absMassFlux << " "
            << netMassFlux << " "
            << momentumFlux.x() << " "
            << momentumFlux.y() << " "
            << momentumFlux.z()
            << std::endl;
    }



    membraneImpulse_ = Zero;
    nReflections_ = 0;
    nTransmissions_ = 0;

    nTransAtoB_ = 0;
    nTransBtoA_ = 0;

    transmittedNpAtoB_ = 0.0;
    transmittedNpBtoA_ = 0.0;

    transmittedMassAtoB_ = 0.0;
    transmittedMassBtoA_ = 0.0;

    transmittedMomentum_ = Zero;
    lastReportTime_ = currentTime;
}



void Foam::uniGasReflectiveParticleMembranePatch::initialConfiguration()
{}


void Foam::uniGasReflectiveParticleMembranePatch::controlMol
(
    uniGasParcel& p,
    uniGasParcel::trackingData& td
)
{
    const label faceI = p.face();

    vector nF = mesh_.faceAreas()[faceI];
    vector nw = p.normal();

    vector& U = p.U();

    //
    const vector preU = U;

    label fA = coupledFacesA_.find(faceI);
    label fB = coupledFacesB_.find(faceI);

    nF /= mag(nF);
    nw /= mag(nw);

    scalar U_dot_nw = U & nw;
    vector Ut = U - U_dot_nw*nw;

    label typeId = p.typeId();

    // Wall temperature is selected separately for fA/fB sides.

    scalar mass = cloud_.constProps(typeId).mass();

    Random& rndGen = cloud_.rndGen();

    scalar d = nF & U;

    if(d > 0) // front/master side of the coupled membrane
    {
        if(fA != -1)
        {
            const scalar T = temperatureMaster_;
            scalar pRandom = rndGen.sample01<scalar>();

            if( pRandom <= p_ ) // reflect molecule
            {

                // Construct a robust tangential basis.
                // Component-wise scaling cannot produce a tangential component
                // when the incident velocity is exactly parallel to the face normal.
                vector tw1 = Ut;

                if (mag(tw1) < SMALL)
                {
                    // Select an axis that is not parallel to nw
                    const vector axis =
                        (mag(nw.x()) < 0.9)
                      ? vector(1, 0, 0)
                      : vector(0, 1, 0);

                    // Project the selected axis onto the tangential plane
                    tw1 = axis - (axis & nw)*nw;
                }

                tw1 /= mag(tw1);

                // Second tangential unit vector
                vector tw2 = nw ^ tw1;
                tw2 /= mag(tw2);


                
                // Return direction must be opposite to the incident
                // face-normal direction.
                const vector nReturn = (d > 0) ? -nF : nF;

                const scalar normalSpeed =
                    sqrt
                    (
                        -2.0
                       *log
                        (
                            max
                            (
                                1
                              - rndGen.sample01<scalar>(),
                                VSMALL
                            )
                        )
                    );

                // Diffuse velocity relative to the membrane.
                U =
                    sqrt(physicoChemical::k.value()*T/mass)
                   *(
                        rndGen.GaussNormal<scalar>()*tw1
                      + rndGen.GaussNormal<scalar>()*tw2
                      + normalSpeed*nReturn
                    );

                // Transform from membrane-relative to absolute velocity.
                U += velocity_;


		// Momentum transferred from particle to membrane.
		// Particle momentum change is m*(U - preU), therefore
		// membrane impulse is -m*(U - preU) = m*(preU - U).
		membraneImpulse_ += cloud_.nParticle()*mass*(preU - U);


                td.switchProcessor = false;

                ++nReflections_;

        writeLocalMembraneForce();
            }
            else
            {
                ++nTransmissions_;

                // Transmission statistics: front/master side to back/slave side
                ++nTransAtoB_;

                const scalar Np = cloud_.nParticle();

                transmittedNpAtoB_ += Np;
                transmittedMassAtoB_ += Np*mass;
                transmittedMomentum_ += Np*mass*preU;

                writeLocalMembraneForce();
            }
        }
    }
    else if (d < 0) // back/slave side of the coupled membrane
    {
        if(fB != -1)
        {
            const scalar T = temperatureSlave_;
            scalar pRandom = rndGen.sample01<scalar>();

            if( pRandom <= p_ ) // reflect molecule
            {

                // Construct a robust tangential basis.
                // Component-wise scaling cannot produce a tangential component
                // when the incident velocity is exactly parallel to the face normal.
                vector tw1 = Ut;

                if (mag(tw1) < SMALL)
                {
                    // Select an axis that is not parallel to nw
                    const vector axis =
                        (mag(nw.x()) < 0.9)
                      ? vector(1, 0, 0)
                      : vector(0, 1, 0);

                    // Project the selected axis onto the tangential plane
                    tw1 = axis - (axis & nw)*nw;
                }

                tw1 /= mag(tw1);

                // Second tangential unit vector
                vector tw2 = nw ^ tw1;
                tw2 /= mag(tw2);


                
                // Return direction must be opposite to the incident
                // face-normal direction.
                const vector nReturn = (d > 0) ? -nF : nF;

                const scalar normalSpeed =
                    sqrt
                    (
                        -2.0
                       *log
                        (
                            max
                            (
                                1
                              - rndGen.sample01<scalar>(),
                                VSMALL
                            )
                        )
                    );

                // Diffuse velocity relative to the membrane.
                U =
                    sqrt(physicoChemical::k.value()*T/mass)
                   *(
                        rndGen.GaussNormal<scalar>()*tw1
                      + rndGen.GaussNormal<scalar>()*tw2
                      + normalSpeed*nReturn
                    );

                // Transform from membrane-relative to absolute velocity.
                U += velocity_;


		// Momentum transferred from particle to membrane.
		// Particle momentum change is m*(U - preU), therefore
		// membrane impulse is -m*(U - preU) = m*(preU - U).
		membraneImpulse_ += cloud_.nParticle()*mass*(preU - U);


                td.switchProcessor = false;

                ++nReflections_;

        writeLocalMembraneForce();
            }
            else
            {
                ++nTransmissions_;

                // Transmission statistics: back/slave side to front/master side
                ++nTransBtoA_;

                const scalar Np = cloud_.nParticle();

                transmittedNpBtoA_ += Np;
                transmittedMassBtoA_ += Np*mass;
                transmittedMomentum_ -= Np*mass*preU;

                writeLocalMembraneForce();
            }
        }
    }
}


void Foam::uniGasReflectiveParticleMembranePatch::output
(
    const fileName& fixedPathName,
    const fileName& timePath
)
{

    calculateProperties();
}


void Foam::uniGasReflectiveParticleMembranePatch::updateProperties
(
    const dictionary& dict
)
{
    // the main properties should be updated first
    uniGasCyclicBoundary::updateProperties(dict);
}


// ************************************************************************* //
