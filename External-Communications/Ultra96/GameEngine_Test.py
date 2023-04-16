import multiprocessing as mp
import time

from EvalClient import EvalClient

from GameEngine import GameEngine

from Test import get_queue, print_logs


init_json = '{ "p1": { "hp": 100, "action": "none", "bullets": 6, "grenades": 2, "shield_time": 0, "shield_health": 0, "num_deaths": 0, "num_shield": 3 }, "p2": { "hp": 100, "action": "none", "bullets": 6, "grenades": 2, "shield_time": 0, "shield_health": 0, "num_deaths": 0, "num_shield": 3 } } '

actions = {
    "1h": "p1_hit",
    "2h": "p2_hit",
    "1f": "p1_shoot",
    "2f": "p2_shoot",
    "1g": "p1_grenade",
    "2g": "p2_grenade",
    "1s": "p1_shield",
    "2s": "p2_shield",
    "1r": "p1_reload",
    "2r": "p2_reload",
}
connected_to_eval = mp.Value("i", 1)
opp_in_frames = mp.Array("i", [0] * 2)
action_queue = mp.Queue()
update_beetle_queue = mp.Queue()
eval_req_queue = mp.Queue()
eval_resp_queue = mp.Queue()
eval_req_console_queue = mp.Queue()
eval_resp_console_queue = mp.Queue()
vis_queue = mp.Queue()
logs_queue = mp.Queue()

engine = GameEngine(
    connected_to_eval,
    opp_in_frames,
    action_queue,
    update_beetle_queue,
    eval_req_queue,
    eval_resp_queue,
    vis_queue,
    eval_req_console_queue,
    eval_resp_console_queue,
    logs_queue,
)

evalclient = EvalClient("127.0.0.1", 2108, eval_req_queue, eval_resp_queue, logs_queue)
evalclient.connect()


def put_actions(action_queue):
    turns = [
        [
            "p1_hit",
            "p2_shoot",
        ],
        [
            "p1_hit",
            "p2_shoot",
            "p1_reload",
        ],
        [
            "p1_hit",
            "p2_shoot",
            "p1_reload",
        ],
        [
            "p1_shoot",
            "p2_hit",
            "p2_grenade",
        ],
        [
            "p1_reload",
            "p2_grenade",
        ],
        [
            "p1_reload",
            "p2_grenade",
        ],
        [
            "p1_hit",
            "p2_shoot",
            "p1_reload",
        ],
        [
            "p1_shoot",
            "p2_hit",
            "p2_grenade",
        ],
    ]

    for turn in turns:
        for x in turn:
            time.sleep(0.5)
            player, action = x.split("_")
            if x.endswith("grenade"):
                opp_in_frames[1] = 1
            else:
                opp_in_frames[1] = 0
            action_queue.put((player, action))
            eval_resp_queue.put(init_json)

        time.sleep(3)


engine_process = mp.Process(target=engine.run)
eval_client_process = mp.Process(target=evalclient.run)
vis_queue_process = mp.Process(target=get_queue, args=(vis_queue,))
eval_queue_process = mp.Process(target=get_queue, args=(eval_req_queue,))
eval_queue_process = mp.Process(target=get_queue, args=(eval_req_queue,))
action_process = mp.Process(target=put_actions, args=(action_queue,))

log_process = mp.Process(target=print_logs, args=(logs_queue,))

try:
    engine_process.start()
    eval_client_process.start()

    action_process.start()
    vis_queue_process.start()
    eval_queue_process.start()
    log_process.start()

    engine_process.join()
    eval_client_process.join()
    action_process.join()
    vis_queue_process.join()
    eval_queue_process.join()
    log_process.join()
except KeyboardInterrupt:
    print("\nShutting Down")
    engine_process.terminate()
    eval_client_process.terminate()
    action_process.terminate()
    vis_queue_process.terminate()
    eval_queue_process.terminate()
    log_process.terminate()
