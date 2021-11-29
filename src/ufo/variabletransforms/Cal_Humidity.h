/*
 * (C) Crown copyright 2020, Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef UFO_VARIABLETRANSFORMS_CAL_HUMIDITY_H_
#define UFO_VARIABLETRANSFORMS_CAL_HUMIDITY_H_

#include <algorithm>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "ufo/filters/VariableTransformParametersBase.h"
#include "ufo/variabletransforms/TransformBase.h"

namespace ufo {

/// Configuration parameters for the variable transformation of specific humidity to
/// relative humidity
class Cal_RelativeHumidityParameters: public VariableTransformParametersBase {
  OOPS_CONCRETE_PARAMETERS(Cal_RelativeHumidityParameters, VariableTransformParametersBase);

 public:
  /// Should we allow super-saturated relative humidity? [Optional]:
  /// By default \e AllowSuperSaturation is set to \e false.
  /// See ReadTheDoc for more details
  oops::Parameter<bool> AllowSuperSaturation{"AllowSuperSaturation", false, this};
};

/*!
* \brief Relative Humidity filter
*
* Performs a variable conversion from specific_humidity, temperature, and
* pressure to relative humidity. The newly calculated variable is included in the same
* obs space.
*
* Example:
*
* \code{.yaml}
* obs filters:
* - filter: Variables Transform
*   Transform: ["RelativeHumidity"]
*   Formulation: Sonntag    # Using Sonntag formulation
* \endcode
*
* See Cal_RelativeHumidityParameters for filter setup.
*/
class Cal_RelativeHumidity : public TransformBase {
 public:
  typedef Cal_RelativeHumidityParameters Parameters_;

  Cal_RelativeHumidity(const Parameters_ &options,
                       const ObsFilterData &data,
                       const std::shared_ptr<ioda::ObsDataVector<int>> &flags);
  // Run variable conversion
  void runTransform(const std::vector<bool> &apply) override;

 private:
  bool allowSuperSaturation_;
  // list of specific implementation(s) - This is controlled by "method"
  void methodDEFAULT(const std::vector<bool> &apply);
  void methodUKMOmixingratio(const std::vector<bool> &apply);
  void methodUKMO(const std::vector<bool> &apply);
};


/*!
* \brief Specific Humidity filter
*
* Performs a variable conversion from relative_humidity, temperature, and
* pressure to specific humidity. The newly calculated variable is included in the same
* obs space.
*
* Example:
*
* \code{.yaml}
* obs filters:
* - filter: Variables Transform
*   Transform: ["SpecificHumidity"]
*   Method: UKMO            # Using UKMO method and UKMO default formulation
*   Formulation: Sonntag    # Using Sonntag formulation
* \endcode
*
* See VariableTransformParametersBase for filter setup.
*/
class Cal_SpecificHumidity : public TransformBase {
 public:
  Cal_SpecificHumidity(const GenericVariableTransformParameters &options,
                       const ObsFilterData &data,
                       const std::shared_ptr<ioda::ObsDataVector<int>> &flags);
  // Run check
  void runTransform(const std::vector<bool> &apply) override;

 private:
  // list of specific implementation(s) - This is controlled by "method"
  void methodDEFAULT(const std::vector<bool> &apply);
};
}  // namespace ufo

#endif  // UFO_VARIABLETRANSFORMS_CAL_HUMIDITY_H_
