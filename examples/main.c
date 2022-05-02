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

