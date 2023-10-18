from h9 import H9Frame, send_frame
import RPi.GPIO as GPIO

print("Loading:", __name__, "module")
print(__name__, "spec:", __spec__)

#
# H9Frame(type, destination_id, data, priority, seqnum, source_id, dlc)
#
#f = H9Frame(1, 44, seqnum=12, data=[1])

relay=3 #GPIO2

reg_10 = 0

GPIO.setmode(GPIO.BOARD)
GPIO.setup(relay, GPIO.OUT)

def on_frame(frame):
    print(__name__, "on_frame()")
    global reg_10
    if frame.type == H9Frame.TYPE_GET_REG:
        print("Reg:", frame.data[0])
        res = H9Frame()
        res.priority = H9Frame.PRIORITY_LOW
        res.type = H9Frame.TYPE_REG_VALUE
        res.seqnum = frame.seqnum
        res.destination_id = frame.source_id
        res.source_id = 100
        res.data = (frame.data[0], reg_10)

        if frame.data[0] == 11:
            res.data = (frame.data[0], GPIO.input(relay))
        send_frame(res)

    elif frame.type == H9Frame.TYPE_SET_REG:
        print("Reg:", frame.data[0])
        res = H9Frame()
        res.priority = H9Frame.PRIORITY_LOW
        res.type = H9Frame.TYPE_REG_EXTERNALLY_CHANGED
        res.seqnum = frame.seqnum
        res.destination_id = frame.source_id
        res.source_id = 100
        res.data = (frame.data[0], reg_10)

        if frame.data[0] == 11:
            if frame.data[1]:
                GPIO.output(relay, GPIO.HIGH)
                res.data = (frame.data[0], 1)
            else:
                GPIO.output(relay, GPIO.LOW)
                res.data = (frame.data[0], 0)

        send_frame(res)
