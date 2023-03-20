import datetime as dt
import math

import numpy as np

import pickle5 as pickle
import pynq
from Helper import Actions

from torch import nn

INPUT_SIZE = 30
OUTPUT_SIZE = 3

NUMBER_OF_SENSOR_FEATURES = 6
START_MOVE_WINDOW_SIZE = 8
PREDICTION_WINDOW_SIZE = 30
REQUIRED_WINDOW_SIZE = PREDICTION_WINDOW_SIZE - START_MOVE_WINDOW_SIZE
THRESHOLD = 0.11


# class MLP(nn.Module):
#     """
#     Multilayer Perceptron.
#     """
#
#     def init(self):
#         super().init()
#         self.layers = nn.Sequential(
#             nn.Linear(INPUT_SIZE, LAYER_1_SIZE),
#             nn.LeakyReLU(),
#             nn.Linear(LAYER_1_SIZE, OUTPUT_SIZE),
#             nn.Softmax(dim=1),
#         )
#
#     def forward(self, x):
#         """Forward pass"""
#         return self.layers(x)


class HWAccel:
    def __init__(self) -> None:
        self.prev = 0
        self.count = 0
        self.total = 0
        self.overlay = pynq.Overlay("mlp_fpga_design.bit")

        self.dma = self.overlay.axi_dma_0

        self.in_buffer = pynq.allocate(shape=(60,), dtype=np.double)
        self.out_buffer = pynq.allocate(shape=(4,), dtype=np.double)
        self.window_data = np.empty((0, NUMBER_OF_SENSOR_FEATURES))

        # self.variables to be used when start of move is identified
        self.start_of_move_flag = 1
        self.action_sample_count = 0
        self.action_arr = np.empty((0, NUMBER_OF_SENSOR_FEATURES))

        # self.pickled_model = pickle.load(open("mlp_model.pkl", "rb"))

    # def get_action(self, msg):
    #     for i, value in enumerate(msg.split(",")):
    #         self.in_buffer[i] = value
    #
    #     self.dma.sendchannel.transfer(self.in_buffer)
    #     self.dma.recvchannel.transfer(self.out_buffer)
    #     action = Actions.glove[self.out_buffer.argmax()]
    #
    #     return action

    def replace_nan(self, arr):
        for idx, val in enumerate(arr[START_MOVE_WINDOW_SIZE - 1]):
            if np.isnan(val):
                arr[START_MOVE_WINDOW_SIZE - 1, idx] = np.nan_to_num(
                    arr[START_MOVE_WINDOW_SIZE - 1, idx],
                    np.median(arr[0 : START_MOVE_WINDOW_SIZE - 2, idx]),
                )

    def get_action(self, data):
        data = np.reshape(data, (1, NUMBER_OF_SENSOR_FEATURES))

        if self.start_of_move_flag == 0:
            # replace any NaNs at the start of data stream with 0
            if len(self.window_data) < START_MOVE_WINDOW_SIZE:
                if np.isnan(data).any():
                    data = np.nan_to_num(data)

            prev_window_data = self.window_data
            self.window_data = np.append(self.window_data, data, axis=0)

            if len(self.window_data) > START_MOVE_WINDOW_SIZE:
                self.window_data = np.delete(self.window_data, 0, axis=0)

                # replace any NaNs with the median of the previous datapoints
                if np.isnan(data).any():
                    self.window_data = self.replace_nan(self.window_data)

                # calculate energy of window and prev window
                rms_acc_curr_window = 0
                rms_acc_prev_window = 0
                for idx, row in enumerate(self.window_data):
                    rms_acc_curr_window = rms_acc_curr_window + self.rmsValue(
                        self.window_data.T[0:3], idx
                    )
                    rms_acc_prev_window = rms_acc_prev_window + self.rmsValue(
                        prev_window_data.T[0:3], idx
                    )

                energy_curr_window = rms_acc_curr_window / START_MOVE_WINDOW_SIZE
                energy_prev_window = rms_acc_prev_window / len(prev_window_data)

                # start of move identifier, find if increase in energy of the windows is > threshold
                if energy_curr_window - energy_prev_window > THRESHOLD:
                    self.action_arr = np.append(
                        self.action_arr, self.window_data, axis=0
                    )
                    self.start_of_move_flag = 1
        else:
            self.action_arr = np.append(self.action_arr, data, axis=0)
            self.action_sample_count = self.action_sample_count + 1
            if self.action_sample_count == REQUIRED_WINDOW_SIZE:
                prediction = self.segment_move(self.action_arr)

                # reset flags and action window
                self.start_of_move_flag = 0
                self.action_sample_count = 1
                self.action_arr = np.empty((0, NUMBER_OF_SENSOR_FEATURES))

                return prediction

        return Actions.no

    def segment_move(self, action_window):
        # find statistical features of action and feed it to model

        action = {
            "acc_x_mean": action_window[:, 0].mean(),
            "acc_x_std": action_window[:, 0].std(),
            "acc_x_rms": self.rmsValue(action_window, 0),
            "acc_x_min": action_window[:, 0].min(),
            "acc_x_max": action_window[:, 0].max(),
            "acc_y_mean": action_window[:, 1].mean(),
            "acc_y_std": action_window[:, 1].std(),
            "acc_y_rms": self.rmsValue(action_window, 1),
            "acc_y_min": action_window[:, 1].min(),
            "acc_y_max": action_window[:, 1].max(),
            "acc_z_mean": action_window[:, 2].mean(),
            "acc_z_std": action_window[:, 2].std(),
            "acc_z_rms": self.rmsValue(action_window, 2),
            "acc_z_min": action_window[:, 2].min(),
            "acc_z_max": action_window[:, 2].max(),
            "gyro_x_mean": action_window[:, 3].mean(),
            "gyro_x_std": action_window[:, 3].std(),
            "gyro_x_rms": self.rmsValue(action_window, 3),
            "gyro_x_min": action_window[:, 3].min(),
            "gyro_x_max": action_window[:, 3].max(),
            "gyro_y_mean": action_window[:, 4].mean(),
            "gyro_y_std": action_window[:, 4].std(),
            "gyro_y_rms": self.rmsValue(action_window, 4),
            "gyro_y_min": action_window[:, 4].min(),
            "gyro_y_max": action_window[:, 4].max(),
            "gyro_z_mean": action_window[:, 5].mean(),
            "gyro_z_std": action_window[:, 5].std(),
            "gyro_z_rms": self.rmsValue(action_window, 5),
            "gyro_z_min": action_window[:, 5].min(),
            "gyro_z_max": action_window[:, 5].max(),
        }

        action_features = np.array(list(action.values()))

        for i, value in enumerate(action_features):
            self.in_buffer[i] = value

        self.dma.sendchannel.transfer(self.in_buffer)
        self.dma.recvchannel.transfer(self.out_buffer)

        self.dma.sendchannel.wait()
        self.dma.recvchannel.wait()

        for x in self.out_buffer:
            print(x)

        # self.out_buffer = self.pickled_model.predict(self.in_buffer)
        prediction = Actions.glove[np.argmax(self.out_buffer)]
        print("prediction:", prediction)
        return prediction

    def rmsValue(self, arr, col):
        square = 0
        mean = 0.0
        root = 0.0
        n = arr.shape[0]

        # Calculate square
        for column_value in arr[:, col]:
            square += pow(column_value, 2)

        # Calculate Mean
        mean = square / (float)(n)

        # Calculate Root
        root = math.sqrt(mean)

        return root