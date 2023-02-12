import gym
import veins_gym
from veins_gym import veinsgym_pb2
from torch import optim
import numpy as np
import DDQN

observation_space = gym.spaces.Tuple((  \
gym.spaces.Box(low=0, high=4000, shape=(1,), dtype=np.float32),  \
gym.spaces.Box(low=np.array([0.0, 0.0, 0.0], dtype=np.float32), high=np.array([100.0, 100.0, 10000.0], dtype=np.float32), dtype=np.float32),  \
gym.spaces.Box(low=np.array([0.0, 0.0, 0.0], dtype=np.float32), high=np.array([100.0, 100.0, 10000.0], dtype=np.float32), dtype=np.float32),  \
gym.spaces.Box(low=np.array([0.0, 0.0, 0.0], dtype=np.float32), high=np.array([100.0, 100.0, 10000.0], dtype=np.float32), dtype=np.float32),  \
gym.spaces.Box(low=np.array([0.0, 0.0, 0.0], dtype=np.float32), high=np.array([100.0, 100.0, 10000.0], dtype=np.float32), dtype=np.float32),  \
gym.spaces.Box(low=np.array([0.0, 0.0, 0.0], dtype=np.float32), high=np.array([100.0, 100.0, 10000.0], dtype=np.float32), dtype=np.float32),  \
gym.spaces.Box(low=np.array([0.0, 0.0, 0.0], dtype=np.float32), high=np.array([100.0, 100.0, 10000.0], dtype=np.float32), dtype=np.float32),  \
gym.spaces.Box(low=np.array([0.0, 0.0, 0.0], dtype=np.float32), high=np.array([100.0, 100.0, 10000.0], dtype=np.float32), dtype=np.float32),  \
gym.spaces.Box(low=np.array([0.0, 0.0, 0.0], dtype=np.float32), high=np.array([100.0, 100.0, 10000.0], dtype=np.float32), dtype=np.float32),  \
gym.spaces.Box(low=np.array([0.0, 0.0, 0.0], dtype=np.float32), high=np.array([100.0, 100.0, 10000.0], dtype=np.float32), dtype=np.float32),  \
gym.spaces.Box(low=np.array([0.0, 0.0, 0.0], dtype=np.float32), high=np.array([100.0, 100.0, 10000.0], dtype=np.float32), dtype=np.float32),  \
gym.spaces.Box(low=np.array([0.0, 0.0, 0.0], dtype=np.float32), high=np.array([100.0, 100.0, 10000.0], dtype=np.float32), dtype=np.float32),  \
gym.spaces.Box(low=np.array([0.0, 0.0, 0.0], dtype=np.float32), high=np.array([100.0, 100.0, 10000.0], dtype=np.float32), dtype=np.float32),  \
gym.spaces.Box(low=np.array([0.0, 0.0, 0.0], dtype=np.float32), high=np.array([100.0, 100.0, 10000.0], dtype=np.float32), dtype=np.float32),  \
gym.spaces.Box(low=np.array([0.0, 0.0, 0.0], dtype=np.float32), high=np.array([100.0, 100.0, 10000.0], dtype=np.float32), dtype=np.float32),  \
gym.spaces.Box(low=np.array([0.0, 0.0, 0.0], dtype=np.float32), high=np.array([100.0, 100.0, 10000.0], dtype=np.float32), dtype=np.float32),  \
gym.spaces.Box(low=np.array([0.0, 0.0, 0.0], dtype=np.float32), high=np.array([100.0, 100.0, 10000.0], dtype=np.float32), dtype=np.float32),  \
gym.spaces.Box(low=np.array([0.0, 0.0, 0.0], dtype=np.float32), high=np.array([100.0, 100.0, 10000.0], dtype=np.float32), dtype=np.float32),  \
gym.spaces.Box(low=np.array([0.0, 0.0, 0.0], dtype=np.float32), high=np.array([100.0, 100.0, 10000.0], dtype=np.float32), dtype=np.float32),  \
gym.spaces.Box(low=np.array([0.0, 0.0, 0.0], dtype=np.float32), high=np.array([100.0, 100.0, 10000.0], dtype=np.float32), dtype=np.float32),  \
gym.spaces.Box(low=np.array([0.0, 0.0, 0.0], dtype=np.float32), high=np.array([100.0, 100.0, 10000.0], dtype=np.float32), dtype=np.float32),  \
gym.spaces.Box(low=np.array([0.0, 0.0, 0.0], dtype=np.float32), high=np.array([100.0, 100.0, 10000.0], dtype=np.float32), dtype=np.float32),  \
gym.spaces.Box(low=np.array([0.0, 0.0, 0.0], dtype=np.float32), high=np.array([100.0, 100.0, 10000.0], dtype=np.float32), dtype=np.float32),  \
gym.spaces.Box(low=np.array([0.0, 0.0, 0.0], dtype=np.float32), high=np.array([100.0, 100.0, 10000.0], dtype=np.float32), dtype=np.float32),  \
gym.spaces.Box(low=np.array([0.0, 0.0, 0.0], dtype=np.float32), high=np.array([100.0, 100.0, 10000.0], dtype=np.float32), dtype=np.float32),  \
gym.spaces.Box(low=np.array([0.0, 0.0, 0.0], dtype=np.float32), high=np.array([100.0, 100.0, 10000.0], dtype=np.float32), dtype=np.float32),  \
gym.spaces.Box(low=np.array([0.0, 0.0, 0.0], dtype=np.float32), high=np.array([100.0, 100.0, 10000.0], dtype=np.float32), dtype=np.float32)   \
))

action_space = gym.spaces.MultiBinary(5)

def serialize_action(actions):
    """Serialize a list of floats into an action."""
    reply = veinsgym_pb2.Reply()
    reply.action.multi_binary.values.extend(actions)
    return reply.SerializeToString()

gym.register(
    id="veins-v1",
    entry_point="veins_gym:VeinsEnv",
    kwargs={
        "action_serializer": serialize_action,
        "scenario_dir": "./smartHighway/simulations/interstate_5",
        "run_veins": True,  # do not start veins through Veins-Gym
        "port": 5555,  # pick a port to use
        "timeout": 1000.0,  # new timeout value (in seconds)
        "print_veins_stdout": True,  # enable (debug) output of veins
    },
)

# Run training on veins env
env = gym.make("veins-v1")
_epsilon_decay_schedule_kwargs = {
    "decay_factor": 0.99,
    "minimum_epsilon": 1e-2,
}
epsilon_decay_schedule = lambda n: DDQN.power_decay_schedule(n, **_epsilon_decay_schedule_kwargs)
_optimizer_kwargs = {
    "lr": 1e-3,
    "betas": (0.9, 0.999),
    "eps": 1e-08,
    "weight_decay": 0,
    "amsgrad": False,
}
optimizer_fn = lambda parameters: optim.Adam(parameters, **_optimizer_kwargs)
_agent_kwargs = {
    "state_size": gym.spaces.utils.flatdim(observation_space),
    "action_size": 1<<action_space.n, 
    "number_hidden_units": 64,
    "optimizer_fn": optimizer_fn,
    "epsilon_decay_schedule": epsilon_decay_schedule,
    "batch_size": 64,
    "buffer_size": 100000,
    "alpha": 1e-3,
    "gamma": 0.99,
    "update_frequency": 4,
    "double_dqn": True,  # True uses Double DQN; False uses DQN 
    "seed": None,
    "load_checkpoint_path": None
}
double_dqn_agent = DDQN.DeepQAgent(**_agent_kwargs)

double_dqn_scores = DDQN.train(double_dqn_agent,
                          env,
                          "double-dqn-checkpoint.pth",
                          number_episodes=100,
                          target_score=200)

# Graph output
import pandas as pd
import matplotlib.pyplot as plt

double_dqn_scores = pd.Series(double_dqn_scores, name="scores")

fig, axes = plt.subplots(1, 1, figsize=(10, 6), sharex=True, sharey=True)
_ = double_dqn_scores.plot(ax=axes, label="Double DQN Scores")
_ = (double_dqn_scores.rolling(window=100)
                      .mean()
                      .rename("Rolling Average")
                      .plot(ax=axes))
_ = axes.legend()
_ = axes.set_ylabel("Score")
_ = axes.set_xlabel("Episode Number")
fig.show()
fig.savefig("training_results")

np.save("scores", double_dqn_scores)

'''
# Previous training script
env.reset(return_info=True)
done = False
rewards = []
while not done:
    random_action = env.action_space.sample()
    observation, reward, done, info = env.step(random_action)
    rewards.append(reward)
print("Number of steps taken:", len(rewards))
print("Mean reward:", sum(rewards) / len(rewards))
'''
