import threading
import socket
import sys
import struct

from test_text_server.src.protos import message_pb2

sock = None

def input():
    buffer = bytes()
    while True:
        try:
            buffer += sock.recv(1)
        except Exception as ex:
            print(ex)
            break

        if len(buffer) < 4:
            continue

        data_size = struct.unpack(">I", buffer[0:4])[0]

        if len(buffer) < data_size + 4:
            continue

        pb_data = buffer[4:data_size+4]
#        print("IN:", ','.join(
#            [ '{:02x}'.format(x) for x in pb_data ]
#        ), " => ", len(pb_data), " bytes")

        buffer = buffer[data_size+4:]

        pb = message_pb2.TextLine()
        pb.ParseFromString(pb_data)
        print(pb.contents)
    return

def output():
    while True:
        line = sys.stdin.readline()
        if line == "":
            break
        pb = message_pb2.TextLine()
        pb.contents = line
        data = pb.SerializeToString()
        data_size = len(data)
        tx = struct.pack(">I",len(data))


#        print("OUT:", ','.join(
#            [ '{:02x}'.format(x) for x in tx ] +
#            [ '{:02x}'.format(x) for x in data ]
#        ), " => ", len(tx)+len(data), " bytes")

        sock.send(tx)
        sock.send(data)
    sock.close()

def main():

    global sock

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    sock.connect(("127.0.0.1", 7600))

    threads = [
        threading.Thread(target = input),
        threading.Thread(target = output),
    ]
    _ = [
        x.start()
        for x
        in threads
    ]

    _ = [
        x.join()
        for x
        in threads
    ]


if __name__ == "__main__":
    main()

