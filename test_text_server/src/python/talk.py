import threading
import socket
import sys
import struct
import os
import select
import fcntl
import time


from test_text_server.src.protos import message_pb2

sock = None
pipe_read = None
pipe_write = None

def socket_to_stdout():
    buffer = bytes()
    global pipe_write

    while True:
        try:
            r = sock.recv(1)
            if len(r) == 0:
                break
            buffer += r
        except Exception as ex:
            print(ex)
            break

        if len(buffer) < 4:
            continue

        data_size = struct.unpack(">I", buffer[0:4])[0]

        if len(buffer) < data_size + 4:
            continue

        pb_data = buffer[4:data_size+4]
        #print("IN:", ','.join(
        #   [ '{:02x}'.format(x) for x in pb_data ]
        #), " => ", len(pb_data), " bytes")

        buffer = buffer[data_size+4:]

        pb = message_pb2.TextLine()
        pb.ParseFromString(pb_data)
        print(pb.contents)
    sys.stdin.close()
    os.write(pipe_write, b'.')
    sock.close()

def stdin_to_socket():
    global pipe_read
    buffer = ''
    while True:
        (readable, _, _) = select.select([sys.stdin, pipe_read], [], [])
        if pipe_read in readable:
            break
        inp = sys.stdin.read()

        buffer += inp

        #print("BUFFER=", buffer)
        #print("BUFFER:", ','.join(
        #    [ '{:02x}'.format(ord(x)) for x in buffer ]
        #))

        if "\r" in buffer or "\n" in buffer:
            #print("LINE!")
            line, _, buffer = buffer.partition('\r')
            pb = message_pb2.TextLine()
            pb.contents = line
            data = pb.SerializeToString()
            data_size = len(data)
            tx = struct.pack(">I",len(data))

            #print("OUT:", ','.join(
            #    [ '{:02x}'.format(x) for x in tx ] +
            #    [ '{:02x}'.format(x) for x in data ]
            #), " => ", len(tx)+len(data), " bytes")

            sock.send(tx)
            sock.send(data)
    print("OUTPUT QUIT")
    sock.close()

def main():

    global sock
    global pipe_read
    global pipe_write

    fd = sys.stdin.fileno()
    fl = fcntl.fcntl(fd, fcntl.F_GETFL)
    fcntl.fcntl(fd, fcntl.F_SETFL, fl | os.O_NONBLOCK)

    (pipe_read, pipe_write) = os.pipe()

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    sock.connect(("127.0.0.1", 7600))

    threads = [
        threading.Thread(target = socket_to_stdout),
        threading.Thread(target = stdin_to_socket),
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

