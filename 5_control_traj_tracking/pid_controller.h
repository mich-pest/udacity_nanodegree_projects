/**********************************************
 * Self-Driving Car Nano-degree - Udacity
 *  Created on: December 11, 2020
 *      Author: Mathilde Badoual
 **********************************************/

#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

class PID {
public:

   /**
   * DONE: Create the PID class
   **/

    /*
    * Errors
    */
    double cte;         // Current CTE (Cross Track Error)
    double D_cte;       // derivative of CTE for this time interval
    double D2_cte;      // summation of CTEs

    /*
    * Coefficients
    */
    double tau_p;       // Coefficient for proportional part
    double tau_d;       // Coefficient for derivative part
    double tau_i;       // Coefficient for integral part

    /*
    * Output limits
    */
    double output_lim_mini;
    double output_lim_maxi;
  
    /*
    * Delta time
    */
    double delta_time;  // Small time interval used for derivatives

    /*
    * Constructor
    */
    PID();

    /*
    * Destructor.
    */
    virtual ~PID();

    /*
    * Initialize PID.
    */
    void Init(double Kp, double Ki, double Kd, double output_lim_max, double output_lim_min);

    /*
    * Update the PID error variables given cross track error.
    */
    void UpdateError(double cte);

    /*
    * Calculate the total PID error.
    */
    double TotalError();
  
    /*
    * Update the delta time.
    */
    double UpdateDeltaTime(double new_delta_time);
};

#endif //PID_CONTROLLER_H


