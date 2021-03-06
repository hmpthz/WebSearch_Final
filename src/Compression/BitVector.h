#pragma once
#include "../global.h"


namespace Bits {
    /* map x: [0, 1] -> [0, 2**BITS-1-sub] */
    template<uint32_t BITS, uint32_t sub = 0>
    inline uint32_t map(float x) {
        constexpr float scale = (float)(1 << BITS) - sub - 0.501f;
        return std::lroundf(x * scale);
    }

    /* normalize q: [0, 2**BITS-1-sub] -> [0, 1) */
    template<uint32_t BITS, uint32_t sub = 0>
    inline float norm(uint32_t q) {
        constexpr float scale = 1.0f / ((float)(1 << BITS) - sub - 0.501f);
        return q * scale;
    }


    /*
        each block contains many bits, which are packed contiguously
        but different blocks may be separated by byte
    */
    class Vec :public std::pmr::vector<uint8_t> {
        using vector::vector;
    public:
        void print_bits(size_t pos, size_t len) {
            for (auto i = pos; i < pos + len; i++) {
                auto b = (*this)[i];
                for (int bit = 0; bit < 8; bit++) {
                    std::printf("%d", (b >> 7) & 1);
                    b = b << 1;
                }
                std::printf(" ");
            }
            std::printf("\n");
        }
    };


    /* start from a byte, sequentially read N bits at a time */
    template<uint32_t BITS>
    class ForwardIter {
    protected:
        Vec* bytes;
        /* pointer of current byte */
        uint8_t* ptr;
        /* current byte which remove bits that've been read */
        uint8_t b;
        /* remaining # of bits that haven't been read in current byte */
        int remain_bits_in_byte;

    public:
        ForwardIter(Vec& a) :bytes(&a), ptr(NULL), b(0), remain_bits_in_byte(0) {}
        /* start from i_th byte */
        void set_byte(size_t i);
        /* get current byte and move pointer to next byte */
        uint8_t next_byte();
        /* get the next N bits, if remain_bits_in_byte are not enough, move to next byte */
        uint32_t next();
    };


    template<uint32_t BITS>
    class BackInserter {
    protected:
        Vec* bytes;
        uint8_t* ptr;
        /* remaining # of bits that haven't been filled in current byte */
        int remain_bits_in_byte;

    public:
        BackInserter(Vec& a) :bytes(&a) { end_byte(); }
        void append_byte(uint8_t b);
        /* append N bits, if remain_bits_in_byte are not enough, append a new byte */
        void append(uint32_t x);
        /* start from a new byte */
        void end_byte();
    };
}


#include "BitVector.ipp"
