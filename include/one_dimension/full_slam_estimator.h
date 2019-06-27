#pragma once

#include "one_dimension/estimator_base.h"
#include "one_dimension/range_meas.h"
//#include "one_dimension/factors/odometry_constraint.h"
//#include "one_dimension/factors/range_constraint.h"
#include "one_dimension/factors/position_constraint.h"
#include "one_dimension/factors/velocity_constraint.h"

using std::vector;

using ceres::Solver;

class FullSLAMEstimator : public EstimatorBase
{
public:
  // t - current time (seconds)
  // z - measurement
  // R - covariance
  virtual void positionCallback(const double& t, const double& z,
                                const double& R)
  {
    position_stddev_ = sqrt(R);
    position_meas.push_back(z);
    position_optimized.push_back(z);
  }

  virtual void odometryCallback(const double& t, const double& z,
                                const double& R)
  {
    //odom_R_ = R;
    //odom_values.push_back(z);
  }

  virtual void rangeCallback(const double& t, const RangeMeas& z)
  {
    //range_stddev_ = sqrt(z.range_variance);
    //const double range_value = z.ranges[0];
    //range_readings.push_back(range_value);
  }

  void solve()
  {
    velocity_optimized = 2.;
    position_optimized = position_meas;

    ceres::Problem problem;

    for (int i = 0; i < position_meas.size(); i++)
    {
      // Create and add a cost for the position Contraint for pose i
      problem.AddResidualBlock(
          PositionConstraint::Create(position_meas[i], position_stddev_), NULL,
          &position_optimized[i]);

      if (i > 0)
      {
        // Create a cost for the velocity constraint between pose i, i-1
        problem.AddResidualBlock(VelocityConstraint::Create(velocity_stddev),
                                 NULL, &(position_optimized[i - 1]),
                                 &(position_optimized[i]), &velocity_optimized);
      }
    }

    ceres::Solver::Options solver_options;
    solver_options.minimizer_progress_to_stdout = true;

    Solver::Summary summary;
    printf("Solving...\n");
    Solve(solver_options, &problem, &summary);
    printf("Done.\n");
    //std::cout << summary.FullReport() << "\n";
    std::cout << "Final odom: " << velocity_optimized << std::endl;
  }

  double velocity_optimized;
  double velocity_stddev = 0.001;

  double position_stddev_;
  vector<double> position_meas;
  vector<double> position_optimized;

  //double odom_R_;
  //vector<double> odom_values;

  //double range_stddev_;
  //vector<double> range_readings;
  //vector<double> range_solved;
};

