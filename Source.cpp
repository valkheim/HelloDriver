#include <ntddk.h>

static UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\HelloDriver");
static UNICODE_STRING SymLinkName = RTL_CONSTANT_STRING(L"\\??\\HelloDriver");
static PDEVICE_OBJECT DeviceObject = nullptr;

void Unload(_In_ PDRIVER_OBJECT DriverObject)
{
	UNREFERENCED_PARAMETER(DriverObject);

	IoDeleteSymbolicLink(&SymLinkName);
	IoDeleteDevice(DeviceObject);
	KdPrint(("Driver unloaded\n"));
}

EXTERN_C_START
NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);
	auto status = STATUS_SUCCESS;

	status = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("failed to create device\n"));
		return status;
	}

	status = IoCreateSymbolicLink(&SymLinkName, &DeviceName);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("failed to create symlink\n"));
		IoDeleteDevice(DeviceObject);
		return status;
	}

	DriverObject->DriverUnload = Unload;
	DbgPrint("Driver loaded\r");
	return status;
}
EXTERN_C_END