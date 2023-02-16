import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

first_100 = np.load("training/episode_100/scores.npy")
next_100 = np.load("training/episode_200/scores.npy")
next_500 = np.load("training/episode_700/scores.npy")
next_740 = np.load("training/episode_1440/scores.npy")
training_scores = np.concatenate((first_100, next_100, next_500, next_740))
scores = pd.Series(training_scores, name="scores")

fig, axes = plt.subplots(1, 1, figsize=(10, 6), sharex=True, sharey=True)
_ = scores.plot(ax=axes, label="Double DQN Scores")
_ = (scores.rolling(window=100)
                      .mean()
                      .rename("Rolling Average")
                      .plot(ax=axes))
_ = axes.legend()
_ = axes.set_ylabel("Score")
_ = axes.set_xlabel("Episode Number")
fig.show()
fig.savefig("figure")

