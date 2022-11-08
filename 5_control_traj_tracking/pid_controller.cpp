/**********************************************
 * Self-Driving Car Nano-degree - Udacity
 *  Created on: December 11, 2020
 *      Author: Mathilde Badoual
 **********************************************/

#include "pid_controller.h"
#include <vector>
#include <iostream>
#include <math.h>

#define INITIAL_ERROR 0.0

using namespace std;

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kpi, double Kii, double Kdi, double output_lim_maxi, double output_lim_mini) {
   /**
   * DONE: Initialize PID coefficients (and errors, if needed)
   **/
   this->tau_p = Kpi;
   this->tau_i = Kii;
   this->tau_d = Kdi;
   this->delta_time = 0;

   this->output_lim_maxi = output_lim_maxi;
   this->output_lim_mini = output_lim_mini;

   this->cte      = INITIAL_ERROR;
   this->D_cte    = INITIAL_ERROR;
   this->D2_cte   = INITIAL_ERROR;

}


void PID::UpdateError(double cte) {
   /**
   * DONE: Update PID errors based on cte.
   **/
   if (this->delta_time > 0)
      this->D_cte = (cte - this->cte) / this->delta_time;  // Derivative = (y_t - y_t-1)/ delta_t
   else    
      this->D_cte = 0.0;

   this->D2_cte   += cte * this->delta_time;
   this->cte      =  cte;
}

double PID::TotalError() {
   /**
   * DONE: Calculate and return the total error
    * The code should return a value in the interval [output_lim_mini, output_lim_maxi]
   */
    double control = (this->tau_p * this->cte) + (tau_d * this->D_cte) + (tau_i * this->D2_cte);
    if(control > this->output_lim_maxi){
      control = this->output_lim_maxi;
    }else if(control < this->output_lim_mini){
      control = this->output_lim_mini;
    }
    return control;
}

double PID::UpdateDeltaTime(double new_delta_time) {
   /**
   * DONE: Update the delta time with new value
   */
  this->delta_time = new_delta_time;
  return this->delta_time;
}