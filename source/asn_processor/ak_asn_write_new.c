#include "ak_asn_codec_new.h"

// TODO: проверить необходимость в подключении файлов

#ifdef LIBAKRYPT_HAVE_STDLIB_H
#include <stdlib.h>
#else
#error Library cannot be compiled without stdlib.h header
#endif
#ifdef LIBAKRYPT_HAVE_STRING_H
#include <string.h>
#else
#error Library cannot be compiled without string.h header
#endif
#ifdef LIBAKRYPT_HAVE_CTYPE_H
#include <ctype.h>
#include <stdio.h>
#else
#error Library cannot be compiled without ctype.h header
#endif

/*________________________________________________________________________  new realization ____________________________________________________________________*/

static int ak_asn_encode_tlv(ak_asn_tlv p_tlv, ak_byte** pp_pos, ak_byte* p_end)
{
    int error;      /* Код ошибки */
    ak_uint32 size; /* Размер кодируемых данных */

    if(!pp_pos || !p_end || !p_tlv)
        return ak_error_null_pointer;

    /* Вычисляем длину кодируемых в данный момент данных,
     * чтобы проверить выход за границы выделенной памяти */
    ak_asn_get_size(p_tlv, &size);
    if((*pp_pos + size) > p_end)
        return ak_error_message(ak_error_out_of_memory, __func__, "need more memory for encoding");

    if(p_tlv->m_tag & CONSTRUCTED) /* Кодирование составных данных */
    {
        ak_uint8 index; /* Индекс элемента в массиве элементов составного объекта */

        if((error = new_asn_put_tag(p_tlv->m_tag, pp_pos)) != ak_error_ok)
            return ak_error_message(error, __func__, "failure in adding tag");

        if((error = new_asn_put_len(p_tlv->m_data_len, p_tlv->m_len_byte_cnt, pp_pos)) != ak_error_ok)
            return ak_error_message(error, __func__, "failure in adding data length");

        for(index = 0; index < p_tlv->m_data.m_constructed_data->m_curr_size; index++)
        {
            if((error = ak_asn_encode_tlv(p_tlv->m_data.m_constructed_data->m_arr_of_data[index], pp_pos, p_end)) != ak_error_ok)
                return error; /* Никакого сообщения не выводится, иначе выведется много одинаковых строчек (из-за рекурсии) */
        }
    }
    else /* Кодирование примитивных данных */
    {
        new_asn_put_tag(p_tlv->m_tag, pp_pos);
        new_asn_put_len(p_tlv->m_data_len, p_tlv->m_len_byte_cnt, pp_pos);
        memcpy(*pp_pos, p_tlv->m_data.m_primitive_data, p_tlv->m_data_len);
        (*pp_pos) +=  p_tlv->m_data_len;
    }

    return ak_error_ok;
}

int ak_asn_encode(ak_asn_tlv p_tlv, ak_byte** pp_asn_data, ak_uint32* p_size)
{
    /* TODO FIXME: уточнить, нужно ли создавать отдельную переменную указатель на данные
                   и потом записывать ее значение во входной параметр? */
    int error; /* Код ошибки */

    if(!pp_asn_data || !p_size || !p_tlv)
        return ak_error_null_pointer;

    /* Пересчитываем длины, чтобы защититься от нежелательных ошибок */
    if((error = ak_asn_update_size(p_tlv)) != ak_error_ok)
        return ak_error_message(error, __func__, "failure in recalculating size");

    /* Вычисляем объем памяти, необходимый для кодирования данных */
    if((error = ak_asn_get_size(p_tlv, p_size)) != ak_error_ok)
        return ak_error_message(error, __func__, "failure in getting size");

    /* Выделяем память */
    *pp_asn_data = (ak_byte*)malloc(*p_size);
    if(!(*pp_asn_data))
    {
        *p_size = 0;
        return ak_error_out_of_memory;
    }

    /* Кодируем данные */
    if((error = ak_asn_encode_tlv(p_tlv, pp_asn_data, (*pp_asn_data) + (*p_size))) != ak_error_ok)
        return ak_error_message(error, __func__, "failure in encoding ASN.1 data");

    /* Возвращаем указатель на данные в их начало (он смещается внутри ф-ии ak_asn_encode_tlv()) */
    (*pp_asn_data) -= *p_size;

    return ak_error_ok;
}

int ak_asn_fill_root_tlv(ak_asn_tlv p_tlv, tag data_tag, ak_pointer p_data)
{
    if(data_tag & CONSTRUCTED)
    {
        if(ak_asn_create_constructed_tlv(p_tlv, data_tag, ak_true) != ak_error_ok)
            return -1;
    }
    else
    {

    }

    return ak_error_ok;
}


int ak_asn_add(ak_asn_tlv p_tlv, tag data_tag, ak_pointer p_data, int (*encode)(ak_pointer, ak_pointer))
{
    if(data_tag & CONSTRUCTED)
    {
        ak_uint8 obj_pos;
        ak_asn_tlv p_obj;

        obj_pos = p_tlv->m_data.m_constructed_data->m_curr_size;
        p_obj = malloc(sizeof(s_asn_tlv_t));
        if(!p_obj)
            return ak_error_out_of_memory;

        if(ak_asn_create_constructed_tlv(p_obj, data_tag, ak_true) != ak_error_ok)
            return -1;

        if(obj_pos == p_tlv->m_data.m_constructed_data->m_alloc_size)
        {
            //FIXME реализовать перевыделение памяти
            return -1;
        }

        p_tlv->m_data.m_constructed_data->m_arr_of_data[obj_pos] = p_obj;
        p_tlv->m_data_len = 1 + p_obj->m_data_len + p_obj->m_data_len;
        p_tlv->m_len_byte_cnt = new_asn_get_len_byte_cnt(p_tlv->m_data_len);
    }

    return ak_error_ok;
}


/*________________________________________________________________________  /new realization ____________________________________________________________________*/


/* ----------------------------------------------------------------------------------------------- */
/*! @param tag тег
    @param pp_buff указатель на область памяти, в которую записывается результат кодирования
    @return В случае успеха функция возввращает ak_error_ok (ноль).
    В противном случае, возвращается код ошибки.                                                   */
/* ----------------------------------------------------------------------------------------------- */
int new_asn_put_tag(tag tag, ak_byte** pp_buff)
{
    if (!pp_buff)
        return ak_error_message(ak_error_null_pointer, __func__, "bad pointer to buffer");

    **pp_buff = tag;
    (*pp_buff)++;

    return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! @param len длина данных
    @param p_buff указатель на область памяти, в которую записывается результат кодирования
    @return В случае успеха функция возввращает ak_error_ok (ноль).
    В противном случае, возвращается код ошибки.                                                   */
/* ----------------------------------------------------------------------------------------------- */
int new_asn_put_len(size_t len, ak_uint32 len_byte_cnt, ak_byte** pp_buff)
{
//    if (!pp_buff)
//        return ak_error_message(ak_error_null_pointer, __func__, "bad pointer to buffer");
//
//    if (!len_byte_cnt)
//        return ak_error_message(ak_error_null_pointer, __func__, "wrong length");

    if (len_byte_cnt == 1)
    {
        (**pp_buff) = (ak_byte) len;
        (*pp_buff)++;
    }
    else
    {
        (**pp_buff) = (ak_byte) (0x80u ^ (uint8_t) (--len_byte_cnt));
        (*pp_buff)++;

        do
        {
            (**pp_buff) = (ak_byte) ((len >> (8u * --len_byte_cnt)) & 0xFFu);
            (*pp_buff)++;
        }while (len_byte_cnt != 0);
    }

    return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! @param val входное значение (описание формата значения находится в определении типа integer)
    @param pp_buff указатель на область памяти, в которую записывается результат кодирования
    @return В случае успеха функция возввращает ak_error_ok (ноль).
    В противном случае, возвращается код ошибки.                                                   */
/* ----------------------------------------------------------------------------------------------- */
int new_asn_put_int(integer val, ak_byte** pp_buff, ak_uint32* p_size)
{
    bool_t high_bit;
    ak_uint8 val_len_byte_cnt;
    if (!pp_buff)
        return ak_error_null_pointer;

    if (val <= 0xFFu)
        val_len_byte_cnt = 1;
    else if (val <= 0xFFFFu)
        val_len_byte_cnt = 2;
    else if (val <= 0xFFFFFFu)
        val_len_byte_cnt = 3;
    else if (val <= 0xFFFFFFFFu)
        val_len_byte_cnt = 4;
    else
    {
        *pp_buff = NULL;
        *p_size = 0;
        return ak_error_invalid_value;
    }

    /* Проверям, возведен ли старший бит в числе */
    if((val >> ((val_len_byte_cnt - 1) * 8)) & 0x80)
    {
        high_bit = ak_true;
        *p_size = val_len_byte_cnt + 1;
    }
    else
    {
        high_bit = ak_false;
        *p_size = val_len_byte_cnt;
    }

    *pp_buff = malloc(*p_size);
    if(!(*pp_buff))
    {
        *p_size = 0;
        return ak_error_out_of_memory;
    }

    if(high_bit)
    {
        *((*pp_buff)++) = 0x00;
    }

    while(--val_len_byte_cnt > 0)
    {
        *((*pp_buff)++) = (ak_byte)((val >> (val_len_byte_cnt * 8)) & 0xFF);
    }

    **pp_buff = (ak_byte)(val & 0xFF);

//    if (!val.mp_value)
//        return ak_error_message(ak_error_null_pointer, __func__, "null value");
//
//    if (!val.m_positive && !(val.mp_value[0] & 0x80u))
//        return ak_error_message(ak_error_wrong_asn1_encode, __func__, "wrong asn1 encode");
//
//    if (!val.m_positive)
//    {
//        memcpy(pp_buff, val.mp_value, val.m_val_len);
//    }
//    else
//    {
//        if (val.mp_value[0] & 0x80u)
//        {
//            *pp_buff = 0x00;
//            pp_buff++;
//        }
//
//        memcpy(pp_buff, val.mp_value, val.m_val_len);
//    }

    return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! @param str входная UTF-8 строка
    @param pp_buff указатель на область памяти, в которую записывается результат кодирования
    @return В случае успеха функция возввращает ak_error_ok (ноль).
    В противном случае, возвращается код ошибки.                                                   */
/* ----------------------------------------------------------------------------------------------- */
int new_asn_put_utf8string(utf8_string str, ak_byte** pp_buff, ak_uint32* p_size)
{
    *p_size = (ak_uint32)strlen((char*)str);

    if (!str || !pp_buff)
        return ak_error_null_pointer;

    *pp_buff = malloc(*p_size);
    if(!(*pp_buff))
    {
        *p_size = 0;
        return ak_error_out_of_memory;
    }

    memcpy(*pp_buff, str, *p_size);

    return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! @param src массив октетов
    @param pp_buff указатель на область памяти, в которую записывается результат кодирования
    @return В случае успеха функция возввращает ak_error_ok (ноль).
    В противном случае, возвращается код ошибки.                                                   */
/* ----------------------------------------------------------------------------------------------- */
int new_asn_put_octetstr(octet_string src, ak_byte** pp_buff, ak_uint32* p_size)
{
    if (!pp_buff || !src.mp_value)
        return ak_error_null_pointer;

    *pp_buff = malloc(src.m_val_len);
    if(!(*pp_buff))
        return ak_error_out_of_memory;

    memcpy(*pp_buff, src.mp_value, src.m_val_len);

    return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! @param src входная строка
    @param pp_buff указатель на область памяти, в которую записывается результат кодирования
    @return В случае успеха функция возввращает ak_error_ok (ноль).
    В противном случае, возвращается код ошибки.                                                   */
/* ----------------------------------------------------------------------------------------------- */
int new_asn_put_vsblstr(visible_string str, ak_byte** pp_buff, ak_uint32* p_size)
{
    *p_size = (ak_uint32)strlen(str);
    if (!str || !pp_buff)
        return ak_error_null_pointer;

    *pp_buff = malloc(*p_size);
    if(!(*pp_buff))
    {
        *p_size = 0;
        return ak_error_out_of_memory;
    }

    memcpy(*pp_buff, str, strlen(str));

    return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! На данный момент добавляются только идентификаторы, у который первое число 1 или 2,
    а второе не превосходит 32

    @param obj_id входная строка, содержая идентификатор в виде чисел, разделенных точками
    @param pp_buff указатель на область памяти, в которую записывается результат кодирования
    @return В случае успеха функция возввращает ak_error_ok (ноль).
    В противном случае, возвращается код ошибки.                                                   */
/* ----------------------------------------------------------------------------------------------- */
int new_asn_put_objid(object_identifier obj_id, ak_byte** pp_buff, ak_uint32* p_size)
{
    // FIXME: Реализовать кодирования произовльного идентификатора
    ak_uint64 num;
    object_identifier p_objid_end;

    if (!obj_id || !pp_buff)
        return ak_error_null_pointer;

    *p_size = new_asn_get_oid_byte_cnt(obj_id);

    *pp_buff = malloc(*p_size);
    if(!(*pp_buff))
    {
        *p_size = 0;
        return ak_error_out_of_memory;
    }

    num = strtoul((char *) obj_id, &p_objid_end, 10);
    obj_id = ++p_objid_end;
    num = num * 40 + strtol((char *) obj_id, &p_objid_end, 10);
    *((*pp_buff)++) = (ak_byte) num;

    while (*p_objid_end != '\0')
    {
        obj_id = ++p_objid_end;
        num = strtoul((char *) obj_id, &p_objid_end, 10);

        if (num > 0x7Fu)
        {
            ak_byte seven_bits;
            int8_t i;
            i = 3;
            while (i > 0)
            {
                seven_bits = (ak_byte) ((num >> ((uint8_t) i * 7u)) & 0x7Fu);
                if (seven_bits)
                    *((*pp_buff)++) = (ak_byte) (0x80u ^ seven_bits);
                i--;
            }
        }

        *((*pp_buff)++) = (ak_byte) (num & 0x7Fu);
    }

    return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! @param src входные данные (описание формата данных находится в определении типа bit_string)
    @param pp_buff указатель на область памяти, в которую записывается результат кодирования
    @return В случае успеха функция возввращает ak_error_ok (ноль).
    В противном случае, возвращается код ошибки.                                                   */
/* ----------------------------------------------------------------------------------------------- */
int new_asn_put_bitstr(bit_string src, ak_byte** pp_buff, ak_uint32* p_size)
{
    if (!src.mp_value || !pp_buff)
        return ak_error_null_pointer;

    if (src.m_unused > 7 || !src.m_val_len)
        return ak_error_invalid_value;

    /* 1 дополнительный байт для хранения кол-ва неиспользуемых бит в послднем байте данных */
    *p_size = 1 + src.m_val_len;

    *pp_buff = malloc(*p_size);
    if(!(*pp_buff))
    {
        *p_size = 0;
        return ak_error_out_of_memory;
    }

    *((*pp_buff)++) = src.m_unused;
    memcpy(*pp_buff, src.mp_value, src.m_val_len);

    return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! @param val входное значение
    @param pp_buff указатель на область памяти, в которую записывается результат кодирования
    @return В случае успеха функция возввращает ak_error_ok (ноль).
    В противном случае, возвращается код ошибки.                                                   */
/* ----------------------------------------------------------------------------------------------- */
int new_asn_put_bool(boolean val, ak_byte** pp_buff, ak_uint32* p_size)
{
    if (!pp_buff)
        return ak_error_null_pointer;

    *p_size = 1;

    *pp_buff = malloc(*p_size);
    if(!(*pp_buff))
    {
        *p_size = 0;
        return ak_error_out_of_memory;
    }

    if (val)
        **pp_buff = 0xFFu;
    else
        **pp_buff = 0x00;

    return ak_error_ok;
}

/* ----------------------------------------------------------------------------------------------- */
/*! @param time строка в формате "YYYY-MM-DD HH:MM:SS.[ms] UTC"
    @param pp_buff указатель на область памяти, в которую записывается результат кодирования
    @return В случае успеха функция возввращает ak_error_ok (ноль).
    В противном случае, возвращается код ошибки.                                                   */
/* ----------------------------------------------------------------------------------------------- */
int new_asn_put_generalized_time(generalized_time time, ak_byte** pp_buff, ak_uint32* p_size)
{
    if (!time || !pp_buff)
        return ak_error_null_pointer;

    *p_size = new_asn_get_gentime_byte_cnt(time);
    if (*p_size < 15)
        return ak_error_message(ak_error_invalid_value, __func__, "wrong length of time string");

    *pp_buff = malloc(*p_size);
    if(!(*pp_buff))
    {
        *p_size = 0;
        return ak_error_out_of_memory;
    }

    /* YYYY */
    for (uint8_t i = 0; i < 4; i++)
    {
        if (!isdigit(*time))
            return ak_error_message(ak_error_wrong_asn1_encode, __func__, "wrong format of year value");
        *((*pp_buff)++) = (ak_byte) *(time++);
    }
    time++;

    /* MM */
    for (uint8_t i = 0; i < 2; i++)
    {
        if (!isdigit(*time))
            return ak_error_message(ak_error_wrong_asn1_encode, __func__, "wrong format of month value");
        *((*pp_buff)++) = (ak_byte) *(time++);
    }
    time++;

    /* DD */
    for (uint8_t i = 0; i < 2; i++)
    {
        if (!isdigit(*time))
            return ak_error_message(ak_error_wrong_asn1_encode, __func__, "wrong format of day value");
        *((*pp_buff)++) = (ak_byte) *(time++);
    }
    time++;

    /* HH */
    for (uint8_t i = 0; i < 2; i++)
    {
        if (!isdigit(*time))
            return ak_error_message(ak_error_wrong_asn1_encode, __func__, "wrong format of hour value");
        *((*pp_buff)++) = (ak_byte) *(time++);
    }
    time++;

    /* MM */
    for (uint8_t i = 0; i < 2; i++)
    {
        if (!isdigit(*time))
            return ak_error_message(ak_error_wrong_asn1_encode, __func__, "wrong format of minute value");
        *((*pp_buff)++) = (ak_byte) *(time++);
    }
    time++;

    /* SS */
    for (uint8_t i = 0; i < 2; i++)
    {
        if (!isdigit(*time))
            return ak_error_message(ak_error_wrong_asn1_encode, __func__, "wrong format of second value");
        *((*pp_buff)++) = (ak_byte) *(time++);
    }

    /* .mmm */
    if (*time == '.')
    {
        int8_t ms_cnt = (int8_t) ((strchr(time, ' ') - time) - 1);
        if (ms_cnt == 0)
            return ak_error_message(ak_error_wrong_asn1_encode, __func__, "quota of second absent");

        if (time[ms_cnt] == '0')
            return ak_error_message(ak_error_wrong_asn1_encode,
                                    __func__,
                                    "wrong format of quota of second value (it can't end by 0 symbol)");

        *((*pp_buff)++) = (ak_byte) *(time++); // помещаем символ точки
        for (uint8_t i = 0; i < ms_cnt; i++)
        {
            if (!isdigit(*time))
                return ak_error_message(ak_error_wrong_asn1_encode, __func__, "wrong format of quota of second value");
            *((*pp_buff)++) = (ak_byte) *(time++);
        }
    }

    **pp_buff = 'Z';

    return ak_error_ok;
}

///* ----------------------------------------------------------------------------------------------- */
///*! Функция добавляет стандартный тег ASN.1, длину данных, данные (если они присутствуют).
//    В случае, когда tag_number = TSEQUENCE | TSET, параметр p_data должен быть равен NULL,
//    а seq_or_set_len содержать длину данных, которые объединются в объект sequence или set.
//    В остальных случаях параметр p_data указывает на область памяти с данными, а seq_or_set_len
//    должен быть равен 0.
//
//    @param tag_number номер стандартного тега
//    @param p_data указатель на область памяти, в которой находятся данные для кодирования
//    @param seq_or_set_len длина закодированных данных
//    @param p_main_ps указатель на главный объект типа s_ptr_server
//    @param p_result указатель на объект типа s_ptr_server, в который поместиться блок
//           закодированных данных
//    @return В случае успеха функция возввращает ak_error_ok (ноль).
//    В противном случае, возвращается код ошибки.                                                   */
///* ----------------------------------------------------------------------------------------------- */
//int new_asn_put_universal_tlv(uint8_t tag_number,
//                          void *p_data,
//                          size_t seq_or_set_len,
//                          s_ptr_server *p_main_ps,
//                          s_ptr_server *p_result) {
//    int error;
//    size_t value_len;
//    size_t len_byte_cnt;
//
//    if (!tag_number || !p_main_ps || !p_result)
//        return ak_error_message(ak_error_null_pointer, __func__, "input argument is null");
//
//    value_len = 0;
//    len_byte_cnt = 0;
//
//    if (tag_number == TOCTET_STRING)
//    {
//        octet_string str = *((octet_string *) p_data);
//
//        if (!p_data)
//            return ak_error_message(ak_error_null_pointer, __func__, "null value");
//
//        if (!str.mp_value)
//            return ak_error_message(ak_error_null_pointer, __func__, "null value");
//
//        value_len = str.m_val_len;
//        len_byte_cnt = new_asn_get_len_byte_cnt(value_len);
//        if (!len_byte_cnt)
//            return ak_error_message(ak_error_wrong_asn1_encode, __func__, "wrong asn1 encode");
//
//        if ((error = ps_move_cursor(p_main_ps, 1 + len_byte_cnt + value_len)) != ak_error_ok)
//            return ak_error_message(error, __func__, "problems with moving cursor");
//
//        if ((error = new_asn_put_octetstr(str, p_main_ps->mp_curr + 1 + len_byte_cnt)) != ak_error_ok)
//            return ak_error_message(error, __func__, "problems with adding octet string value");
//    }
//    else if (tag_number == TINTEGER)
//    {
//        integer num = *((integer *) p_data);
//        if (!p_data)
//            return ak_error_message(ak_error_null_pointer, __func__, "null value");
//
//        if (!num.mp_value)
//            return ak_error_message(ak_error_null_pointer, __func__, "null value");
//
//        value_len = num.m_val_len;
//        if (num.m_positive && (num.mp_value[0] & 0x80u))
//            value_len += 1;
//
//        len_byte_cnt = new_asn_get_len_byte_cnt(value_len);
//        if (!len_byte_cnt)
//            return ak_error_message(ak_error_wrong_asn1_encode, __func__, "wrong asn1 encode");
//
//        if ((error = ps_move_cursor(p_main_ps, 1 + len_byte_cnt + value_len)) != ak_error_ok)
//            return ak_error_message(error, __func__, "problems with moving cursor");
//
//        if ((error = new_asn_put_int(num, p_main_ps->mp_curr + len_byte_cnt + 1)) != ak_error_ok)
//            return ak_error_message(error, __func__, "problems with adding integer value");
//    }
//    else if (tag_number == TBIT_STRING)
//    {
//        bit_string str = *((bit_string *) p_data);
//
//        if (!p_data)
//            return ak_error_message(ak_error_null_pointer, __func__, "null value");
//
//        if (!str.mp_value)
//            return ak_error_message(ak_error_null_pointer, __func__, "null value");
//
//        value_len = str.m_val_len + 1; // 1 - для хранения кол-ва неиспользуемых битов
//
//        len_byte_cnt = new_asn_get_len_byte_cnt(value_len);
//        if (!len_byte_cnt)
//            return ak_error_message(ak_error_wrong_asn1_encode, __func__, "wrong asn1 encode");
//
//        if ((error = ps_move_cursor(p_main_ps, 1 + len_byte_cnt + value_len)) != ak_error_ok)
//            return ak_error_message(error, __func__, "problems with moving cursor");
//
//        if ((error = new_asn_put_bitstr(str, p_main_ps->mp_curr + len_byte_cnt + 1)) != ak_error_ok)
//            return ak_error_message(error, __func__, "problems with adding bit string value");
//    }
//    else if (tag_number == TGENERALIZED_TIME)
//    {
//        generalized_time time = *((generalized_time *) p_data);
//
//        if (!p_data)
//            return ak_error_message(ak_error_null_pointer, __func__, "null value");
//
//        value_len = new_asn_get_gentime_byte_cnt(time);
//        len_byte_cnt = new_asn_get_len_byte_cnt(value_len);
//
//        if (!len_byte_cnt)
//            return ak_error_message(ak_error_wrong_asn1_encode, __func__, "wrong asn1 encode");
//
//        if ((error = ps_move_cursor(p_main_ps, 1 + len_byte_cnt + value_len)) != ak_error_ok)
//            return ak_error_message(error, __func__, "problems with moving cursor");
//
//        if ((error = new_asn_put_generalized_time(time, p_main_ps->mp_curr + len_byte_cnt + 1)) != ak_error_ok)
//            return ak_error_message(error, __func__, "problems with adding generalized time value");
//    }
//    else if (tag_number == TOBJECT_IDENTIFIER)
//    {
//        object_identifier oid = *((object_identifier *) p_data);
//
//        if (!p_data)
//            return ak_error_message(ak_error_null_pointer, __func__, "null value");
//
//        value_len = new_asn_get_oid_byte_cnt(oid);
//
//        len_byte_cnt = new_asn_get_len_byte_cnt(value_len);
//        if (!len_byte_cnt)
//            return ak_error_message(ak_error_wrong_asn1_encode, __func__, "wrong asn1 encode");
//
//        if ((error = ps_move_cursor(p_main_ps, 1 + len_byte_cnt + value_len)) != ak_error_ok)
//            return ak_error_message(error, __func__, "problems with moving cursor");
//
//        if ((error = new_asn_put_objid(oid, p_main_ps->mp_curr + len_byte_cnt + 1)) != ak_error_ok)
//            return ak_error_message(error, __func__, "problems with adding object identifier value");
//    }
//    else if (tag_number == TUTF8_STRING)
//    {
//        utf8_string str = *((utf8_string *) p_data);
//        value_len = strlen((char *) str);
//
//        if (!p_data)
//            return ak_error_message(ak_error_null_pointer, __func__, "null value");
//
//        len_byte_cnt = new_asn_get_len_byte_cnt(value_len);
//        if (!len_byte_cnt)
//            return ak_error_message(ak_error_wrong_asn1_encode, __func__, "wrong asn1 encode");
//
//        if ((error = ps_move_cursor(p_main_ps, 1 + len_byte_cnt + value_len)) != ak_error_ok)
//            return ak_error_message(error, __func__, "problems with moving cursor");
//
//        if ((error = new_asn_put_utf8string(str, p_main_ps->mp_curr + len_byte_cnt + 1)) != ak_error_ok)
//            return ak_error_message(error, __func__, "problems with adding utf8 string value");
//    }
//    else if (tag_number == TBOOLEAN)
//    {
//        boolean bval = *((boolean *) p_data);
//
//        if (!p_data)
//            return ak_error_message(ak_error_null_pointer, __func__, "null value");
//
//        value_len = 1;
//
//        len_byte_cnt = new_asn_get_len_byte_cnt(value_len);
//        if (!len_byte_cnt)
//            return ak_error_message(ak_error_wrong_asn1_encode, __func__, "wrong asn1 encode");
//
//        if ((error = ps_move_cursor(p_main_ps, 1 + len_byte_cnt + value_len)) != ak_error_ok)
//            return ak_error_message(error, __func__, "problems with moving cursor");
//
//        if ((error = new_asn_put_bool(bval, p_main_ps->mp_curr + len_byte_cnt + 1)) != ak_error_ok)
//            return ak_error_message(error, __func__, "problems with adding boolean value");
//    }
//    else if (tag_number == TSEQUENCE || tag_number == TSET)
//    {
//        value_len = seq_or_set_len;
//        tag_number |= CONSTRUCTED;
//
//        len_byte_cnt = new_asn_get_len_byte_cnt(value_len);
//        if (!len_byte_cnt)
//            return ak_error_message(ak_error_wrong_asn1_encode, __func__, "wrong asn1 encode");
//
//        if ((error = ps_move_cursor(p_main_ps, 1 + len_byte_cnt)) != ak_error_ok)
//            return ak_error_message(error, __func__, "problems with moving cursor");
//    }
//
//    new_asn_put_tag(tag_number, p_main_ps->mp_curr);
//
//    if ((error = new_asn_put_len(value_len, p_main_ps->mp_curr + 1)) != ak_error_ok)
//        return ak_error_message(error, __func__, "problem with adding data length");
//
//    if ((error = ps_set(p_result, p_main_ps->mp_curr, 1 + len_byte_cnt + value_len, PS_U_MODE)) != ak_error_ok)
//        return ak_error_message(error, __func__, "problems with making union of asn data");
//    return error;
//}
//

/* ----------------------------------------------------------------------------------------------- */
/*! @param len длина данных
    @return Кол-во байтов, необходимое для хранения закодированной длины.                          */
/* ----------------------------------------------------------------------------------------------- */
ak_uint8 new_asn_get_len_byte_cnt(size_t len)
{
    if (len < 0x80u && len >= 0)
        return 1;
    if (len <= 0xFFu)
        return 2;
    if (len <= 0xFFFFu)
        return 3;
    if (len <= 0xFFFFFFu)
        return 4;
    if (len <= 0xFFFFFFFFu)
        return 5;
    else
        return 0;
}

/* ----------------------------------------------------------------------------------------------- */
/*! @param oid строка, содержая идентификатор в виде чисел, разделенных точками
    @return Кол-во байтов, необходимое для хранения закодированного идентификатора.                */
/* ----------------------------------------------------------------------------------------------- */
ak_uint8 new_asn_get_oid_byte_cnt(object_identifier oid) {
    ak_uint8 byte_cnt;
    object_identifier p_end;
    size_t num;

    if (!oid)
        return 0;

    byte_cnt = 1;

    /* Пропускаем 2 первых идентификатора */
    strtoul((char *) oid, &p_end, 10);
    oid = ++p_end;
    strtol((char *) oid, &p_end, 10);

    while (*p_end != '\0')
    {
        oid = ++p_end;
        num = (size_t) strtol((char *) oid, &p_end, 10);
        if (num <= 0x7Fu)             /*                               0111 1111 -  7 бит */
            byte_cnt += 1;
        else if (num <= 0x3FFFu)      /*                     0011 1111 1111 1111 - 14 бит */
            byte_cnt += 2;
        else if (num <= 0x1FFFFFu)    /*           0001 1111 1111 1111 1111 1111 - 21 бит */
            byte_cnt += 3;
        else if (num <= 0x0FFFFFFFu)  /* 0000 1111 1111 1111 1111 1111 1111 1111 - 28 бит */
            byte_cnt += 4;
        else
            return 0;
    }
    return byte_cnt;
}

/* ----------------------------------------------------------------------------------------------- */
/*! @param time строка, содержая время в формате "YYYY-MM-DD HH:MM:SS.[ms] UTC"
    @return Кол-во байтов, необходимое для хранения закодированного времени.                       */
/* ----------------------------------------------------------------------------------------------- */
ak_uint8 new_asn_get_gentime_byte_cnt(generalized_time time) {
    if (!time)
        return 0;
    /*
     * 8 имеет след. смысл:
     * - из строки "YYYY-MM-DD HH:MM:SS.[ms] UTC" удалить символы "-- :: UTC"
     * - добавить символ "Z"
     * Примечание: эл-ов ms может быть неограниченное кол-во.
    */
    return (ak_uint8) (strlen(time) - 8);
}
