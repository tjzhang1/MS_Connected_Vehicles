import gym
import veins_gym

gym.register(
    id="veins-v1",
    entry_point="veins_gym:VeinsEnv",
    kwargs={
        "scenario_dir": "./smartHighway/simulations/interstate_5",
        "run_veins": False,  # do not start veins through Veins-Gym
        "port": 5555,  # pick a port to use
        "timeout": 5.0,  # new timeout value (in seconds)
    },
)

env = gym.make("veins-v1")

env.reset()
