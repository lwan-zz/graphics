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
    cout << "derivative not supported" << endl;
    exit(1);
  }

  /*
  cout << h00 << endl;
  cout << h10 << endl;
  cout << h01 << endl;
  cout << h11 << endl;
  */

  T output = h00 * position0 + h10 * tangent0 + h01 * position1 + h11 * tangent1;
  //cout << "----/cubicSplineUnitInterval------" << endl;
  return output;
}

// Returns a state interpolated between the values directly before and after the
// given time.
template <class T>
inline T Spline<T>::evaluate(double time, int derivative) {
  // TODO (Animation) Task 1b

  // Handle some edge cases specified in wiki
  if (knots.size() < 1) { // no knots
    return T(); 
  } else if (knots.size() == 1) { // 1 knot
    if (derivative > 0) { return T(); }
    else { return knots.begin()->second; }
  }
  
  // handle if query time is lower than first or greater than last
  if (time < knots.begin()->first) {
    //cout << "time < knots begin" << endl;
    //cout << "first knot: " << knots.begin()->first << endl;
    if (derivative > 0) { 
      return T(); 
    } else { 
      return knots.begin()->second; 
    }
  } else if (time > (--knots.end())->first) {
    //cout << "time > knots end" << endl;
    //cout << "last knot: " << (--knots.end())->first << endl;
    if (derivative > 0) { 
      return T(); 
    } else { 
      return knots.end()->second; 
    }
  }

  // normalize time
  double norm_time = (time - knots.begin()->first) / ((--knots.end())->first - knots.begin()->first);
  //cout << "time: " << time << endl;
  //cout << "norm_time: " << norm_time << endl;
  //cout << "num knots: " << knots.size() << endl;

  // temporary containers for knots, slopes
  pair<double, T> k0, k1, k2, k3;
  T m1, m2;

  // print knots
  /*
  cout << "all knots" << endl;
  for (KnotIter ki = knots.begin(); ki != knots.end(); ki++) {
    //cout << "t: " << ki->first << endl;
    ki->second.print();
  }
  */
  
  k1 = *(knots.lower_bound(time));
  k2 = *(knots.upper_bound(time));

  //cout << "knot points" << endl;
  if (abs(knots.lower_bound(time)->first - knots.begin()->first) < EPS_D) {
    // make virtual knot
    //cout << "making k0" << endl;
    k0.first = k1.first - (k2.first - k1.first);
    k0.second = k1.second - (k2.second - k1.second);
  }

  if (abs(knots.upper_bound(time)->first - (--knots.end())->first) < EPS_D) {
    // make virtual knot
    //cout << "making k3" << endl;
    k3.first = k2.first + (k2.first - k1.first);
    k3.second = k2.second + (k2.second - k1.second);
  }
  
  /*
  k0.second.print();
  k1.second.print();
  k2.second.print();
  k3.second.print();  
  */
  
  //cout << "tangents" << endl;
  
  m1 = (k2.second - k0.second) / (k2.first - k0.first);
  m2 = (k3.second - k1.second) / (k3.first - k1.first);

  if (k2.first - k0.first < EPS_D) {
    m1 = Vector3D();
  }
  if (k3.first - k1.first < EPS_D) {
    m2 = Vector3D();
  }
  
  //m1.print();
  //m2.print();

  T output = cubicSplineUnitInterval(k1.second, k2.second, m1, m2, norm_time, derivative);
  
  /*
  cout << "output" << endl;
  output.print();
  cout << "--------" << endl;
  getchar();
  */

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
