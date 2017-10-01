// https://groups.google.com/forum/#!topic/libmodbus/W0CK3_qmE38

float swap_float(float f)
{
    uint32_t *fp = (uint32_t*)&f;
    *fp = ((*fp << 16) & 0xffff0000) | ((*fp >> 16) & 0x0000ffff);
    return f;
}
