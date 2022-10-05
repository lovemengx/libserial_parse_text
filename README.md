## libserial_parse_text

适用于单片机实现字符串命令行解析的基础库

具有以下几种特点：

* 支持不定长命令行，libserial_parse_text 逐个字符解码，可以支持不定长的命令参数解析。
* 内存空间占用可控，libserial_parse_text 可使用静态内存，也可以使用动态内存，内存空间可控。
* 分割符可灵活定义，libserial_parse_text 支持自定义分割符和忽略符号，面向对象设计，可多个实例应用

## 接口介绍

```C
// 缓存大小包含了接口内部所使用的数据结构空间
typedef struct{
	char *buf;					// 缓存地址, 用于存储解析后的字符串
	unsigned int total;			// 缓存大小, 标明该内存空间的总长度
}libserial_parse_buf_t;

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
*	备注:	双引号内的字符不会被转换
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
```

## Sample

```C
#include <stdio.h>
#include "libserial_parse_text.h"

#define iprintf(format,...) 	printf("[inf]%s():%05d " format , __func__, __LINE__,##__VA_ARGS__)

/*---------------------------------------------------------------------
*	函数: 	dynamic_mem_newline_example
*	功能:	动态内存版本以行为分割符的示例代码
*---------------------------------------------------------------------*/
int dynamic_mem_newline_example(const char* string)
{
	libserial_parse_buf_t* spbuf = NULL;
	unsigned int len = 0x00, i = 0x00;

	// 创建缓冲区
	if ((spbuf = libserial_parse_create(512)) == NULL) {
		printf("reate parse buf failed.\n");
		return -1;
	}

	// 初始化缓冲区
	if ((len = libserial_parse_init(spbuf)) == 0x00) {
		printf("memory is too small.\n");
		libserial_parse_release(spbuf);
		return -1;
	}

	// 开始解析字符串
	printf("total:%d  len:%d\n", spbuf->total, len);
	for (i = 0; i < strlen(string); i++) {
		if ((len = libserial_parse_text_nl(spbuf, string[i])) > 0) {
			printf("[parse] : %-2d->[%s]\n", len, spbuf->buf);
		}
	}

	// 检查是否还有剩下的字符串
	if ((len = libserial_parse_text_finish(spbuf)) > 0) {
		printf("[finish]: %-2d->[%s]\n", len, spbuf->buf);
	}

	// 释放动态内存
	libserial_parse_release(spbuf);
	return 0;
}

/*---------------------------------------------------------------------
*	函数: 	static_mem_newline_example
*	功能:	静态内存版本以行为分割符的示例代码
*---------------------------------------------------------------------*/
int static_mem_newline_example(const char* string)
{
	char buff[512] = { 0 };
	libserial_parse_buf_t spbuf;
	unsigned int len = 0x00, i = 0x00;

	// 指定静态内存
	spbuf.buf = buff;
	spbuf.total = sizeof(buff);

	// 初始化缓冲区
	if ((len = libserial_parse_init(&spbuf)) == 0x00) {
		printf("memory is too small.\n");
		return -1;
	}

	// 开始解析字符串
	printf("total:%d  len:%d\n", spbuf.total, len);
	for (i = 0; i < strlen(string); i++) {
		if ((len = libserial_parse_text_nl(&spbuf, string[i])) > 0) {
			printf("[parse] : %-2d->[%s]\n", len, spbuf.buf);
		}
	}

	// 检查是否还有剩下的字符串
	if ((len = libserial_parse_text_finish(&spbuf)) > 0) {
		printf("[finish]: %-2d->[%s]\n", len, spbuf.buf);
	}

	return 0;
}

/*---------------------------------------------------------------------
*	函数: 	static_mem_divide_example
*	功能:	静态内存版本以自定义分割符的示例代码
*---------------------------------------------------------------------*/
int static_mem_divide_example(const char* string)
{
	char buff[512] = { 0 };
	libserial_parse_buf_t spbuf;
	unsigned int len = 0x00, i = 0x00;

	// 指定静态内存
	spbuf.buf = buff;
	spbuf.total = sizeof(buff);

	// 初始化缓冲区
	if ((len = libserial_parse_init(&spbuf)) == 0x00) {
		printf("memory is too small.\n");
		return -1;
	}

	// 命令格式假定:  命令名称 参数1, 参数2, 参数3
	libserial_parse_set_divide(&spbuf, ' ');	// 设置分隔字符为空格, 提取命令名称
	libserial_parse_set_ignore(&spbuf, '\0');	// 设置忽略字符为字符串结束符, 即不忽略任何字符

	// 开始解析字符串
	printf("total:%d  len:%d\n", spbuf.total, len);
	for (i = 0; i < strlen(string); i++) {
		if ((len = libserial_parse_text(&spbuf, string[i])) > 0) {
			printf("[parse] : %-2d->[%s]\n", len, spbuf.buf);
			libserial_parse_set_divide(&spbuf, ','); // 设置分隔字符为逗号, 提取参数
			libserial_parse_set_ignore(&spbuf, ' '); // 设置忽略字符为空格, 忽略参数中的空格字符
		}
	}

	// 检查是否还有剩下的字符串
	if ((len = libserial_parse_text_finish(&spbuf)) > 0) {
		printf("[finish]: %-2d->[%s]\n", len, spbuf.buf);
	}

	return 0;
}

// 解析命令
int serial_command_text(libserial_parse_buf_t* spbuf, const char* string)
{
	unsigned int i = 0x00;

	for (i = 0; i < strlen(string); i++) {
		if (libserial_parse_text(spbuf, string[i])) {
			return 1;
		}
	}

	return !!libserial_parse_text_finish(spbuf);
}

// 解析命令参数
int serial_exec_command(libserial_parse_buf_t* spbuf, const char* command, const char* paramet)
{
	unsigned int i = 0x00;

	printf("[command]: [%s]\n", command);

	for (i = 0; i < strlen(paramet); i++) {
		if (libserial_parse_text(spbuf, paramet[i])) {
			printf("[paramet]: [%s]\n", spbuf->buf);
		}
	}

	if (libserial_parse_text_finish(spbuf)) {
		printf("[paramet]: [%s]\n", spbuf->buf);
	}
	printf("\n");
	
	// 执行命令代码......

	return 0;
}

/*---------------------------------------------------------------------
*	函数: 	serial_command_parse_example
*	功能:	静态内存版本完整的命令行解析示例代码
*---------------------------------------------------------------------*/
int serial_command_parse_example(const char* string)
{
	unsigned int i = 0x00, j = 0x00;
	libserial_parse_buf_t* spbuf1 = libserial_parse_create(256);
	libserial_parse_buf_t* spbuf2 = libserial_parse_create(64);
	libserial_parse_buf_t* spbuf3 = libserial_parse_create(256);

	// 初始化缓冲区
	libserial_parse_init(spbuf1);
	libserial_parse_init(spbuf2);
	libserial_parse_init(spbuf3);

	// 命令格式假定:  命令名称 参数1,参数2,参数3
	libserial_parse_set_divide(spbuf2, ' ');	// 设置解析命令的分隔字符为空格, 提取命令名称
	libserial_parse_set_ignore(spbuf2, '\0');	// 设置解析命令的忽略字符为字符串结束符, 即不忽略任何字符
	libserial_parse_set_divide(spbuf3, ',');    // 设置解析参数的分隔字符为逗号, 提取参数
	libserial_parse_set_ignore(spbuf3, ' ');    // 设置解析参数的忽略字符为空格, 忽略参数中的空格字符

	// 模拟串口收到的数据
	for (i = 0; i < strlen(string); i++)
	{
		// 解析字符串
		if (libserial_parse_text_nl(spbuf1, string[i]) == 0) {
			continue;
		}

		// 解析命令
		printf("[parse]:   [%s]\n", spbuf1->buf);
		if (serial_command_text(spbuf2, spbuf1->buf) == 0) {
			continue;
		}

		// 解析参数并执行命令
		serial_exec_command(spbuf3, spbuf2->buf, spbuf1->buf + strlen(spbuf2->buf));
	}

	// 释放缓冲区
	libserial_parse_release(spbuf1);
	libserial_parse_release(spbuf2);
	libserial_parse_release(spbuf3);

	return 0;
}

int main(void)
{
	const char* string = "@ABCDEFGHIJKLAA@\n\nQQ:1007566569\r\nlovemengx@qq.com\n123456789#++++++.....";
	const char* strcmd = "reg_w 0x01=0x20, 0x02= 0x15, 0x03=0x56";
	const char* strcmds = "wifi \"my wifi\",12345678\nvideo udp,192.168.1.115,8000\nreg_w 0x01=0x20, 0x02= 0x15, 0x03=0x56\nreg_r 0x01,0x02,0x05,0x06\nreboot\n";

	// 静态内存解析以行分隔的字符串示例
	printf("------------------------------------\n");
	printf("static memory newline example...\n");
	printf("------------------------------------\n");
	static_mem_newline_example(string);

	// 动态内存解析以行分隔的字符串示例
	printf("------------------------------------\n");
	printf("dynamic memory newline example...\n");
	printf("------------------------------------\n");
	dynamic_mem_newline_example(string);
	printf("------------------------------------\n");

	// 动态内存解析以自定义分隔和忽略的字符串示例
	printf("------------------------------------\n");
	printf("dynamic memory divide example...\n");
	printf("------------------------------------\n");
	static_mem_divide_example(strcmd);
	printf("------------------------------------\n");

	// 解析命令、参数并执行命令的完整示例
	printf("------------------------------------\n");
	printf("command parse example...\n");
	printf("------------------------------------\n");
	serial_command_parse_example(strcmds);
	printf("------------------------------------\n");

	return 0;
}

```
![](/images/sample_result.jpg "sample result")

