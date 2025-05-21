#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

//buffer struct for ease
typedef struct {
    uint8_t *buffer;
    int buffer_size;
    int bit_position;
} PacketBuffer;

//encode
uint32_t encode_fixed(float value, float min_val, float max_val, int n_bits) {
    //minmax fil
    if (value < min_val) value = min_val;
    if (value > max_val) value = max_val;

    uint32_t levels = (1u << n_bits) - 1; //max int
    float scaled = (value - min_val) * levels / (max_val - min_val); //scale
    return (uint32_t)(scaled + 0.5f); // yuvarlama
}

//decode enc
float decode_fixed(uint32_t encoded, float min_val, float max_val, int n_bits) {
    uint32_t levels = (1u << n_bits) - 1; //max int
    return min_val + ((float)encoded * (max_val - min_val) / levels); //scale
}

uint32_t get_nbit_mask(int n_bits) {
    return (1u << n_bits) - 1;
}


void packet_buffer_init(PacketBuffer *packet_buffer, uint8_t *buf, int buf_size){
    packet_buffer->buffer=buf;
    packet_buffer->buffer_size=buf_size;
    memset(buf, 0, buf_size);
    packet_buffer->bit_position=0;
}

//add integer to buff
int packet_buffer_add_int(PacketBuffer * packet_buffer, int bit_count, int value){
    uint32_t field_value;
    field_value = ((uint32_t)value) & get_nbit_mask(bit_count);
}

//add float repr to buffer
int packet_buffer_add_float(PacketBuffer * packet_buffer, int bit_count, float min_value, float max_value, float value){
    uint32_t field_value;
    field_value = encode_fixed(value, min_value,max_value,bit_count);

    for (int i = 0; i < bit_count; i++)
    {
        int byte_index = (packet_buffer->bit_position + i) / 8;
        int bit_index = (packet_buffer->bit_position+i)%8;
        if(byte_index>=packet_buffer->buffer_size) return -1;
        if (field_value & (1u<<i))
        {
            packet_buffer->buffer[byte_index] |= (1u<<bit_index);
        }   
    }
    packet_buffer->bit_position += bit_count;
    
}

//write to file
void write_packet_to_file(const char *filename, const uint8_t *buffer, int bytes_count){
    FILE *file=fopen(filename, "wb");
    if(!file){
        return;
    }
    fwrite(buffer, 1, bytes_count, file);
    fclose(file);
}

//buffer byte count
int packet_buffer_bytes_used(const PacketBuffer *packet_buffer){
    return (packet_buffer->bit_position+7)/8;
}

// test
int main() {
    
    uint8_t buffer[32];
    PacketBuffer packet_buffer;
    packet_buffer_init(&packet_buffer, buffer, sizeof(buffer));

    packet_buffer_add_float(&packet_buffer, 20, 10, 30, 19);

    int packet_size = packet_buffer_bytes_used(&packet_buffer);

    write_packet_to_file("packet.def", buffer, packet_size);

    return 0;
}
