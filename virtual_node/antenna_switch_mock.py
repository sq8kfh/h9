from h9 import H9Frame, send_frame

antenna = 0
id = 32
reg = {
    "antenna": 0,
    "current": 0,
    "num_of_ant": 8,
    "ant_1": "an 1",
    "ant_2": "an 2",
    "ant_3": "an 3",
    "ant_4": "an 4",
    "ant_5": "an 5",
    "ant_6": "an 6",
    "ant_7": "an 7",
    "ant_8": "an 8"
}

def on_frame(frame):
    print(__name__, "on_frame()")
    global antenna
    if frame.type == H9Frame.TYPE_GET_REG:
        print("Reg:", frame.data[0])
        res = H9Frame()
        res.priority = H9Frame.PRIORITY_LOW
        res.type = H9Frame.TYPE_REG_VALUE
        res.seqnum = frame.seqnum
        res.destination_id = frame.source_id
        res.source_id = id
        if frame.data[0] == 10:
            res.data = (frame.data[0], reg["antenna"])
        elif frame.data[0] == 11:
            res.data = (frame.data[0], *reg["current"].to_bytes(2, 'big'))
        elif frame.data[0] == 12:
            res.data = (frame.data[0], reg["num_of_ant"])
        elif frame.data[0] == 13:
            tmp = (reg["ant_1"] + "\0\0\0\0\0\0").encode()[:6]
            res.data = (frame.data[0], *tmp)
        elif frame.data[0] == 14:
            tmp = (reg["ant_2"] + "\0\0\0\0\0\0").encode()[:6]
            res.data = (frame.data[0], *tmp)
        elif frame.data[0] == 15:
            tmp = (reg["ant_3"] + "\0\0\0\0\0\0").encode()[:6]
            res.data = (frame.data[0], *tmp)
        elif frame.data[0] == 16:
            tmp = (reg["ant_4"] + "\0\0\0\0\0\0").encode()[:6]
            res.data = (frame.data[0], *tmp)
        elif frame.data[0] == 17:
            tmp = (reg["ant_5"] + "\0\0\0\0\0\0").encode()[:6]
            res.data = (frame.data[0], *tmp)
        elif frame.data[0] == 18:
            tmp = (reg["ant_6"] + "\0\0\0\0\0\0").encode()[:6]
            res.data = (frame.data[0], *tmp)
        elif frame.data[0] == 19:
            tmp = (reg["ant_7"] + "\0\0\0\0\0\0").encode()[:6]
            res.data = (frame.data[0], *tmp)
        elif frame.data[0] == 20:
            tmp = (reg["ant_8"] + "\0\0\0\0\0\0").encode()[:6]
            res.data = (frame.data[0], *tmp)
        else:
            res.type = H9Frame.TYPE_ERROR
            res.data = (H9Frame.ERROR_INVALID_REGISTER,)
        send_frame(res)

    elif frame.type == H9Frame.TYPE_SET_REG:
        print("Reg:", frame.data[0])

        res = H9Frame()
        res.priority = H9Frame.PRIORITY_LOW
        res.type = H9Frame.TYPE_REG_EXTERNALLY_CHANGED
        res.seqnum = frame.seqnum
        res.destination_id = frame.source_id
        res.source_id = id

        if frame.data[0] != 10:
            res.type = H9Frame.TYPE_ERROR
            res.data = (H9Frame.ERROR_INVALID_REGISTER,)
            pass
        else:
            antenna = frame.data[1]
            res.data = (frame.data[0], antenna)

        send_frame(res)
