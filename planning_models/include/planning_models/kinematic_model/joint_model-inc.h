/*********************************************************************
* Software License Agreement (BSD License)
*
*  Copyright (c) 2012, Willow Garage, Inc.
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of Willow Garage, Inc. nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

/*------------------------------------------------------*/
/*   DO NOT INCLUDE THIS FILE DIRECTLY                  */
/*------------------------------------------------------*/

/** \brief A joint from the robot. Models the transform that
    this joint applies in the kinematic chain. A joint
    consists of multiple variables. In the simplest case, when
    the joint is a single DOF, there is only one variable and
    its name is the same as the joint's name. For multi-DOF
    joints, each variable has a local name (e.g., \e x, \e y)
    but the full variable name as seen from the outside of
    this class is a concatenation of the "joint name"."local
    name" (e.g., a joint named 'base' with local variables 'x'
    and 'y' will store its full variable names as 'base.x' and
    'base.y'). Local names are never used to reference
    variables directly. */
class JointModel
{
  friend class KinematicModel;
public:
  
  /** \brief The different types of joints we support */
  enum JointType
    {
      UNKNOWN, REVOLUTE, PRISMATIC, PLANAR, FLOATING, FIXED
    };
  
  /** \brief The datatype for the joint bounds */
  typedef std::vector<std::pair<double, double> > Bounds;
  
  
  /** \brief Construct a joint named \e name */
  JointModel(const std::string& name);
  
  virtual ~JointModel(void);
  
  /** \brief Get the name of the joint */
  const std::string& getName(void) const
  {
    return name_;
  }
  
  /** \brief Get the type of joint */
  JointType getType(void) const
  {
    return type_;
  }
  
  /** \brief The index of this joint when traversing the kinematic tree in depth first fashion */
  int getTreeIndex(void) const
  {
    return tree_index_;
  }
  
  /** \brief Get the link that this joint connects to. The
      robot is assumed to start with a joint, so the root
      joint will return a NULL pointer here. */
  const LinkModel* getParentLinkModel(void) const
  {
    return parent_link_model_;
  }
  
  /** \brief Get the link that this joint connects to. There will always be such a link */
  const LinkModel* getChildLinkModel(void) const
  {
    return child_link_model_;
  }
  
  /** @name Reason about the variables that make up this joint
      @{ */
  
  /** \brief Get the names of the variables that make up this joint, in the order they appear in corresponding states.
      For single DOF joints, this will be just the joint name. */
  const std::vector<std::string>& getVariableNames(void) const
  {
    return variable_names_;
  }
  
  /** \brief Get the names of the variable suffixes that are attached to joint names to construct the variable names. For single DOF joints, this will be empty. */
  const std::vector<std::string>& getLocalVariableNames(void) const
  {
    return local_names_;
  }
  
  /** \brief Check if a particular variable is known to this joint */
  bool hasVariable(const std::string &variable) const
  {
    return variable_index_.find(variable) != variable_index_.end();
  }
  
  /** \brief Get the number of variables that describe this joint */
  unsigned int getVariableCount(void) const
  {
    return variable_names_.size();
  }
  
  /** \brief The set of variables that make up the state value of a joint are stored in some order. This map
      gives the position of each variable in that order, for each variable name */
  const std::map<std::string, unsigned int>& getVariableIndexMap(void) const
  {
    return variable_index_;
  }
  /** @} */
  
  /** @name Functionality specific to computing state values
      @{ */
  
  /** \brief Provide a default value for the joint given the default joint variable bounds (maintained internally).
      Most joints will use the default implementation provided in this base class, but the quaternion
      for example needs a different implementation. The map is NOT cleared; elements are only added (or overwritten). */
  void getDefaultValues(std::map<std::string, double> &values) const
  {
    getDefaultValues(values, variable_bounds_);
  }
  
  /** \brief Provide a default value for the joint given the joint bounds.
      Most joints will use the default implementation provided in this base class, but the quaternion
      for example needs a different implementation. The map is NOT cleared; elements are only added (or overwritten). */
  void getDefaultValues(std::map<std::string, double> &values, const Bounds &other_bounds) const;
  
  /** \brief Provide a default value for the joint given the default joint variable bounds (maintained internally).
      Most joints will use the default implementation provided in this base class, but the quaternion
      for example needs a different implementation. The vector is NOT cleared; elements are only added with push_back */
  void getDefaultValues(std::vector<double> &values) const
  {
    getDefaultValues(values, variable_bounds_);
  }
  
  /** \brief Provide a default value for the joint given the joint variable bounds.
      Most joints will use the default implementation provided in this base class, but the quaternion
      for example needs a different implementation. The vector is NOT cleared; elements are only added with push_back */
  virtual void getDefaultValues(std::vector<double> &values, const Bounds &other_bounds) const = 0;
  
  /** \brief Provide random values for the joint variables (within default bounds). The map is NOT cleared; elements are only added (or overwritten). */
  void getRandomValues(random_numbers::RandomNumberGenerator &rng, std::map<std::string, double> &values) const
  {
    getRandomValues(rng, values, variable_bounds_);
  }
  
  /** \brief Provide random values for the joint variables (within specified bounds). The map is NOT cleared; elements are only added (or overwritten). */
  void getRandomValues(random_numbers::RandomNumberGenerator &rng, std::map<std::string, double> &values, const Bounds &other_bounds) const;
  
  /** \brief Provide random values for the joint variables (within default bounds). The vector is NOT cleared; elements are only added with push_back */
  void getRandomValues(random_numbers::RandomNumberGenerator &rng, std::vector<double> &values) const
  {
    getRandomValues(rng, values, variable_bounds_);
  }
  
  /** \brief Provide random values for the joint variables (within specified bounds). The vector is NOT cleared; elements are only added with push_back */
  virtual void getRandomValues(random_numbers::RandomNumberGenerator &rng, std::vector<double> &values, const Bounds &other_bounds) const = 0;
  
  /** \brief Provide random values for the joint variables (within specified bounds). The vector is NOT cleared; elements are only added with push_back */
  virtual void getRandomValuesNearBy(random_numbers::RandomNumberGenerator &rng, std::vector<double> &values, const Bounds &other_bounds,
                                     const std::vector<double> &near, const double distance) const = 0;
  
  /** @} */
  
  /** @name Functionality specific to verifying bounds
      @{ */
  
  /** \brief Check if the set of values for the variables of this joint are within bounds. */
  bool satisfiesBounds(const std::vector<double> &values, double margin = 0.0) const
  {
    return satisfiesBounds(values, variable_bounds_, margin);
  }
  
  /** \brief Check if the set of values for the variables of this joint are within bounds, up to some margin. */
  virtual bool satisfiesBounds(const std::vector<double> &values, const Bounds &other_bounds, double margin) const = 0;
  
  /** \brief Force the specified values to be inside bounds and normalized. Quaternions are normalized, continuous joints are made between -Pi and Pi. */
  void enforceBounds(std::vector<double> &values) const
  {
    enforceBounds(values, variable_bounds_);
  }
  
  /** \brief Force the specified values to be inside bounds and normalized. Quaternions are normalized, continuous joints are made between -Pi and Pi. */
  virtual void enforceBounds(std::vector<double> &values, const Bounds &other_bounds) const = 0;
  
  /** \brief Get the lower and upper bounds for a variable. Return false if the variable was not found */
  bool getVariableBounds(const std::string& variable, std::pair<double, double>& bounds) const;
  
  /** \brief Get the variable bounds for this joint, in the same order as the names returned by getVariableNames() */
  const Bounds& getVariableBounds(void) const
  {
    return variable_bounds_;
  }
  
  /** \brief Get variable limits as a message type */
  virtual std::vector<moveit_msgs::JointLimits> getVariableLimits(void) const;

  /** \brief Override joint limits */
  void setLimits(const std::vector<moveit_msgs::JointLimits>& jlim)
  {
    user_specified_limits_ = jlim;
  }
  
  /** \brief Get the joint limits specified by the user with setLimits() or the default joint limits using getVariableLimits(), if no joint limits were specified. */
  std::vector<moveit_msgs::JointLimits> getLimits(void) const
  {
    return user_specified_limits_.empty() ? getVariableLimits() : user_specified_limits_;
  }

  /** @} */
  
  /** \brief Compute the distance between two joint states of the same model (represented by the variable values) */
  virtual double distance(const std::vector<double> &values1, const std::vector<double> &values2) const = 0;
  
  
  /** \brief Get the dimension of the state space that corresponds to this joint */
  virtual unsigned int getStateSpaceDimension(void) const = 0;
  
  
  /** \brief Get the joint this one is mimicking */
  const JointModel* getMimic(void) const
  {
    return mimic_;
  }
  
  /** \brief If mimicking a joint, this is the offset added to that joint's value */
  double getMimicOffset(void) const
  {
    return mimic_offset_;
  }
  
  /** \brief If mimicking a joint, this is the multiplicative factor for that joint's value */
  double getMimicFactor(void) const
  {
    return mimic_factor_;
  }
  
  /** \brief The joint models whose values would be modified if the value of this joint changed */
  const std::vector<const JointModel*>& getMimicRequests(void) const
  {
    return mimic_requests_;
  }
  
  /** \brief Get the maximum velocity of this joint. If the result is zero, the value is considered not to be specified. */
  double getMaximumVelocity(void) const
  {
    return max_velocity_;
  }
  
  /** \brief Get the factor that should be applied to the value returned by distance() when that value is used in compound distances */
  double getDistanceFactor(void) const
  {
    return distance_factor_;
  }
  
  /** \brief Set the factor that should be applied to the value returned by distance() when that value is used in compound distances */
  void setDistanceFactor(double factor)
  {
    distance_factor_ = factor;
  }
  
  /** \brief Computes the state that lies at time @e t in [0, 1] on the segment that connects @e from state to @e to state.
      The memory location of @e state is not required to be different from the memory of either
      @e from or @e to. */
  virtual void interpolate(const std::vector<double> &from, const std::vector<double> &to, const double t, std::vector<double> &state) const = 0;
  
  /** \brief Get the extent of the state space (the maximum value distance() can ever report) */
  virtual double getMaximumExtent(const Bounds &other_bounds) const = 0;

  double getMaximumExtent(void) const
  {
    return getMaximumExtent(variable_bounds_);
  }
    
  /** @name Computing transforms 
      @{ */
  
  /** \brief Given the joint values for a joint, compute the corresponding transform */
  virtual void computeTransform(const std::vector<double>& joint_values, Eigen::Affine3d &transf) const = 0;
  
  /** \brief Given the transform generated by joint, compute the corresponding joint values */
  virtual void computeJointStateValues(const Eigen::Affine3d& transform, std::vector<double> &joint_values) const = 0;
  
  /** \brief Update a transform so that it corresponds to
      the new joint values assuming that \e transf was
      previously set to identity and that only calls to updateTransform() were issued afterwards */
  virtual void updateTransform(const std::vector<double>& joint_values, Eigen::Affine3d &transf) const = 0;
  
  /** @} */
  
protected:
  
  /** \brief Name of the joint */
  std::string                                       name_;
  
  /** \brief The type of joint */
  JointType                                         type_;
  
  /** \brief The local names to use for the variables that make up this joint */
  std::vector<std::string>                          local_names_;
  
  /** \brief The full names to use for the variables that make up this joint */
  std::vector<std::string>                          variable_names_;
  
  /** \brief The bounds for each variable (low, high) in the same order as variable_names_ */
  Bounds                                            variable_bounds_;
  
  /** \brief The maximum velocity of this joint. If zero, the value is considered not to be specified. */
  double                                            max_velocity_;
  
  /** \brief Map from variable names to the corresponding index in variable_names_ */
  std::map<std::string, unsigned int>               variable_index_;
  
  /** \brief The link before this joint */
  LinkModel                                        *parent_link_model_;
  
  /** \brief The link after this joint */
  LinkModel                                        *child_link_model_;
  
  /** \brief The joint this one mimics (NULL for joints that do not mimic) */
  JointModel                                       *mimic_;
  
  /** \brief The offset to the mimic joint */
  double                                            mimic_factor_;
  
  /** \brief The multiplier to the mimic joint */
  double                                            mimic_offset_;
  
  /** \brief The set of joints that should get a value copied to them when this joint changes */
  std::vector<const JointModel*>                    mimic_requests_;
  
  /** \brief The factor applied to the distance between two joint states */
  double                                            distance_factor_;

  /** \brief User specified limits for this joint */
  std::vector<moveit_msgs::JointLimits>             user_specified_limits_;

  /** \brief The index assigned to this joint when traversing the kinematic tree in depth first fashion */
  int                                               tree_index_;
};
