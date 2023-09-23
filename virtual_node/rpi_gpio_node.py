import h9

print(dir(h9.H9Frame))

print(type(h9.H9Frame))

frame = h9.H9Frame()
print(dir(frame.seqnum))
#
print(h9.H9Frame.seqnum)

frame.seqnum = 12
print(frame.seqnum)

frame.seqnum = 1200000000000
print(frame.seqnum)

def on_frame(frame):
    print(type(frame))
    print("Yuppi! ", frame.seqnum)

#input("Please enter the message:\n")
