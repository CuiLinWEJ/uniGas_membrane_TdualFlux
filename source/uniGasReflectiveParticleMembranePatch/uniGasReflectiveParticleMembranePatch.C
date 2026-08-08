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
    temperatureFront_
    (
        propsDict_.getOrDefault<scalar>
        (
            "temperatureFront",
            propsDict_.getOrDefault<scalar>("temperature", 200.0)
        )
    ),
    temperatureBack_
    (
        propsDict_.getOrDefault<scalar>
        (
            "temperatureBack",
            propsDict_.getOrDefault<scalar>("temperature", 200.0)
        )
    ),
    velocity_(propsDict_.get<vector>("velocity")),
    nReflections_(0),
    nTransmissions_(0),
    nTransFrontToBack_(0),
    nTransBackToFront_(0),
    transmittedNpFrontToBack_(0.0),
    transmittedNpBackToFront_(0.0),
    transmittedMassFrontToBack_(0.0),
    transmittedMassBackToFront_(0.0),
    transmittedMomentum_(Zero),
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

    membraneImpulse_ = Zero;
    nReflections_ = 0;
    nTransmissions_ = 0;

    nTransFrontToBack_ = 0;
    nTransBackToFront_ = 0;

    transmittedNpFrontToBack_ = 0.0;
    transmittedNpBackToFront_ = 0.0;

    transmittedMassFrontToBack_ = 0.0;
    transmittedMassBackToFront_ = 0.0;

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

    // Write transmitted-particle flux statistics.
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

        const scalar absNp =
            transmittedNpFrontToBack_ + transmittedNpBackToFront_;
        const scalar netNp =
            transmittedNpFrontToBack_ - transmittedNpBackToFront_;

        const scalar absMass =
            transmittedMassFrontToBack_ + transmittedMassBackToFront_;
        const scalar netMass =
            transmittedMassFrontToBack_ - transmittedMassBackToFront_;

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
                << "nTransFrontToBack nTransBackToFront "
                << "transmittedNpFrontToBack transmittedNpBackToFront "
                << "transmittedMassFrontToBack transmittedMassBackToFront "
                << "absNumberFlux netNumberFlux "
                << "absMassFlux netMassFlux "
                << "momentumFluxX momentumFluxY momentumFluxZ"
                << std::endl;
        }

        fs  << currentTime << " "
            << reportDeltaT << " "
            << procNo << " "
            << localArea << " "
            << nTransFrontToBack_ << " "
            << nTransBackToFront_ << " "
            << transmittedNpFrontToBack_ << " "
            << transmittedNpBackToFront_ << " "
            << transmittedMassFrontToBack_ << " "
            << transmittedMassBackToFront_ << " "
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

    nTransFrontToBack_ = 0;
    nTransBackToFront_ = 0;

    transmittedNpFrontToBack_ = 0.0;
    transmittedNpBackToFront_ = 0.0;

    transmittedMassFrontToBack_ = 0.0;
    transmittedMassBackToFront_ = 0.0;

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

    const vector preU = U;

    // coupledFacesA_ is the configured/control patch (front), while
    // coupledFacesB_ is its cyclic neighbour patch (back).
    const label frontFaceIndex = coupledFacesA_.find(faceI);
    const label backFaceIndex = coupledFacesB_.find(faceI);

    const bool onFront = (frontFaceIndex != -1);
    const bool onBack = (backFaceIndex != -1);

    nF /= mag(nF);
    nw /= mag(nw);

    scalar U_dot_nw = U & nw;
    vector Ut = U - U_dot_nw*nw;

    const label typeId = p.typeId();
    const scalar mass = cloud_.constProps(typeId).mass();
    Random& rndGen = cloud_.rndGen();

    // d describes the velocity relative to the normal of the *current*
    // tracking face.  For a local cyclic crossing the current face is the
    // receiving face; for a processor-cyclic crossing it can still be the
    // sending face.  Therefore d alone is not a physical front/back label.
    const scalar d = nF & U;

    const bool incidentFromFront =
        (onFront && d > 0) || (onBack && d < 0);

    const bool incidentFromBack =
        (onBack && d > 0) || (onFront && d < 0);

    if (!incidentFromFront && !incidentFromBack)
    {
        return;
    }

    const scalar T =
        incidentFromFront ? temperatureFront_ : temperatureBack_;

    const scalar pRandom = rndGen.sample01<scalar>();

    if (pRandom <= p_) // diffuse reflection
    {
        // Construct a robust tangential basis.  Component-wise scaling
        // cannot create a tangential component when the incident velocity is
        // exactly parallel to the face normal.
        vector tw1 = Ut;

        if (mag(tw1) < SMALL)
        {
            const vector axis =
                (mag(nw.x()) < 0.9)
              ? vector(1, 0, 0)
              : vector(0, 1, 0);

            tw1 = axis - (axis & nw)*nw;
        }

        tw1 /= mag(tw1);

        vector tw2 = nw ^ tw1;
        tw2 /= mag(tw2);

        // The return direction is defined from the current tracking face.
        // This preserves the original local/processor-cyclic handling while
        // temperature selection is now tied to the physical incident side.
        const vector nReturn = (d > 0) ? -nF : nF;

        const scalar normalSpeed =
            sqrt
            (
                -2.0
               *log
                (
                    max
                    (
                        1 - rndGen.sample01<scalar>(),
                        VSMALL
                    )
                )
            );

        U =
            sqrt(physicoChemical::k.value()*T/mass)
           *(
                rndGen.GaussNormal<scalar>()*tw1
              + rndGen.GaussNormal<scalar>()*tw2
              + normalSpeed*nReturn
            );

        U += velocity_;

        // Momentum transferred from particle to membrane.
        membraneImpulse_ +=
            cloud_.nParticle()*mass*(preU - U);

        td.switchProcessor = false;
        ++nReflections_;

        writeLocalMembraneForce();
    }
    else // transmission
    {
        ++nTransmissions_;

        const scalar Np = cloud_.nParticle();

        if (incidentFromFront)
        {
            // Physical transmission: front -> back.
            ++nTransFrontToBack_;
            transmittedNpFrontToBack_ += Np;
            transmittedMassFrontToBack_ += Np*mass;
            transmittedMomentum_ += Np*mass*preU;
        }
        else
        {
            // Physical transmission: back -> front.
            ++nTransBackToFront_;
            transmittedNpBackToFront_ += Np;
            transmittedMassBackToFront_ += Np*mass;
            transmittedMomentum_ -= Np*mass*preU;
        }

        writeLocalMembraneForce();
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
    // The main properties should be updated first.
    uniGasCyclicBoundary::updateProperties(dict);
}


// ************************************************************************* //
