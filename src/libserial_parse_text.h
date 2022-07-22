/**
******************************************************************************
* @文件		libserial_parse_text.h
* @版本		V1.0.1
* @日期
* @概要		适用于单片机实现串口命令行解析的基础库, 支持动静态内存方式
* @作者		lovemengx	email:lovemengx@qq.com
******************************************************************************
* @注意  	All rights reserved
******************************************************************************
*/
#ifndef __LIB_SERIAL_PARSE_TEXT_H_
#define __LIB_SERIAL_PARSE_TEXT_H_

#ifdef __cplusplus
extern "C" {
#endif

// 缓存大小包含了接口内部所使用的数据结构空间
typedef struct{
	char *buf;					// 缓存地址, 用于存储解析后的字符串
	unsigned int total;			// 缓存大小, 标明该内存空间的总长度
}libserial_parse_buf_t;

#define LIBSERIAL_PARSE_SHIFT_NORMAL		0		// 不转换
#define LIBSERIAL_PARSE_SHIFT_LOWER 		1		// 转换为小写字母
#define LIBSERIAL_PARSE_SHIFT_UPPER 		2		// 转换为大写字母

/*---------------------------------------------------------------------
*	函数: 	libserial_parse_create
*	功能:	使用接口内部申请指定可用大小的空间(包含内部数据结构所用空间)
*	参数:	size: 申请可用缓冲区大小 
*	返回:	NULL: 申请内存空间失败		>0: 申请成功
*---------------------------------------------------------------------*/
libserial_parse_buf_t *libserial_parse_create(unsigned int size);

/*---------------------------------------------------------------------
*	函数: 	libserial_parse_release
*	功能:	释放接口内部申请的内存空间
*	参数:	spbuf: 由 libserial_parse_create() 创建的内存空间
*	返回:	无返回值
*---------------------------------------------------------------------*/
void libserial_parse_release(libserial_parse_buf_t *spbuf);

/*---------------------------------------------------------------------
*	函数: 	libserial_parse_internal_size
*	功能:	返回内部数据结构占用字节数
*	参数:	无需参数
*	返回:	内部数据结构占用字节数(不同字长的处理器可能不一样)
*---------------------------------------------------------------------*/
unsigned int libserial_parse_internal_size();

/*---------------------------------------------------------------------
*	函数: 	libserial_parse_init
*	功能:	使用用户提供的或创建接口的缓冲区, 初始化内部数据结构
*	参数:	spbuf: 缓冲区  size: 缓冲区大小
*	返回:	0: 不满足最小长度要求  >0: 可存储最长文本的长度 
*	备注:	默认设置忽略字符为: '\0', 默认设置分隔字符为: '\n'
*---------------------------------------------------------------------*/
unsigned int libserial_parse_init(libserial_parse_buf_t *spbuf);

/*---------------------------------------------------------------------
*	函数: 	libserial_parse_reset_buf
*	功能:	重置解析器缓冲区
*	参数:	spbuf: 缓冲区
*	返回:	无返回值
*	备注:	不影响 libserial_parse_set_divide()\libserial_parse_set_ignore()
*---------------------------------------------------------------------*/
void libserial_parse_reset_buf(libserial_parse_buf_t *spbuf);

/*---------------------------------------------------------------------
*	函数: 	libserial_parse_set_divide
*	功能:	设定分隔字符
*	参数:	splbuf: 缓冲区  divide: 分隔字符
*	返回:	无返回值
*---------------------------------------------------------------------*/
void libserial_parse_set_divide(libserial_parse_buf_t *spbuf, char divide);

/*---------------------------------------------------------------------
*	函数: 	libserial_parse_set_ignore
*	功能:	设定忽略字符, 传入 '\0' 代表不忽略任何字符
*	参数:	splbuf: 缓冲区  ignore: 忽略字符
*	返回:	无返回值
*	备注:	双引号内的字符不会受此限制
*---------------------------------------------------------------------*/
void libserial_parse_set_ignore(libserial_parse_buf_t *spbuf, char ignore);

/*---------------------------------------------------------------------
*	函数: 	libserial_parse_set_shift
*	功能:	设置大小写字母转换
*	参数:	splbuf: 缓冲区  shift: 0:不做转换 1:转换为小写字母 2:转换为大写字母
*	返回:	无返回值
*---------------------------------------------------------------------*/
void libserial_parse_set_shift(libserial_parse_buf_t *spbuf, char shift);

/*---------------------------------------------------------------------
*	函数: 	libserial_parse_text
*	功能:	解析以指定符号分隔或跳过的文本
*	参数:	splbuf: 缓冲区  indata: 输入数据
*	返回:	0: 正在解析  	>0:解析完成, 返回文本长度(不包含 '\0')
*	备注:	没有设定分隔符则会一直返回0, 数据超出缓存长度会返回解析完成
*---------------------------------------------------------------------*/
unsigned int libserial_parse_text(libserial_parse_buf_t *spbuf, char indata);

/*---------------------------------------------------------------------
*	函数: 	libserial_parse_text_nl  (nl -> new line)
*	功能:	解析以换行符为终止符的文本(支持 '\n' 和 '\r\n')
*	参数:	splbuf: 缓冲区  indata: 输入数据
*	返回:	0: 正在解析  	>0:解析完成, 返回文本长度(不包含 '\0')
*	备注:	不受 libserial_parse_set_divide()\libserial_parse_set_ignore() 影响
*---------------------------------------------------------------------*/
unsigned int libserial_parse_text_nl(libserial_parse_buf_t *spbuf, char indata);

/*---------------------------------------------------------------------
*	函数: 	libserial_parse_finish
*	功能:	获取当前仍在缓冲区的字符数据
*	参数:	splbuf: 缓冲区 
*	返回:	0: 没有数据  	>0:剩余字符串长度(不包含 '\0')
*---------------------------------------------------------------------*/
unsigned int libserial_parse_text_finish(libserial_parse_buf_t *spbuf);

#ifdef __cplusplus
}
#endif

#endif
