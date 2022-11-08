# ---------------------------------------------------------------------
# Project "Track 3D-Objects Over Time"
# Copyright (C) 2020, Dr. Antje Muntzinger / Dr. Andreas Haja.
#
# Purpose of this file : Kalman filter class
#
# You should have received a copy of the Udacity license together with this program.
#
# https://www.udacity.com/course/self-driving-car-engineer-nanodegree--nd013
# ----------------------------------------------------------------------
#

# imports
import numpy as np

# add project directory to python path to enable relative imports
import os
import sys
PACKAGE_PARENT = '..'
SCRIPT_DIR = os.path.dirname(os.path.realpath(os.path.join(os.getcwd(), os.path.expanduser(__file__))))
sys.path.append(os.path.normpath(os.path.join(SCRIPT_DIR, PACKAGE_PARENT)))
import misc.params as params

class Filter:
    '''Kalman filter class'''
    def __init__(self):
        pass

    def F(self):
        # Step 1.1: implement and return system matrix F
        return np.matrix([[1, 0, 0, params.dt, 0, 0],
                        [0, 1, 0, 0, params.dt, 0],
                        [0, 0, 1, 0, 0, params.dt],
                        [0, 0, 0, 1, 0, 0],
                        [0, 0, 0, 0, 1, 0],
                        [0, 0, 0, 0, 0, 1]])

    def Q(self):
        # Step 1.2: implement and return process noise covariance Q
        q1 = ((params.dt**3)/3) * params.q 
        q2 = ((params.dt**2)/2) * params.q 
        q3 = params.dt * params.q 
        return np.matrix([[q1, 0, 0, q2, 0, 0],
                        [0, q1, 0, 0, q2, 0], 
                        [0, 0, q1, 0, 0, q2],
                        [q2, 0, 0, q3, 0, 0],
                        [0, q2, 0, 0, q3, 0],
                        [0, 0, q2, 0, 0, q3]])

    def predict(self, track):
        # Step 1.3: predict state x and estimation error covariance P to next timestep and save x and P in track
        F = self.F()
        x = F * track.x # state prediction
        P = F * track.P * F.transpose() + self.Q() # covariance prediction
        track.set_x(x)
        track.set_P(P)
         

    def update(self, track, meas):
        # Step 1.4: update state x and covariance P with associated measurement, save x and P in track
        gamma = self.gamma(track, meas)
        print("gamma = ", gamma)
        H = meas.sensor.get_H(track.x) 
        S = self.S(track, meas, H)
        K = track.P * H.transpose() * np.linalg.inv(S) # Kalman gain
        x = track.x + K * gamma # state update
        I = np.identity(params.dim_state)
        P = (I - K * H) * track.P # covariance update
        track.set_x(x)
        track.set_P(P)

        track.update_attributes(meas)
    
    def gamma(self, track, meas):
        # Step 1.5: calculate and return residual gamma
        H = meas.sensor.get_H(track.x)
        return (meas.z - H * track.x) # residual

    def S(self, track, meas, H):
        # Step 1.6: calculate and return covariance of residual S
        S = H * track.P * H.transpose() + meas.R # covariance of residual
        return S