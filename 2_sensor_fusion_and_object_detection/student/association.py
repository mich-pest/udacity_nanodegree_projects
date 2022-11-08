# ---------------------------------------------------------------------
# Project "Track 3D-Objects Over Time"
# Copyright (C) 2020, Dr. Antje Muntzinger / Dr. Andreas Haja.
#
# Purpose of this file : Data association class with single nearest neighbor association and gating based on Mahalanobis distance
#
# You should have received a copy of the Udacity license together with this program.
#
# https://www.udacity.com/course/self-driving-car-engineer-nanodegree--nd013
# ----------------------------------------------------------------------
#

# imports
import numpy as np
from scipy.stats.distributions import chi2

# add project directory to python path to enable relative imports
import os
import sys
PACKAGE_PARENT = '..'
SCRIPT_DIR = os.path.dirname(os.path.realpath(os.path.join(os.getcwd(), os.path.expanduser(__file__))))
sys.path.append(os.path.normpath(os.path.join(SCRIPT_DIR, PACKAGE_PARENT)))

import misc.params as params 

class Association:
    '''Data association class with single nearest neighbor association and gating based on Mahalanobis distance'''
    def __init__(self):
        self.association_matrix = np.matrix([])
        self.unassigned_tracks = []
        self.unassigned_meas = []
        
    def associate(self, track_list, meas_list, KF):
        # Step 3: association:
        # - replace association_matrix with the actual association matrix based on Mahalanobis distance (see below) for all tracks and all measurements
        # - update list of unassigned measurements and unassigned tracks
        N = len(track_list) # N tracks
        M = len(meas_list) # M measurements

        self.association_matrix = np.matrix([])  # reset matrix
        self.unassigned_tracks = []  # reset lists
        self.unassigned_meas = []

        if len(meas_list) > 0:
            self.unassigned_meas = list(range(M)) # [*range(M)] same as list(range(M)), ok from Python 3.5 
        if len(track_list) > 0:
            self.unassigned_tracks = list(range(N)) # [*range(N)] same as list(range(N)), ok from Python 3.5 
        if len(meas_list) > 0 and len(track_list) > 0:
            self.association_matrix = np.asmatrix(np.inf * np.ones((N, M)))

        # fill association matrix with Mahalanobis distances between all tracks and all measurements
        for i in range(N):
            for j in range(M):
                dist = self.MHD(track_list[i], meas_list[j], KF)
                if self.gating(dist, meas_list[j].sensor):
                    self.association_matrix[i,j] = dist
                
    def get_closest_track_and_meas(self):
        # Step 3: find closest track and measurement:
        # - find minimum entry in association matrix
        # - delete row and column
        # - remove corresponding track and measurement from unassigned_tracks and unassigned_meas
        # - return this track and measurement

        min_row, min_col = np.unravel_index(np.argmin(self.association_matrix, axis=None), self.association_matrix.shape)
        d = self.association_matrix[min_row, min_col]
        if d < np.inf:
            # delete row and column in association matrix for closest track and measurement
            self.association_matrix = np.delete(self.association_matrix, min_row, 0)
            self.association_matrix = np.delete(self.association_matrix, min_col, 1)

            # update this track with this measurement
            update_track = self.unassigned_tracks[min_row] 
            update_meas = self.unassigned_meas[min_col]

            # remove from list
            self.unassigned_tracks.remove(update_track) 
            self.unassigned_meas.remove(update_meas)
            return update_track, update_meas  
             
        else:
            return np.nan, np.nan  

    def gating(self, MHD, sensor): 
        # Step 3: return True if measurement lies inside gate, otherwise False
        d_chi = chi2.ppf(params.gating_threshold, sensor.dim_meas)
        if MHD < d_chi:
            return True
        else:
            return False
        
    def MHD(self, track, meas, KF):
        # Step 3: calculate and return Mahalanobis distance
        H = meas.sensor.get_H(track.x)
        S = KF.S(track, meas, H)
        mhd = np.transpose(meas.z - H * track.x) * np.linalg.inv(S) * (meas.z - H * track.x)
        
        return mhd
    
    def associate_and_update(self, manager, meas_list, KF):
        # associate measurements and tracks
        self.associate(manager.track_list, meas_list, KF)
    
        # update associated tracks with measurements
        while self.association_matrix.shape[0]>0 and self.association_matrix.shape[1]>0:
            
            # search for next association between a track and a measurement
            ind_track, ind_meas = self.get_closest_track_and_meas()
            if np.isnan(ind_track):
                print('---no more associations---')
                break
            track = manager.track_list[ind_track]
            
            # check visibility, only update tracks in fov    
            if not meas_list[0].sensor.in_fov(track.x):
                continue
            
            # Kalman update
            print('update track', track.id, 'with', meas_list[ind_meas].sensor.name, 'measurement', ind_meas)
            KF.update(track, meas_list[ind_meas])
            
            # update score and track state 
            manager.handle_updated_track(track)
            
            # save updated track
            manager.track_list[ind_track] = track
            
        # run track management 
        manager.manage_tracks(self.unassigned_tracks, self.unassigned_meas, meas_list)
        
        for track in manager.track_list:            
            print('track', track.id, 'score =', track.score)