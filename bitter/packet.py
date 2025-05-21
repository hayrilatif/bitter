import json, base64

class FloatRepresentation():
    def __init__(self, min_value, max_value, bit_count):
        self. min_value=min_value
        self.max_value=max_value
        self.bit_count=bit_count

    def decode(self, repr):
        levels = (1 << self.bit_count) - 1
        return ((repr * (self.max_value - self.min_value)) / levels) + self.min_value 

    def definition(self):
        return {
            "type": "float",
            "min_value": self.min_value,
            "max_value": self.max_value,
            "bit_count": self.bit_count
        }

class IntRepresentation():
    def __init__(self, bit_count):
        self.bit_count=bit_count

    def decode(self, repr):
        return repr

    def definition(self):
        return {
            "type": "int",
            "bit_count": self.bit_count
        }

class Packet():
    def __init__(self):
        self.packet_meta = {}
    
    def add(self,name, repr):
        self.packet_meta[name] = repr

    def add_int(self,name, bit_count):
        self.packet_meta[name] =IntRepresentation(bit_count)

    def add_float(self,name, bit_count, min_value, max_value ):
        self.packet_meta[name] = FloatRepresentation(min_value, max_value, bit_count)

    # tum paket bilgiini json ile ifade eder.
    def get_definition(self):
        packet_definition = {name: self.packet_meta[name].definition() for name in self.packet_meta}
        return json.dumps(packet_definition)


    def decode_base64(self, code):
        data = base64.b64decode(code)
        as_int = int.from_bytes(data, "little")
        bitpos = 0
        result = {}
        for name in self.packet_meta:
            rep = self.packet_meta[name]
            bit_count = rep.bit_count
            mask = (1 << bit_count) - 1
            raw = (as_int >> bitpos) & mask
            result[name] = rep.decode(raw)
            bitpos += bit_count
        return result

    def decode_binary(self, filename):
        with open(filename, "rb") as f:
            data = f.read()
        as_int = int.from_bytes(data, "little")
        bitpos = 0
        result = {}
        for name in self.packet_meta:
            rep = self.packet_meta[name]
            nbits = rep.bit_count
            mask = (1 << nbits) - 1
            raw = (as_int >> bitpos) & mask
            result[name] = rep.decode(raw)
            bitpos += nbits
        return result
