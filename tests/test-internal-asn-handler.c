//
// Created by Anton Sakharov on 2019-06-26.
//

#include <stdio.h>
#include <stdlib.h>
#include "asn_processor/ak_asn_codec_new.h"

/* Исходные данные */
ak_byte test_data[] = {0x30, 0x82, 0x03, 0x39, 0x02, 0x01, 0x00, 0xA0, 0x42, 0x30, 0x40, 0x04, 0x10, 0x8B, 0x78, 0x48, 0x50, 0x1A, 0x78, 0x91, 0xBF, 0x8C, 0x15, 0x5B, 0x80, 0x15, 0x8A, 0x28, 0x5E, 0xA0, 0x2C, 0x30, 0x2A, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x05, 0x0C, 0x30, 0x1D, 0x04, 0x08, 0xE9, 0xC2, 0xCF, 0xAB, 0x57, 0xFE, 0x3E, 0x9D, 0x02, 0x02, 0x07, 0xD0, 0x02, 0x01, 0x20, 0x30, 0x0A, 0x06, 0x08, 0x2A, 0x85, 0x03, 0x07, 0x01, 0x01, 0x04, 0x02, 0x30, 0x82, 0x02, 0xEE, 0xA3, 0x82, 0x01, 0x73, 0xA0, 0x82, 0x01, 0x6F, 0xBB, 0x82, 0x01, 0x6B, 0x06, 0x08, 0x2A, 0x85, 0x03, 0x07, 0x01, 0x01, 0x05, 0x01, 0x30, 0x82, 0x01, 0x5D, 0x30, 0x12, 0x0C, 0x10, 0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x74, 0x65, 0x73, 0x74, 0x20, 0x6B, 0x65, 0x79, 0x30, 0x39, 0x04, 0x10, 0xD0, 0x35, 0x6A, 0x5A, 0x3B, 0xFD, 0x4D, 0xB6, 0xF6, 0x5D, 0x82, 0x78, 0xB4, 0xA5, 0x69, 0x9B, 0x03, 0x03, 0x06, 0xC0, 0x00, 0x18, 0x0F, 0x32, 0x30, 0x31, 0x39, 0x30, 0x35, 0x32, 0x31, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x5A, 0x80, 0x0F, 0x32, 0x30, 0x32, 0x30, 0x30, 0x35, 0x32, 0x31, 0x32, 0x33, 0x35, 0x39, 0x35, 0x39, 0x5A, 0xA1, 0x82, 0x01, 0x0A, 0xA2, 0x82, 0x01, 0x06, 0x02, 0x01, 0x02, 0x31, 0x81, 0x86, 0xA2, 0x81, 0x83, 0x02, 0x01, 0x04, 0x30, 0x12, 0x04, 0x10, 0x8B, 0x78, 0x48, 0x50, 0x1A, 0x78, 0x91, 0xBF, 0x8C, 0x15, 0x5B, 0x80, 0x15, 0x8A, 0x28, 0x5E, 0x30, 0x1E, 0x06, 0x07, 0x2A, 0x85, 0x03, 0x02, 0x02, 0x0D, 0x01, 0x30, 0x13, 0x06, 0x07, 0x2A, 0x85, 0x03, 0x02, 0x02, 0x1F, 0x01, 0x04, 0x08, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x04, 0x4A, 0x30, 0x48, 0x04, 0x40, 0x7D, 0xD1, 0xF0, 0x00, 0x6D, 0x82, 0x65, 0x30, 0xF0, 0x3E, 0x5A, 0x4D, 0x48, 0xF9, 0x63, 0xB0, 0xED, 0x77, 0x8E, 0x66, 0x96, 0x3B, 0x9F, 0xB5, 0x8B, 0xC4, 0x2C, 0x0A, 0xE7, 0x91, 0x27, 0xF4, 0xC4, 0xD1, 0xC5, 0x21, 0xB5, 0xB0, 0x0E, 0xB4, 0xDD, 0xBD, 0x10, 0x08, 0x6D, 0x16, 0xAD, 0xCE, 0xEA, 0xC7, 0xA6, 0x07, 0x9D, 0xFD, 0xEC, 0xAF, 0xD9, 0x7B, 0x84, 0x00, 0x9E, 0x37, 0x4C, 0xC9, 0x04, 0x04, 0xFF, 0xFF, 0xFF, 0xFF, 0x30, 0x78, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x07, 0x01, 0x30, 0x1F, 0x06, 0x08, 0x2A, 0x85, 0x03, 0x02, 0x04, 0x03, 0x02, 0x02, 0x30, 0x13, 0x04, 0x08, 0x23, 0xBE, 0x18, 0x7F, 0xDF, 0xB0, 0x67, 0xEB, 0x06, 0x07, 0x2A, 0x85, 0x03, 0x02, 0x02, 0x1F, 0x01, 0x80, 0x4A, 0x04, 0x44, 0xC2, 0xC2, 0x3B, 0xC3, 0x69, 0x30, 0xA8, 0x7F, 0x02, 0xBE, 0x6F, 0x68, 0x64, 0x10, 0xFB, 0xFE, 0x9E, 0x77, 0x3B, 0x89, 0xAD, 0x91, 0x4E, 0x39, 0x22, 0x72, 0x23, 0xDC, 0x60, 0xFE, 0x74, 0x55, 0xA5, 0x12, 0xE9, 0x2D, 0xA3, 0xA6, 0xB1, 0xE9, 0xF6, 0x25, 0x89, 0x01, 0xB7, 0xB4, 0xE7, 0xCE, 0xDB, 0x0D, 0xC4, 0xE4, 0xF2, 0x74, 0x1A, 0x03, 0x41, 0x56, 0x64, 0x31, 0x17, 0x47, 0x9A, 0x3B, 0x00, 0x08, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xA3, 0x82, 0x01, 0x73, 0xA0, 0x82, 0x01, 0x6F, 0xBB, 0x82, 0x01, 0x6B, 0x06, 0x08, 0x2A, 0x85, 0x03, 0x07, 0x01, 0x01, 0x05, 0x01, 0x30, 0x82, 0x01, 0x5D, 0x30, 0x12, 0x0C, 0x10, 0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x74, 0x65, 0x73, 0x74, 0x20, 0x6B, 0x65, 0x79, 0x30, 0x39, 0x04, 0x10, 0x91, 0xDB, 0x82, 0xFC, 0x71, 0x4C, 0x57, 0x7E, 0x21, 0x7E, 0xEB, 0x79, 0x6A, 0x20, 0x83, 0xB1, 0x03, 0x03, 0x06, 0xC0, 0x00, 0x18, 0x0F, 0x32, 0x30, 0x31, 0x39, 0x30, 0x35, 0x32, 0x31, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x5A, 0x80, 0x0F, 0x32, 0x30, 0x32, 0x30, 0x30, 0x35, 0x32, 0x31, 0x32, 0x33, 0x35, 0x39, 0x35, 0x39, 0x5A, 0xA1, 0x82, 0x01, 0x0A, 0xA2, 0x82, 0x01, 0x06, 0x02, 0x01, 0x02, 0x31, 0x81, 0x86, 0xA2, 0x81, 0x83, 0x02, 0x01, 0x04, 0x30, 0x12, 0x04, 0x10, 0x8B, 0x78, 0x48, 0x50, 0x1A, 0x78, 0x91, 0xBF, 0x8C, 0x15, 0x5B, 0x80, 0x15, 0x8A, 0x28, 0x5E, 0x30, 0x1E, 0x06, 0x07, 0x2A, 0x85, 0x03, 0x02, 0x02, 0x0D, 0x01, 0x30, 0x13, 0x06, 0x07, 0x2A, 0x85, 0x03, 0x02, 0x02, 0x1F, 0x01, 0x04, 0x08, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x04, 0x4A, 0x30, 0x48, 0x04, 0x40, 0xA8, 0x30, 0xFE, 0x49, 0xB7, 0x75, 0x09, 0xA9, 0xEC, 0x7A, 0x8B, 0x6E, 0x2C, 0xD3, 0xFB, 0x72, 0x02, 0xCA, 0xB0, 0xA3, 0x2E, 0xAC, 0xFC, 0x65, 0x04, 0x72, 0xCE, 0x37, 0x89, 0xC7, 0x04, 0xD3, 0x99, 0x4E, 0x99, 0x14, 0x8B, 0x85, 0x43, 0x74, 0xCF, 0x4D, 0x3C, 0x97, 0x48, 0xFA, 0xEA, 0x39, 0x47, 0x30, 0x89, 0x7A, 0xB0, 0x93, 0x39, 0xAA, 0xB5, 0x12, 0x34, 0x42, 0x4A, 0x6C, 0xC8, 0x87, 0x04, 0x04, 0xFF, 0xFF, 0xFF, 0xFF, 0x30, 0x78, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x07, 0x01, 0x30, 0x1F, 0x06, 0x08, 0x2A, 0x85, 0x03, 0x02, 0x04, 0x03, 0x02, 0x02, 0x30, 0x13, 0x04, 0x08, 0x5E, 0x53, 0xFC, 0xEE, 0x3D, 0x56, 0xED, 0x40, 0x06, 0x07, 0x2A, 0x85, 0x03, 0x02, 0x02, 0x1F, 0x01, 0x80, 0x4A, 0x04, 0x44, 0x55, 0xD7, 0xBC, 0x71, 0x76, 0x74, 0xB9, 0x41, 0xE4, 0x4B, 0x0C, 0xAC, 0x2E, 0x5D, 0x97, 0xAB, 0xD1, 0xD6, 0xB3, 0x1F, 0x8D, 0x9C, 0x10, 0x7C, 0x92, 0xAB, 0x34, 0x88, 0x0A, 0x7F, 0xBD, 0xFF, 0x3A, 0x22, 0xB6, 0x04, 0xE5, 0xA0, 0xB9, 0xCE, 0x00, 0xF6, 0x38, 0xA6, 0xFC, 0xEC, 0x96, 0x32, 0x3C, 0x8B, 0x06, 0x1B, 0x62, 0x2C, 0x9F, 0x13, 0x8B, 0x1C, 0xA2, 0xC8, 0x3B, 0x39, 0xCA, 0x36, 0x00, 0x08, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF};


static void asn_print_universal(tag data_tag, ak_uint32 data_len, ak_byte* p_data)
{
    bit_string bit_string_data;
    if ((data_tag & UNIVERSAL) == 0)
    {
        switch (data_tag & 0x1F)
        {
        case TBOOLEAN:
            if(*p_data == 0x00)
                printf("False\n");
            else
                printf("True\n");
            break;
        case TINTEGER:
            //FIXME: переделать под вовод нормельного значения
            ak_asn_print_hex_data(p_data, data_len);
            break;
        case TBIT_STRING:
            new_asn_get_bitstr(p_data, data_len, &bit_string_data);

            for(size_t i = 0; i < bit_string_data.m_val_len; i++)
            {
                ak_uint8 unused_bits = 0;
                if (i == bit_string_data.m_val_len - 1)
                    unused_bits = bit_string_data.m_unused;

                for(ak_int8 j = 7; j >= (ak_int8)unused_bits; j--)
                {
                    ak_uint8 bit = (bit_string_data.mp_value[i] >> j) & (ak_uint8)0x01;
                    printf("%u", bit);
                }
            }
            putchar('\n');

            break;
        default: printf("bad data");
        }
    }

}

int main(void)
{
    /* Структура, хранящая результат декодирования данных */
    s_asn_tlv_t root_tlv;

    /* Массив закодированных данных */
    ak_byte* p_encoded_data;
    /* Размер данных после кодирования */
    ak_uint32 size = 0;

    /* Результат теста */
    bool_t test_result = ak_true;

    /* Инициализируем библиотеку */
    if (ak_libakrypt_create(ak_function_log_stderr) != ak_true)
    {
        return ak_libakrypt_destroy();
    }

    /* Декодируем данные */
    ak_asn_decode(test_data, sizeof(test_data), &root_tlv);

    /* Выводим декодированые данные в виде дерева */
    printf("Decoded data:\n");
    ak_asn_print_tree(&root_tlv);


    /* Кодируем данные обратно */
    ak_asn_encode(&root_tlv, &p_encoded_data, &size);

    /* Выводим исходную ASN.1 последовательность */
    printf("%-20s", "Original data : ");
    ak_asn_print_hex_data(test_data, sizeof(test_data));
    putchar('\n');

    /* Выводим закодированную ASN.1 последовательность */
    printf("%-20s", "Encoded data : ");
    ak_asn_print_hex_data(p_encoded_data, size);
    putchar('\n');

    /* Сравниваем размер исходных данных с закодированными */
    if(size != sizeof(test_data))
    {
        test_result = ak_false;
        printf("Sizes differ.\n");
    }

    /* Сравниваем исходные данные с закодированными */
    if(test_result)
    {
        for (ak_uint32 i = 0; i < size; i++)
        {
            if (test_data[i] != p_encoded_data[i])
            {
                printf("Data differ at %u byte\n", i + 1);
                test_result = ak_false;
                break;
            }
        }
    }

    if(test_result)
        printf("Test passed!\n");
    else
        printf("Test failed!\n");


    ak_uint8 test_bit_str[] = {0x07, 0xFF, 0x00};


    asn_print_universal(TBIT_STRING, 3, test_bit_str);

    /* Деинициализируем библиотеку */
    return ak_libakrypt_destroy();
}
