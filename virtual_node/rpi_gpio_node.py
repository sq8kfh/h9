from h9 import H9Frame, send_frame

print("Loading:", __name__, "module")
print(__name__, "spec:", __spec__)

print("dir(H9Frame)", dir(H9Frame))
# print("h9:", h9.__name__)
# print(h9.__name__, "spec:", h9.__spec__)

#
# H9Frame(type, destination_id, data, priority, seqnum, source_id, dlc)
#
#f = H9Frame(1, 44, seqnum=12, data=[1])

def on_frame(frame):
    print(__name__, "on_frame()")
    if frame.type == H9Frame.TYPE_GET_REG:
        print("Reg:", frame.data[0])
        res = H9Frame()
        res.priority = H9Frame.PRIORITY_LOW
        res.type = H9Frame.TYPE_REG_VALUE
        res.seqnum = frame.seqnum
        res.destination_id = frame.source_id
        res.source_id = 111
        res.data = (frame.data[0], 100)
        send_frame(res)

#input("Please enter the message:\n")
