/**
******************************************************************************
* @文件		libserial_parse_text.c
* @版本		V1.0.1
* @日期
* @概要		适用于单片机实现串口命令行解析的基础库, 支持动静态内存方式
* @作者		lovemengx	email:lovemengx@qq.com
******************************************************************************
* @注意  	All rights reserved
******************************************************************************
*/
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "libserial_parse_text.h"

// 缓冲区描述信息
#pragma pack(1)
typedef struct {
	char *buf;				// 存储数据的位置
	unsigned int   idx;		// 存储数据的索引
	unsigned int   len;		// 记录解析后的数据长度
	unsigned int space;		// 有效缓冲区空间的大小
}parse_buffer_t;	
#pragma pack()	

// 解析器配置信息
#pragma pack(1)
typedef struct {
	char shift; 			// 转换为大写字母(0:不做转换 1:转换为小写字母 2:转换为大写字母)
	char divide;			// 存储用户配置的分隔字符
	char ignore;			// 存储用户配置的忽略字符
}parse_config_t;
#pragma pack()	

#pragma pack(1)
typedef struct {
	char dqu:1;				// 双引号处理
}parse_status_t;
#pragma pack()

// 解析器对象
#pragma pack(1)
typedef struct {
	parse_buffer_t buf;		// 缓冲区
	parse_status_t sta;		// 内部状态
	parse_config_t cfg;		// 配置信息
}parse_object_t;
#pragma pack()

/*---------------------------------------------------------------------
*	函数: 	get_parse_object
*	功能:	从用户提供的内存中获取缓冲区描述信息的位置
*	参数:	spbuf: 用户提供的内存 
*	返回:	缓冲区描述信息
*---------------------------------------------------------------------*/
static inline parse_object_t *get_parse_object(libserial_parse_buf_t *spbuf)
{
	return (parse_object_t *)(spbuf->buf + spbuf->total - sizeof(parse_object_t));
}

/*---------------------------------------------------------------------
*	函数: 	libserial_parse_preprocess
*	功能:	对数据进行预处理
*	参数:	obj: 解析器对象		indata: 输入数据
*	返回:	缓冲区描述信息
*---------------------------------------------------------------------*/
static inline int libserial_parse_preprocess(parse_object_t *obj, char indata)
{
	// 移除双引号并标记好
	if('\"' == indata){
		obj->sta.dqu = !obj->sta.dqu;
		return 1;
	}

	// 如果是在双引号内的字符串则让忽略失效
	if(!obj->sta.dqu && obj->cfg.ignore == indata){
		return 1;
	}
	
	// 移除前置空格
	if(0 == obj->buf.idx && ' ' == indata){
		return 1;
	}

	return 0;
}

/*---------------------------------------------------------------------
*	函数: 	libserial_parse_internal_size
*	功能:	返回内部数据结构占用字节数
*	参数:	无需参数
*	返回:	内部数据结构占用字节数 
*---------------------------------------------------------------------*/
unsigned int libserial_parse_internal_size()
{
	return sizeof(parse_object_t);
}

/*---------------------------------------------------------------------
*	函数: 	libserial_parse_create
*	功能:	使用接口内部申请指定可用大小的空间(包含内部数据结构所用空间)
*	参数:	size: 申请可用缓冲区大小 
*	返回:	NULL: 申请内存空间失败		>0: 申请成功
*---------------------------------------------------------------------*/
libserial_parse_buf_t *libserial_parse_create(unsigned int size)
{
	libserial_parse_buf_t *spbuf = NULL;
	
	spbuf = (libserial_parse_buf_t *)malloc(sizeof(libserial_parse_buf_t));
	if(0 == size || NULL == spbuf){
		return NULL;
	}
	
	spbuf->total = size + sizeof(parse_object_t);
	if((spbuf->buf = (char *)malloc(spbuf->total)) == NULL){
		free(spbuf);
		return NULL;
	}

	return spbuf;
}

/*---------------------------------------------------------------------
*	函数: 	libserial_parse_release
*	功能:	释放接口内部申请的内存空间
*	参数:	spbuf: 由 libserial_parse_create() 创建的内存空间
*	返回:	无返回值
*---------------------------------------------------------------------*/
void libserial_parse_release(libserial_parse_buf_t *spbuf)
{
	if (spbuf) {
		spbuf->total = 0;
		free(spbuf->buf);
		free(spbuf);
	}
	return ;
}

/*---------------------------------------------------------------------
*	函数: 	libserial_parse_init
*	功能:	使用用户提供的或创建接口的缓冲区, 初始化内部数据结构
*	参数:	spbuf: 缓冲区  size: 缓冲区大小
*	返回:	0: 不满足最小长度要求  >0: 可存储最长文本的长度 
*	备注:	默认设置忽略字符为: '\0', 默认设置分隔字符为: '\n'
*---------------------------------------------------------------------*/
unsigned int libserial_parse_init(libserial_parse_buf_t *spbuf)
{
	parse_object_t *obj = get_parse_object(spbuf);
	
	memset(spbuf->buf, 0x00, spbuf->total);
	if(spbuf->total < sizeof(parse_object_t) + 1){
		return 0;
	}
	
	obj->sta.dqu	= 0x00;
	obj->cfg.ignore = '\0';
	obj->cfg.divide = '\n';
	obj->cfg.shift	= 0x00;
	obj->buf.buf   	= spbuf->buf;
	obj->buf.idx   	= 0x00;
	obj->buf.len   	= 0x00;
	obj->buf.space 	= spbuf->total - sizeof(parse_object_t);

	return obj->buf.space;
}

/*---------------------------------------------------------------------
*	函数: 	libserial_parse_reset_buf
*	功能:	重置解析器缓冲区
*	参数:	spbuf: 缓冲区
*	返回:	无返回值
*	备注:	不影响 libserial_parse_set_divide()\libserial_parse_set_ignore()
*---------------------------------------------------------------------*/
void libserial_parse_reset_buf(libserial_parse_buf_t *spbuf)
{
	parse_object_t *obj = get_parse_object(spbuf);
	obj->sta.dqu 	= 0x00;
	obj->buf.idx	= 0x00;
	obj->buf.len	= 0x00;
	obj->buf.buf[0] = '\0';
	return ;
}

/*---------------------------------------------------------------------
*	函数: 	libserial_parse_set_divide
*	功能:	设定分隔字符
*	参数:	splbuf: 缓冲区  divide: 分隔字符
*	返回:	无返回值
*---------------------------------------------------------------------*/
void libserial_parse_set_divide(libserial_parse_buf_t *spbuf, char divide)
{
	parse_object_t *obj = get_parse_object(spbuf);
	obj->cfg.divide = divide;
}

/*---------------------------------------------------------------------
*	函数: 	libserial_parse_set_ignore
*	功能:	设定忽略字符, 传入 '\0' 代表不忽略任何字符
*	参数:	splbuf: 缓冲区  ignore: 忽略字符
*	返回:	无返回值
*	备注:	双引号内的字符不会受此限制
*---------------------------------------------------------------------*/
void libserial_parse_set_ignore(libserial_parse_buf_t *spbuf, char ignore)
{
	parse_object_t *obj = get_parse_object(spbuf);
	obj->cfg.ignore = ignore;
}

/*---------------------------------------------------------------------
*	函数: 	libserial_parse_set_shift
*	功能:	设置大小写字母转换
*	参数:	splbuf: 缓冲区  shift: 0:不做转换 1:转换为小写字母 2:转换为大写字母
*	返回:	无返回值
*---------------------------------------------------------------------*/
void libserial_parse_set_shift(libserial_parse_buf_t *spbuf, char shift)
{
	parse_object_t *obj = get_parse_object(spbuf);
	obj->cfg.shift = shift;
}

/*---------------------------------------------------------------------
*	函数: 	libserial_parse_text
*	功能:	解析以指定符号分隔或跳过的文本
*	参数:	splbuf: 缓冲区  indata: 输入数据
*	返回:	0: 正在解析  	>0:解析完成, 返回文本长度(不包含 '\0')
*	备注:	没有设定分隔符则会一直返回0, 数据超出缓存长度会返回解析完成
*---------------------------------------------------------------------*/
unsigned int libserial_parse_text(libserial_parse_buf_t *spbuf, char indata)
{
	parse_object_t *obj = get_parse_object(spbuf);

	// 对数据进行预处理
	if(libserial_parse_preprocess(obj, indata) == 1){
		return 0;
	}
	
	// 匹配分隔符
	if(obj->cfg.divide == indata){
		obj->buf.len = obj->buf.idx;
		obj->buf.idx = 0;
		obj->sta.dqu = 0;
		return obj->buf.len;
	}
	
	// 大小写数据转换
	if(LIBSERIAL_PARSE_SHIFT_LOWER == obj->cfg.shift || LIBSERIAL_PARSE_SHIFT_UPPER == obj->cfg.shift){
		indata = (LIBSERIAL_PARSE_SHIFT_LOWER == obj->cfg.shift) ? tolower(indata) : toupper(indata);
	}

	// 复制数据
	if(obj->buf.idx < obj->buf.space - 1){
		obj->buf.buf[obj->buf.idx+1] = '\0';
		obj->buf.buf[obj->buf.idx++] = indata;
		obj->buf.len = obj->buf.idx;
	}
	
	// 长度检查
	if(obj->buf.idx >= obj->buf.space - 1){
		obj->buf.idx = 0;
		obj->sta.dqu = 0;
		return obj->buf.len;
	}
	
	return 0;
}

/*---------------------------------------------------------------------
*	函数: 	libserial_parse_text_nl  (nl -> new line)
*	功能:	解析以换行符为终止符的文本(支持 '\n' 和 '\r\n')
*	参数:	splbuf: 缓冲区  indata: 输入数据
*	返回:	0: 正在解析  	>0:解析成功, 返回文本长度(不包含 '\0')
*	备注:	不受 libserial_parse_set_divide()\libserial_parse_set_ignore() 影响
*---------------------------------------------------------------------*/
unsigned int libserial_parse_text_nl(libserial_parse_buf_t *spbuf, char indata)
{
	parse_object_t *obj = get_parse_object(spbuf);

	// 检查换行字符
	if('\n' == indata)
	{
		if(obj->buf.idx && '\r' == obj->buf.buf[obj->buf.idx-1]){
			obj->buf.idx--;
			obj->buf.buf[obj->buf.idx] = '\0';
		}
		obj->buf.len = obj->buf.idx;
		obj->buf.idx = 0;
		return obj->buf.len;
	}
	
	// 大小写数据转换
	if(LIBSERIAL_PARSE_SHIFT_LOWER == obj->cfg.shift || LIBSERIAL_PARSE_SHIFT_UPPER == obj->cfg.shift){
		indata = (LIBSERIAL_PARSE_SHIFT_LOWER == obj->cfg.shift) ? tolower(indata) : toupper(indata);
	}

	// 拷贝数据
	if(obj->buf.idx < obj->buf.space - 1){
		obj->buf.buf[obj->buf.idx+1] = '\0';
		obj->buf.buf[obj->buf.idx++] = indata;
		obj->buf.len = obj->buf.idx;
	}
	
	//长度检查
	if(obj->buf.idx >= obj->buf.space - 1){
		obj->buf.idx = 0;
		return obj->buf.len;
	}
	
	return 0;
}

/*---------------------------------------------------------------------
*	函数: 	libserial_parse_finish
*	功能:	获取当前仍在缓冲区的字符数据
*	参数:	splbuf: 缓冲区 
*	返回:	0: 没有数据  	>0:剩余字符串长度(不包含 '\0')
*---------------------------------------------------------------------*/
unsigned int libserial_parse_text_finish(libserial_parse_buf_t *spbuf)
{
	parse_object_t *obj = get_parse_object(spbuf);
	obj->buf.len = obj->buf.idx;
	obj->buf.idx = 0;
	obj->sta.dqu = 0;
	return obj->buf.len;
}
