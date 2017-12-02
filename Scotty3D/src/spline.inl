// Given a time between 0 and 1, evaluates a cubic polynomial with
// the given endpoint and tangent values at the beginning (0) and
// end (1) of the interval.  Optionally, one can request a derivative
// of the spline (0=no derivative, 1=first derivative, 2=2nd derivative).
template <class T>
inline T Spline<T>::cubicSplineUnitInterval(
    const T& position0, const T& position1, const T& tangent0,
    const T& tangent1, double normalizedTime, int derivative) {
  // TODO (Animation) Task 1a
  double time = normalizedTime;
  double time2 = pow(normalizedTime, 2);
  double time3 = pow(normalizedTime, 3);

  double h00, h10, h01, h11;

  //cout << "---cubicSplineUnitInterval---"<< endl;
  if (derivative == 0) {
    //cout << 0 << endl;
    h00 = 2 * time3 - 3 * time2 + 1;
    h10 = time3 - 2 * time2 + time;
    h01 = -2 * time3 + 3 * time2;
    h11 = time3 - time2;
  } else if (derivative == 1) {
    //cout << 1 << endl;
    h00 = 6 * time2 - 6 * time;
    h10 = 3 * time2 - 4 * time + 1;
    h01 = -6 * time2 + 6 * time;
    h11 = 3 * time2 - 2 * time;
  } else if (derivative == 2) {
    //cout << 2 << endl;
    h00 = 12 * time - 6;
    h10 = 6 * time - 4;
    h01 = -12 * time + 6;
    h11 = 6 * time - 2;
  } else {
    return T();
  }

  
  //cout << h00 << endl;
  //cout << h10 << endl;
  //cout << h01 << endl;
  //cout << h11 << endl;
  

  T output = h00 * position0 + h10 * tangent0 + h01 * position1 + h11 * tangent1;
  //cout << "----/cubicSplineUnitInterval------" << endl;
  return output;
}

// Returns a state interpolated between the values directly before and after the
// given time.
template <class T>
inline T Spline<T>::evaluate(double time, int derivative) {

  // Handle some edge cases specified in wiki
  if (knots.size() < 1) { // no knots
    return T(); 
  }

  KnotIter firstKnotIt = knots.begin();
  KnotIter lastKnotIt = prev(knots.end());

  if (knots.size() == 1) { // 1 knot
    //cout << "oneknot" << endl;
    if (derivative > 0) { 
      return T(); 
    } else { 
      return firstKnotIt->second; 
    }
  }
  
  // handle if query time is lower than first or greater than last
  if (time <= firstKnotIt->first) {
    //cout << "early" << endl;
    if (derivative > 0) { 
      return T(); 
    } else { 
      return firstKnotIt->second; 
    }
  } else if (time >= lastKnotIt->first) {
    //cout << "late" << endl;
    if (derivative > 0) { 
      return T(); 
    } else { 
      return lastKnotIt->second; 
    }
  }


  //cout << "more than 2" << endl;
  // containers for knots, slopes
  pair<double, T> k0, k1, k2, k3;
  T m1, m2;

  KnotIter k1_it = knots.lower_bound(time);
  KnotIter k2_it = next(knots.lower_bound(time));

  k1 = *k1_it;
  k2 = *k2_it;

  if (k1_it == knots.begin()) {
    // make virtual knot
    k0.first = k1.first - (k2.first - k1.first);
    k0.second = k1.second - (k2.second - k1.second);
  } else {
    k0.first = prev(k1_it)->first;
    k0.second = prev(k1_it)->second;
  }

  if (knots.upper_bound(time) == prev(knots.end())) {
    // make virtual knot
    //cout << "making k3" << endl;
    k3.first = k2.first + (k2.first - k1.first);
    k3.second = k2.second + (k2.second - k1.second);
  } else {
    k3.first = next(k2_it)->first;
    k3.second = next(k2_it)->second;
  }
  
  m1 = (k2.first - k1.first) * (k2.second - k0.second) / (k2.first - k0.first);
  m2 = (k2.first - k1.first) * (k3.second - k1.second) / (k3.first - k1.first);

  // normalize time
  double norm_time = (time - k1.first) / (k2.first - k1.first);

  T output;

  if (derivative == 0) {
    output = cubicSplineUnitInterval(k1.second, k2.second, m1, m2, norm_time, derivative);
  } else if (derivative == 1) {
    output = cubicSplineUnitInterval(k1.second, k2.second, m1, m2, norm_time, derivative) / 
             (k2.first - k1.first);
  } else if (derivative == 2) {
    output = cubicSplineUnitInterval(k1.second, k2.second, m1, m2, norm_time, derivative) /
             (pow((k2.first - k1.first), 2));
  } else {
    output = T();
  }

  return output;
}



// Removes the knot closest to the given time,
//    within the given tolerance..
// returns true iff a knot was removed.
template <class T>
inline bool Spline<T>::removeKnot(double time, double tolerance) {
  // Empty maps have no knots.
  if (knots.size() < 1) {
    return false;
  }

  // Look up the first element > or = to time.
  typename std::map<double, T>::iterator t2_iter = knots.lower_bound(time);
  typename std::map<double, T>::iterator t1_iter;
  t1_iter = t2_iter;
  t1_iter--;

  if (t2_iter == knots.end()) {
    t2_iter = t1_iter;
  }

  // Handle tolerance bounds,
  // because we are working with floating point numbers.
  double t1 = (*t1_iter).first;
  double t2 = (*t2_iter).first;

  double d1 = fabs(t1 - time);
  double d2 = fabs(t2 - time);

  if (d1 < tolerance && d1 < d2) {
    knots.erase(t1_iter);
    return true;
  }

  if (d2 < tolerance && d2 < d1) {
    knots.erase(t2_iter);
    return t2;
  }

  return false;
}

// Sets the value of the spline at a given time (i.e., knot),
// creating a new knot at this time if necessary.
template <class T>
inline void Spline<T>::setValue(double time, T value) {
  knots[time] = value;
}

template <class T>
inline T Spline<T>::operator()(double time) {
  return evaluate(time);
}
