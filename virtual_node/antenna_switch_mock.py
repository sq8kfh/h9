from h9 import H9Frame, send_frame

antenna = 0
id = 100

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
        if frame.data[0] != 10:
            res.type = H9Frame.TYPE_ERROR
            res.data = (H9Frame.ERROR_INVALID_REGISTER,)
            pass
        else:
            res.data = (frame.data[0], antenna)

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
