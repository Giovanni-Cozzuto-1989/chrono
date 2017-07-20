// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2014 projectchrono.org
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Authors: Alessandro Tasora
// =============================================================================

#include "chrono/geometry/ChLineNurbs.h"

namespace chrono {
namespace geometry {

// Register into the object factory, to enable run-time dynamic creation and persistence
CH_FACTORY_REGISTER(ChLineNurbs)

ChLineNurbs::ChLineNurbs() {
    std::vector< ChVector<> > mpoints = { ChVector<>(-1,0,0), ChVector<>(1,0,0)};
    this->SetupData(1, mpoints);
}

ChLineNurbs::ChLineNurbs(int morder,                ///< order p: 1= linear, 2=quadratic, etc.
                std::vector< ChVector<> >& mpoints, ///< control points, size n. Required: at least n >= p+1
                ChVectorDynamic<>* mknots,          ///< knots, size k. Required k=n+p+1. If not provided, initialized to uniform. 
                ChVectorDynamic<>* weights          ///< weights, size w. Required w=n. If not provided, all weights as 1. 
                ) {
    this->SetupData(1, mpoints, mknots, weights);
}

ChLineNurbs::ChLineNurbs(const ChLineNurbs& source) : ChLine(source) {
    
    this->points = source.points;
    this->p = source.p;
    this->knots = source.knots;
    this->weights = source.weights;
}

void ChLineNurbs::Evaluate(
                ChVector<>& pos, 
                const double parU, 
                const double parV, 
                const double parW) const {

        ChVectorDynamic<> mR(this->p+1);
        ChBasisToolsNurbs::BasisEvaluate(this->p, parU, this->weights, this->knots, mR);

        int spanU = ChBasisToolsBspline::FindSpan(this->p, parU, this->knots);
        
        pos = VNULL;
        int uind = spanU - p;
        for (int i = 0; i <= this->p; i++) {
            pos += points[uind + i] * mR(i);
        }
}



void ChLineNurbs::SetupData( int morder,                ///< order p: 1= linear, 2=quadratic, etc.
                    std::vector< ChVector<> >& mpoints, ///< control points, size n. Required: at least n >= p+1
                    ChVectorDynamic<>* mknots,          ///< knots, size k. Required k=n+p+1. If not provided, initialized to uniform. 
                    ChVectorDynamic<>* weights          ///< weights, size w. Required w=n. If not provided, all weights as 1. 
                    ) {
    if (morder < 1) 
        throw ChException ("ChLineNurbs::SetupData requires order >= 1."); 

    if (mpoints.size() < morder+1) 
        throw ChException ("ChLineNurbs::SetupData requires at least order+1 control points.");

    if (mknots && mknots->GetLength() != (mpoints.size()+morder+1))
        throw ChException ("ChLineNurbs::SetupData: knots must have size=n_points+order+1");

    if (weights && weights->GetLength() != mpoints.size())
        throw ChException ("ChLineNurbs::SetupData: weights must have size=n_points");

    this->p = morder;
    this->points = mpoints;
    int n = points.size();
    int k = n+p+1;

    if (mknots)
        this->knots.CopyFromMatrix(*mknots);
    else {
        this->knots.Reset(n+p+1);
        ChBasisToolsBspline::ComputeKnotUniformMultipleEnds(this->knots,p);
    }

    if (weights)
        this->weights.CopyFromMatrix(*weights);
    else {
        this->weights.Reset(n);
        this->weights.FillElem(1.0);
    }

}



}  // end namespace geometry
}  // end namespace chrono
