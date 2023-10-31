import struct 

foo = b'\x01\x00'

bar = struct.unpack('h', foo)

print(bar)