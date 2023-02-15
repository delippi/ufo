/*
 * (C) Copyright 2023- UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <Eigen/Core>

#include <memory>
#include <string>
#include <vector>

#include "ioda/ObsVector.h"

#include "oops/base/Variables.h"
#include "oops/interface/ObsErrorBase.h"
#include "oops/util/parameters/Parameters.h"
#include "oops/util/parameters/RequiredParameter.h"

#include "ufo/ObsTraits.h"

namespace ioda {
  class ObsSpace;
}

namespace ufo {

/// \brief Parameters for obs errors with correlations between obs in one group
///        set by obs space.obsgrouping
class ObsErrorCrossGroupCovParameters : public oops::ObsErrorParametersBase {
  OOPS_CONCRETE_PARAMETERS(ObsErrorCrossGroupCovParameters, ObsErrorParametersBase)
 public:
  /// GrouInput file containing correlations or covariances. If covariances are
  /// specified, they will be converted to correlations.
  oops::RequiredParameter<std::string> var{"correlation variable name",
        "Group/Name obs variable that correlations should be computed for (note: "
        "this variable should be the same variable as obs space is grouped on", this};
  oops::RequiredParameter<double> lscale{"correlation lengthscale",
        "Gaspari-Cohn correlation lengthscale", this};
};

// -----------------------------------------------------------------------------
/// \brief Observation error covariance matrix with correlations between
///        obs in one group (set by obs space.obsgrouping)
/// \details Correlations are computed as GC(x1-x2) where x1, x2 are values of
///          Group/Name variable `var`. Only correlations between variables in
///          the same record are considered, variables in different records are
///          considered uncorrelated. Obs error standard
///          deviations are read from ObsSpace as ObsError group.
///          Full observation error covariance matrix is R = D^{1/2} * C * D^{1/2}
///          where D^{1/2} is a diagonal matrix with stddev_ (ObsError group)
///          on the diagonal, and C is the correlation matrix.
class ObsErrorCrossGroupCov : public oops::interface::ObsErrorBase<ObsTraits> {
 public:
  /// The type of parameters passed to the constructor.
  /// This typedef is used by the ObsErrorFactory.
  typedef ObsErrorCrossGroupCovParameters Parameters_;

  /// Initialize observation errors
  ObsErrorCrossGroupCov(const Parameters_ &, ioda::ObsSpace &,
                        const eckit::mpi::Comm &timeComm);

  /// Update obs error standard deviations to be equal to \p stddev
  void update(const ioda::ObsVector & stddev) override;

  /// Multiply \p y by this observation error covariance
  /// Computed as R * dy = D^{1/2} * C * D^{1/2} * dy
  /// where D^{1/2} - diagonal matrix with stddev_ on the diagonal
  ///       C - correlations
  void multiply(ioda::ObsVector & y) const override;

  /// Multiply \p y by inverse of this observation error covariance
  /// Computed as R^{-1} * dy = D^{-1/2} * C^{-1] * D^{-1/2} * dy
  /// where D^{1/2} - diagonal matrix with stddev_ on the diagonal
  ///       C - correlations
  void inverseMultiply(ioda::ObsVector & y) const override;

  /// Generate \p y as a random perturbation
  void randomize(ioda::ObsVector & y) const override;

  /// Save obs error standard deviations under \p name group name
  void save(const std::string & name) const override;

  /// Return RMS of obs error standard deviations
  double getRMSE() const override {return stddev_.rms();}

  /// Return obs errors std deviation
  std::unique_ptr<ioda::ObsVector> getObsErrors() const override;

  /// Return inverse of obs error variance
  std::unique_ptr<ioda::ObsVector> getInverseVariance() const override;

 private:
  /// Print covariance details (for logging)
  void print(std::ostream &) const override;
  /// Observation error standard deviations
  ioda::ObsVector stddev_;
  /// Variables for which correlations are defined (same as ObsSpace::obsvariables())
  const oops::Variables vars_;
  /// Correlations between variables
  std::vector<Eigen::MatrixXd> varcorrelations_;
};

// -----------------------------------------------------------------------------

}  // namespace ufo
