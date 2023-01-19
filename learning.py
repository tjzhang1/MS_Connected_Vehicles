import gym
import veins_gym
from veins_gym import veinsgym_pb2

def serialize_action(actions):
    """Serialize a list of floats into an action."""
    reply = veinsgym_pb2.Reply()
#    trafficLightCtl = reply.action.tuple.values.add()
#    RSUBroadcastCtl = reply.action.tuple.values.add()
#    trafficLightCtl.box.values.extend(actions[0])
#    RSUBroadcastCtl.multi_binary.values.extend(actions)
    reply.action.multi_binary.values.extend(actions)
    return reply.SerializeToString()

gym.register(
    id="veins-v1",
    entry_point="veins_gym:VeinsEnv",
    kwargs={
        "action_serializer": serialize_action,
        "scenario_dir": "./smartHighway/simulations/interstate_5",
        "run_veins": False,  # do not start veins through Veins-Gym
        "port": 5555,  # pick a port to use
        "timeout": 1000.0,  # new timeout value (in seconds)
        "print_veins_stdout": True,  # enable (debug) output of veins
    },
)

env = gym.make("veins-v1")

env.reset(return_info=True)
done = False
rewards = []
while not done:
    random_action = env.action_space.sample()
    observation, reward, done, info = env.step(random_action)
    rewards.append(reward)
print("Number of steps taken:", len(rewards))
print("Mean reward:", sum(rewards) / len(rewards))
