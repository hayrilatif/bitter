from bitter.packet import Packet

pb = Packet()

pb.add_float("test_deger",20,10,30)

print(pb.get_definition())

with open("packet.def", "rb") as f:
    data = f.read()


decoded = pb.decode_binary("packet.def")
print(decoded)