#ifndef CONF_H
#define CONF_H

namespace simploce {

  /**
   * A space.
   */
  const char SPACE = ' ';

  /**
   * Precison for numbers written to an output stream.
   */
  const int PRECISION = 5;

  /**
   * Field width for numbers written to an output stream.
   */
  const int WIDTH = 13;

  /**
   * A small number.
   */
  const double SMALL = 1.0e-07;

  /**
   * A large number.
   */
  const double LARGE = 1.0e+30;

  /**
   * Radius of water. In nm.
   */
  const double R_WATER{0.14};

}

#endif
