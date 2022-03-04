#include "lusb0_usb.h"
#include <time.h>
#include <iostream>
#include <stdio.h>


using namespace std;

usb_dev_handle * my_device;
struct usb_bus * bus;
struct usb_device * device;

int EP_IN = 0;
int EP_OUT = 0;


bool find_my_device(UINT32 PID, UINT32 VID);


int main()
{

	char receiveByte[512];
	memset(receiveByte, 0, sizeof(receiveByte));
	char op[10];
	int count = 0;

	usb_set_debug(1);

	if (find_my_device(0x434A, 0x5540))
	{
		while (true)
		{

			std::cout << "输入命令：";

			cin >> op;
			if (op[0] == 'w')
			{
				char writeByte[10];
				for (int i = 0; i < sizeof(writeByte); i++)
				{
					writeByte[i] = 50;
				}

				count = usb_bulk_write(my_device, EP_OUT, (char *)writeByte, 10, 10000);
				std::cout << "写入数据量:" << count << endl << endl;
			}
			else if (op[0] == 'r')
			{
				count = usb_bulk_read(my_device, EP_IN, receiveByte, 10, 100);
				std::cout << "收到数据数量：" << count << endl << "收到数据：" << receiveByte << endl << endl;
				memset(receiveByte, 0, count);
			}
			else if (op[0] == 't')
			{

				char writeByte[10];
				for (int i = 0; i < sizeof(writeByte); i++)
				{
					writeByte[i] = -66;
				}

				SYSTEMTIME time1, time2;
				GetLocalTime(&time1);

				for (int i = 0; i < 1; i++)
				{

					count = usb_bulk_write(my_device, EP_OUT, (char *)writeByte, 10, 0);
					if (count != 10)
					{
						std::cout << "写入错误！" << endl;
						break;
					}

					count = usb_bulk_read(my_device, EP_IN, receiveByte, 10, 500);
					if (count != 10)
					{
						std::cout << "error:" << count << endl;
					}
					receiveByte[11] = '\0';
					std::cout << "收到数据数量：" << count << endl << "收到数据：" << receiveByte << endl << endl;
				}
				GetLocalTime(&time2);

				//std::cout << "读取 1MB 数据用时:" << time2.wMilliseconds - time1.wMilliseconds << "ms" << endl;
				//std::cout << "速度:" << 1000.0f / (time2.wMilliseconds - time1.wMilliseconds) << "MB/s" << endl << endl;
			}
			else if (op[0] == 'e')
			{
				break;
			}
		}
	}
	else
	{
		std::cout << "没找到设备" << endl;
		return 0;
	}

	//注销接口，释放资源，和usb_claim_interface搭配使用。
	usb_release_interface(my_device, 0);
	usb_close(my_device);
	std::cout << "设备已关闭！" << endl;

	return 0;

}


bool find_my_device(UINT32 VID, UINT32 PID)
{
	bool isFind = false;
	// 环境初始化
	usb_init();
	// 寻找usb总线
	usb_find_busses();
	// 遍历总线上的设备
	usb_find_devices();

	// 遍历总线
	for (bus = usb_busses; bus; bus->next)
	{
		// 遍历总线上的设备找到需要的设备
		for (device = bus->devices; device; device->next)
		{
			//寻找指定的PID,VID设备
			if (device->descriptor.idProduct == PID && device->descriptor.idVendor == VID)
			{
				std::cout << "找到相关设备!" << endl;
				my_device = usb_open(device);

				//USB的相关配置

				char str[64];

				usb_get_string_simple(my_device, device->descriptor.iManufacturer, str, sizeof(str));
				std::cout << "设备制造商 : " << str << endl;
				usb_get_string_simple(my_device, device->descriptor.iProduct, str, sizeof(str));
				std::cout << "产品名字 : " << str << endl;

				int ret = usb_claim_interface(my_device, 2);

				// 获取端点的地址
				int ep = device->config->interface->altsetting->endpoint->bEndpointAddress;

				if (ep > 0x0f)
				{
					EP_IN = ep;
					EP_OUT = ep - 0x80;
				}
				else
				{
					EP_OUT = ep;
					EP_IN = ep + 0x80;
				}
				std::cout << "EP_IN:" << nouppercase << EP_IN << ", EP_OUT:" << nouppercase << EP_OUT << endl << endl;


				return true;
			}
		}
	}


	return isFind;
}
